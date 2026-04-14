#include "OptimizedSolver.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int main() {
    EncryptoMon encryptoMon;
    OptimizedSolver solver(encryptoMon);
    Pokemon pokemon;

    // jump start EVs: 228B3F6: 0xF94A2
    // calculator input 1: 2384898
    // calculator input 2: 23848C4: 0xF94CE
    // calculator output: 0xF94FA
    // dot artist live: 0xF91A2 - 0xF91A2 + 0x78

    const uint32_t boxDataStartOffset = 0xC318;
    const uint32_t pokeStartOffset = 0x19B50;
    // const uint32_t boxDataId = (pokeStartOffset - boxDataStartOffset) / 0x88;
    const uint32_t pokemonDataOffset = 0x18; // offset inside pokemon data block

    const uint8_t cmdSize = 0x6;
    const int dotArtistEntries = 0x78;
    uint32_t dotArtistOffsets[] = {0x138A, 0x138A + 0x10F594};
    const uint16_t jumps[] = {0x16, 0x1A};
    const int entries = dotArtistEntries * ARRAY_SIZE(dotArtistOffsets) * ARRAY_SIZE(jumps);
    uint8_t data[entries][cmdSize] = {};

    int idx = 0;
    for (int i = 0; i < ARRAY_SIZE(dotArtistOffsets); ++i) {
        auto dotArtistOffset = dotArtistOffsets[i];
        for (uint32_t offset = 0; offset < dotArtistEntries; offset++) {
            const uint32_t totalOffset = dotArtistOffset + offset - (pokeStartOffset + pokemonDataOffset + cmdSize);
            for (int j = 0; j < ARRAY_SIZE(jumps); ++j) {
                const uint8_t jump = jumps[j];
                const uint8_t dataEntry[cmdSize] = {static_cast<uint8_t>(jump & 0xFF),
                                                    static_cast<uint8_t>((jump >> 8) & 0xFF),
                                                    static_cast<uint8_t>(totalOffset & 0xFF),
                                                    static_cast<uint8_t>((totalOffset >> 8) & 0xFF),
                                                    static_cast<uint8_t>((totalOffset >> 16) & 0xFF),
                                                    static_cast<uint8_t>((totalOffset >> 24) & 0xFF)};
                std::memcpy(data[idx], dataEntry, cmdSize);
                ++idx;
            }
        }
    }

    {
        std::ofstream out("dot_artist_data.bin", std::ios::binary);
        out.write(reinterpret_cast<const char *>(data), sizeof(data));
        out.close();
        std::cout << "Wrote " << sizeof(data) << " bytes to dot_artist_data.bin" << std::endl;
    }

    std::vector<std::string> pokemonFiles = {
        R"(abra.pkm)",
        // R"(chatot.pkm)",
        // R"(haunter.pkm)",
        // R"(magikarp.pkm)",
    };

    for (const auto &file : pokemonFiles) {
        if (encryptoMon.loadBinaryPokemon(file, pokemon)) {
            std::cout << "Loaded Pokemon data from " << file << std::endl;
            encryptoMon.shuffleBlocks(pokemon);
            for (int i = 0; i < entries; ++i)
                solver.solve(pokemon, data[i], cmdSize, pokemonDataOffset);
        }
    }

    return 1;
}