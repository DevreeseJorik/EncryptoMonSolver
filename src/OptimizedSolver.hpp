#ifndef OPTIMIZEDSOLVER_H
#define OPTIMIZEDSOLVER_H

#include "EncryptoMon.hpp"
#include "moves.hpp"
#include <map>
#include <set>
#include <vector>

struct Position {
    uint8_t offset;
    std::vector<std::vector<uint8_t>> sequences;
};

struct Position4Byte {
    uint8_t offset;
    std::vector<uint32_t> sequences;
};

const int MediumSlowExperienceCurve[100] = {
    1,      9,      57,     96,     135,    179,    236,    314,     419,    560,    742,    973,    1261,
    1612,   2035,   2535,   3120,   3798,   4575,   5460,   6458,    7577,   8825,   10208,  11735,  13411,
    15244,  17242,  19411,  21760,  24294,  27021,  29949,  33084,   36435,  40007,  43808,  47846,  52127,
    56660,  61450,  66505,  71833,  77440,  83335,  89523,  96012,   102810, 109923, 117360, 125126, 133229,
    141677, 150476, 159635, 169159, 179056, 189334, 199999, 211060,  222522, 234393, 246681, 259392, 272535,
    286115, 300140, 314618, 329555, 344960, 360838, 377197, 394045,  411388, 429235, 447591, 466464, 485862,
    505791, 526260, 547274, 568841, 590969, 613664, 636935, 660787,  685228, 710266, 735907, 762160, 789030,
    816525, 844653, 873420, 902835, 932903, 963632, 995030, 1027103, 1059860};

struct ChecksumContribution {
    date metDate;
    uint8_t metLevel;
    uint8_t currentLevel;
    uint32_t experiencePoints;
    Items heldItem;
};

struct BestResult {
    uint16_t checksum;
    Pokemon pokemon;
    uint8_t evs[6];
    uint16_t evCount;
    uint8_t perfectEvs;
    uint16_t diffFromPerfect;
};

class OptimizedSolver {
  public:
    OptimizedSolver(EncryptoMon &encryptoMon);

    bool solve(Pokemon &pokemon, const uint8_t *data, size_t size, uint8_t offset);

    void setValidPokemon();
    void setValidPokemon(std::set<PokemonName> validPokemon);
    void setValidPokemon(Pokemon &pokemon);

    void setValidItems();
    void setValidItems(std::set<Items> validItems);

    void setValidMoves();
    void setValidMoves(std::set<MoveName> validMoves);
    void setValidMoves(Pokemon &pokemon);

    int setMaxEVs(int maxEVs) { return m_maxEVs = maxEVs; }
    int getMaxEVs() const { return m_maxEVs; }

    void setLogging(bool logging) { m_logging = logging; }
    bool getLogging() const { return m_logging; }

    void printBestResult();

  private:
    int levelToExperience(int level, ExperienceGroup group) const;
    void generateExperienceTables();
    int getExperienceForLevel(int level, ExperienceGroup group) const { return m_experienceCurves[group][level - 1]; }

    bool isBlockHeaderValid(Pokemon &pokemon, uint8_t &blockRelOffset, uint8_t remainingSize) const;
    bool isBlockAValid(Pokemon &pokemon, uint8_t &blockRelOffset, uint8_t remainingSize);
    bool isBlockBValid(Pokemon &pokemon, uint8_t &blockRelOffset, uint8_t remainingSize, uint8_t offset) const;
    bool isBlockCValid(Pokemon &pokemon, uint8_t &blockRelOffset, uint8_t remainingSize) const { return false; }
    bool isBlockDValid(Pokemon &pokemon, uint8_t &blockRelOffset, uint8_t remainingSize) const;

    bool isDataValid(Pokemon &pokemon, uint8_t offset, uint8_t size);

    BlockType getBlockType(Pokemon &pokemon, uint8_t offset) const;
    uint8_t getBlockRelativeOffset(Pokemon &pokemon, uint8_t offset) const;
    uint8_t getBlockOffset(Pokemon &pokemon, BlockType blockType) const;

    std::vector<uint16_t> solveSequence(Pokemon &pokemon, const uint8_t *data, size_t size, uint8_t offset);
    std::map<uint16_t, std::vector<ChecksumContribution>> computeChecksumContributions(Pokemon &pokemon);

    EncryptoMon &m_encryptoMon;
    int m_experienceCurves[NumberOfExperienceGroups][MAX_LEVELS];

    std::set<PokemonName> m_validPokemon;
    std::set<Items> m_validItems;
    std::set<MoveName> m_validMoves;

    bool m_logging = false;
    int m_maxEVs = 510;
    BestResult m_bestResult;
};

#endif // OPTIMIZEDSOLVER_H