#ifndef ENCRYPTOMON_H
#define ENCRYPTOMON_H

#include "structs.h"
#include <array>

class EncryptoMon {
public:
    bool loadBinaryPokemon(const std::string& fileName, Pokemon& pokemon);

    bool dumpBinaryPokemon(const std::string& fileName, const Pokemon& pokemon);

    void preparePokemon(Pokemon& pokemon);

    void EncryptPokemon(Pokemon& pokemon);

    void DecryptPokemon(Pokemon& pokemon);

    uint8_t* GetBlockIds(Pokemon& pokemon);

    uint8_t GetBlockOrder(Pokemon& pokemon);

    void ShuffleBlocks(Pokemon& pokemon);

    void UnshuffleBlocks(Pokemon& pokemon);

    void calculateChecksum(Pokemon& pokemon);
};


#endif // ENCRYPTOMON_H