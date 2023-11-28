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
    ShuffleBlocks(pokemon);
}

// Function to encrypt Pokémon data
void EncryptoMon::EncryptPokemon(Pokemon& pokemon) {
    // Implement your encryption logic here
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
void EncryptoMon::DecryptPokemon(Pokemon& pokemon) {
    // Implement your decryption logic here
    // You can modify the pokemon.blocks array or other fields as needed
    EncryptPokemon(pokemon);
}

uint8_t* EncryptoMon::GetBlockIds(Pokemon& pokemon) {
    uint8_t order = GetBlockOrder(pokemon);
    uint8_t* blockIds = new uint8_t[4] {
        blockAPositions[order],
        blockBPositions[order],
        blockCPositions[order],
        blockDPositions[order]
    };
    return blockIds;
}

// Function to get Block Order
uint8_t EncryptoMon::GetBlockOrder(Pokemon& pokemon) {
    return ((pokemon.pid & 0x3E000) >> 13) % 24;
}

// Function to shuffle the order of blocks
void EncryptoMon::ShuffleBlocks(Pokemon& pokemon) {
    uint8_t* blockIds = GetBlockIds(pokemon);

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
void EncryptoMon::UnshuffleBlocks(Pokemon& pokemon) {
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
