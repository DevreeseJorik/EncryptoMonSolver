#pragma once
#include "EncryptoMon.hpp"
#include "structs.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <vector>

struct LCRNG4 {
    uint32_t seed;
    explicit LCRNG4(uint32_t s) : seed(s) {}
    uint32_t next() { return seed = seed * 0x41C64E6D + 0x6073; }
    uint16_t nextUShort() { return static_cast<uint16_t>(next() >> 16); }
    // Method J style — division (no mod bias)
    uint16_t nextUShortJ(uint16_t max) { return nextUShort() / static_cast<uint16_t>(0xffffu / max + 1); }
    // Method K style
    uint16_t nextUShortK(uint16_t max) { return nextUShort() % max; }
    void advance(uint32_t n) { for (uint32_t i = 0; i < n; ++i) next(); }
};

enum class Gen4Game : uint8_t { Diamond = 10, Pearl = 11, Platinum = 12, HeartGold = 7, SoulSilver = 8 };
enum class Gen4Method : uint8_t { Method1, MethodJ, MethodK };
enum class Gen4Lead : uint8_t { None, Synchronize, CuteCharmF, CuteCharmM, Pressure, MagnetPull, Static, CompoundEyes };

inline bool isHGSS(Gen4Game g) { return g == Gen4Game::HeartGold || g == Gen4Game::SoulSilver; }

struct Gen4Profile {
    uint16_t tid = 0;
    uint16_t sid = 0;
    Gen4Game game = Gen4Game::Diamond;
    std::array<uint16_t, 8> otName = {0xFFFF}; // Gen 4 char ID encoding, 0xFFFF = terminator
    uint8_t otGender = 0;                       // 0=male, 1=female
};


struct Gen4RNGResult {
    uint32_t seed;
    uint32_t advances;
    uint32_t pid;
    uint16_t rawIV1; // hp|(atk<<5)|(def<<10)
    uint16_t rawIV2; // spe|(spa<<5)|(spd<<10)
    std::array<uint8_t, 6> ivs; // [HP, Atk, Def, SpA, SpD, Spe]
    uint8_t nature;
    uint8_t ability; // 0 or 1 (slot index)
    uint8_t gender;  // 0=male, 1=female, 2=genderless
    uint8_t level;
    uint8_t encounterSlot; // 255 for static encounters
    uint16_t species;      // 0 means "use params.species"
};

struct Gen4StaticParams {
    Gen4Method method = Gen4Method::Method1;
    Gen4Lead lead = Gen4Lead::None;
    uint8_t syncNature = 0;    // used when lead == Synchronize (0–24)
    uint16_t species = 0;
    uint8_t level = 5;
    uint8_t genderRatio = 0;   // 0=male-only, 254=female-only, 255=genderless, else female threshold
    bool fixedGender = false;
};

struct WildEncounterSlot {
    uint16_t species = 0;
    uint8_t minLevel = 1;
    uint8_t maxLevel = 1;
    uint8_t genderRatio = 0;
    bool fixedGender = false;
};

struct Gen4WildParams {
    Gen4Method method = Gen4Method::MethodJ;
    Gen4Lead lead = Gen4Lead::None;
    uint8_t syncNature = 0;
    std::array<WildEncounterSlot, 12> slots = {};
    uint8_t desiredSlot = 255;    // 255 = any
    uint16_t desiredSpecies = 0;  // 0 = any
};

struct Gen4BuildParams {
    uint16_t heldItem = 0;
    uint32_t experience = 0;
    std::array<uint16_t, 4> moves = {};
    std::array<uint8_t, 4> movePP = {};
    std::array<uint8_t, 6> evs = {};  // [HP, Atk, Def, Spe, SpA, SpD]
    std::array<uint16_t, 11> nickname = {0xFFFF}; // 0xFFFF at [0] = not nicknamed
    bool isNicknamed = false;
    uint8_t languageOfOrigin = 2; // 2 = English
    uint8_t encounterType = 0;    // 0=gift/static, 1=grass
    uint8_t pokeBall = 4;         // 4 = Poké Ball
    uint16_t metLocation = 0;
    date metDate = {6, 5, 1};     // year (offset from 2000), month, day
    uint8_t friendship = 70;
    uint8_t markings = 0;
};

class Gen4Generator {
public:
    explicit Gen4Generator(EncryptoMon &em) : m_em(em) {}

    // Generate from a fixed seed; initialAdvances frames are skipped before collecting.
    std::vector<Gen4RNGResult> generate(uint32_t seed, uint32_t initialAdvances, uint32_t maxAdvances,
                                        const Gen4Profile &profile, const Gen4StaticParams &params) const;

    std::vector<Gen4RNGResult> generateWild(uint32_t seed, uint32_t initialAdvances, uint32_t maxAdvances,
                                             const Gen4Profile &profile, const Gen4WildParams &params) const;

    // Build a fully-populated, encrypted Pokemon struct from an RNG result.
    Pokemon buildPokemon(const Gen4RNGResult &result, const Gen4Profile &profile,
                         const Gen4BuildParams &buildParams) const;

    // Adjust heldItem so that the pokemon's checksum == target.
    // pokemon must be in decrypted + unshuffled (canonical) form on entry,
    // and will be shuffled + encrypted on return.
    void finalizeWithChecksum(Pokemon &pokemon, uint16_t target) const;

    // Multithreaded exhaustive seed search over [seedMin, seedMax].
    // callback is called from worker threads — must be thread-safe.
    void searchSeeds(uint32_t seedMin, uint32_t seedMax, uint32_t maxAdvances, const Gen4Profile &profile,
                     const Gen4StaticParams &params, const std::function<void(const Gen4RNGResult &)> &callback,
                     uint32_t numThreads = 0) const;

private:
    EncryptoMon &m_em;

    static uint8_t calcGender(uint32_t pid, uint8_t genderRatio, bool fixedGender);
    static uint8_t grassSlot(uint16_t value); // same thresholds for J and K

    std::vector<Gen4RNGResult> generateMethod1(uint32_t seed, uint32_t initialAdvances, uint32_t maxAdvances,
                                                const Gen4Profile &profile, const Gen4StaticParams &params) const;
    std::vector<Gen4RNGResult> generateMethodJ_static(uint32_t seed, uint32_t initialAdvances, uint32_t maxAdvances,
                                                       const Gen4Profile &profile, const Gen4StaticParams &params) const;
    std::vector<Gen4RNGResult> generateMethodK_static(uint32_t seed, uint32_t initialAdvances, uint32_t maxAdvances,
                                                       const Gen4Profile &profile, const Gen4StaticParams &params) const;
    std::vector<Gen4RNGResult> generateMethodJ_wild(uint32_t seed, uint32_t initialAdvances, uint32_t maxAdvances,
                                                     const Gen4Profile &profile, const Gen4WildParams &params) const;
    std::vector<Gen4RNGResult> generateMethodK_wild(uint32_t seed, uint32_t initialAdvances, uint32_t maxAdvances,
                                                     const Gen4Profile &profile, const Gen4WildParams &params) const;
};
