#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstdint>
#include <string>
#include <array>

// Define struct for each of the Pokémon Gen 4 blocks
extern uint8_t blockAPositions[24];
extern uint8_t blockBPositions[24];
extern uint8_t blockCPositions[24];
extern uint8_t blockDPositions[24];

// Block A
struct BlockA {
    uint16_t speciesID;
    uint16_t heldItem;
    uint16_t otID;
    uint16_t otSecretID;
    uint32_t experiencePoints;
    uint8_t friendshipEggSteps;
    uint8_t ability;
    uint8_t markings;
    uint8_t languageOfOrigin;
    uint8_t hpEVs;
    uint8_t attackEVs;
    uint8_t defenseEVs;
    uint8_t speedEVs;
    uint8_t spAtkEVs;
    uint8_t spDefEVs;
    uint8_t coolContestStat;
    uint8_t beautyContestStat;
    uint8_t cuteContestStat;
    uint8_t smartContestStat;
    uint8_t toughContestStat;
    uint8_t sheen;
    uint32_t sinnohRibbonsSet1;
};

// Block B
struct BlockB {
    uint16_t moveset[4];
    uint8_t movePP[4];
    uint8_t movePPUps[4];
    uint32_t individualValues;
    uint32_t hoennRibbonSet;
    uint8_t flags;
    uint8_t shinyLeaves;
    uint8_t unused[2];
    uint16_t platinumEggLocation;
    uint16_t platinumMetAtLocation;
};

// Block C
struct BlockC {
    uint16_t nickname[11];
    uint8_t unused;
    uint8_t gameOfOrigin;
    uint32_t sinnohRibbonsSet2;
    uint8_t unused2[4];
};

// Block D
struct BlockD {
    uint16_t otName[8];
    uint8_t dateEggReceived[3];
    uint8_t dateMet[3];
    uint16_t diamondPearlEggLocation;
    uint16_t diamondPearlMetAtLocation;
    uint8_t pokerus;
    uint8_t pokeBall;
    uint8_t metAtLevelandGender;
    uint8_t encounterType;
    uint8_t hgssPokeBall;
    uint8_t performance;
};

struct Block {
    uint8_t dummyTemplate[0x20];
};

// Pokémon struct

struct Pokemon {
    uint32_t pid;
    uint16_t badEggFlag;
    uint16_t checksum;
    Block block_data[4];
};

#define MAX_ITEMS 0x1D0 
#define MAX_MOVES 0x1D3
#define MAX_EVS 510

#endif // STRUCTS_H