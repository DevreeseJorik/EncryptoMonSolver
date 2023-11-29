#include "EncryptoMonSolver.h"
#include <iostream>
#include <functional>

void EncryptoMonSolver::startSearch(Pokemon& pokemon, EncryptoMon& encryptoMon, TweakConfig& config) {
    tweakPokemon(pokemon, encryptoMon, config);    
}

void EncryptoMonSolver::tweakPokemon(Pokemon& pokemon, EncryptoMon& encryptoMon, TweakConfig& config) {
    tweakItems(pokemon, encryptoMon, config);
}

void EncryptoMonSolver::tweakItems(Pokemon& pokemon, EncryptoMon& encryptoMon, TweakConfig& config) {
    if (!config.tweakItems) { tweakMoves(pokemon, encryptoMon, config); return; }
    for (int item = 0; item <= MAX_ITEMS; item++) {
        std::cout << "Searching item: " + std::to_string(item) << std::endl;
        encryptoMon.setItem(pokemon, item);
        tweakMoves(pokemon, encryptoMon, config);
    }
}; 

void EncryptoMonSolver::tweakMoves(Pokemon& pokemon, EncryptoMon& encryptoMon, TweakConfig& config) {
    if (!config.tweakMoves) { finishTweak(pokemon, encryptoMon, config); return; }
    finishTweak(pokemon, encryptoMon, config);
}

// Pass by value, as we want to shuffle blocks and encrypt the data
void EncryptoMonSolver::finishTweak(Pokemon pokemon, EncryptoMon& encryptoMon, TweakConfig& config) {
    Pokemon preEncryptPokemon = pokemon;
    encryptoMon.processPokemon(pokemon);

    uint8_t seq[] = { 0x5A };
    bool seq_flag = checkSequence(pokemon, seq, sizeof(seq) / sizeof(seq[0]), 0x10);

    if (seq_flag) {
        std::cout << "Found Sequence!" << std::endl;
        encryptoMon.dumpBinaryPokemon("poke_output/poke_" + std::to_string(pokemon.checksum) + ".bin", preEncryptPokemon);
        encryptoMon.dumpBinaryPokemon("poke_output/poke_" + std::to_string(pokemon.checksum) + "enc.bin", pokemon);
    }
    else {
        std::cout << "Sequence not found" << std::endl;
    }
}

bool EncryptoMonSolver::checkSequence(Pokemon& pokemon, uint8_t* seq, uint8_t seq_len, uint8_t offs) {
    // Treat the entire Pokemon structure as a uint8_t array
    uint8_t* pokemonBytes = reinterpret_cast<uint8_t*>(&pokemon);

    // Check if the sequence at the specified offset matches the given sequence
    for (uint8_t i = 0; i < seq_len; ++i) {
        if (pokemonBytes[offs + i] != seq[i]) {
            // Sequence mismatch
            return false;
        }
    }

    // Sequence matches
    return true;
}