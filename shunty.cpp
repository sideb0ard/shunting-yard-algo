#include <algorithm>
#include <iostream>
#include <map>
#include <range/v3/all.hpp>
#include <sstream>
#include <stack>
#include <string>

#include "shunty.hpp"

namespace
{

const std::vector<std::string> ops = {"<<", ">>", "^", "|", "~", "&",
                                      "+",  "-",  "*", "/", "%", "t"};
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

bool is_valid_char(char ch)
{
    if (isdigit(ch))
        return true;

    static char acceptable[] = {'<', '>', '^', '|', '~', '&', '+', '-',
                                '*', '/', '%', '(', ')', 't', 0};

    for (auto &c : acceptable)
    {
        if (ch == c)
            return true;
    }
    return false;
}

OperatorType which_op(char c)
{
    switch (c)
    {
    case ('<'):
        return OperatorType::LEFT_SHIFT;
        break;
    case ('>'):
        return OperatorType::RIGHT_SHIFT;
        break;
    case ('^'):
        return OperatorType::XOR;
        break;
    case ('|'):
        return OperatorType::OR;
        break;
    case ('~'):
        return OperatorType::NOT;
        break;
    case ('&'):
        return OperatorType::AND;
        break;
    case ('+'):
        return OperatorType::PLUS;
        break;
    case ('-'):
        return OperatorType::MINUS;
        break;
    case ('*'):
        return OperatorType::MULTIPLY;
        break;
    case ('/'):
        return OperatorType::DIVIDE;
        break;
    case ('%'):
        return OperatorType::MODULO;
        break;
    default:
        return OperatorType::UNUSED;
    }
}

} // namespace

Symbol::Symbol(SymbolType type, int value) : sym_type{type}, value{value} {}

Symbol::Symbol(SymbolType type, std::string identifier, OperatorType op_type)
    : sym_type{type}, identifier{identifier}, op_type{op_type}
{
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

std::ostream &operator<<(std::ostream &out, const Symbol &s)
{
    if (s.sym_type == SymbolType::NUMBER)
        out << s.value;
    else if (s.sym_type == SymbolType::OP)
        out << s.identifier;
    else if (s.sym_type == SymbolType::LEFT_PARENS)
        out << " ( ";
    else if (s.sym_type == SymbolType::RIGHT_PARENS)
        out << " ) ";
    else if (s.sym_type == SymbolType::TEE_TOKEN)
        out << " t ";
    return out;
}

std::stack<Symbol> convert_to_infix(std::vector<Symbol> tokens)
{
    std::stack<Symbol> operator_stack;
    std::stack<Symbol> output_queue;

    for (const Symbol &t : tokens)
    {
        std::cout << "\nLooking at " << t << std::endl;
        if (t.sym_type == SymbolType::NUMBER)
        {
            output_queue.push(t);
            std::cout << "Add token to output" << std::endl;
        }
        else if (t.sym_type == SymbolType::OP)
        {
            while (operator_stack.size() > 0)
            {
                Symbol &top_of_stack = operator_stack.top();

                if (top_of_stack.precedence > t.precedence ||
                    (top_of_stack.precedence == t.precedence &&
                     top_of_stack.associativity == SymbolAssociativity::LEFT))
                {
                    if (top_of_stack.sym_type == SymbolType::LEFT_PARENS)
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
            std::cout << "PUSH token to stack" << t << std::endl;
            operator_stack.push(t);
        }
        else if (t.sym_type == SymbolType::LEFT_PARENS)
        {
            std::cout << "Push token to stack" << std::endl;
            operator_stack.push(t);
        }
        else if (t.sym_type == SymbolType::RIGHT_PARENS)
        {
            bool left_parens_found = false;
            while (operator_stack.size() > 0 && !left_parens_found)
            {
                std::cout << "Pop stack" << std::endl;
                Symbol &top_of_stack = operator_stack.top();
                operator_stack.pop();
                if (top_of_stack.sym_type == SymbolType::LEFT_PARENS)
                    left_parens_found = true;
                else
                    output_queue.push(top_of_stack);
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

std::vector<Symbol> extract_symbols_from_line(const std::string &line)
{
    std::vector<Symbol> symbols;

    auto tokens = line | ranges::view::split(' ');
    for (const std::string &token : tokens)
    {
        std::cout << "looking at " << token << std::endl;
        int token_len = token.size();
        for (int i = 0; i < token_len; i++)
        {
            if (isdigit(token[i]))
            {
                std::cout << "digit:" << token[i] << std::endl;
                std::ostringstream num;
                while (i < (token_len - 1) && isdigit(token[i + 1]))
                    num << token[i++];
                num << token[i];
                symbols.push_back(
                    Symbol(SymbolType::NUMBER, std::stoi(num.str())));
            }
            else if (token[i] == '<' || token[i] == '>')
            {
                char shifty = token[i];
                i++;
                if (token[i] != shifty)
                {
                    std::cout << "Mismatched angle brackets! BARF!"
                              << std::endl;
                    return std::vector<Symbol>();
                }
                std::cout << "SHIFT:" << token[i] << std::endl;
                if (shifty == '<')
                    symbols.push_back(
                        Symbol(SymbolType::OP, "<<", OperatorType::LEFT_SHIFT));
                else
                    symbols.push_back(Symbol(SymbolType::OP, ">>",
                                             OperatorType::RIGHT_SHIFT));
            }
            else if (token[i] == '(')
            {
                std::cout << "PARENS:" << token[i] << std::endl;
                symbols.push_back(
                    Symbol(SymbolType::LEFT_PARENS, "(", OperatorType::UNUSED));
            }
            else if (token[i] == ')')
            {
                std::cout << "PARENS:" << token[i] << std::endl;
                symbols.push_back(Symbol(SymbolType::RIGHT_PARENS, "(",
                                         OperatorType::UNUSED));
            }
            else if (is_valid_char(token[i]))
            {
                std::cout << "VALID CHAR:" << token[i] << std::endl;
                if (token[i] == 't')
                {
                    std::cout << "T:" << token[i] << std::endl;
                    symbols.push_back(Symbol(SymbolType::TEE_TOKEN, "t",
                                             OperatorType::UNUSED));
                }
                else
                {
                    std::cout << "OP!" << token[i] << std::endl;
                    OperatorType op_type = which_op(token[i]);
                    std::string ident{token[i]};
                    std::cout << "IDENTIFIER: " << ident << std::endl;
                    symbols.push_back(Symbol(SymbolType::OP, ident, op_type));
                }
            }
            else
            {
                std::cout << "Um, think your bitshift is biffed!" << std::endl;
                return std::vector<Symbol>();
            }
        }
    }
    return symbols;
}
