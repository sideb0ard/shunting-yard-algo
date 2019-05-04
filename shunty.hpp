#include <stack>
#include <string>

template <typename T> void show_stack(std::stack<T> s);
{
    while (!s.empty())
    {
        std::cout << ' ' << s.top();
        s.pop();
    }
    std::cout << std::endl;
}

enum class SymbolType
{
    NUMBER,
    OP,
    LEFT_PARENS,
    RIGHT_PARENS,
    TEE_TOKEN
};

enum class SymbolAssociativity
{
    LEFT,
    RIGHT
};

struct Symbol
{
    Symbol(SymbolType type, int value);              // NUMBER
    Symbol(SymbolType type) : type{type};            // LEFT or RIGHT PARENS
    Symbol(SymbolType type, std::string identifier); // OP
    Symbol(SymbolType type);                         // TEE_TOKEN

    SymbolType type;

    // used if type is NUMBER
    int value;

    // used if type is OP
    std::string identifier;
    std::function<int(int, int)> func;
    int precedence;
    SymbolAssociativity associativity;
};

std::ostream &operator<<(std::ostream &out, const Symbol &s)
{
    if (s.type == SymbolType::NUMBER)
        out << s.value;
    else if (s.type == SymbolType::OP)
        out << s.identifier;
    else if (s.type == SymbolType::LEFT_PARENS)
        out << " ( ";
    else if (s.type == SymbolType::RIGHT_PARENS)
        out << " ) ";
    return out;
}

std::stack<Symbol> convert_to_infix(const std::string &line);
