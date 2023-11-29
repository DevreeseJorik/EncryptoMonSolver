#include <fstream>
#include <iostream>
#include "EncryptoMon.h"

// Function to load binary data into a Pokemon struct
bool EncryptoMon::loadBinaryPokemon(const std::string& fileName, Pokemon& pokemon) {
    // Open the binary file in input mode
    std::ifstream file(fileName, std::ios::binary);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return false;
    }

    // Read data into the Pokemon struct
    file.read(reinterpret_cast<char*>(&pokemon), sizeof(Pokemon));

    // Close the file
    file.close();

    return true;
}

// Function to dump a Pokemon struct into a binary file
bool EncryptoMon::dumpBinaryPokemon(const std::string& fileName, const Pokemon& pokemon) {
    // Open the binary file in output mode
    std::ofstream file(fileName, std::ios::binary);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return false;
    }

    // Write data from the Pokemon struct to the file
    file.write(reinterpret_cast<const char*>(&pokemon), sizeof(Pokemon));

    // Check if the write operation was successful
    if (!file) {
        std::cerr << "Error writing data to file: " << fileName << std::endl;
        return false;
    }

    // Close the file
    file.close();

    return true;
}

// Function to prepare for encryption, by calculating checksum, shuffling blocks
void EncryptoMon::preparePokemon(Pokemon& pokemon) {
    calculateChecksum(pokemon);
    shuffleBlocks(pokemon);
}

// Function to process the entire input pokemon to encrypted state
void EncryptoMon::processPokemon(Pokemon& pokemon) {
    preparePokemon(pokemon);
    encryptPokemon(pokemon);
}

// Function to encrypt Pokémon data
void EncryptoMon::encryptPokemon(Pokemon& pokemon) {
    // You can modify the pokemon.blocks array or other fields as needed
        /* LCRNG is seeded with the Checksum */
    /* Advance the LCRNG, XOR its 16 most significant bits with each 16-bit word of ABCD Block data */
    uint32_t state = pokemon.checksum;
    uint16_t* data = (uint16_t*)pokemon.block_data;
    for (uint32_t i = 0; i < 4 * 16; i++)
    {
        state = state * 0x41C64E6D + 0x00006073; // advance LCRNG
        data[i] ^= state >> 16; // encrypt with top half of state
    }
}

// Function to decrypt Pokémon data
void EncryptoMon::decryptPokemon(Pokemon& pokemon) {
    encryptPokemon(pokemon);
}

uint8_t* EncryptoMon::getBlockIds(Pokemon& pokemon) {
    uint8_t order = getBlockOrder(pokemon);
    uint8_t* blockIds = new uint8_t[4] {
        blockAPositions[order],
        blockBPositions[order],
        blockCPositions[order],
        blockDPositions[order]
    };
    return blockIds;
}

BlockA* EncryptoMon::getBlockAPreShuffle(Pokemon& pokemon) {
    return &pokemon.block_data[0];
}

BlockB* EncryptoMon::getBlockBPreShuffle(Pokemon& pokemon) {
    return &pokemon.block_data[1];
}

BlockC* EncryptoMon::getBlockCPreShuffle(Pokemon& pokemon) {
    return &pokemon.block_data[2];
}

BlockD* EncryptoMon::getBlockDPreShuffle(Pokemon& pokemon) {
    return &pokemon.block_data[3];
}

BlockA* EncryptoMon::getBlockAAfterShuffle(Pokemon& pokemon) {
    uint8_t order = getBlockOrder(pokemon);
    return &pokemon.block_data[blockAPositions[order]];
}

BlockB* EncryptoMon::getBlockBAfterShuffle(Pokemon& pokemon) {
    uint8_t order = getBlockOrder(pokemon);
    return &pokemon.block_data[blockBPositions[order]];
}

BlockC* EncryptoMon::getBlockCAfterShuffle(Pokemon& pokemon) {
    uint8_t order = getBlockOrder(pokemon);
    return &pokemon.block_data[blockCPositions[order]];
}

BlockD* EncryptoMon::getBlockDAfterShuffle(Pokemon& pokemon) {
    uint8_t order = getBlockOrder(pokemon);
    return &pokemon.block_data[blockDPositions[order]];
}

// Function to get Block Order
uint8_t EncryptoMon::getBlockOrder(Pokemon& pokemon) {
    return ((pokemon.pid & 0x3E000) >> 13) % 24;
}

// Function to shuffle the order of blocks
void EncryptoMon::shuffleBlocks(Pokemon& pokemon) {
    uint8_t* blockIds = getBlockIds(pokemon);

    // Rearrange the block_data array based on the new order
    Block temp[4];
    for (int i = 0; i < 4; ++i) {
        temp[i] = pokemon.block_data[i];
    }

    // Copy the rearranged blocks back to the original array
    for (int i = 0; i < 4; ++i) {
        pokemon.block_data[i] = temp[blockIds[i]];
    }

}

// Function to unshuffle the order of blocks
void EncryptoMon::unshuffleBlocks(Pokemon& pokemon) {
    // Implement logic to revert the order of blocks to the original state
    // You may need to store the original order before shuffling
}

void EncryptoMon::calculateChecksum(Pokemon& pokemon) {
    /* Sets the checksum of a pokemon by summing all of its Block data */
    uint16_t* data = (uint16_t*)&pokemon.block_data;
    uint16_t checksum = 0;
    for (uint32_t i = 0; i < 4 * 16; i++)
    {
        checksum += data[i];
    }
    pokemon.checksum = checksum;
}

bool EncryptoMon::checkSequence(Pokemon& pokemon, uint8_t* seq, uint8_t seq_len, uint8_t offs) {
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