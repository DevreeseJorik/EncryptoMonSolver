#include "SaveCorruptionSolver.hpp"

#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
    EncryptoMon em;

    SaveCorruptionSolver::Config cfg;
    cfg.nickname = {0x0001, 0x01B5};
    cfg.otName = {0x0001, 0x01B5};

    SaveCorruptionSolver solver(em, cfg);

    auto collisions = solver.findValidCollisions();
    std::cout << "Valid collision entries in BoxData: " << collisions.size() << "\n";
    for (const auto &c : collisions) {
        uint32_t box = c.pokemonIndex / 30;
        uint32_t slot = c.pokemonIndex % 30;
        std::cout << "  [" << std::setw(3) << c.pokemonIndex << "] "
                  << "box " << box << " slot " << slot << "  overwrite " << static_cast<int>(c.overwriteLen)
                  << " bytes\n";
    }
    std::cout << "\n";

    // Files are expected to be 136-byte box-format (encrypted + shuffled) pokemon.
    std::vector<std::string> pokemonFiles;
    const std::filesystem::path startersDir = "starters";
    if (!std::filesystem::is_directory(startersDir)) {
        std::cerr << "starters/ directory not found (run from project root)\n";
        return 1;
    }
    for (const auto &entry : std::filesystem::directory_iterator(startersDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin")
            pokemonFiles.push_back(entry.path().string());
    }
    std::sort(pokemonFiles.begin(), pokemonFiles.end());

    std::cout << "Loaded " << pokemonFiles.size() << " starter file(s):\n";
    for (const auto &f : pokemonFiles)
        std::cout << "  " << f << "\n";
    std::cout << "\n";

    solver.solve(pokemonFiles, SaveCorruptionSolver::makeBlockAPos2Enumerator());

    return 0;
}
