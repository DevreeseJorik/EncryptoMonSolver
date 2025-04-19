#ifndef ENCRYPTOMON_H
#define ENCRYPTOMON_H

#include "structs.hpp"
#include <array>

const int CHECKSUMS = 0xFFFF;
const int XOR_MASK_SIZE = 0x40;

class EncryptoMon {
  public:
    bool loadBinaryPokemon(const std::string &filePath, Pokemon &pokemon);
    bool dumpBinaryPokemon(const std::string &filePath, Pokemon &pokemon);
    void preparePokemon(Pokemon &pokemon);
    void processPokemon(Pokemon &pokemon);
    void generateXORMasks();
    void encryptPokemon(Pokemon &pokemon);
    void encryptSection(Pokemon &pokemon, uint8_t offset, uint8_t size);
    void encryptSection(Pokemon &pokemon, uint8_t offset);
    void decryptPokemon(Pokemon &pokemon);
    void decryptSection(Pokemon &pokemon, uint8_t offset, uint8_t size);
    void decryptSection(Pokemon &pokemon, uint8_t offset);
    uint8_t *getBlockIds(Pokemon &pokemon) const;
    BlockA *getBlockAPreShuffle(Pokemon &pokemon) const;
    BlockB *getBlockBPreShuffle(Pokemon &pokemon) const;
    BlockC *getBlockCPreShuffle(Pokemon &pokemon) const;
    BlockD *getBlockDPreShuffle(Pokemon &pokemon) const;
    BlockA *getBlockAAfterShuffle(Pokemon &pokemon) const;
    BlockB *getBlockBAfterShuffle(Pokemon &pokemon) const;
    BlockC *getBlockCAfterShuffle(Pokemon &pokemon) const;
    BlockD *getBlockDAfterShuffle(Pokemon &pokemon) const;
    uint8_t getBlockOrder(Pokemon &pokemon) const;
    void shuffleBlocks(Pokemon &pokemon);
    void unshuffleBlocks(Pokemon &pokemon);
    uint16_t calculateChecksum(Pokemon &pokemon);
    void setChecksum(Pokemon &pokemon);
    void setChecksum(Pokemon &pokemon, uint16_t checksum);
    uint16_t getChecksum(Pokemon &pokemon) const;

    // Block A
    uint16_t getSpeciesID(Pokemon &pokemon) const;
    void setSpeciesID(Pokemon &pokemon, uint16_t speciesID);
    uint16_t getHeldItem(Pokemon &pokemon) const;
    void setHeldItem(Pokemon &pokemon, uint16_t heldItem);
    uint16_t getOTID(Pokemon &pokemon) const;
    void setOTID(Pokemon &pokemon, uint16_t otID);
    uint16_t getOTSecretID(Pokemon &pokemon) const;
    void setOTSecretID(Pokemon &pokemon, uint16_t otSecretID);
    uint32_t getExperiencePoints(Pokemon &pokemon) const;
    void setExperiencePoints(Pokemon &pokemon, uint32_t experiencePoints);
    uint8_t getFriendship(Pokemon &pokemon) const;
    void setFriendship(Pokemon &pokemon, uint8_t friendship);
    uint8_t getAbility(Pokemon &pokemon) const;
    void setAbility(Pokemon &pokemon, uint8_t ability);
    uint8_t getMarkings(Pokemon &pokemon) const;
    void setMarkings(Pokemon &pokemon, uint8_t markings);
    uint8_t getLanguageOfOrigin(Pokemon &pokemon) const;
    void setLanguageOfOrigin(Pokemon &pokemon, uint8_t languageOfOrigin);
    uint8_t getHPEVs(Pokemon &pokemon) const;
    void setHPEVs(Pokemon &pokemon, uint8_t hpEVs);
    uint8_t getAttackEVs(Pokemon &pokemon) const;
    void setAttackEVs(Pokemon &pokemon, uint8_t attackEVs);
    uint8_t getDefenseEVs(Pokemon &pokemon) const;
    void setDefenseEVs(Pokemon &pokemon, uint8_t defenseEVs);
    uint8_t getSpeedEVs(Pokemon &pokemon) const;
    void setSpeedEVs(Pokemon &pokemon, uint8_t speedEVs);
    uint8_t getSpAtkEVs(Pokemon &pokemon) const;
    void setSpAtkEVs(Pokemon &pokemon, uint8_t spAtkEVs);
    uint8_t getSpDefEVs(Pokemon &pokemon) const;
    void setSpDefEVs(Pokemon &pokemon, uint8_t spDefEVs);
    uint8_t getCoolContestStat(Pokemon &pokemon) const;
    void setCoolContestStat(Pokemon &pokemon, uint8_t coolContestStat);
    uint8_t getBeautyContestStat(Pokemon &pokemon) const;
    void setBeautyContestStat(Pokemon &pokemon, uint8_t beautyContestStat);
    uint8_t getCuteContestStat(Pokemon &pokemon) const;
    void setCuteContestStat(Pokemon &pokemon, uint8_t cuteContestStat);
    uint8_t getSmartContestStat(Pokemon &pokemon) const;
    void setSmartContestStat(Pokemon &pokemon, uint8_t smartContestStat);
    uint8_t getToughContestStat(Pokemon &pokemon) const;
    void setToughContestStat(Pokemon &pokemon, uint8_t toughContestStat);
    uint8_t getSheen(Pokemon &pokemon) const;
    void setSheen(Pokemon &pokemon, uint8_t sheen);
    uint32_t getSinnohRibbonsSet1(Pokemon &pokemon) const;
    void setSinnohRibbonsSet1(Pokemon &pokemon, uint32_t sinnohRibbonsSet1);

    // Block B
    void getMoveset(Pokemon &pokemon, uint16_t moveset[4]) const;
    void setMoveset(Pokemon &pokemon, const uint16_t moveset[4]);
    void setMove(Pokemon &pokemon, uint16_t move, size_t index);
    void getMovePP(Pokemon &pokemon, uint8_t movePP[4]) const;
    void setMovePP(Pokemon &pokemon, uint16_t pp, size_t index);
    void setMovePP(Pokemon &pokemon, const uint8_t movePP[4]);
    void getMovePPUps(Pokemon &pokemon, uint8_t movePPUps[4]) const;
    void setMovePPUps(Pokemon &pokemon, const uint8_t movePPUps[4]);
    uint32_t getIndividualValues(Pokemon &pokemon) const;
    void setIndividualValues(Pokemon &pokemon, uint32_t individualValues);
    uint32_t getHoennRibbonSet(Pokemon &pokemon) const;
    void setHoennRibbonSet(Pokemon &pokemon, uint32_t hoennRibbonSet);
    uint8_t getFlags(Pokemon &pokemon) const;
    void setFlags(Pokemon &pokemon, uint8_t flags);
    uint8_t getShinyLeaves(Pokemon &pokemon) const;
    void setShinyLeaves(Pokemon &pokemon, uint8_t shinyLeaves);
    uint16_t getPlatinumEggLocation(Pokemon &pokemon) const;
    void setPlatinumEggLocation(Pokemon &pokemon, uint16_t platinumEggLocation);
    uint16_t getPlatinumMetAtLocation(Pokemon &pokemon) const;
    void setPlatinumMetAtLocation(Pokemon &pokemon, uint16_t platinumMetAtLocation);

    // Block C
    void getNickname(Pokemon &pokemon, uint16_t nickname[11]) const;
    void setNickname(Pokemon &pokemon, const uint16_t nickname[11]);
    uint8_t getUnused(Pokemon &pokemon) const;
    void setUnused(Pokemon &pokemon, uint8_t unused);
    uint8_t getGameOfOrigin(Pokemon &pokemon) const;
    void setGameOfOrigin(Pokemon &pokemon, uint8_t gameOfOrigin);
    uint32_t getSinnohRibbonsSet2(Pokemon &pokemon) const;
    void setSinnohRibbonsSet2(Pokemon &pokemon, uint32_t sinnohRibbonsSet2);
    uint32_t getUnused2(Pokemon &pokemon) const;
    void setUnused2(Pokemon &pokemon, uint32_t unused2);

    // Block D
    void getOTName(Pokemon &pokemon, uint16_t otName[8]) const;
    void setOTName(Pokemon &pokemon, const uint16_t otName[8]);
    date getDateEggReceived(Pokemon &pokemon) const;
    void setDateEggReceived(Pokemon &pokemon, date dateEggReceived);
    date getMetAtDate(Pokemon &pokemon) const;
    void setMetAtDate(Pokemon &pokemon, date metAtDate);
    void setMetAtDate(Pokemon &pokemon, int day, int month, int year);
    uint16_t getDiamondPearlEggLocation(Pokemon &pokemon) const;
    void setDiamondPearlEggLocation(Pokemon &pokemon, uint16_t diamondPearlEggLocation);
    uint16_t getDiamondPearlMetAtLocation(Pokemon &pokemon) const;
    void setDiamondPearlMetAtLocation(Pokemon &pokemon, uint16_t diamondPearlMetAtLocation);
    uint8_t getPokerus(Pokemon &pokemon) const;
    void setPokerus(Pokemon &pokemon, uint8_t pokerus);
    uint8_t getPokeBall(Pokemon &pokemon) const;
    void setPokeBall(Pokemon &pokemon, uint8_t pokeBall);
    uint8_t getMetAtLevel(Pokemon &pokemon) const;
    void setMetAtLevel(Pokemon &pokemon, uint8_t metAtLevel);
    uint8_t getGender(Pokemon &pokemon) const;
    void setGender(Pokemon &pokemon, uint8_t gender);
    uint8_t getEncounterType(Pokemon &pokemon) const;
    void setEncounterType(Pokemon &pokemon, uint8_t encounterType);
    uint8_t getHGSSPokeBall(Pokemon &pokemon) const;
    void setHGSSPokeBall(Pokemon &pokemon, uint8_t hgssPokeBall);
    uint8_t getPerformance(Pokemon &pokemon) const;
    void setPerformance(Pokemon &pokemon, uint8_t performance);
};

#endif // ENCRYPTOMON_H