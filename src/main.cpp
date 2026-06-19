#include "SaveCorruptionSolver.hpp"

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
    EncryptoMon em;

    VersionConfig version = {Game::DP, Language::JP};

    SaveCorruptionSolver::Config cfg;
    cfg.version = version;
    cfg.nickname = {0x0001, 0x01B5, version.nicknameMaxLen()};
    cfg.otName = {0x0001, 0x01B5, version.otNameMaxLen()};

    BoxDataSave boxDataSave;
    if (!SaveCorruptionSolver::buildBoxData(em, 0, "data/jp_box_misc.bin", boxDataSave))
        return 1;

    {
        constexpr int FOOTER_SKIP = 6;
        std::ifstream ff("data/jp_box_footer.bin", std::ios::binary);
        if (!ff) {
            std::cerr << "Cannot open jp_box_footer.bin\n";
            return 1;
        }
        ff.seekg(FOOTER_SKIP);
        ff.read(reinterpret_cast<char *>(&boxDataSave.footer), sizeof(StorageBlockFooter));
        if (ff.gcount() != sizeof(StorageBlockFooter)) {
            std::cerr << "jp_box_footer.bin too short after skip\n";
            return 1;
        }
    }

    constexpr size_t CRC_LEN = offsetof(BoxDataSave, footer);
    uint16_t calcCRC = SaveCorruptionSolver::crc16CCITT(reinterpret_cast<const uint8_t *>(&boxDataSave), CRC_LEN);
    std::cout << std::hex << std::uppercase << std::setfill('0');
    std::cout << "Calculated CRC : 0x" << std::setw(4) << calcCRC << "\n";
    std::cout << "Footer CRC     : 0x" << std::setw(4) << boxDataSave.footer.checksum
              << (calcCRC == boxDataSave.footer.checksum ? "  MATCH" : "  MISMATCH") << "\n";
    std::cout << std::dec << std::setfill(' ') << "\n";

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

    solver.solveParallel(pokemonFiles, boxDataSave, boxDataSave.footer.checksum);

    return 0;
}
