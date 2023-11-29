#ifndef CONFIG_H
#define CONFIG_H

#include <sstream>
#include <string>

struct TweakConfig {
    bool tweakItems;
    bool tweakMoves;
    bool tweakEffortValues;
    bool resetEffortValues;
};

class Config {
public:
    void loadTweakEnvFile(const std::string& filePath, TweakConfig& config);
};

#endif // CONFIG_H