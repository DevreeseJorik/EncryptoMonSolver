#include "EncryptoMonSolver.hpp"
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <set>
#include <cmath>

static inline int16_t calculateChecksumDifference(uint16_t currentChecksum, uint16_t desiredChecksum) {
    return static_cast<int16_t>(desiredChecksum) - static_cast<int16_t>(currentChecksum);
}

EncryptoMonSolver::EncryptoMonSolver(EncryptoMon& encryptoMon):
        m_encryptoMon(encryptoMon)
{
    m_encryptoMon.generateXORMasks();
    generateExperienceTables();
}

int EncryptoMonSolver::levelToExperience(int level, ExperienceGroup group) const {
    switch (group) {
        case MediumFast:
            return std::pow(level, 3);
        case Erratic:
            if (level < 50)
                return (std::pow(level, 3) * (100 - level)) / 50;
            else if (level < 68)
                return (std::pow(level, 3) * (150 - level)) / 100;
            else if (level < 98)
                return (std::pow(level, 3) * ((1911 - (10 * level)) / 3)) / 500;
            else 
                return (std::pow(level, 3) * (160 - level)) / 100;
        case Fluctuating:
            if (level < 15)
                return (std::pow(level, 3) * (((level + 1) / 3) + 24)) / 50;
            if (level < 36)
                return (std::pow(level, 3) * (level + 14)) / 50;
            else
                return (std::pow(level, 3) * ((level / 2) + 32)) / 50;
        case MediumSlow:
            // return (6/5 * std::pow(level, 3)) - (15 * std::pow(level, 2)) + (100 * level) - 140; // Gen 2
            return MediumSlowExperienceCurve[level - 1];
        case Fast:
            return 4 * std::pow(level, 3) / 5;
        case Slow:
            return 5 * std::pow(level, 3) / 4;
        default:
            return 0;
    }
}

void EncryptoMonSolver::generateExperienceTables() {
    for (int group = 0; group < NumberOfExperienceGroups; ++group) {
        for (int level = 1; level <= 100; ++level) {
            int experience = levelToExperience(level, static_cast<ExperienceGroup>(group));
            m_experienceCurves[group][level - 1] = experience;
        }
    }
}

bool EncryptoMonSolver::checkSequence(Pokemon& pokemon, const uint8_t* seq, uint8_t seq_len, uint8_t offs) const {
    return std::equal(seq, seq + seq_len, reinterpret_cast<uint8_t*>(&pokemon) + offs);
}

bool EncryptoMonSolver::checkSequence(Pokemon& pokemon, uint32_t seq, uint8_t offs) const {
    return *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(&pokemon) + offs) == seq;
}

bool EncryptoMonSolver::checkSequences(Pokemon& pokemon, const std::vector<Position>& positions) {
    std::vector<uint16_t> checksums;
    for (uint32_t checksum = 0; checksum <= 0xFFFF; checksum++) {
        pokemon.checksum = checksum;
        m_encryptoMon.encryptPokemon(pokemon);
        for (const auto& position : positions) {
            if (position.offset + position.sequences[0].size() > sizeof(Pokemon)) {
                std::cerr << "Invalid offset" << std::endl;
                continue;
            }
            
            for (const auto& sequence : position.sequences) {
                if (checkSequence(pokemon, sequence.data(), sequence.size(), position.offset)) {
                    std::cout << "Sequence at offset " << static_cast<unsigned int>(position.offset) << " with checksum " << checksum << std::endl;
                    std::cout << std::hex;
                    for (int i = 0; i < sizeof(Pokemon); ++i) {
                        if (i % 16 == 0) {
                            std::cout << std::endl;
                            std::cout << std::hex << std::setfill('0') << std::setw(2) << i << " ";
                        }
                        if (i % 16 == 8) {
                            std::cout << " ";
                        }
                        if (i >= position.offset && i < position.offset + sequence.size()) {
                            std::cout << "\033[1;31m";
                        } else {
                            std::cout << "\033[0m";
                        }
                        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(reinterpret_cast<uint8_t*>(&pokemon)[i]) << " ";
                    }
                    std::cout << "\033[0m";
                    std::cout << std::dec << std::endl << std::endl;
                    checksums.push_back(checksum);
                }
            }
        }

        m_encryptoMon.decryptPokemon(pokemon);
    }
    return checksums.size() > 0;
}

std::vector<Position4Byte> EncryptoMonSolver::reduceSequences(Pokemon& pokemon, const std::vector<Position4Byte>& positions, uint32_t checksum) {
    std::vector<Position4Byte> results;
    for (const auto& position : positions) {
        m_encryptoMon.setChecksum(pokemon, checksum);
        m_encryptoMon.encryptSection(pokemon, position.offset - 8);
        Position4Byte resultPosition = position;
        resultPosition.sequences.clear();
        for (const auto& sequence : position.sequences) {
            if (checkSequence(pokemon, sequence, position.offset)) {
                resultPosition.sequences.push_back(sequence);
            }
        }
        m_encryptoMon.decryptSection(pokemon, position.offset - 8);
        if (!resultPosition.sequences.empty()) {
            results.push_back(resultPosition);
        }
    }
    return results;
}

std::map<uint16_t, std::vector<Position4Byte>> EncryptoMonSolver::reduceSequences(Pokemon& pokemon, const std::vector<Position4Byte>& positions) {
    std::map<uint16_t, std::vector<Position4Byte>> results;
    for (uint32_t checksum = 0; checksum <= 0xFFFF; checksum++) {
        auto result = reduceSequences(pokemon, positions, checksum);
        if (!result.empty())
            results[checksum] = result;
    }

    return results;
}

std::map<uint16_t, std::vector<ChecksumContribution>> EncryptoMonSolver::precomputeChecksumContributions(Pokemon &pokemon) {
    std::map<uint16_t, std::vector<ChecksumContribution>> checksumMap;


    uint8_t gender = m_encryptoMon.getGender(pokemon);
    ExperienceGroup experienceGroup = getExperienceGroup(static_cast<PokemonName>(m_encryptoMon.getSpeciesID(pokemon)));
    for (int year = 0; year <= 50; ++year) {
        for (int month = 1; month <= 12; ++month) {
            for (int day = 1; day <= 31; ++day) {
                uint16_t dateChecksum = ((year << 8) | month) - ((0 << 8) | 1); // note this bitshifting would have to be reversed for dateeggreceived due to allignment
                dateChecksum += ((day << 8) | 0) - ((1 << 8) | 0);
                for (uint8_t metLevel = 7; metLevel <= 25; metLevel++) {
                    uint8_t metLevelChecksum = ((metLevel | gender << 7) << 0) - ((1 | gender << 7) << 0);
                    //for (int level = metLevel; level <= 100; ++level) {
                        uint8_t level = metLevel;
                        uint32_t experiencePoints = levelToExperience(level, experienceGroup);
                        uint16_t experienceChecksum = (experiencePoints >> 16) + (experiencePoints & 0xFFFF) - 1;

                        uint16_t totalChecksum = dateChecksum + experienceChecksum + metLevelChecksum;

                        date metDate = {static_cast<uint8_t>(year), static_cast<uint8_t>(month), static_cast<uint8_t>(day)};
                        ChecksumContribution contribution = {metDate, metLevel, experiencePoints};
                        checksumMap[totalChecksum].push_back(contribution);
                    //}
                }
            }
        }
    }

    return checksumMap;
}

bool EncryptoMonSolver::solveForSequences(Pokemon& pokemon, const std::vector<Position4Byte>& positions) {
    m_encryptoMon.setMetAtDate(pokemon, 1, 1, 2000 - 2000);
    m_encryptoMon.setExperiencePoints(pokemon, 1);
    m_encryptoMon.setMetAtLevel(pokemon, 1);

    std::map<uint16_t, std::vector<Position4Byte>> possibleChecksums = reduceSequences(pokemon, positions);

    bool result = false;
    for (const auto& [checksum, positions] : possibleChecksums) {
        uint16_t currentChecksum = m_encryptoMon.calculateChecksum(pokemon);
        int16_t checksumDifference = calculateChecksumDifference(currentChecksum, checksum);

        if (m_checksumMap.find(checksumDifference) != m_checksumMap.end()) {
            for (const auto& contribution : m_checksumMap[checksumDifference]) {
                m_encryptoMon.setChecksum(pokemon, checksum);
                m_encryptoMon.setMetAtDate(pokemon, contribution.metDate);
                m_encryptoMon.setMetAtLevel(pokemon, contribution.metLevel);
                m_encryptoMon.setExperiencePoints(pokemon, contribution.experiencePoints);
            
                std::cout << "Found a match with the following data:\n"
                          << "\tChecksum: " << std::hex << checksum << std::dec << "\n"
                          << "\tName: " << toString(static_cast<PokemonName>(m_encryptoMon.getSpeciesID(pokemon))) << "\n"
                          << "\tLevel: " << m_encryptoMon.getExperiencePoints(pokemon) << "\n"
                          << "\tDate: " << static_cast<int>(contribution.metDate.day) << "/"
                          << static_cast<int>(contribution.metDate.month) << "/"
                          << static_cast<int>(contribution.metDate.year) << "\n"
                          << "\tMet Level: " << static_cast<int>(m_encryptoMon.getMetAtLevel(pokemon)) << "\n"
                          << "\tHeld Item: " << toString(static_cast<Items>(m_encryptoMon.getHeldItem(pokemon))) << "\n"
                          << std::endl;
            
                m_encryptoMon.encryptPokemon(pokemon);
                std::cout << std::hex;
                for (int i = 0; i < sizeof(Pokemon); ++i) {
                    if (i % 16 == 0) {
                        std::cout << std::endl;
                        // std::cout << std::hex << std::setfill('0') << std::setw(2) << i << " ";
                    }
                    if (i % 16 == 8) {
                        std::cout << " ";
                    }
                    std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(reinterpret_cast<uint8_t*>(&pokemon)[i]) << " ";
                }
                std::cout << std::endl;
            
                m_encryptoMon.decryptPokemon(pokemon);
                result = true;
            }
        } else {
            m_encryptoMon.setChecksum(pokemon, checksum);

            std::cout << "match for checksum " << std::hex << checksum << std::dec << " but could not auto-manipulate one\n"
                      << "\tName: " << toString(static_cast<PokemonName>(m_encryptoMon.getSpeciesID(pokemon))) << "\n"
                      << "\tHeld Item: " << toString(static_cast<Items>(m_encryptoMon.getHeldItem(pokemon))) << "\n"
                      << std::endl;
        
            std::cout << "Guess at what data would look like:" << std::endl;
            m_encryptoMon.encryptPokemon(pokemon);
            std::cout << std::hex;
            for (int i = 0; i < sizeof(Pokemon); ++i) {
                if (i % 16 == 0) {
                    std::cout << std::endl;
                    // std::cout << std::hex << std::setfill('0') << std::setw(2) << i << " ";
                }
                if (i % 16 == 8) {
                    std::cout << " ";
                }
                std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(reinterpret_cast<uint8_t*>(&pokemon)[i]) << " ";
            }
            std::cout << std::endl;
        
            m_encryptoMon.decryptPokemon(pokemon);
        }
    }

    return result;
}

bool EncryptoMonSolver::solveForControllables(Pokemon& pokemon, const std::vector<Position4Byte>& positions) {
    // disable except for chatot, debug testing

    // m_encryptoMon.setMove(pokemon, static_cast<uint16_t>(MoveName::Fly), 0);
    // m_encryptoMon.setMovePP(pokemon, 15, 0);
        
    // m_encryptoMon.setMove(pokemon, static_cast<uint16_t>(MoveName::Peck), 1);
    // m_encryptoMon.setMovePP(pokemon, 35, 1);

    // m_encryptoMon.setMove(pokemon, static_cast<uint16_t>(MoveName::Growl), 2);
    // m_encryptoMon.setMovePP(pokemon, 40, 2);

    // m_encryptoMon.setMove(pokemon, static_cast<uint16_t>(MoveName::MirrorMove), 1);
    // m_encryptoMon.setMovePP(pokemon, 20, 1);

    // m_encryptoMon.setMove(pokemon, static_cast<uint16_t>(MoveName::Fly), 0);
    // m_encryptoMon.setMovePP(pokemon, 15, 0);

    m_checksumMap = precomputeChecksumContributions(pokemon);

    bool result = false;
    m_encryptoMon.setFriendship(pokemon, 255);

    // for (int item = 0; item < TM92; ++item) { 
    //     m_encryptoMon.setHeldItem(pokemon, item);
    //     if (solveForSequences(pokemon, positions))
    //         result = true;
    // }

    m_encryptoMon.setHeldItem(pokemon, TM83);
    if (solveForSequences(pokemon, positions))
        result = true;

    return result;
}
