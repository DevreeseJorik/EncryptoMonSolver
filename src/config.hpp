#ifndef CONFIG_H
#define CONFIG_H

#include <sstream>
#include <string>

struct TweakConfig {
    bool tweakItems;
    bool tweakMoves;
};

class Config {
public:
    void loadTweakEnvFile(const std::string& filePath, TweakConfig& config);
};

#endif // CONFIG_H