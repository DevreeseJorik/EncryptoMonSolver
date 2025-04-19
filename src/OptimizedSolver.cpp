#include "OptimizedSolver.hpp"
#include "learnSet.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <set>
#include <vector>

static inline int16_t calculateChecksumDifference(uint16_t currentChecksum, uint16_t desiredChecksum) {
    return static_cast<int16_t>(desiredChecksum) - static_cast<int16_t>(currentChecksum);
}

OptimizedSolver::OptimizedSolver(EncryptoMon &encryptoMon) : m_encryptoMon(encryptoMon) {
    m_encryptoMon.generateXORMasks();
    generateExperienceTables();
    setValidPokemon();
    setValidItems();
    setValidMoves();
    m_bestResult = {0, Pokemon(), {0}, 0xFFFF, 0, 0xFFFF};
}

int OptimizedSolver::levelToExperience(int level, ExperienceGroup group) const {
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
        return MediumSlowExperienceCurve[level - 1];
    case Fast:
        return 4 * std::pow(level, 3) / 5;
    case Slow:
        return 5 * std::pow(level, 3) / 4;
    default:
        return 0;
    }
}

void OptimizedSolver::generateExperienceTables() {
    for (int group = 0; group < NumberOfExperienceGroups; ++group)
        for (int level = 1; level <= 100; ++level)
            m_experienceCurves[group][level - 1] = levelToExperience(level, static_cast<ExperienceGroup>(group));
}

void OptimizedSolver::setValidPokemon() {
    m_validPokemon.clear();
    for (int i = static_cast<int>(PokemonName::None) + 1; i < static_cast<int>(PokemonName::NumberOfPokemon); ++i)
        m_validPokemon.insert(static_cast<PokemonName>(i));
}

void OptimizedSolver::setValidPokemon(std::set<PokemonName> validPokemon) { m_validPokemon = validPokemon; }

void OptimizedSolver::setValidPokemon(Pokemon &pokemon) {
    m_validPokemon = std::set<PokemonName>{static_cast<PokemonName>(m_encryptoMon.getSpeciesID(pokemon))};
}

void OptimizedSolver::setValidItems() {
    m_validItems.clear();
    for (int i = static_cast<int>(Items::None); i < static_cast<int>(Items::NumberOfItems); ++i)
        m_validItems.insert(static_cast<Items>(i));
}

void OptimizedSolver::setValidItems(std::set<Items> validItems) { m_validItems = validItems; }

void OptimizedSolver::setValidMoves() {
    m_validMoves.clear();
    for (int i = static_cast<int>(MoveName::None); i < static_cast<int>(MoveName::NumberOfMoves); ++i)
        m_validMoves.insert(static_cast<MoveName>(i));
}

void OptimizedSolver::setValidMoves(std::set<MoveName> validMoves) { m_validMoves = validMoves; }

void OptimizedSolver::setValidMoves(Pokemon &pokemon) {
    m_validMoves.clear();
    m_validMoves.insert(MoveName::None);
    std::vector<LearnSetEntry> learnSet = learnSetMap.at(static_cast<PokemonName>(m_encryptoMon.getSpeciesID(pokemon)));
    for (const auto &entry : learnSet)
        m_validMoves.insert(entry.move);
}

BlockType OptimizedSolver::getBlockType(Pokemon &pokemon, uint8_t offset) const {
    const uint8_t headerSize = 0x8;
    const uint8_t blockSize = 0x20;
    if (offset < headerSize)
        return TBlockHeader;

    uint8_t order = m_encryptoMon.getBlockOrder(pokemon);
    uint8_t blockAOffset = headerSize + blockAPositions[order] * blockSize;
    if (offset >= blockAOffset && offset < blockAOffset + blockSize)
        return TBlockA;

    uint8_t blockBOffset = headerSize + blockBPositions[order] * blockSize;
    if (offset >= blockBOffset && offset < blockBOffset + blockSize)
        return TBlockB;

    uint8_t blockCOffset = headerSize + blockCPositions[order] * blockSize;
    if (offset >= blockCOffset && offset < blockCOffset + blockSize)
        return TBlockC;

    uint8_t blockDOffset = headerSize + blockDPositions[order] * blockSize;
    if (offset >= blockDOffset && offset < blockDOffset + blockSize)
        return TBlockD;

    return TBlockInvalid;
}

uint8_t OptimizedSolver::getBlockRelativeOffset(Pokemon &pokemon, uint8_t offset) const {
    const uint8_t headerSize = 0x8;
    const uint8_t blockSize = 0x20;
    if (offset < headerSize)
        return offset;

    return (offset - headerSize) % blockSize;
}

uint8_t OptimizedSolver::getBlockOffset(Pokemon &pokemon, BlockType blockType) const {
    uint8_t order = m_encryptoMon.getBlockOrder(pokemon);

    switch (blockType) {
    case TBlockHeader:
        return 0;
    case TBlockA:
        return 0x8 + blockAPositions[order] * 0x20;
    case TBlockB:
        return 0x8 + blockBPositions[order] * 0x20;
    case TBlockC:
        return 0x8 + blockCPositions[order] * 0x20;
    case TBlockD:
        return 0x8 + blockDPositions[order] * 0x20;
    default:
        return 0; // Invalid type
    }
}

bool OptimizedSolver::isBlockHeaderValid(Pokemon &pokemon, uint8_t &blockRelOffset, uint8_t remainingSize) const {
    if (blockRelOffset < 0x4) { // PID: assume always valid
        blockRelOffset = 4;
        return true;
    }

    if (blockRelOffset < 0x6) {
        if (pokemon.badEggFlag)
            return false;

        blockRelOffset = 6;
        return true;
    }

    if (blockRelOffset < 0x8) {
        blockRelOffset = 8;
        return true;
    }

    return false;
}

bool OptimizedSolver::isBlockAValid(Pokemon &pokemon, uint8_t &blockRelOffset, uint8_t remainingSize) {
    if (blockRelOffset < 0x2) {
        uint16_t species = m_encryptoMon.getSpeciesID(pokemon);
        PokemonName speciesName = static_cast<PokemonName>(species);

        if (blockRelOffset == 0) {
            if (remainingSize > 1) {
                auto it = std::find(m_validPokemon.begin(), m_validPokemon.end(), speciesName);
                if (it == m_validPokemon.end())
                    return false;

                blockRelOffset = 2;
                return true;
            }

            blockRelOffset = 1;
            return true;
        }

        for (const auto &validPokemon : m_validPokemon) {
            if (species & 0xFF == static_cast<uint16_t>(validPokemon) & 0xFF) {
                blockRelOffset = 2;
                return true;
            }
        }

        return false;
    }

    if (blockRelOffset < 0x4) {
        uint16_t heldItem = m_encryptoMon.getHeldItem(pokemon);
        Items heldItemName = static_cast<Items>(heldItem);
        if (blockRelOffset == 2) {
            if (remainingSize > 1) {
                auto it = std::find(m_validItems.begin(), m_validItems.end(), heldItemName);
                if (it == m_validItems.end())
                    return false;

                blockRelOffset = 4;
                return true;
            }

            blockRelOffset = 3;
            return true;
        }

        for (const auto &validItem : m_validItems) {
            if (heldItem & 0xFF == static_cast<uint16_t>(validItem) & 0xFF) {
                blockRelOffset = 4;
                return true;
            }
        }
        return false;
    }

    if (blockRelOffset < 0x8) { // TID/SID... to do
        return false;
    }

    if (blockRelOffset < 0xC) { // experience
        ExperienceGroup group = static_cast<ExperienceGroup>(m_encryptoMon.getSpeciesID(pokemon) & 0xFF);
        uint32_t experiencePoints = m_encryptoMon.getExperiencePoints(pokemon);
        if (experiencePoints > getExperienceForLevel(100, group))
            return false;

        blockRelOffset = 0xC;
        return true;
    }

    if (blockRelOffset < 0xD) {
        uint8_t friendship = m_encryptoMon.getFriendship(pokemon);
        if (friendship != 255)
            return false;

        blockRelOffset = 0xD;
        return true;
    }

    if (blockRelOffset < 0xE) { // ability
        return false;
    }

    if (blockRelOffset < 0xF) { // markings
        blockRelOffset++;
        return true;
    }

    if (blockRelOffset < 0x10) { // language
        return false;
    }

    if (blockRelOffset < 0x16) { // EVs
        const uint16_t startEvField = 0x10;

        uint8_t evs[6];
        evs[0] = m_encryptoMon.getHPEVs(pokemon);
        evs[1] = m_encryptoMon.getAttackEVs(pokemon);
        evs[2] = m_encryptoMon.getDefenseEVs(pokemon);
        evs[3] = m_encryptoMon.getSpAtkEVs(pokemon);
        evs[4] = m_encryptoMon.getSpDefEVs(pokemon);
        evs[5] = m_encryptoMon.getSpeedEVs(pokemon);

        uint16_t totalEvs = 0;
        uint8_t perfectEvs = 0;
        uint16_t diffFromPerfect = 0;
        for (int i = 0; i < (blockRelOffset - startEvField) + 1; ++i) {
            if (evs[i] <= 100 && evs[i] % 10 == 0)
                perfectEvs++;
            diffFromPerfect += std::max(static_cast<int>(evs[i] - 100), 0) + evs[i] % 10;
            totalEvs += evs[i];
        }

        if (totalEvs > getMaxEVs())
            return false;

        if (
            // perfectEvs >= m_bestResult.perfectEvs
            //     || diffFromPerfect < m_bestResult.diffFromPerfect
            diffFromPerfect < 10 && remainingSize == 1) {
            m_bestResult.checksum = m_encryptoMon.getChecksum(pokemon);
            m_bestResult.evs[0] = evs[0];
            m_bestResult.evs[1] = evs[1];
            m_bestResult.evs[2] = evs[2];
            m_bestResult.evs[3] = evs[3];
            m_bestResult.evs[4] = evs[4];
            m_bestResult.evs[5] = evs[5];
            m_bestResult.evCount = totalEvs;
            m_bestResult.perfectEvs = perfectEvs;
            m_bestResult.diffFromPerfect = diffFromPerfect;
            std::memcpy(&m_bestResult.pokemon, &pokemon, sizeof(Pokemon));
            printBestResult();
        }

        blockRelOffset++;
        return true;
    }

    return false; // ribbons, contest stats, sheen
}

bool OptimizedSolver::isBlockBValid(Pokemon &pokemon, uint8_t &blockRelOffset, uint8_t remainingSize,
                                    uint8_t offset) const {
    // TODO: check duplicate moves through offset
    if (blockRelOffset < 0x8) {
        uint16_t moveSet[4];
        m_encryptoMon.getMoveset(pokemon, moveSet);

        int moveSlot = blockRelOffset / 2;
        uint16_t move = moveSet[moveSlot];

        if (blockRelOffset % 2 == 0) {
            if (remainingSize > 1) {
                auto it = std::find(m_validMoves.begin(), m_validMoves.end(), static_cast<MoveName>(move));
                if (it == m_validMoves.end())
                    return false;

                if (moveSlot == 1 && move == static_cast<uint16_t>(MoveName::None))
                    return false;

                blockRelOffset += 2;
                return true;
            }

            for (const auto &validMove : m_validMoves) {
                if (move & 0xFF == static_cast<uint16_t>(validMove) & 0xFF) {
                    blockRelOffset++;
                    return true;
                }
            }
            return true;
        }

        for (const auto &validMove : m_validMoves) {
            if (move & 0xFF00 == static_cast<uint16_t>(validMove) & 0xFF00) {
                blockRelOffset++;
                return true;
            }
        }

        return false;
    }

    return false;
}

bool OptimizedSolver::isBlockDValid(Pokemon &pokemon, uint8_t &blockRelOffset, uint8_t remainingSize) const {
    if (blockRelOffset == 0x1E) {
        if (m_encryptoMon.getHGSSPokeBall(pokemon) != 0x0)
            return false;
        blockRelOffset++;
        return true;
    }

    if (blockRelOffset == 0x1F) {
        if (m_encryptoMon.getPerformance(pokemon) != 0x0)
            return false;
        blockRelOffset++;
        return true;
    }

    return false;
}

/* expects Pokémon in decrypted format, shuffled format */
bool OptimizedSolver::isDataValid(Pokemon &pokemon, uint8_t offset, uint8_t size) {
    uint8_t blockOffset, blockRelOffset = 0;
    uint8_t currentOffset = offset;
    uint8_t remainingSize = offset + size - currentOffset;

    while (currentOffset < offset + size) {
        remainingSize = offset + size - currentOffset;
        blockOffset = getBlockOffset(pokemon, getBlockType(pokemon, currentOffset));
        blockRelOffset = getBlockRelativeOffset(pokemon, currentOffset);

        switch (getBlockType(pokemon, currentOffset)) {
        case TBlockHeader:
            if (!isBlockHeaderValid(pokemon, blockRelOffset, remainingSize))
                return false;
            break;
        case TBlockA:
            if (!isBlockAValid(pokemon, blockRelOffset, remainingSize))
                return false;
            break;
        case TBlockB:
            if (!isBlockBValid(pokemon, blockRelOffset, remainingSize, offset))
                return false;
            break;
        case TBlockC:
            if (!isBlockCValid(pokemon, blockRelOffset, remainingSize))
                return false;
            break;
        case TBlockD:
            if (!isBlockDValid(pokemon, blockRelOffset, remainingSize))
                return false;
            break;
        default:
            return false;
        }

        currentOffset = blockOffset + blockRelOffset;
    }

    return true;
}

std::map<uint16_t, std::vector<ChecksumContribution>> OptimizedSolver::computeChecksumContributions(Pokemon &pokemon) {
    std::map<uint16_t, std::vector<ChecksumContribution>> checksumMap;

    uint8_t gender = m_encryptoMon.getGender(pokemon);
    ExperienceGroup experienceGroup = getExperienceGroup(static_cast<PokemonName>(m_encryptoMon.getSpeciesID(pokemon)));
    for (int year = 0; year <= 50; ++year) {
        for (int month = 1; month <= 12; ++month) {
            for (int day = 1; day <= 31; ++day) {
                uint16_t dateCS = (((year << 8) | month) - ((0 << 8) | 1));
                dateCS += ((day << 8) | 0) - ((1 << 8) | 0);
                for (uint8_t metLevel = 3; metLevel <= 30; metLevel++) {
                    uint8_t metLevelCS = ((metLevel | gender << 7) << 0) - ((1 | gender << 7) << 0);
                    uint32_t experiencePoints = levelToExperience(metLevel, experienceGroup);
                    uint16_t experienceCS = (experiencePoints >> 16) + (experiencePoints & 0xFFFF) - 1;
                    for (uint16_t item = 0; item < static_cast<uint16_t>(Items::NumberOfItems); ++item) {
                        uint16_t heldItemCS = item;
                        uint16_t totalCS = dateCS + experienceCS + metLevelCS + heldItemCS;

                        date metDate = {static_cast<uint8_t>(year), static_cast<uint8_t>(month),
                                        static_cast<uint8_t>(day)};
                        ChecksumContribution contribution = {metDate, metLevel, metLevel, experiencePoints,
                                                             static_cast<Items>(heldItemCS)};
                        checksumMap[totalCS].push_back(contribution);
                    }
                }
            }
        }
    }

    return checksumMap;
}

std::vector<uint16_t> OptimizedSolver::solveSequence(Pokemon &pokemon, const uint8_t *data, size_t size,
                                                     uint8_t offset) {
    std::vector<uint16_t> checksums;

    Pokemon pokemonCopy = pokemon;
    std::memcpy(reinterpret_cast<uint8_t *>(&pokemonCopy) + offset, data, size);
    std::cout << "Calculating checksum contributions, this may take a while...\n" << std::endl;
    std::map<uint16_t, std::vector<ChecksumContribution>> checksumMap = computeChecksumContributions(pokemonCopy);

    for (int checksum = 0; checksum <= 0xFFFF; ++checksum) {
        m_encryptoMon.setChecksum(pokemonCopy, static_cast<uint16_t>(checksum));
        m_encryptoMon.decryptSection(pokemonCopy, offset - 0x8, size);

        if (isDataValid(pokemonCopy, offset, size)) {
            checksums.push_back(checksum);

            uint16_t currentChecksum = m_encryptoMon.calculateChecksum(pokemonCopy);
            int16_t checksumDifference = calculateChecksumDifference(currentChecksum, checksum);

            if (checksumMap.find(checksumDifference) == checksumMap.end()) {
                m_encryptoMon.setChecksum(pokemonCopy, checksum);
                std::cout << "match for checksum " << std::hex << checksum << std::dec
                          << " but could not auto-manipulate one\n"
                          << std::endl;
                std::cout << "Current checksum: " << std::hex << currentChecksum << std::dec << "\n"
                          << "Checksum difference: " << std::hex << checksumDifference << std::dec << "\n"
                          << std::endl;
                std::cout << "Current Pokemon data:" << std::endl;

                std::cout << std::hex;
                for (int i = 0; i < sizeof(Pokemon); ++i) {
                    if (i % 16 == 0)
                        std::cout << std::endl;
                    else if (i % 16 == 8)
                        std::cout << " ";
                    std::cout << std::hex << std::setfill('0') << std::setw(2)
                              << static_cast<unsigned int>(reinterpret_cast<uint8_t *>(&pokemonCopy)[i]) << " ";
                }
                std::cout << std::dec << std::endl;
            } else {

                for (const auto &contribution : checksumMap[checksumDifference]) {
                    m_encryptoMon.setChecksum(pokemonCopy, checksum);
                    m_encryptoMon.setMetAtDate(pokemonCopy, contribution.metDate);
                    m_encryptoMon.setMetAtLevel(pokemonCopy, contribution.metLevel);
                    m_encryptoMon.setExperiencePoints(pokemonCopy, contribution.experiencePoints);
                    m_encryptoMon.setHeldItem(pokemonCopy, static_cast<uint16_t>(contribution.heldItem));

                    std::cout << "Found a match with the following data:\n"
                              << "\tChecksum: " << std::hex << checksum << std::dec << "\n"
                              << "\tName: "
                              << toString(static_cast<PokemonName>(m_encryptoMon.getSpeciesID(pokemonCopy))) << "\n"
                              << "\tLevel: " << static_cast<int>(contribution.currentLevel) << "\n"
                              << "\tDate: " << static_cast<int>(contribution.metDate.day) << "/"
                              << static_cast<int>(contribution.metDate.month) << "/"
                              << static_cast<int>(contribution.metDate.year) << "\n"
                              << "\tMet Level: " << static_cast<int>(m_encryptoMon.getMetAtLevel(pokemonCopy)) << "\n"
                              << "\tHeld Item: " << toString(static_cast<Items>(m_encryptoMon.getHeldItem(pokemonCopy)))
                              << "\n"
                              << std::endl;

                    m_encryptoMon.encryptPokemon(pokemonCopy);
                    std::cout << std::hex;
                    for (int i = 0; i < sizeof(Pokemon); ++i) {
                        if (i % 16 == 0)
                            std::cout << std::endl;
                        else if (i % 16 == 8)
                            std::cout << " ";
                        std::cout << std::hex << std::setfill('0') << std::setw(2)
                                  << static_cast<unsigned int>(reinterpret_cast<uint8_t *>(&pokemonCopy)[i]) << " ";
                    }
                    std::cout << std::dec << std::endl;
                    std::cout << std::endl;
                    m_encryptoMon.decryptPokemon(pokemonCopy);
                }
            }
        }

        m_encryptoMon.encryptSection(pokemonCopy, offset - 0x8, size);
    }

    return checksums;
}

bool OptimizedSolver::solve(Pokemon &pokemon, const uint8_t *data, size_t size, uint8_t offset) {
    setValidPokemon(pokemon);
    setValidItems(); // allow all items
    setValidMoves(pokemon);

    m_encryptoMon.setFriendship(pokemon, 255);
    m_encryptoMon.setMetAtDate(pokemon, 1, 1, 2000 - 2000);
    m_encryptoMon.setExperiencePoints(pokemon, 1);
    m_encryptoMon.setMetAtLevel(pokemon, 1);
    m_encryptoMon.setHeldItem(pokemon, static_cast<uint16_t>(Items::None));

    return solveSequence(pokemon, data, size, offset).empty();
}

void OptimizedSolver::printBestResult() {
    std::cout << "Checksum: " << std::hex << m_bestResult.checksum << std::dec << std::endl;
    std::cout << "EVs: ";
    for (int i = 0; i < 6; ++i) {
        std::cout << static_cast<int>(m_bestResult.evs[i]) << " ";
    }
    std::cout << std::endl;
    std::cout << "EV count: " << m_bestResult.evCount << std::endl;
    std::cout << "Perfect EVs: " << static_cast<int>(m_bestResult.perfectEvs) << std::endl;
    std::cout << "Diff from perfect: " << m_bestResult.diffFromPerfect << std::endl;

    m_encryptoMon.setChecksum(m_bestResult.pokemon, m_bestResult.checksum);
    m_encryptoMon.decryptPokemon(m_bestResult.pokemon);

    std::cout << "Data: ";
    std::cout << std::hex;
    for (int i = 0; i < sizeof(Pokemon); ++i) {
        if (i % 16 == 0) {
            std::cout << std::endl;
            // std::cout << std::hex << std::setfill('0') << std::setw(2) << i << " ";
        }
        if (i % 16 == 8) {
            std::cout << " ";
        }
        std::cout << std::hex << std::setfill('0') << std::setw(2)
                  << static_cast<unsigned int>(reinterpret_cast<uint8_t *>(&m_bestResult.pokemon)[i]) << " ";
    }
    std::cout << std::dec << std::endl;
    std::cout << std::endl;

    m_encryptoMon.encryptPokemon(m_bestResult.pokemon);
}