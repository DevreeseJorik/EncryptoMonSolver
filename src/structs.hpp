#pragma once

#include "items.hpp"
#include "pokemon.hpp"
#include "moves.hpp"

#include <cstdint>
#include <string>
#include <array>

extern uint8_t blockAPositions[24];
extern uint8_t blockBPositions[24];
extern uint8_t blockCPositions[24];
extern uint8_t blockDPositions[24];

enum BlockType {
    TBlockHeader = 0,
    TBlockA = 1,
    TBlockB = 2,
    TBlockC = 3,
    TBlockD = 4,
    TBlockInvalid = 5,
};

struct BlockA {
    uint16_t speciesID;
    uint16_t heldItem;
    uint16_t otID;
    uint16_t otSecretID;
    uint32_t experiencePoints;
    uint8_t friendship; // used as egg steps when egg
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

struct BlockC {
    uint16_t nickname[11];
    uint8_t unused;
    uint8_t gameOfOrigin;
    uint32_t sinnohRibbonsSet2;
    uint32_t unused2;
};

struct date {
    uint8_t year;
    uint8_t month;
    uint8_t day;
};

struct BlockD {
    uint16_t otName[8];
    date dateEggReceived;
    date metAtDate;
    uint16_t diamondPearlEggLocation;
    uint16_t diamondPearlMetAtLocation;
    uint8_t pokerus;
    uint8_t pokeBall;
    uint8_t metAtLevel : 7;
    uint8_t gender : 1;
    uint8_t encounterType;
    uint8_t hgssPokeBall;
    uint8_t performance; // 'walking mood'
};

struct Block {
    uint8_t dummyTemplate[0x20];
};

struct Pokemon {
    uint32_t pid;
    uint16_t badEggFlag;
    uint16_t checksum;
    Block block_data[4];
};

#define MAX_LEVELS 100
#define MAX_ITEMS 0x1D0 
#define MAX_MOVES 0x1D3