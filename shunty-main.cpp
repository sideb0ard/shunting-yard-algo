#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include "shunty.hpp"

int main(int argc, char **argv)
{
    std::cout << "MEERP!\n";

    std::string line;
    while (!std::getline(std::cin, line).fail())
    {
        // std::vector<Symbol> symbs = extract_symbols_from_line(line);
        // std::cout << "Len of symbs:" << symbs.size() << std::endl;
        // for (auto s : symbs)
        //    std::cout << s << " ";
        // std::cout << std::endl;
        std::stack<Symbol> output_queue =
            convert_to_infix(extract_symbols_from_line(line));
        show_stack(output_queue);
    }
}
