#ifndef ENCRYPTOMONSOLVER_H
#define ENCRYPTOMONSOLVER_H

#include "EncryptoMon.h"
#include "config.h"

class EncryptoMonSolver {
public:
    // Start the search process
    void startSearch(Pokemon& pokemon, EncryptoMon& encryptoMon, TweakConfig& config);

private:
    // Tweak the entire Pokemon
    void tweakPokemon(Pokemon& pokemon, EncryptoMon& encryptoMon, TweakConfig& config);

    // Tweak the items of the Pokemon
    void tweakItems(Pokemon& pokemon, EncryptoMon& encryptoMon, TweakConfig& config);

    // Tweak the moves of the Pokemon
    void tweakMoves(Pokemon& pokemon, EncryptoMon& encryptoMon, TweakConfig& config);

    // Pass by value, as we'll want to shuffle and encrypt the copy
    void finishTweak(Pokemon pokemon, EncryptoMon& encryptoMon, TweakConfig& config);

    // Check if a specific sequence is present at a given offset in the Pokemon's data
    bool checkSequence(Pokemon& pokemon, uint8_t* seq, uint8_t seq_len, uint8_t offs);
};

#endif // ENCRYPTOMONSOLVER_H