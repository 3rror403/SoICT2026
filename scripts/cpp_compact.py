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
    protected.update("size begin end cbegin cend rbegin rend resize clear reserve empty front back data capacity max_size push_back pop_back emplace_back emplace insert erase swap find first second value value_or has_value reset get release move open close read write str eof fail good getline peek ignore tellg seekg count now duration max min".split())
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
