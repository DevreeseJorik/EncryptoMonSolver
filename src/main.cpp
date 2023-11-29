#include "EncryptoMon.h"
#include <iostream>

int main() {
    EncryptoMon encryptoMon;
    Pokemon test;

    // Load Pokemon data from binary file
    if (encryptoMon.loadBinaryPokemon(R"(poke.bin)", test)) {
        encryptoMon.processPokemon(test);
        // Save encrypted Pokemon data to a new file
        encryptoMon.dumpBinaryPokemon(R"(enc_poke.bin)", test);
    }

    return 1;
}