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
            encryptoMon.shuffleBlocks(pokemon);
            for (const auto &cmd : jumpCmds) {
                uint8_t data[2] = {cmd[0], cmd[1]};
                solver.solve(pokemon, data, 2, 0x86);
            }
        }
    }

    return 1;
}