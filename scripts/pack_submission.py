#!/usr/bin/env python3
"""Inline the current core/ALNS sources into one standalone C++17 submission."""

import argparse
import hashlib
import math
import json

from cpp_compact import TOKEN, compact, macro_compact, shorten_aliases
from submission_prune import prune_unused, strip_submission_validation
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
LOCAL_INCLUDE = re.compile(r'^\s*#\s*include\s*"([^"]+)"\s*$')
OPTIMIZATION_PRAGMAS = {
    'O2': '',
    'O3': '#pragma GCC optimize("O3")\n',
    'O3-unroll': '#pragma GCC optimize("O3","unroll-loops")\n',
}
DEFAULT_OPTIMIZATION = 'O3-unroll'


def discard_diagnostics(source):
    """Replace console logging statements with no-ops, preserving control flow."""
    tokens = [m for m in TOKEN.finditer(source) if m.lastgroup not in ('space', 'comment')]
    replacements = []
    for i in range(len(tokens) - 3):
        if ([m.group() for m in tokens[i:i + 2]] != ['std', '::']
                or tokens[i + 2].group() not in ('cout', 'cerr', 'clog')
                or tokens[i + 3].group() != '<<'):
            continue
        depth = 0
        for token in tokens[i + 4:]:
            value = token.group()
            if value == ';' and depth == 0:
                replacements.append((tokens[i].start(), token.start()))
                break
            if value in ('(', '[', '{'):
                depth += 1
            elif value in (')', ']', '}'):
                depth -= 1
        else:
            raise ValueError('unterminated console logging statement')
    for start, end in reversed(replacements):
        source = source[:start] + '(void)0' + source[end:]
    return source


def library_sources(name):
    cmake = (ROOT / 'CMakeLists.txt').read_text(encoding='utf-8')
    match = re.search(r'add_library\(\s*' + re.escape(name) + r'\s+([^)]*)\)', cmake)
    if not match:
        raise ValueError(f'cannot find {name} sources in CMakeLists.txt')
    paths = re.findall(r'\bsrc/[^\s()]+\.cpp\b', match.group(1))
    if not paths:
        raise ValueError(f'no sources found for {name}')
    return [ROOT / path for path in paths]


def pack(output, time_limit, seed, limit, readable, optimization=DEFAULT_OPTIMIZATION):
    headers = set()
    inputs = {}

    def inline(path):
        path = path.resolve()
        if not path.is_relative_to(ROOT):
            raise ValueError(f'include outside repository: {path}')
        if path.suffix == '.hpp':
            if path in headers:
                return ''
            headers.add(path)
        raw = path.read_bytes()
        relative = path.relative_to(ROOT).as_posix()
        inputs[relative] = hashlib.sha256(raw).hexdigest()
        source = raw.decode('utf-8')
        # The standalone judge package uses the standard-library route master.
        source = re.sub(
            r'^#ifdef SOICT_ROUTE_POOL_CPLEX\n(.*?)^#endif\n',
            lambda match: match.group(1).split('#else\n', 1)[1]
            if '#else\n' in match.group(1) else '', source, flags=re.M | re.S)
        if relative == 'src/io/InstanceIO.cpp':
            start = source.index('void write_instance(')
            source = source[:start] + '} // namespace soict\n'
        elif relative == 'src/io/SolutionIO.cpp':
            start = source.index('Solution read_solution(')
            end = source.index('void write_solution(', start)
            source = source[:start] + source[end:]

        elif relative == 'src/search/alns/ALNS.cpp':
            # The feedback counter affects pricing, so retain it outside logging.
            source = source.replace('std::cerr<<"path_feedback_phase="<<++phase',
                                    '++phase; std::cerr<<"path_feedback_phase="<<phase')
            # Judge Config has no log directory; omit development logging only.
            start = source.index('    std::ofstream trace, operator_log;')
            end = source.index('    Solution empty;', start)
            source = source[:start] + source[end:]
            start = source.index('    const auto report_progress =')
            end = source.index('    while (', start)
            source = source[:start] + source[end:]
            start = source.index('                std::ostringstream line;')
            end = source.index('                std::cerr << line.str();', start) + len('                std::cerr << line.str();')
            source = source[:start] + source[end:]
            source = source.replace('        report_progress();\n', '')
            start = source.index('        if (iterations % 1000 == 0) {')
            end = source.index('    if (route_pool) {', start)
            source = source[:start] + '    }\n' + source[end:]
            start = source.index('        if (!config.log_directory.empty()) {')
            end = source.index('    if (config.check) best.check();', start)
            source = source[:start] + '    }\n' + source[end:]
            start = source.index('    if (trace) trace << iterations')
            end = source.index('    Result result;', start)
            source = source[:start] + source[end:]
            start = source.index('    if (clean_cut)\n')
            end = source.index('    return result;', start)
            source = source[:start] + source[end:]
            # Default path generation always selects paths_only: incomplete
            # banks use candidate sampling, and complete banks use enumeration.
            # The judge has no CLI, so its unused ALNS fallback can be omitted.
            source = re.sub(
                r'^#include "search/alns/(?:Balance.hpp|operators/Destroy/[^"\n]+|'
                r'operators/Repair/(?:GreedyInsertion|ProfitInsertion).hpp)"\n',
                '', source, flags=re.M)
            source = source.replace('    std::mt19937_64 rng(config.seed);\n', '')
            start = source.index('    std::vector<std::unique_ptr<DestroyOperator>> destroys;')
            end = source.index('    LocalSearch local_search;', start)
            source = source[:start] + source[end:]
            start = source.index('    if (!paths_only) {')
            end = source.index('    if (config.check) current.check();', start)
            source = source[:start] + source[end:]
            start = source.index('    double temperature =')
            end = source.index('    if (route_pool) {', start)
            source = source[:start] + source[end:]
            source = source.replace('result.iterations = iterations;', 'result.iterations = 0;')
            source = source.replace('result.annealing_complete = temperature <= minimum_temperature;',
                                    'result.annealing_complete = false;')
        source = prune_unused(relative, source)
        source = strip_submission_validation(relative, source)
        source = re.sub(r'^#include <(?:span|bit|filesystem)>\n', '', source, flags=re.M)
        source = source.replace(
            'const std::span<const std::size_t> subset(requests.data(), remaining);',
            'const std::vector<std::size_t> subset(requests.begin(), requests.begin() + remaining);')
        source = source.replace('std::span<const std::size_t>', 'const std::vector<std::size_t>&')
        source = source.replace('std::bit_width(', 'soict_bit_width(')
        source = source.replace('std::erase_if(', 'soict_erase_if(')
        # Keep every validation check, but abbreviate judge-only exception text.
        source = re.sub(
            r'(throw std::(?:logic_error|invalid_argument|runtime_error)\()"(?:\\.|[^"\\])*"',
            r'\1"error"', source)
        result = [f'\n// BEGIN {relative}\n']
        for line in source.splitlines(keepends=True):
            if re.fullmatch(r'\s*#\s*pragma\s+once\s*', line):
                continue
            match = LOCAL_INCLUDE.fullmatch(line.rstrip('\r\n'))
            if match:
                target = ROOT / 'include' / match.group(1)
                if not target.is_file():
                    target = path.parent / match.group(1)
                if not target.is_file():
                    raise FileNotFoundError(f'{relative}: unresolved include {match.group(1)}')
                result.append(inline(target))
            else:
                result.append(line)
        result.append(f'\n// END {relative}\n')
        return ''.join(result)

    pieces = ['// Generated judge package; C++17; no external libraries.\n',
              '#include <cstddef>\n#include <limits>\n#include <algorithm>\n#include <vector>\n',
              'template<class T, class P> void soict_erase_if(std::vector<T>& values, P predicate) {\n'
              'values.erase(std::remove_if(values.begin(), values.end(), predicate), values.end());\n}\n',
              'inline std::size_t soict_bit_width(std::size_t value) {\n'
              'return value ? std::numeric_limits<unsigned long long>::digits - '
              '__builtin_clzll(static_cast<unsigned long long>(value)) : 0;\n}\n']
    config_source = (ROOT / 'include/search/alns/Config.hpp').read_text()
    for flag in ('route_pool_repair', 'path_generation', 'candidate_sampling'):
        if not re.search(r'bool\s+' + flag + r'\s*=\s*true\s*;', config_source):
            raise ValueError(f'path-only judge package requires default {flag}=true')
    if not re.search(r'bool\s+check\s*=\s*false\s*;', config_source):
        raise ValueError('judge package requires default check=false')
    excluded = {'Instance.cpp', 'SyntheticGenerator.cpp', 'SolutionValidation.cpp', 'Balance.cpp',
                'RouteRemoval.cpp', 'OpportunityRemoval.cpp', 'GreedyInsertion.cpp',
                'RegretInsertion.cpp', 'ProfitInsertion.cpp'}
    for path in library_sources('soict_core') + library_sources('soict_alns'):
        if path.name not in excluded:
            pieces.append(inline(path))
    pieces.append('#include <iostream>\n#include <chrono>\n')
    pieces.append(f'''
// Judge entry point. Include input parsing in the five-minute wall budget.
int main() {{
    const auto started = std::chrono::steady_clock::now();
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);
        const auto instance = soict::read_instance(std::cin);
        soict::alns::Config config;
        config.seed = {seed}ULL;
        const double elapsed = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - started).count();
        const double allowance = {time_limit:.17g};
        const double reserve = allowance > 2 ? 1.0 : allowance * 0.1;
        config.time_limit = allowance - elapsed - reserve;
        if (config.time_limit <= 0) {{
            soict::Solution empty;
            empty.routes.resize(instance.taxis.size());
            soict::write_solution(std::cout, empty, instance);
        }} else {{
            const auto result = soict::alns::solve(instance, config);
            soict::write_solution(std::cout, result.solution, instance);
        }}
        return 0;
}}
''')
    source = discard_diagnostics(''.join(pieces))
    source = '\n'.join(line.rstrip() for line in source.splitlines()) + '\n'
    pragma = OPTIMIZATION_PRAGMAS[optimization]
    # Reserve the directive before compression, then prepend it verbatim.
    # The compactor only handles includes; a global pragma has no body position
    # to preserve and must never participate in identifier or macro rewriting.
    body_limit = limit - len(pragma)
    if body_limit <= 0:
        raise ValueError('character limit is smaller than the optimization pragma')
    packed, renames = compact(source)
    if len(packed) > body_limit:
        packed = macro_compact(packed, body_limit)
    packed, renames = shorten_aliases(packed, renames)
    packed = pragma + packed
    if len(packed) > limit:
        raise ValueError(f'packed source is {len(packed)} characters, exceeds CMS limit {limit}')
    if output.resolve() in [ROOT / name for name in inputs]:
        raise ValueError('output would overwrite a source input')
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(packed, encoding='utf-8')
    if readable:
        readable.parent.mkdir(parents=True, exist_ok=True)
        readable.write_text(pragma + source, encoding='utf-8')
    metadata = {
        'characters': len(packed), 'limit': limit, 'standard': 'c++17',
        'time_limit_seconds': time_limit, 'seed': seed,
        'optimization': optimization,
        'sha256': hashlib.sha256(packed.encode()).hexdigest(),
        'source_sha256': inputs, 'identifier_renames': renames,
    }
    output.with_suffix('.meta.json').write_text(json.dumps(metadata, indent=2) + '\n', encoding='utf-8')
    print(f'Wrote {output}: {len(packed)}/{limit} characters; {len(inputs)} source/header files')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=ROOT / 'submission/submission.cpp')
    parser.add_argument('--time-limit', type=float, default=300.0,
                        help='judge wall budget including input, reserving one second above two seconds (default: 300)')
    parser.add_argument('--seed', type=int, default=1)
    parser.add_argument('--max-characters', type=int, default=50000)
    parser.add_argument('--readable-out', type=Path, help='optional readable judge package for review')
    parser.add_argument('--optimization', choices=OPTIMIZATION_PRAGMAS,
                        default=DEFAULT_OPTIMIZATION,
                        help='GCC optimization pragma; O2 uses the compiler command options '
                             f'(default: {DEFAULT_OPTIMIZATION})')
    args = parser.parse_args()
    if not math.isfinite(args.time_limit) or args.time_limit <= 0:
        parser.error('time limit must be finite and positive')
    if not 0 <= args.seed < 2**64:
        parser.error('seed must fit an unsigned 64-bit integer')
    if args.max_characters <= 0:
        parser.error("character limit must be positive")
    pack(args.out.resolve(), args.time_limit, args.seed, args.max_characters, args.readable_out,
         args.optimization)


if __name__ == '__main__':
    main()
