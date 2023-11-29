#ifndef ENCRYPTOMON_H
#define ENCRYPTOMON_H

#include "structs.h"
#include <array>

class EncryptoMon {
public:
    bool loadBinaryPokemon(const std::string& fileName, Pokemon& pokemon);

    bool dumpBinaryPokemon(const std::string& fileName, const Pokemon& pokemon);

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

    bool checkSequence(Pokemon& pokemon, uint8_t* seq, uint8_t seq_len, uint8_t offs);
};


#endif // ENCRYPTOMON_H