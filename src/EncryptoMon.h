#ifndef ENCRYPTOMON_H
#define ENCRYPTOMON_H

#include "structs.h"
#include <array>

class EncryptoMon {
public:
    bool loadBinaryPokemon(const std::string& filePath, Pokemon& pokemon);

    bool dumpBinaryPokemon(const std::string& filePath, const Pokemon& pokemon);

    void preparePokemon(Pokemon& pokemon);

    void processPokemon(Pokemon& pokemon);

    void encryptPokemon(Pokemon& pokemon);

    void decryptPokemon(Pokemon& pokemon);

    uint8_t* getBlockIds(Pokemon& pokemon);

    BlockA* getBlockAPreShuffle(Pokemon& pokemon);

    BlockB* getBlockBPreShuffle(Pokemon& pokemon);

    BlockC* getBlockCPreShuffle(Pokemon& pokemon);

    BlockD* getBlockDPreShuffle(Pokemon& pokemon);

    BlockA* getBlockAAfterShuffle(Pokemon& pokemon);

    BlockB* getBlockBAfterShuffle(Pokemon& pokemon);

    BlockC* getBlockCAfterShuffle(Pokemon& pokemon);

    BlockD* getBlockDAfterShuffle(Pokemon& pokemon);

    uint8_t getBlockOrder(Pokemon& pokemon);

    void shuffleBlocks(Pokemon& pokemon);

    void unshuffleBlocks(Pokemon& pokemon);

    void calculateChecksum(Pokemon& pokemon);

    void setItem(Pokemon& pokemon, uint16_t item);

    uint8_t* getEffortValuePointer(Pokemon& pokemon);
};


#endif // ENCRYPTOMON_H