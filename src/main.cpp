#include "EncryptoMonSolver.h"
#include <iostream>

int main() {
    TweakConfig tConfig;
    Config config;
    EncryptoMonSolver solver;
    EncryptoMon encryptoMon;
    Pokemon pokemon;

    config.loadTweakEnvFile(R"(.env)", tConfig);

    // Load Pokemon data from binary file
    //if (encryptoMon.loadBinaryPokemon(R"(poke.bin)", pokemon)) {
    //    encryptoMon.setItem(pokemon, 0x1E);
    //    encryptoMon.processPokemon(pokemon);
    //    // Save encrypted Pokemon data to a new file
    //    encryptoMon.dumpBinaryPokemon(R"(enc_poke.bin)", test);
    //}

    if (encryptoMon.loadBinaryPokemon(R"(poke.bin)", pokemon)) {
        solver.startSearch(pokemon, encryptoMon, tConfig);

    }

    return 1;
}