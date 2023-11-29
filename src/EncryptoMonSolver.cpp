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
    if (!config.tweakItems) { tweakEffortValues(pokemon, encryptoMon, config); return; }
    for (int item = 0; item <= MAX_ITEMS; item++) {
        encryptoMon.setItem(pokemon, item);
        tweakEffortValues(pokemon, encryptoMon, config);
    }
}; 

void EncryptoMonSolver::tweakEffortValues(Pokemon& pokemon, EncryptoMon& encryptoMon, TweakConfig& config) {
    if (!config.tweakEffortValues) { tweakMoves(pokemon, encryptoMon, config); return; }
    uint8_t* Evs = encryptoMon.getEffortValuePointer(pokemon);
    if (config.resetEffortValues) {
        for (int i = 0; i < 6; i++) {
            Evs[i] = 0;
        }
    }
    generateEffortValueCombination(pokemon, encryptoMon, config, Evs, 0);
}

// Recursive function, recursivity = EV count (6)
void EncryptoMonSolver::generateEffortValueCombination(Pokemon& pokemon, EncryptoMon& encryptoMon, TweakConfig& config, uint8_t* Evs, uint8_t evId) {
    if (evId > 5) {
        return;
    }

    uint16_t sum = 0;
    for (int i = 0; i < 6; i++) {
        sum += Evs[i];
    }

    if (sum > MAX_EVS) {
        return;
    }

    tweakMoves(pokemon, encryptoMon, config);

    for (int ev = 0; ev < 0xFF; ev++) {
        Evs[evId] = ev;
        generateEffortValueCombination(pokemon, encryptoMon, config, Evs, evId + 1);
    }   

    Evs[evId] = 0;
}

void EncryptoMonSolver::tweakMoves(Pokemon& pokemon, EncryptoMon& encryptoMon, TweakConfig& config) {
    if (!config.tweakMoves) { finishTweak(pokemon, encryptoMon, config); return; }
    finishTweak(pokemon, encryptoMon, config);
}

// Pass by value, as we want to shuffle blocks and encrypt the data
void EncryptoMonSolver::finishTweak(Pokemon pokemon, EncryptoMon& encryptoMon, TweakConfig& config) {
    Pokemon postEncryptPokemon = pokemon;
    encryptoMon.processPokemon(postEncryptPokemon);

    checkSequences(pokemon, postEncryptPokemon, encryptoMon, config);
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

void EncryptoMonSolver::checkSequences(Pokemon& preEncryptPokemon, Pokemon& postEncryptPokemon, EncryptoMon& encryptoMon, TweakConfig& config) {
    bool seq_flag;

    uint8_t reg_jump_commands[] = { 0x16, 0x1A };
    uint8_t seq[6];

    for (const uint8_t& cmd : reg_jump_commands) {
        seq[0] = cmd;

        for (uint32_t value = 0xFFFFBEEE; value < 0xFFFFCEEE; value += 2) {
            for (size_t i = 0; i < 4; ++i) {
                seq[2 + i] = (value >> (i * 8)) & 0xFF;
            }

            seq_flag = checkSequence(postEncryptPokemon, seq, sizeof(seq) / sizeof(seq[0]), 0x10);

            if (seq_flag) {
                std::cout << "Found Sequence!" << std::endl;
                encryptoMon.dumpBinaryPokemon("poke_output/poke_" + std::to_string(preEncryptPokemon.checksum) + ".bin", preEncryptPokemon);
                encryptoMon.dumpBinaryPokemon("poke_output/poke_" + std::to_string(postEncryptPokemon.checksum) + "enc.bin", postEncryptPokemon);
            }

        }
    }

}