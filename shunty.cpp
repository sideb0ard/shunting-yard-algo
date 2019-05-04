#include <algorithm>
#include <iostream>
#include <map>
#include <range/v3/all.hpp>
#include <stack>
#include <string>

#include "shunty.hpp"

const std::vector<std::string> ops = {"<<", ">>", "^", "|", "~", "&",
                                      "+",  "-",  "*", "/", "%", "t"};

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

struct Symbol
{
    Symbol(SymbolType type, int value) : type{type}, value{value} {}; // NUMBER
    Symbol(SymbolType type) : type{type} {};        // LEFT or RIGHT PARENS
    Symbol(SymbolType type, std::string identifier) // OP
        : type{type}, identifier{identifier}
    {
        if (identifier.empty())
            return;

        if (identifier == "^")
        {
            precedence = 4;
            associativity = SymbolAssociativity::RIGHT;
            func = [](int a, int b) {
                int tot = 0;
                for (int i = 0; i < b; ++i)
                {
                    tot += a * a;
                };
                return tot;
            };
        }
        else if (identifier == "*")
        {
            precedence = 3;
            associativity = SymbolAssociativity::LEFT;
            func = [](int a, int b) { return a * b; };
        }
        else if (identifier == "/")
        {
            precedence = 3;
            associativity = SymbolAssociativity::LEFT;
            func = [](int a, int b) { return a / b; };
        }
        else if (identifier == "+")
        {
            precedence = 2;
            associativity = SymbolAssociativity::LEFT;
            func = [](int a, int b) { return a + b; };
        }
        else if (identifier == "-")
        {
            precedence = 2;
            associativity = SymbolAssociativity::LEFT;
            func = [](int a, int b) { return a - b; };
        }
    }

    SymbolType type;

    // type is NUMBER
    int value;

    // type is OP
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

bool is_number(const std::string &s)
{
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) {
                             return !std::isdigit(c);
                         }) == s.end();
}

bool is_operator(const std::string &s)
{
    if (std::find(ops.begin(), ops.end(), s) != ops.end())
    {
        return true;
    }
    return false;
}

bool is_parenthesis(const std::string &s, SymbolAssociativity sym)
{
    if ("(" == s && sym == SymbolAssociativity::LEFT)
        return true;
    else if (")" == s && sym == SymbolAssociativity::RIGHT)
        return true;

    return false;
}

std::stack<Symbol> convert_to_infix(const std::string &line)
{
    std::stack<Symbol> operator_stack;
    std::stack<Symbol> output_queue;

    auto tokens = line | ranges::view::split(' ');
    for (const std::string &t : tokens)
    {
        std::cout << "\nLooking at " << t << std::endl;
        if (is_number(t))
        {
            output_queue.push(Symbol(SymbolType::NUMBER, std::stoi(t)));
            std::cout << "Add token to output" << std::endl;
            // show_stack(output_queue);
            // show_stack(operator_stack);
        }
        else if (is_operator(t))
        {
            Symbol new_op = Symbol(SymbolType::OP, t);
            while (operator_stack.size() > 0)
            {
                Symbol &top_of_stack = operator_stack.top();

                if (top_of_stack.precedence > new_op.precedence ||
                    (top_of_stack.precedence == new_op.precedence &&
                     top_of_stack.associativity == SymbolAssociativity::LEFT))
                {
                    if (top_of_stack.type == SymbolType::LEFT_PARENS)
                        break;
                    operator_stack.pop();
                    output_queue.push(top_of_stack);
                    std::cout << "POP stack to output: " << top_of_stack
                              << std::endl;
                }
                else
                {
                    std::cout << "ELSE!" << std::endl;
                    break;
                }
            }
            std::cout << "PUSH token to stack" << new_op << std::endl;
            operator_stack.push(new_op);
        }
        else if (is_parenthesis(t, SymbolAssociativity::LEFT))
        {
            std::cout << "Push token to stack" << std::endl;
            operator_stack.push(Symbol(SymbolType::LEFT_PARENS));
        }
        else if (is_parenthesis(t, SymbolAssociativity::RIGHT))
        {
            bool left_parens_found = false;
            while (operator_stack.size() > 0 && !left_parens_found)
            {
                std::cout << "Pop stack" << std::endl;
                Symbol &top_of_stack = operator_stack.top();
                operator_stack.pop();
                if (top_of_stack.type == SymbolType::LEFT_PARENS)
                    left_parens_found = true;
                else
                    output_queue.push(top_of_stack);
                // show_stack(output_queue);
                // show_stack(operator_stack);
            }
            if (!left_parens_found)
            {
                std::cout << "ERRR! doesn't compute!" << std::endl;
            }
        }
    }
    while (operator_stack.size() > 0)
    {
        Symbol &top_of_stack = operator_stack.top();
        operator_stack.pop();
        output_queue.push(top_of_stack);
    }

    return output_queue;
}

int main(int argc, char **argv)
{
    std::cout << "MEERP!\n";

    std::string line;
    while (!std::getline(std::cin, line).fail())
    {
        std::stack<Symbol> output_queue = convert_to_infix(line);
        show_stack(output_queue);
    }
}
