#pragma once

#include "items.hpp"
#include "moves.hpp"
#include "pokemon.hpp"

#include <array>
#include <cstdint>
#include <string>

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

struct MailPokeIcon {
    uint16_t data; // TODO: figure out format
};

struct MailMessage {
    uint16_t sentenceGroup;
    uint16_t sentenceIndex;
    uint16_t inputField[2];
};
struct Mail {
    uint32_t tidsid;
    uint8_t sex;
    uint8_t gameRegion;
    uint8_t gameVersion;
    uint8_t mailType;
    uint16_t name[8];
    MailPokeIcon pokeIcon[4];
    MailMessage msg[3];
};

struct Seal {
    uint8_t id;
    uint8_t x;
    uint8_t y;
};

struct BallCapsule {
    Seal seals[0x8];
};

struct BattleData {
    union {
        struct {
            uint8_t asleepRounds : 3;
            uint8_t poisoned : 1;
            uint8_t burned : 1;
            uint8_t frozen : 1;
            uint8_t paralyzed : 1;
            uint8_t toxic : 1;
        } statusFlags;
        uint8_t statusByte;
    };
    uint8_t unknownFlags;
    uint16_t unknown2;
    uint8_t level;
    uint8_t capsuleIndex;
    uint16_t currentHP;
    uint16_t maxHP;
    uint16_t attack;
    uint16_t defense;
    uint16_t speed;
    uint16_t spAttack;
    uint16_t spDefense;
    Mail mailData;
    BallCapsule ballCapsule;
};

struct ExtendedPokemon {
    Pokemon pokemon;
    BattleData battleData;
};

static_assert(sizeof(BlockA) == 32, "BlockA must be 32 bytes");
static_assert(sizeof(BlockB) == 32, "BlockB must be 32 bytes");
static_assert(sizeof(BlockC) == 32, "BlockC must be 32 bytes");
static_assert(sizeof(BlockD) == 32, "BlockD must be 32 bytes");
static_assert(sizeof(Block)  == 32, "Block must be 32 bytes");
static_assert(sizeof(Pokemon) == 136, "Pokemon must be 136 bytes");

// Box storage (no extended party data — 136 bytes per slot)
struct Box {
    Pokemon pokemon[30];
};

struct BoxData {
    uint32_t currentBoxID;
    Box      boxes[18];
    // TODO: add box name and background data structs
};

#define MAX_LEVELS 100
#define MAX_ITEMS 0x1D0
#define MAX_MOVES 0x1D3
