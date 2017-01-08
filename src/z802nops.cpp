/*
 * @author Romain Giot <giot.romain@gmail.com>
 * @licence GPL
 * @date 01/2017
 */



/**
 * Count the number of nops a SIMPLE z80 code can do
 * Limitations :
 * - do not take into account insttructions having 2 durations
 * - do not take into account instructions whose durations depends on BC
 * - do not take into account branching possibilities and sub-functions
 *
 * Timing extracted from :
 * - http://quasar.cpcscene.net/doku.php?id=iassem:timings
 */


#include <string>
#include <regex>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cassert>

#include "z802nops.h"
#include "utils.h"

using namespace std;
using namespace std::regex_constants;


namespace z80tonops {



const std::string encode_possibilities(const std::vector<std::string> & possibilities);

const std::string REG8_A{"A"};
const std::string REG8_B{"B"};
const std::string REG8_C{"C"};
const std::string REG8_D{"D"};
const std::string REG8_E{"E"};
const std::string REG8_H{"H"};
const std::string REG8_L{"L"};
const std::string REG8_R{"R"};
const std::string REG8_I{"I"};
const std::string REG8_IXH{"IXH"};
const std::string REG8_IXL{"IXL"};
const std::string REG8_IYH{"IYH"};
const std::string REG8_IYL{"IYL"};

const std::string REG16_AF{"AF"};
const std::string REG16_BC{"BC"};
const std::string REG16_DE{"DE"};
const std::string REG16_HL{"HL"};
const std::string REG16_IX{"IX"};
const std::string REG16_IY{"IY"};
const std::string REG16_SP{"SP"};

const std::string OP_JR{"JR"};
const std::string OP_JP{"JP"};

const std::string OP_ADD{"ADD"};
const std::string OP_DEC{"DEC"};
const std::string OP_INC{"INC"};
const std::string OP_LD{"LD"};
const std::string OP_OUT{"OUT"};

const std::string OP_AND{"AND"};
const std::string OP_OR{"OR"};
const std::string OP_XOR{"XOR"};


const std::string COND_Z{"Z"};
const std::string COND_NZ{"NZ"};
const std::string COND_C{"C"};
const std::string COND_NC{"NC"};
const std::string COND_P{"P"};
const std::string COND_PE{"PE"};
const std::string COND_PO{"PO"};

const std::string COND_all{encode_possibilities({
        COND_Z, COND_NZ,
        COND_C, COND_NC,
        COND_P, COND_PE, COND_PO
    })};

const std::string TEXT_PARENTHESIS_LEFT{"\\("};
const std::string TEXT_PARENTHESIS_RIGHT{"\\)"};
const std::string REGEX_PARENTHESIS_LEFT{"(?:"};
const std::string REGEX_PARENTHESIS_RIGHT{")"};



const std::string REGEX_OR{"|"};
const std::string REGEX_WHITESPACES_MANDATORY{"[[:space:]]+"};
const std::string REGEX_WHITESPACES_OPTIONNAL{"[[:space:]]*"};
const std::string REGEX_START{"^"};
const std::string REGEX_END{"$"};

const std::string PORT{"\\(C\\)"};

const std::string VALUE{encode_possibilities({
        "[^()]+",
        string("\\(.*[^)]"),
        string("[^(].*\\)")
    })}; // XXX This regex seems buggy
const std::string MEM {
    TEXT_PARENTHESIS_LEFT+ ".+" + TEXT_PARENTHESIS_RIGHT
};



const std::string encode_possibilities(const std::vector<std::string> & possibilities) {
    std::string res = REGEX_PARENTHESIS_LEFT + possibilities.front() + REGEX_PARENTHESIS_RIGHT;
    for(size_t pos=1; pos<possibilities.size(); ++ pos) {
        res +=  REGEX_OR + REGEX_PARENTHESIS_LEFT + possibilities[pos] + REGEX_PARENTHESIS_RIGHT;
    }
    return REGEX_PARENTHESIS_LEFT + res + REGEX_PARENTHESIS_RIGHT;
}


const std::string encode_mem_register(const std::string & reg) {
    return TEXT_PARENTHESIS_LEFT + REGEX_WHITESPACES_OPTIONNAL + reg + REGEX_WHITESPACES_OPTIONNAL + TEXT_PARENTHESIS_RIGHT;
}

const std::string REG8_common{encode_possibilities({
        REG8_A,
        REG8_B,
        REG8_C,
        REG8_D,
        REG8_E,
        REG8_H,
        REG8_L
    })};
const std::string REG16_common{encode_possibilities({
        REG16_AF,
        REG16_BC,
        REG16_DE,
        REG16_HL
    })};
const std::string MEM_REG16_common{encode_possibilities({
        encode_mem_register(REG16_AF),
        encode_mem_register(REG16_BC),
        encode_mem_register(REG16_DE),
        encode_mem_register(REG16_HL)
    })};

const std::string MEM_REG16_HL = encode_mem_register(REG16_HL);
const std::string MEM_REG16_IX = encode_mem_register(REG16_IX);
const std::string MEM_REG16_IY = encode_mem_register(REG16_IY);

constexpr auto regex_flags = icase | nosubs | ECMAScript ;

auto R(const std::string & opcode) {
    const std::string line = REGEX_START + REGEX_WHITESPACES_OPTIONNAL + opcode + REGEX_END;
#ifndef NDEBUG
    cerr << line << endl;
#endif
    return std::make_pair(line, std::regex(line, regex_flags));
}

auto R(const std::string & opcode, const std::string & arg1) {
    const std::string line = REGEX_START + REGEX_WHITESPACES_OPTIONNAL + opcode + REGEX_WHITESPACES_MANDATORY +  arg1 + REGEX_WHITESPACES_OPTIONNAL + REGEX_END;
#ifndef NDEBUG
    cerr << line << endl;
#endif
    return std::make_pair(line, std::regex(line, regex_flags));
}

auto R(const std::string & opcode, const std::string & arg1, const std::string & arg2) {
    const std::string line = REGEX_START + REGEX_WHITESPACES_OPTIONNAL + opcode + REGEX_WHITESPACES_MANDATORY + arg1  + REGEX_WHITESPACES_OPTIONNAL + string(",") + REGEX_WHITESPACES_OPTIONNAL + arg2 + REGEX_WHITESPACES_OPTIONNAL + REGEX_END;
#ifndef NDEBUG
    cerr << line << endl;
#endif
    return std::make_pair(line, std::regex(line, regex_flags));
}


// XXX Attention order is VERY important / an opcode can match several regexes...
const std::vector< std::pair< std::pair<std::string, std::regex> , Timing> > lut{
    // Exchanges
    {R("EXX"), 1},
    {R("EX", REG16_HL, REG16_DE), 1},
    {R("EX", REG16_AF, REG16_AF + REGEX_WHITESPACES_OPTIONNAL + std::string("'")), 1},

    // LD 16 bits

    {R(OP_LD, REG8_A, MEM_REG16_common), 2},
    {R(OP_LD, REG16_HL, MEM), 5},
    {R(OP_LD, REG16_SP, MEM), 6},
    {R(OP_LD, REG16_common, MEM), 6}, // XXX HL must be treated before
    {R(OP_LD, REG16_common, VALUE), 3},

    {R(OP_LD, MEM_REG16_HL, VALUE), 3},

    {R(OP_LD, MEM, REG16_HL), 5},
    {R(OP_LD, MEM, REG16_SP), 6},
    {R(OP_LD, MEM, REG16_common), 6}, // XXX HL must be treated before


    // LD 8 bits

    {R(OP_LD, REG8_common, REG8_common), 1},
    {R(OP_LD, REG8_A, MEM), 4},
    {R(OP_LD, MEM, REG8_A), 4},
    {R(OP_LD, REG8_common, VALUE), 2},

    // INC/DEC
    {R(OP_INC,REG16_common), 2},
    {R(OP_INC,REG8_common), 1},
    {R(OP_DEC,REG16_common), 2},
    {R(OP_DEC,REG8_common), 1},

    // ADD
    {R(OP_ADD, MEM_REG16_HL), 2},

    // Logicial operations
    {R(encode_possibilities({OP_AND, OP_OR, OP_XOR}), REG8_common), 1},
    {R(encode_possibilities({"RRA", "RRCA", "RLA", "RLCA"})), 1},

    // Port operations
    {R(OP_OUT, PORT, REG8_common), 4},

    // Jump
    {R(OP_JP, VALUE), 3},
    {R(OP_JP, COND_all, VALUE), 3},
    {R(OP_JP, MEM_REG16_HL), 1},
    {R(OP_JP, MEM_REG16_IX), 2},
    {R(OP_JP, MEM_REG16_IY), 2},

    {R(OP_JR, VALUE), 3},

};




Timing duration(const std::string & instruction) {
    const size_t N = lut.size();
    std::smatch match;
    for(size_t i=0; i<N; ++i) {
        if (std::regex_search(instruction, match, lut[i].first.second)) {
#ifndef NDEBUG
            std::cerr << "Selected regex: " << lut[i].first.first << std::endl;
            std::cerr << "Match size: " << match.size() << std::endl;
            std::cerr << "Match prefix: " << match.prefix() << std::endl;
            for (size_t i = 0; i < match.size(); ++i)
                std::cout << i << ": " << match[i] << '\n';
            std::cout << "Suffix: '" << match.suffix() << "\'\n\n";
#endif
            return lut[i].second;
        }
    }

    std::cerr << "[ERROR] Timing not found for *" << instruction << "*" << std::endl;
    return 0;
}




const std::string extract_instruction_from_line(const std::string & line) {
    std::string opcode;

    auto idx = line.find(";");
    if (idx != std::string::npos) {
        opcode = line.substr(0, idx);
    }
    else {
        opcode = line;
    }

    // Remove the labl
    while(!::isspace(opcode.front()) && opcode.size()>1) {
        opcode.erase(opcode.begin());
    }

    // Remove the uneeded space
    opcode = trim(opcode);

    if (opcode.size() > 0) {
        // upper case it
        std::transform(opcode.begin(), opcode.end(), opcode.begin(), ::toupper);

    }
    return opcode;
}

void treat_stream(istream & stream, ostream & cout) {
    std::string line;
    size_t total_nops = 0;

    cout << "; START COUNTING" << endl;

    // XXX Amazing in 2017 that string manipulation is so shitty ...
    while (std::getline(stream, line)) {
        // Remove the comment
        auto idx = line.find(";");
        const std::string opcode = extract_instruction_from_line(line);
        if (opcode.size() > 0) {

            // Get the amount of nops
            const size_t current_nops = duration(opcode);

            total_nops += current_nops;
            cout << line << "  ; " << current_nops << " nops" << endl;
        }
        else {
            cout << line << endl;
        }
    }
    cout << "; STOP COUNTING" << endl;
    cout << "; Total number of nops = " << total_nops << endl;


}



}
