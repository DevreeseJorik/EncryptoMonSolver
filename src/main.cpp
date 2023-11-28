#include "EncryptoMon.h"

int main() {
    EncryptoMon encryptoMon;
    Pokemon test;

    // Load Pokemon data from binary file
    if (encryptoMon.loadBinaryPokemon(R"(poke.bin)", test)) {
        encryptoMon.preparePokemon(test);
        //encryptoMon.calculateChecksum(test);
        // Encrypt Pokemon data
        encryptoMon.EncryptPokemon(test);

        // Save encrypted Pokemon data to a new file
        encryptoMon.dumpBinaryPokemon(R"(enc_poke.bin)", test);
    }

    return 1;
}