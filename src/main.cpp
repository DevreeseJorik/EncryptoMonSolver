#include "OptimizedSolver.hpp"
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int main() {
    EncryptoMon encryptoMon;
    OptimizedSolver solver(encryptoMon);
    ExtendedPokemon extendedPokemon;

    // generate random numbers to test with
    std::vector<uint16_t> jumps = {};
    for (uint16_t i = 0; i < 0xFFFF; ++i) {
        jumps.push_back(i);
    }
    uint8_t offset = 0;
    for (int i = 0; i < 3; ++i) {
        offset = offsetof(ExtendedPokemon, battleData) + offsetof(BattleData, mailData) + offsetof(Mail, msg) +
                 i * sizeof(MailMessage) + offsetof(MailMessage, inputField);

        std::cout << "Testing battle data jumps for offset 0x" << std::hex << (int)offset << std::dec << std::endl;

        for (auto &file : {// R"(abra.pkm)", R"(chatot.pkm)", R"(haunter.pkm)", R"(magikarp.pkm)",
                           R"(chatot_extended.bin)"}) {
            if (encryptoMon.loadBinaryExtendedPokemon(file, extendedPokemon)) {
                std::cout << "Loaded Extended Pokemon data from " << file << std::endl;

                for (auto &jump : jumps) {
                    // std::cout << "Testing jump: 0x" << std::hex << jump << std::dec << std::endl;
                    if (solver.solveBattleData(extendedPokemon, reinterpret_cast<const uint8_t *>(&jump), sizeof(jump),
                                               offset)) {
                        std::cout << "Successfully solved battle data for jump: 0x" << std::hex << jump << std::dec
                                  << std::endl;

                        // For demonstration, let's encrypt the battle data again and print it out
                        encryptoMon.encryptBattleData(extendedPokemon);
                        std::cout << "Encrypted battle data at offset 0x" << std::hex << (int)offset << ": ";
                        for (size_t i = 0; i < sizeof(BattleData); ++i) {
                            if (i % 16 == 0)
                                std::cout << std::endl;
                            else if (i % 16 == 8)
                                std::cout << " ";
                            std::cout << std::hex << std::setfill('0') << std::setw(2)
                                      << static_cast<unsigned int>(
                                             reinterpret_cast<uint8_t *>(&extendedPokemon.battleData)[i])
                                      << " ";
                        }
                        std::cout << std::endl;
                        // }
                    }
                }
            }
        }
    }
}