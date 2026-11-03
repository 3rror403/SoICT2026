"""Remove components unused by the fixed, indexed judge configuration."""

from cpp_compact import TOKEN
import re


def discard_validation(source):
    """Remove assertion-style guards, retaining mutations inside conditions."""
    tokens = [m for m in TOKEN.finditer(source) if m.lastgroup not in ('space', 'comment')]
    replacements = []
    mutations = {'insert', 'remove', 'remove_requests', 'replace_request',
                 'swap_requests', 'exchange_route_suffixes'}
    for i, token in enumerate(tokens[:-1]):
        if token.group() != 'if' or tokens[i + 1].group() != '(':
            continue
        depth = 1
        end = i + 2
        while depth:
            if tokens[end].group() == '(':
                depth += 1
            elif tokens[end].group() == ')':
                depth -= 1
            end += 1
        if end == len(tokens) or tokens[end].group() != 'throw':
            continue
        finish = end + 1
        while tokens[finish].group() != ';':
            finish += 1
        condition = tokens[i + 2:end - 1]
        replacement = ''
        if any(m.group() in mutations for m in condition):
            replacement = '(void)(' + source[tokens[i + 1].end():tokens[end - 1].start()] + ');'
        replacements.append((token.start(), tokens[finish].end(), replacement))
    for start, end, replacement in reversed(replacements):
        source = source[:start] + replacement + source[end:]
    return source


def unchecked_access(source):
    """All remaining at() calls address vectors with solver-established bounds."""
    tokens = [m for m in TOKEN.finditer(source) if m.lastgroup not in ('space', 'comment')]
    replacements = []
    for i in range(len(tokens) - 2):
        if [m.group() for m in tokens[i:i + 3]] != ['.', 'at', '(']:
            continue
        depth = 1
        end = i + 3
        while depth:
            if tokens[end].group() == '(':
                depth += 1
            elif tokens[end].group() == ')':
                depth -= 1
            end += 1
        replacements.append((tokens[i].start(), tokens[i + 2].end(), '['))
        replacements.append((tokens[end - 1].start(), tokens[end - 1].end(), ']'))
    for start, end, replacement in sorted(replacements, reverse=True):
        source = source[:start] + replacement + source[end:]
    return source


def strip_submission_validation(relative, source):
    if relative == 'src/model/Solution.cpp':
        # Preserve cost subtraction and revenue addition order for valid routes.
        source = '''#include "model/Solution.hpp"
namespace soict {
Evaluation Solution::evaluate(const Instance& instance) const {
    const auto n = instance.passengers.size(), m = instance.parcels.size();
    const auto k_count = instance.taxis.size();
    std::vector<bool> served(n + m, false);
    double benefit = 0;
    for (std::size_t k = 0; k < k_count; ++k) {
        std::size_t previous = k;
        for (const auto node : routes[k]) {
            benefit -= instance.cost(previous, node);
            if (node < k_count + n) served[node - k_count] = true;
            else if (node >= k_count + 2 * n && node < k_count + 2 * n + m)
                served[n + node - k_count - 2 * n] = true;
            previous = node;
        }
    }
    for (std::size_t i = 0; i < n + m; ++i)
        if (served[i]) benefit += i < n ? instance.passengers[i].revenue :
                                        instance.parcels[i - n].revenue;
    return {true, benefit, {}};
}
} // namespace soict
'''
    source = discard_validation(source)
    source = source.replace('    instance.validate();\n', '')
    if relative == 'include/model/Instance.hpp':
        source = source.replace('    void validate() const;\n', '')
    elif relative == 'src/io/InstanceIO.cpp':
        source = source.replace('    std::vector<bool> seen(v, false);\n', '')
        source = source.replace('        seen[id - 1] = true;\n', '')
        source = source.replace(', const char* field', '')
        source = re.sub(r', "[^"\n]*"\)', ')', source)
        source = source.replace('const auto parsed = std::from_chars', 'std::from_chars')
        # GCC 9 supplies integer from_chars, but no floating-point overload.
        # next() returns a bounded view, so give strtod a terminated copy.
        anchor = '}\n\nInstance read_instance('
        overload = '''void read(NumericReader& input, double& value) {
    value = std::strtod(std::string(input.next()).c_str(), nullptr);
}
'''
        if source.count(anchor) != 1:
            raise ValueError('cannot locate numeric reader overload insertion point')
        source = '#include <cstdlib>\n' + source.replace(anchor, overload + anchor)
    elif relative == 'include/search/alns/InsertionScratch.hpp':
        source = re.sub(r'^\s*constexpr auto limit =[^\n]*\n', '', source, flags=re.M)
    return unchecked_access(source)


def erase_block(source, anchor, semicolon=False):
    start = source.index(anchor)
    opening = source.index('{', start)
    depth = 0
    for token in TOKEN.finditer(source, opening):
        if token.lastgroup in ('literal', 'comment'):
            continue
        if token.group() == '{':
            depth += 1
        elif token.group() == '}':
            depth -= 1
            if depth == 0:
                end = token.end()
                if semicolon:
                    assert source[end] == ';'
                    end += 1
                return source[:start] + source[end:]
    raise ValueError(f'unterminated block: {anchor}')


def prune_metrics(relative, source):
    if relative.endswith('.hpp'):
        source = re.sub(r'\s*std::string_view name\(\) const override \{ return "[^"]*"; \}',
                        '', source)
        source = source.replace('    virtual std::string_view name() const = 0;\n', '')
        source = source.replace('[[nodiscard]] ', '')
    if relative == 'src/search/alns/RouteCache.cpp':
        source = re.sub(r'\b(?:arrival_|departure_|load_)\.clear\(\);\s*', '', source)
        source = re.sub(r'^\s*arrival_ = other\.arrival_;[^\n]*\n', '', source, flags=re.M)
        source = re.sub(r'^\s*arrival_\.reserve\(size\);[^\n]*\n', '', source, flags=re.M)
        source = source.replace('    double departure = 0, load = 0;\n', '')
        source = source.replace('        const double arrival = departure + in.time(previous, node);\n', '')
        source = re.sub(r'^\s*(?:departure =|load \+=|arrival_\.push_back|'
                        r'departure_\.push_back|load_\.push_back)[^\n]*\n', '', source, flags=re.M)
    elif relative == 'include/search/alns/RouteCache.hpp':
        source = re.sub(r'^\s*const std::vector<double>& (?:arrival|departure|load)\(\)[^\n]*\n',
                        '', source, flags=re.M)
        source = source.replace('    std::vector<double> arrival_, departure_, load_;\n', '')
    elif relative == 'include/search/alns/InsertionScratch.hpp':
        source = source.replace(': requests_(requests), stride_', ': stride_')
        source = re.sub(r'^\s*std::size_t (?:request_count|bytes)\(\)[^\n]*\n', '', source, flags=re.M)
        source = source.replace('std::size_t requests_, stride_;', 'std::size_t stride_;')
    elif relative == 'src/search/alns/operators/Repair/RoutePoolRepair.cpp':
        source = source.replace('generated_prefixes_ = completed_taxis_ = 0;', 'completed_taxis_ = 0;')
        source = re.sub(r'^\s*(?:generated_prefixes_|total_generated_prefixes_) \+= prefixes;\n',
                        '', source, flags=re.M)
        source = re.sub(r'^\s*const auto (?:phase_start|refinement_start|before|start|before_move|'
                        r'pair_start|triple_start|dual_start|tree_start|pair_nodes)\s*=[^\n]*\n',
                        '', source, flags=re.M)
        source = re.sub(r'^\s*(?:double refinement_seconds|std::vector<double> move_seconds)[^\n]*\n',
                        '', source, flags=re.M)
        source = re.sub(r'^\s*(?:move_seconds\[j\]|move_gain\[j\]|refinement_seconds|'
                        r'refinement_gain)\s*\+=[^\n]*\n', '', source, flags=re.M)
        start = source.index('    for (std::size_t j=0;j<refinement.size();++j)\n        std::cerr')
        end = source.index('    return best.solution();', start)
        source = source[:start] + source[end:]
        source = source.replace('complete_ = false;', '').replace('complete_ = solved.complete;', '')
        source = source.replace('complete_ = !timed_out_;', '')
    elif relative == 'include/search/alns/operators/Repair/RoutePoolRepair.hpp':
        source = re.sub(r'^\s*(?:std::size_t|bool) (?:generated_prefixes|total_generated_prefixes|'
                        r'completed_taxis|columns|nodes|timed_out|master_complete)\(\)[^\n]*\n',
                        '', source, flags=re.M)
        source = source.replace('    bool complete_ = false;\n', '')
        source = source.replace('std::size_t generated_prefixes_ = 0, completed_taxis_ = 0;',
                                'std::size_t completed_taxis_ = 0;')
        source = source.replace('    std::size_t total_generated_prefixes_ = 0;\n', '')
    elif relative == 'src/search/alns/operators/LocalSearch/CleanCut2OptStar.cpp':
        source = re.sub(r'^\s*(?:\+\+calls_|\+\+applied_moves_|total_gain_ \+=)[^\n]*\n',
                        '', source, flags=re.M)
        source = source.replace('                    const double before = state.benefit();\n', '')
    elif relative == 'include/search/alns/operators/LocalSearch/CleanCut2OptStar.hpp':
        source = re.sub(r'^\s*(?:std::size_t|double) (?:calls|applied_moves|total_gain)\(\)[^\n]*\n',
                        '', source, flags=re.M)
        source = re.sub(r'^\s*mutable (?:std::size_t calls_|double total_gain_)[^\n]*\n',
                        '', source, flags=re.M)
    elif relative == 'src/search/alns/ALNS.cpp':
        source = re.sub(r'^\s*const auto (?:initial_completed_taxis|pricing_start|before|pool_start|'
                        r'polish_start)\s*=[^\n]*\n', '', source, flags=re.M)
        source = source.replace('    SearchState candidate = current;\n', '')
        source = source.replace('    CleanCut2OptStar* clean_cut = nullptr;\n', '')
        source = source.replace('        clean_cut = move.get();\n', '')
        source = re.sub(r'^\s*result\.(?:benefit|iterations|seconds|annealing_complete|paths_only) =[^\n]*\n',
                        '', source, flags=re.M)
    elif relative == 'include/search/alns/ALNS.hpp':
        source = re.sub(r'struct Result \{.*?\};', 'struct Result { Solution solution; };',
                        source, flags=re.S)
    elif relative == 'include/search/alns/Config.hpp':
        source = re.sub(r'^\s*(?:bool|double|std::size_t|std::string) (?:temperature|cooling|check|'
                        r'all_unserved_repair|profit_repair|route_removal|opportunity_removal|'
                        r'ls_interval|thompson|log_directory)\b[^\n]*\n', '', source, flags=re.M)
    return source


def prune_unused(relative, source):
    if relative == 'src/search/alns/operators/Repair/InsertionSearch.cpp':
        # The remaining solver always builds and propagates EmptyTaxiIndex.
        start = source.index('namespace {\nInsertionOptions insertion_options_impl(')
        end = source.index('void PositiveInsertionCache::refresh_entry(', start)
        source = source[:start] + source[end:]
        start = source.index('bool RepairInsertionCache::enabled_for(')
        source = source[:start] + '} // namespace soict::alns\n'
        source = erase_block(source, 'void combine(')
        source = erase_block(source, 'bool add_empty_taxis(')
    elif relative == 'include/search/alns/operators/Repair/InsertionSearch.hpp':
        source = source.replace('"search/alns/operators/Repair/RepairOperator.hpp"',
                                '"search/alns/SearchState.hpp"')
        source = erase_block(source, 'class BestInsertionCache {', semicolon=True)
        source = erase_block(source, 'class RepairInsertionCache {', semicolon=True)
        start = source.index('InsertionOptions insertion_options(')
        end = source.index('// The supplied route', start)
        source = source[:start] + source[end:]
        source = erase_block(source, '    [[nodiscard]] std::size_t request_segment_bytes()')
        source = erase_block(source, '    [[nodiscard]] std::size_t storage_bytes()')
    elif relative == 'src/search/alns/operators/LocalSearch/AddRequest.cpp':
        source = source.replace('    const bool indexed = bool(state.empty_taxi_index());\n', '')
        source = source.replace('if (indexed) ', '')
        source = source.replace('indexed ? cache_.best(request) :\n'
                                '                                       fallback_cache_.best(state, request)',
                                'cache_.best(request)')
    elif relative == 'include/search/alns/operators/LocalSearch/AddRequest.hpp':
        source = source.replace('    mutable BestInsertionCache fallback_cache_;\n', '')
    elif relative == 'src/search/alns/operators/LocalSearch/ReplaceRequest.cpp':
        source = source.replace('    if (!use_smd_) return best_choice_reference(state);\n', '')
        source = erase_block(source,
            'std::optional<ReplacementChoice> ReplaceRequest::best_choice_reference(')
    elif relative == 'include/search/alns/operators/LocalSearch/ReplaceRequest.hpp':
        source = source.replace('explicit ReplaceRequest(bool use_smd = true) : use_smd_(use_smd) {}',
                                'ReplaceRequest() = default;')
        source = source.replace('    bool use_smd_;\n', '')
        source = source.replace('    [[nodiscard]] std::optional<ReplacementChoice> best_choice_reference(const SearchState&) const;\n', '')
        source = erase_block(source, '    struct Entry {', semicolon=True)
        source = source.replace('        std::uint64_t route_version = 0;\n', '')
        source = source.replace('        std::vector<Entry> entries;\n', '')
    elif relative == 'src/search/alns/operators/LocalSearch/SwapRequest.cpp':
        source = erase_block(source, '    if (!use_smd_) {')
    elif relative == 'include/search/alns/operators/LocalSearch/SwapRequest.hpp':
        source = source.replace('explicit SwapRequest(bool use_smd = true) : use_smd_(use_smd) {}',
                                'SwapRequest() = default;')
        source = source.replace('    bool use_smd_;\n', '')
    elif relative == 'src/search/alns/operators/Repair/RoutePoolRepair.cpp':
        source = erase_block(source, 'void RoutePoolRepair::write(')
        # This package has no CPLEX backend; the native pricing branch remains.
        source = erase_block(source,
            '    if (route_master_uses_cplex() && std::chrono::steady_clock::now()<deadline) {')
    elif relative == 'include/search/alns/operators/Repair/RoutePoolRepair.hpp':
        source = source.replace('    void write(std::ostream& out) const;\n', '')
    elif relative == 'src/search/alns/SearchState.cpp':
        source = erase_block(source, 'void SearchState::swap(')
        source = erase_block(source, 'void SearchState::check(')
    elif relative == 'include/search/alns/SearchState.hpp':
        source = source.replace('    void swap(SearchState& other) noexcept;\n', '')
        source = source.replace('    void check() const;\n', '')
    elif relative == 'src/search/alns/RouteCache.cpp':
        source = erase_block(source, 'double RouteCandidate::cost_unchecked(')
    elif relative == 'include/search/alns/RouteCache.hpp':
        source = source.replace('    [[nodiscard]] double cost_unchecked() const;\n', '')
    elif relative == 'src/search/alns/ALNS.cpp':
        source = source.replace('    if (config.check) current.check();\n', '')
        source = source.replace('    if (config.check) best.check();\n', '')
    return prune_metrics(relative, source)
