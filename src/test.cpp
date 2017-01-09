/**
 * @author Romain Giot <giot.romain@gmail.com>
 * @licence GPL
 * @date 01/2017
 */


#include <cassert>
#include "z802nops.h"
#include <string>
#include <iostream>

void T(const std::string & opcode, z80tonops::Timing nops) {
    const z80tonops::Timing duration = z80tonops::duration(opcode);
    std::cerr << "Opcode: " << opcode << " Duration: " << static_cast<std::string>(duration) << " Expected: " << static_cast<std::string>(nops) << std::endl;
    assert(duration == nops);
    assert(duration.hasSimpleTiming() == nops.hasSimpleTiming());
}


int main(int argc, char** argv) {
    T(" LD A, 0", 2);
    T(" LD A, 10 + (5+2)", 2);
    T(" LD A, (0)", 4);
    T(" LD (10), A", 4);
    T(" LD HL, (50)", 5);
    T(" LD BC, (50)", 6);
    T(" LD SP, (50)", 6);
    T(" LD BC, 0xbc00 + 12", 3);
    T(" LD HL, XXX", 3);
    T(" OUT (C), C", 4);
    T(" INC B", 1);
    T(" OUT (C), H", 4);
    T(" INC C", 1);
    T(" DEC C", 1);
    T(" OUT (C), C", 4);
    T(" OUT (C), L", 4);
    T(" JR XXX", 3);
    T(" JR C, XXX", {3,2});
}

