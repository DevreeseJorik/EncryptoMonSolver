#ifndef ENCRYPTOMONSOLVER_H
#define ENCRYPTOMONSOLVER_H

#include "EncryptoMon.hpp"
#include <vector>
#include <map>

struct Position {
    uint8_t offset;
    std::vector<std::vector<uint8_t>> sequences;
};

struct Position4Byte {
    uint8_t offset;
    std::vector<uint32_t> sequences;
};

const int MediumSlowExperienceCurve[100] = {
    1,
    9,
    57,
    96,
    135,
    179,
    236,
    314,
    419,
    560,
    742,
    973,
    1261,
    1612,
    2035,
    2535,
    3120,
    3798,
    4575,
    5460,
    6458,
    7577,
    8825,
    10208,
    11735,
    13411,
    15244,
    17242,
    19411,
    21760,
    24294,
    27021,
    29949,
    33084,
    36435,
    40007,
    43808,
    47846,
    52127,
    56660,
    61450,
    66505,
    71833,
    77440,
    83335,
    89523,
    96012,
    102810,
    109923,
    117360,
    125126,
    133229,
    141677,
    150476,
    159635,
    169159,
    179056,
    189334,
    199999,
    211060,
    222522,
    234393,
    246681,
    259392,
    272535,
    286115,
    300140,
    314618,
    329555,
    344960,
    360838,
    377197,
    394045,
    411388,
    429235,
    447591,
    466464,
    485862,
    505791,
    526260,
    547274,
    568841,
    590969,
    613664,
    636935,
    660787,
    685228,
    710266,
    735907,
    762160,
    789030,
    816525,
    844653,
    873420,
    902835,
    932903,
    963632,
    995030,
    1027103,
    1059860
};

struct ChecksumContribution {
    date metDate;
    uint8_t metLevel;
    uint32_t experiencePoints;
};

class EncryptoMonSolver {
public:
    EncryptoMonSolver(EncryptoMon& encryptoMon);

    bool checkSequences(Pokemon& pokemon, const std::vector<Position>& positions);
    bool checkSequences(Pokemon& pokemon, const std::vector<Position4Byte>& positions);
    bool solveForSequences(Pokemon& pokemon, const std::vector<Position4Byte>& positions);
    bool solveForControllables(Pokemon& pokemon, const std::vector<Position4Byte>& positions);

    std::vector<Position4Byte> reduceSequences(Pokemon& pokemon, const std::vector<Position4Byte>& positions, uint32_t checksum);
    std::map<uint16_t, std::vector<Position4Byte>> reduceSequences(Pokemon& pokemon, const std::vector<Position4Byte>& positions);
private:
    bool checkSequence(Pokemon& pokemon, const uint8_t* seq, uint8_t seq_len, uint8_t offs) const;
    bool checkSequence(Pokemon& pokemon, uint32_t seq, uint8_t offs) const;

    int levelToExperience(int level, ExperienceGroup group) const;
    void generateExperienceTables();
    int getExperienceForLevel(int level, ExperienceGroup group) const { return m_experienceCurves[group][level - 1]; }

    std::map<uint16_t, std::vector<ChecksumContribution>> precomputeChecksumContributions(Pokemon &pokemon);

    EncryptoMon& m_encryptoMon;
    int m_experienceCurves[NumberOfExperienceGroups][MAX_LEVELS];
    std::map<uint16_t, std::vector<ChecksumContribution>> m_checksumMap;
};

#endif // ENCRYPTOMONSOLVER_H