"""Token-preserving compaction for this project's C++ submission."""
import collections
import re
import string

TOKEN = re.compile(r'''(?P<space>\s+)|(?P<comment>//[^\n]*|/\*[\s\S]*?\*/)|(?P<literal>(?:u8|u|U|L)?"(?:\\[\s\S]|[^"\\])*"|(?:u8|u|U|L)?'(?:\\[\s\S]|[^'\\])*')|(?P<identifier>[A-Za-z_]\w*)|(?P<number>(?:\d|\.\d)(?:[A-Za-z0-9_.']|(?<=[eEpP])[+-])*)|(?P<operator><=>|>>=|<<=|->\*|\.\.\.|::|->|\+\+|--|<<|>>|&&|\|\||\+=|-=|\*=|/=|%=|&=|\|=|\^=|==|!=|<=|>=|##|[^\s])''')


def lex(source):
    tokens = []
    for match in TOKEN.finditer(source):
        if match.lastgroup not in ('space', 'comment'):
            tokens.append((match.lastgroup, match.group()))
    return tokens


def shorten_aliases(source, renames):
    """Use remaining one-letter names for the most frequent generated aliases."""
    counts = collections.Counter(value for kind, value in lex(source) if kind == 'identifier')
    candidates = set(renames.values()) | set(re.findall(r'^#define\s+(\w+)', source, re.M))
    candidates = [name for name in candidates if len(name) > 1]
    candidates.sort(key=lambda name: (-counts[name] * (len(name) - 1), name))
    free = [name for name in string.ascii_letters if name not in counts]
    changes = dict(zip(candidates, free))
    shortened = TOKEN.sub(
        lambda match: changes.get(match.group(), match.group())
        if match.lastgroup == 'identifier' else match.group(), source)
    expected = [(kind, changes.get(value, value) if kind == 'identifier' else value)
                for kind, value in lex(source)]
    if lex(shortened) != expected:
        raise ValueError('alias shortening changed the C++ token sequence')
    return shortened, {name: changes.get(alias, alias) for name, alias in renames.items()}


def macro_compact(source, limit):
    """Compress repeated token sequences using object-like C++ macros.

    Definitions only refer to earlier macros. Expanding the resulting token
    stream must reproduce the original stream exactly.
    """
    directives, body = source.rsplit('\n', 2)[:2]
    original = [value for _, value in lex(body)]
    tokens = original[:]
    occupied = set(original) | set(re.findall(r'\b\w+\b', directives))
    occupied.update("do if or and not xor int for asm auto bool case char else enum goto long true void break catch class compl const false float short throw union using while bitand bitor delete double friend inline new not_eq or_eq public return signed sizeof static struct switch typeid and_eq xor_eq default alignas alignof continue explicit export extern mutable namespace noexcept nullptr operator private protected register requires template this thread_local typedef typename unsigned virtual volatile wchar_t".split())
    aliases = (a + b for a in string.ascii_letters for b in string.ascii_letters)
    definitions = {}

    def render(values):
        output = []
        previous = None
        for value in values:
            if previous is not None and (
                [v for _, v in lex(previous + value)] != [previous, value]
                or (previous.endswith(('"', "'")) and re.match(r'[A-Za-z_]', value))
            ):
                output.append(' ')
            output.append(value)
            previous = value
        return ''.join(output)

    # Token lengths include a conservative spacing allowance for ranking.
    for _ in range(1200):
        candidates = collections.defaultdict(list)
        for width in (2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64):
            for i in range(len(tokens) - width + 1):
                candidates[tuple(tokens[i:i + width])].append(i)
        alias = next(a for a in aliases if a not in occupied)
        best = None
        best_gain = 0
        for sequence, occurrences in candidates.items():
            count = len(occurrences)
            if count < 2:
                continue
            length = sum(map(len, sequence))
            estimated = (length - len(alias)) * (count - 1) - 12
            if estimated <= best_gain:
                continue
            # Do not count overlapping occurrences as separate replacements.
            positions = []
            end = -1
            width = len(sequence)
            for i in occurrences:
                if i >= end:
                    positions.append(i)
                    end = i + width
            gain = (len(render(sequence)) - len(alias)) * (len(positions) - 1) - 12
            if gain > best_gain:
                best_gain = gain
                best = sequence, positions
        if best is None:
            break
        sequence, positions = best
        definitions[alias] = sequence
        occupied.add(alias)
        updated = []
        end = 0
        for position in positions:
            updated.extend(tokens[end:position])
            updated.append(alias)
            end = position + len(sequence)
        updated.extend(tokens[end:])
        tokens = updated
        if len(definitions) % 10 == 0:
            packed = directives + '\n' + ''.join(
                '#define ' + name + ' ' + render(values) + '\n'
                for name, values in definitions.items()) + render(tokens) + '\n'
            if len(packed) <= limit:
                break

    def expand(values):
        for value in values:
            if value in definitions:
                yield from expand(definitions[value])
            else:
                yield value

    if list(expand(tokens)) != original:
        raise ValueError('macro compression changed the C++ token sequence')
    return directives + '\n' + ''.join(
        '#define ' + name + ' ' + render(values) + '\n'
        for name, values in definitions.items()) + render(tokens) + '\n'


def compact(source):
    directives = []
    body = []
    for line in source.splitlines(keepends=True):
        if line.lstrip().startswith('#'):
            directive = line.strip()
            if not re.fullmatch(r'#\s*include\s*<[^>]+>', directive):
                raise ValueError(f'unsupported preprocessor directive: {directive}')
            if directive not in directives:
                directives.append(directive)
        else:
            body.append(line)
    source = ''.join(body)
    if re.search(r'\b(?:u8|u|U|L)?R"', source):
        raise ValueError('raw string literals require explicit lexer support')
    tokens = lex(source)
    counts = collections.Counter(value for kind, value in tokens if kind == 'identifier')
    # Never rename standard-library references or names used by include directives.
    protected = set(re.findall(r'\b\w+\b', '\n'.join(directives)))
    protected.update(re.findall(r'\bstd::(\w+)', source))
    for chain in re.findall(r'\bstd::[A-Za-z_:]+', source):
        protected.update(chain.split('::'))
    protected.update("alignas alignof and and_eq asm auto bitand bitor bool break case catch char char8_t char16_t char32_t class compl concept const consteval constexpr constinit const_cast continue co_await co_return co_yield decltype default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline int long mutable namespace new noexcept not not_eq nullptr operator or or_eq private protected public register reinterpret_cast requires return short signed sizeof static static_assert static_cast struct switch template this thread_local throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq".split())
    protected.update("size begin end cbegin cend rbegin rend resize clear reserve empty front back data capacity max_size push_back pop_back emplace_back emplace insert erase swap find first second value value_or has_value reset get release move open close read write str eof fail good getline peek ignore tellg seekg count now duration max min assign push pop top at".split())
    protected.update("rdbuf sgetn append remove_prefix c_str".split())
    protected.update("emplace_hint value_type".split())
    protected.update("__builtin_clzll main final override nodiscard maybe_unused deprecated fallthrough noreturn likely unlikely no_unique_address what infinity quiet_NaN signaling_NaN lowest epsilon digits digits10 is_integer is_signed is_exact is_specialized min_exponent max_exponent npos fetch_add fetch_sub load store exchange compare_exchange_weak compare_exchange_strong memory_order_relaxed".split())
    candidates = [name for name in counts if len(name) >= 4 and name not in protected]
    candidates.sort(key=lambda name: (-counts[name] * (len(name) - 3), name))
    renames = {}
    aliases = (a + b for a in string.ascii_letters for b in string.ascii_letters + string.digits)
    for name in candidates:
        alias = next(a for a in aliases if a not in counts and a not in protected)
        renames[name] = alias
    changed = [(kind, renames.get(value, value) if kind == 'identifier' else value) for kind, value in tokens]
    output = []
    previous = None
    for kind, value in changed:
        if previous is not None:
            # Add a space only when concatenation would change token boundaries.
            pair = lex(previous + value)
            if [token for _, token in pair] != [previous, value]:
                output.append(' ')
        output.append(value)
        previous = value
    body = ''.join(output)
    # Guard against accidental token changes before the compiler checks semantics.
    if lex(body) != changed:
        raise ValueError('compaction changed the C++ token sequence')
    return '\n'.join(directives) + '\n' + body + '\n', renames
