#include <stack>
#include <string>

template <typename T> void show_stack(std::stack<T> s)
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

enum class Aryness
{
    UNARY,
    BINARY
};

enum class OperatorType
{
    LEFT_SHIFT,
    RIGHT_SHIFT,
    XOR,
    OR,
    NOT,
    AND,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    UNUSED, // for TEE and PARENS
    NUM_OPS
};

class Symbol
{
  public:
    Symbol(SymbolType type, int value); // NUMBER
    Symbol(SymbolType type, std::string identifier,
           OperatorType op_type); // OP
    SymbolType sym_type;

    // used if type is NUMBER
    int value;

    // used if type is OP
    OperatorType op_type;
    std::string identifier; // used for print
    std::function<int(int, int)> func;
    int precedence;
    SymbolAssociativity associativity;
    Aryness ary;
};

std::ostream &operator<<(std::ostream &out, const Symbol &s);

std::stack<Symbol> convert_to_infix(std::vector<Symbol>);

std::vector<Symbol> extract_symbols_from_line(const std::string &line);
