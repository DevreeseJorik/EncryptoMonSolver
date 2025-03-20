#include <fstream>
#include <iostream>
#include "EncryptoMon.hpp"

uint16_t* xorMasks = (uint16_t*)malloc(CHECKSUMS * XOR_MASK_SIZE * sizeof(uint16_t));

bool EncryptoMon::loadBinaryPokemon(const std::string& filePath, Pokemon& pokemon) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&pokemon), sizeof(Pokemon));
    file.close();
    return true;
}

bool EncryptoMon::dumpBinaryPokemon(const std::string& filePath, Pokemon& pokemon) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(&pokemon), sizeof(Pokemon));

    if (!file) {
        std::cerr << "Error writing data to file: " << filePath << std::endl;
        return false;
    }

    file.close();
    return true;
}

void EncryptoMon::preparePokemon(Pokemon& pokemon) {
    setChecksum(pokemon);
    shuffleBlocks(pokemon);
}

void EncryptoMon::processPokemon(Pokemon& pokemon) {
    setChecksum(pokemon);
    shuffleBlocks(pokemon);
    encryptPokemon(pokemon);
}

void EncryptoMon::generateXORMasks() {
    for (uint32_t c = 0; c < 0xFFFF; c++)
    {
        uint32_t s = c;
        for (uint32_t i = 0; i < 0x40; i++)
        {
            s = s * 0x41C64E6D + 0x6073;
            xorMasks[c * 0x40 + i] = static_cast<uint16_t>(s >> 16);
        }
    }
}

void EncryptoMon::encryptPokemon(Pokemon& pokemon) {
    uint16_t* data = (uint16_t*)pokemon.block_data;
    for (uint32_t i = 0; i < 0x40; i++)
    {
        data[i] ^= xorMasks[pokemon.checksum * 0x40 + i];
    }
}

void EncryptoMon::encryptSection(Pokemon& pokemon, uint8_t offset, uint8_t size) {
    uint16_t* data = (uint16_t*)pokemon.block_data;
    for (uint32_t i = offset / sizeof(uint16_t); i < (offset + size) / sizeof(uint16_t); i++)
    {
        data[i] ^= xorMasks[pokemon.checksum * 0x40 + i];
    }
}

void EncryptoMon::encryptSection(Pokemon& pokemon, uint8_t offset) {
    uint32_t* data = (uint32_t*)pokemon.block_data;
    uint32_t index = offset / sizeof(uint32_t);
    uint32_t* xorMask = (uint32_t*)&xorMasks[pokemon.checksum * 0x40 + index];
    data[index] ^= *xorMask;
}

void EncryptoMon::decryptPokemon(Pokemon& pokemon) {
    encryptPokemon(pokemon);
}

void EncryptoMon::decryptSection(Pokemon& pokemon, uint8_t offset, uint8_t size) {
    encryptSection(pokemon, offset, size);
}

void EncryptoMon::decryptSection(Pokemon& pokemon, uint8_t offset) {
    encryptSection(pokemon, offset);
}


uint8_t* EncryptoMon::getBlockIds(Pokemon& pokemon) const {
    uint8_t order = getBlockOrder(pokemon);
    uint8_t* blockIds = new uint8_t[4] {
        blockAPositions[order],
        blockBPositions[order],
        blockCPositions[order],
        blockDPositions[order]
    };
    return blockIds;
}

BlockA* EncryptoMon::getBlockAPreShuffle(Pokemon& pokemon) const {
    return reinterpret_cast<BlockA*>(&pokemon.block_data[0]);
}

BlockB* EncryptoMon::getBlockBPreShuffle(Pokemon& pokemon) const {
    return reinterpret_cast<BlockB*>(&pokemon.block_data[1]);
}

BlockC* EncryptoMon::getBlockCPreShuffle(Pokemon& pokemon) const {
    return reinterpret_cast<BlockC*>(&pokemon.block_data[2]);
}

BlockD* EncryptoMon::getBlockDPreShuffle(Pokemon& pokemon) const {
    return reinterpret_cast<BlockD*>(&pokemon.block_data[3]);
}

BlockA* EncryptoMon::getBlockAAfterShuffle(Pokemon& pokemon) const {
    uint8_t order = getBlockOrder(pokemon);
    return reinterpret_cast<BlockA*>(&pokemon.block_data[blockAPositions[order]]);
}

BlockB* EncryptoMon::getBlockBAfterShuffle(Pokemon& pokemon) const {
    uint8_t order = getBlockOrder(pokemon);
    return reinterpret_cast<BlockB*>(&pokemon.block_data[blockBPositions[order]]);
}

BlockC* EncryptoMon::getBlockCAfterShuffle(Pokemon& pokemon) const {
    uint8_t order = getBlockOrder(pokemon);
    return reinterpret_cast<BlockC*>(&pokemon.block_data[blockCPositions[order]]);
}

BlockD* EncryptoMon::getBlockDAfterShuffle(Pokemon& pokemon) const {
    uint8_t order = getBlockOrder(pokemon);
    return reinterpret_cast<BlockD*>(&pokemon.block_data[blockDPositions[order]]);
}

uint8_t EncryptoMon::getBlockOrder(Pokemon& pokemon) const {
    return ((pokemon.pid & 0x3E000) >> 13) % 24;
}

void EncryptoMon::shuffleBlocks(Pokemon& pokemon) {
    uint8_t* blockIds = getBlockIds(pokemon);

    Block temp[4];
    for (int i = 0; i < 4; ++i) {
        temp[i] = pokemon.block_data[i];
    }

    for (int i = 0; i < 4; ++i) {
        pokemon.block_data[i] = temp[blockIds[i]];
    }
}

void EncryptoMon::unshuffleBlocks(Pokemon& pokemon) {
    uint8_t* blockIds = getBlockIds(pokemon);

    Block temp[4];
    for (int i = 0; i < 4; ++i) {
        temp[blockIds[i]] = pokemon.block_data[i];
    }

    for (int i = 0; i < 4; ++i) {
        pokemon.block_data[i] = temp[i];
    }

    delete[] blockIds;
}

uint16_t EncryptoMon::calculateChecksum(Pokemon &pokemon) {
    uint16_t* data = (uint16_t*)&pokemon.block_data;
    uint16_t checksum = 0;
    for (uint32_t i = 0; i < 4 * 16; i++)
    {
        checksum += data[i];
    }
    return checksum;
}

void EncryptoMon::setChecksum(Pokemon& pokemon) {
    pokemon.checksum = calculateChecksum(pokemon);
}

void EncryptoMon::setChecksum(Pokemon& pokemon, uint16_t checksum) {
    pokemon.checksum = checksum;
}


// Block A
uint16_t EncryptoMon::getSpeciesID(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->speciesID;
}

void EncryptoMon::setSpeciesID(Pokemon& pokemon, uint16_t speciesID) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->speciesID = speciesID;
}

uint16_t EncryptoMon::getHeldItem(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->heldItem;
}

void EncryptoMon::setHeldItem(Pokemon& pokemon, uint16_t heldItem) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->heldItem = heldItem;
}

uint16_t EncryptoMon::getOTID(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->otID;
}

void EncryptoMon::setOTID(Pokemon& pokemon, uint16_t otID) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->otID = otID;
}

uint16_t EncryptoMon::getOTSecretID(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->otSecretID;
}

void EncryptoMon::setOTSecretID(Pokemon& pokemon, uint16_t otSecretID) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->otSecretID = otSecretID;
}

uint32_t EncryptoMon::getExperiencePoints(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->experiencePoints;
}

void EncryptoMon::setExperiencePoints(Pokemon& pokemon, uint32_t experiencePoints) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->experiencePoints = experiencePoints;
}

uint8_t EncryptoMon::getFriendship(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->friendship;
}

void EncryptoMon::setFriendship(Pokemon& pokemon, uint8_t friendship) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->friendship = friendship;
}

uint8_t EncryptoMon::getAbility(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->ability;
}

void EncryptoMon::setAbility(Pokemon& pokemon, uint8_t ability) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->ability = ability;
}

uint8_t EncryptoMon::getMarkings(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->markings;
}

void EncryptoMon::setMarkings(Pokemon& pokemon, uint8_t markings) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->markings = markings;
}

uint8_t EncryptoMon::getLanguageOfOrigin(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->languageOfOrigin;
}

void EncryptoMon::setLanguageOfOrigin(Pokemon& pokemon, uint8_t languageOfOrigin) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->languageOfOrigin = languageOfOrigin;
}

uint8_t EncryptoMon::getHPEVs(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->hpEVs;
}

void EncryptoMon::setHPEVs(Pokemon& pokemon, uint8_t hpEVs) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->hpEVs = hpEVs;
}

uint8_t EncryptoMon::getAttackEVs(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->attackEVs;
}

void EncryptoMon::setAttackEVs(Pokemon& pokemon, uint8_t attackEVs) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->attackEVs = attackEVs;
}

uint8_t EncryptoMon::getDefenseEVs(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->defenseEVs;
}

void EncryptoMon::setDefenseEVs(Pokemon& pokemon, uint8_t defenseEVs) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->defenseEVs = defenseEVs;
}

uint8_t EncryptoMon::getSpeedEVs(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->speedEVs;
}

void EncryptoMon::setSpeedEVs(Pokemon& pokemon, uint8_t speedEVs) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->speedEVs = speedEVs;
}

uint8_t EncryptoMon::getSpAtkEVs(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->spAtkEVs;
}

void EncryptoMon::setSpAtkEVs(Pokemon& pokemon, uint8_t spAtkEVs) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->spAtkEVs = spAtkEVs;
}

uint8_t EncryptoMon::getSpDefEVs(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->spDefEVs;
}

void EncryptoMon::setSpDefEVs(Pokemon& pokemon, uint8_t spDefEVs) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->spDefEVs = spDefEVs;
}

uint8_t EncryptoMon::getCoolContestStat(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->coolContestStat;
}

void EncryptoMon::setCoolContestStat(Pokemon& pokemon, uint8_t coolContestStat) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->coolContestStat = coolContestStat;
}

uint8_t EncryptoMon::getBeautyContestStat(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->beautyContestStat;
}

void EncryptoMon::setBeautyContestStat(Pokemon& pokemon, uint8_t beautyContestStat) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->beautyContestStat = beautyContestStat;
}

uint8_t EncryptoMon::getCuteContestStat(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->cuteContestStat;
}

void EncryptoMon::setCuteContestStat(Pokemon& pokemon, uint8_t cuteContestStat) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->cuteContestStat = cuteContestStat;
}

uint8_t EncryptoMon::getSmartContestStat(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->smartContestStat;
}

void EncryptoMon::setSmartContestStat(Pokemon& pokemon, uint8_t smartContestStat) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->smartContestStat = smartContestStat;
}

uint8_t EncryptoMon::getToughContestStat(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->toughContestStat;
}

void EncryptoMon::setToughContestStat(Pokemon& pokemon, uint8_t toughContestStat) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->toughContestStat = toughContestStat;
}

uint8_t EncryptoMon::getSheen(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->sheen;
}

void EncryptoMon::setSheen(Pokemon& pokemon, uint8_t sheen) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->sheen = sheen;
}

uint32_t EncryptoMon::getSinnohRibbonsSet1(Pokemon& pokemon) const {
    const BlockA* blockA = getBlockAPreShuffle(pokemon);
    return blockA->sinnohRibbonsSet1;
}

void EncryptoMon::setSinnohRibbonsSet1(Pokemon& pokemon, uint32_t sinnohRibbonsSet1) {
    BlockA* blockA = getBlockAPreShuffle(pokemon);
    blockA->sinnohRibbonsSet1 = sinnohRibbonsSet1;
}

// Block B
void EncryptoMon::getMoveset(Pokemon& pokemon, uint16_t moveset[4]) const {
    const BlockB* blockB = getBlockBPreShuffle(pokemon);
    std::copy(blockB->moveset, blockB->moveset + 4, moveset);
}

void EncryptoMon::setMoveset(Pokemon& pokemon, const uint16_t moveset[4]) {
    BlockB* blockB = getBlockBPreShuffle(pokemon);
    std::copy(moveset, moveset + 4, blockB->moveset);
}

void EncryptoMon::getMovePP(Pokemon& pokemon, uint8_t movePP[4]) const {
    const BlockB* blockB = getBlockBPreShuffle(pokemon);
    std::copy(blockB->movePP, blockB->movePP + 4, movePP);
}

void EncryptoMon::setMovePP(Pokemon& pokemon, const uint8_t movePP[4]) {
    BlockB* blockB = getBlockBPreShuffle(pokemon);
    std::copy(movePP, movePP + 4, blockB->movePP);
}

void EncryptoMon::getMovePPUps(Pokemon& pokemon, uint8_t movePPUps[4]) const {
    const BlockB* blockB = getBlockBPreShuffle(pokemon);
    std::copy(blockB->movePPUps, blockB->movePPUps + 4, movePPUps);
}

void EncryptoMon::setMovePPUps(Pokemon& pokemon, const uint8_t movePPUps[4]) {
    BlockB* blockB = getBlockBPreShuffle(pokemon);
    std::copy(movePPUps, movePPUps + 4, blockB->movePPUps);
}

uint32_t EncryptoMon::getIndividualValues(Pokemon& pokemon) const {
    const BlockB* blockB = getBlockBPreShuffle(pokemon);
    return blockB->individualValues;
}

void EncryptoMon::setIndividualValues(Pokemon& pokemon, uint32_t individualValues) {
    BlockB* blockB = getBlockBPreShuffle(pokemon);
    blockB->individualValues = individualValues;
}

uint32_t EncryptoMon::getHoennRibbonSet(Pokemon& pokemon) const {
    const BlockB* blockB = getBlockBPreShuffle(pokemon);
    return blockB->hoennRibbonSet;
}

void EncryptoMon::setHoennRibbonSet(Pokemon& pokemon, uint32_t hoennRibbonSet) {
    BlockB* blockB = getBlockBPreShuffle(pokemon);
    blockB->hoennRibbonSet = hoennRibbonSet;
}

uint8_t EncryptoMon::getFlags(Pokemon& pokemon) const {
    const BlockB* blockB = getBlockBPreShuffle(pokemon);
    return blockB->flags;
}

void EncryptoMon::setFlags(Pokemon& pokemon, uint8_t flags) {
    BlockB* blockB = getBlockBPreShuffle(pokemon);
    blockB->flags = flags;
}

uint8_t EncryptoMon::getShinyLeaves(Pokemon& pokemon) const {
    const BlockB* blockB = getBlockBPreShuffle(pokemon);
    return blockB->shinyLeaves;
}

void EncryptoMon::setShinyLeaves(Pokemon& pokemon, uint8_t shinyLeaves) {
    BlockB* blockB = getBlockBPreShuffle(pokemon);
    blockB->shinyLeaves = shinyLeaves;
}

uint16_t EncryptoMon::getPlatinumEggLocation(Pokemon& pokemon) const {
    const BlockB* blockB = getBlockBPreShuffle(pokemon);
    return blockB->platinumEggLocation;
}

void EncryptoMon::setPlatinumEggLocation(Pokemon& pokemon, uint16_t platinumEggLocation) {
    BlockB* blockB = getBlockBPreShuffle(pokemon);
    blockB->platinumEggLocation = platinumEggLocation;
}

uint16_t EncryptoMon::getPlatinumMetAtLocation(Pokemon& pokemon) const {
    const BlockB* blockB = getBlockBPreShuffle(pokemon);
    return blockB->platinumMetAtLocation;
}

void EncryptoMon::setPlatinumMetAtLocation(Pokemon& pokemon, uint16_t platinumMetAtLocation) {
    BlockB* blockB = getBlockBPreShuffle(pokemon);
    blockB->platinumMetAtLocation = platinumMetAtLocation;
}

// Block C
void EncryptoMon::getNickname(Pokemon& pokemon, uint16_t nickname[11]) const {
    const BlockC* blockC = getBlockCPreShuffle(pokemon);
    std::copy(blockC->nickname, blockC->nickname + 11, nickname);
}

void EncryptoMon::setNickname(Pokemon& pokemon, const uint16_t nickname[11]) {
    BlockC* blockC = getBlockCPreShuffle(pokemon);
    std::copy(nickname, nickname + 11, blockC->nickname);
}

uint8_t EncryptoMon::getUnused(Pokemon& pokemon) const {
    const BlockC* blockC = getBlockCPreShuffle(pokemon);
    return blockC->unused;
}

void EncryptoMon::setUnused(Pokemon& pokemon, uint8_t unused) {
    BlockC* blockC = getBlockCPreShuffle(pokemon);
    blockC->unused = unused;
}

uint8_t EncryptoMon::getGameOfOrigin(Pokemon& pokemon) const {
    const BlockC* blockC = getBlockCPreShuffle(pokemon);
    return blockC->gameOfOrigin;
}

void EncryptoMon::setGameOfOrigin(Pokemon& pokemon, uint8_t gameOfOrigin) {
    BlockC* blockC = getBlockCPreShuffle(pokemon);
    blockC->gameOfOrigin = gameOfOrigin;
}

uint32_t EncryptoMon::getSinnohRibbonsSet2(Pokemon& pokemon) const {
    const BlockC* blockC = getBlockCPreShuffle(pokemon);
    return blockC->sinnohRibbonsSet2;
}

void EncryptoMon::setSinnohRibbonsSet2(Pokemon& pokemon, uint32_t sinnohRibbonsSet2) {
    BlockC* blockC = getBlockCPreShuffle(pokemon);
    blockC->sinnohRibbonsSet2 = sinnohRibbonsSet2;
}

uint32_t EncryptoMon::getUnused2(Pokemon& pokemon) const {
    const BlockC* blockC = getBlockCPreShuffle(pokemon);
    return blockC->unused2;
}

void EncryptoMon::setUnused2(Pokemon& pokemon, uint32_t unused2) {
    BlockC* blockC = getBlockCPreShuffle(pokemon);
    blockC->unused2 = unused2;
}

// Block D
void EncryptoMon::getOTName(Pokemon& pokemon, uint16_t otName[8]) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    std::copy(blockD->otName, blockD->otName + 8, otName);
}

void EncryptoMon::setOTName(Pokemon& pokemon, const uint16_t otName[8]) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    std::copy(otName, otName + 8, blockD->otName);
}

date EncryptoMon::getDateEggReceived(Pokemon& pokemon) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    return blockD->dateEggReceived;
}

void EncryptoMon::setDateEggReceived(Pokemon& pokemon, date dateEggReceived) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->dateEggReceived = dateEggReceived;
}

date EncryptoMon::getMetAtDate(Pokemon& pokemon) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    return blockD->metAtDate;
}

void EncryptoMon::setMetAtDate(Pokemon& pokemon, date metAtDate) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->metAtDate = metAtDate;
}

void EncryptoMon::setMetAtDate(Pokemon& pokemon, int day, int month, int year) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->metAtDate.day = day;
    blockD->metAtDate.month = month;
    blockD->metAtDate.year = year;
}

uint16_t EncryptoMon::getDiamondPearlEggLocation(Pokemon& pokemon) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    return blockD->diamondPearlEggLocation;
}

void EncryptoMon::setDiamondPearlEggLocation(Pokemon& pokemon, uint16_t diamondPearlEggLocation) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->diamondPearlEggLocation = diamondPearlEggLocation;
}

uint16_t EncryptoMon::getDiamondPearlMetAtLocation(Pokemon& pokemon) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    return blockD->diamondPearlMetAtLocation;
}

void EncryptoMon::setDiamondPearlMetAtLocation(Pokemon& pokemon, uint16_t diamondPearlMetAtLocation) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->diamondPearlMetAtLocation = diamondPearlMetAtLocation;
}

uint8_t EncryptoMon::getPokerus(Pokemon& pokemon) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    return blockD->pokerus;
}

void EncryptoMon::setPokerus(Pokemon& pokemon, uint8_t pokerus) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->pokerus = pokerus;
}

uint8_t EncryptoMon::getPokeBall(Pokemon& pokemon) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    return blockD->pokeBall;
}

void EncryptoMon::setPokeBall(Pokemon& pokemon, uint8_t pokeBall) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->pokeBall = pokeBall;
}

uint8_t EncryptoMon::getMetAtLevel(Pokemon& pokemon) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    return blockD->metAtLevel;
}

void EncryptoMon::setMetAtLevel(Pokemon& pokemon, uint8_t metAtLevel) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->metAtLevel = metAtLevel;
}

uint8_t EncryptoMon::getGender(Pokemon& pokemon) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    return blockD->gender;
}

void EncryptoMon::setGender(Pokemon& pokemon, uint8_t gender) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->gender = gender;
}

uint8_t EncryptoMon::getEncounterType(Pokemon& pokemon) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    return blockD->encounterType;
}

void EncryptoMon::setEncounterType(Pokemon& pokemon, uint8_t encounterType) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->encounterType = encounterType;
}

uint8_t EncryptoMon::getHGSSPokeBall(Pokemon& pokemon) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    return blockD->hgssPokeBall;
}

void EncryptoMon::setHGSSPokeBall(Pokemon& pokemon, uint8_t hgssPokeBall) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->hgssPokeBall = hgssPokeBall;
}

uint8_t EncryptoMon::getPerformance(Pokemon& pokemon) const {
    const BlockD* blockD = getBlockDPreShuffle(pokemon);
    return blockD->performance;
}

void EncryptoMon::setPerformance(Pokemon& pokemon, uint8_t performance) {
    BlockD* blockD = getBlockDPreShuffle(pokemon);
    blockD->performance = performance;
}