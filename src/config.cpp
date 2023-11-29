#include "config.h"
#include <fstream>
#include <iostream>

void Config::loadTweakEnvFile(const std::string& filePath, TweakConfig& config) {
    std::ifstream file(filePath);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, value;

        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            if (key == "TWEAK_ITEMS") {
                config.tweakItems = (value == "TRUE");
            }
            else if (key == "TWEAK_MOVES") {
                config.tweakMoves = (value == "TRUE");
            }
            // Add more keys if needed
        }
    }
}