/**
 * @author Romain Giot <giot.romain@gmail.com>
 * @licence GPL
 * @date 01/2017
 */

#include "z802nops.h"
#include <fstream>
#include <iostream>

void test() {
    std::cout << z80tonops::duration("LD B,E") << std::endl;
}



int main(int argc, char **argv, char** argenv) {

    if (argc == 2) {
        auto istream = std::ifstream(static_cast<const char *>(argv[1]), std::ifstream::in);
        if (istream.good()) {
            z80tonops::treat_stream(istream, std::cout);
        }
        else {
            std::cerr << argv[1] << " may not be a readable file" << std::endl ;
        }
    }
    else {
        z80tonops::treat_stream(std::cin, std::cout);
    }

    return 0;
}
