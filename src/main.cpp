#include "OptimizedSolver.hpp"
#include <iostream>
#include <vector>

int main() {
    EncryptoMon encryptoMon;
    OptimizedSolver solver(encryptoMon);
    Pokemon pokemon;

    const uint8_t size = 0x6;
    const std::vector<std::vector<uint8_t>> jumpCmds = {{0x16, 0x00}, {0x1a, 0x00}};

    std::vector<std::string> pokemonFiles = {
        R"(abra.pkm)",
        R"(chatot.pkm)",
        R"(haunter.pkm)",
        R"(magikarp.pkm)",
    };

    for (const auto &file : pokemonFiles) {
        if (encryptoMon.loadBinaryPokemon(file, pokemon)) {
            std::cout << "Loaded Pokemon data from " << file << std::endl;
            // encryptoMon.decryptPokemon(pokemon);
            // encryptoMon.dumpBinaryPokemon(R"(dec_abra2.pkm)", pokemon);
            encryptoMon.shuffleBlocks(pokemon);
            for (const auto &cmd : jumpCmds) {
                uint8_t data[2] = {cmd[0], cmd[1]};
                solver.solve(pokemon, data, 2, 0x86);
            }
        }
    }

    // const std::vector<std::pair<uint32_t, uint32_t>> ranges = {
    //     {0xFFFE74E8, 0xFFFE755C},
    //     {0xF6A7C, 0xF6AF4},
    //     {0xF8D5E, 0xF8DD6},
    //     {0xFFFE6996, 0xFFFE70AE},
    //     {0xF5F2A, 0xF6642}
    //     // {0x178AB2, 0x178BB2}
    // };

    // solver.setMaxEVs(200);
    // for (const auto& range : ranges) {
    //     for (uint32_t pos = range.first; pos < range.second; ++pos) {
    //         for (const auto& cmd : jumpCmds) {
    //             uint8_t data[size] = {
    //                 cmd[0], cmd[1],
    //                 static_cast<uint8_t>(pos & 0xFF),
    //                 static_cast<uint8_t>((pos >> 8) & 0xFF),
    //                 static_cast<uint8_t>((pos >> 16) & 0xFF),
    //                 static_cast<uint8_t>((pos >> 24) & 0xFF)
    //             };
    //             solver.solve(data, size, 0x18);
    //         }
    //     }
    // }
    // solver.printBestResult();

    // uint8_t data[size] = { 0x1a, 0x00 ,0x01, 0x74, 0xfe, 0xff};
    // solver.setMaxEVs(0xFFFF);
    // if (solver.solve(data, size, 0x18))
    //     std::cout << "Matching sequence found!" << std::endl;
    // solver.printBestResult();

    // std::vector<std::string> pokemonFiles = {
    //     R"(abra.pkm)",
    //     R"(chatot.pkm)",
    //     R"(haunter.pkm)",
    //     R"(magikarp.pkm)",
    // };

    // for (const auto& file : pokemonFiles) {
    //     if (encryptoMon.loadBinaryPokemon(file, pokemon)) {
    //         std::cout << "Loaded Pokemon data from " << file << std::endl;
    //         // encryptoMon.decryptPokemon(pokemon);
    //         //encryptoMon.dumpBinaryPokemon(R"(dec_abra2.pkm)", pokemon);
    //         encryptoMon.shuffleBlocks(pokemon);
    //         encryptoMonSolver.solveForControllables(pokemon, positions);

    //     }
    // }

    return 1;
}