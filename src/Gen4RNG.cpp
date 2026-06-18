#include "Gen4RNG.hpp"

#include <algorithm>
#include <cstring>
#include <mutex>
#include <thread>

uint8_t Gen4Generator::calcGender(uint32_t pid, uint8_t genderRatio, bool fixedGender) {
    if (genderRatio == 255)
        return 2;
    if (genderRatio == 254)
        return 1;
    if (genderRatio == 0)
        return 0;
    if (fixedGender)
        return genderRatio >= 254 ? 1 : 0;
    return (pid & 0xFF) < genderRatio ? 1 : 0;
}

// Grass encounter slot thresholds — same for Method J and K (only RNG generation differs).
uint8_t Gen4Generator::grassSlot(uint16_t value) {
    constexpr uint8_t thresholds[] = {20, 40, 50, 60, 70, 80, 85, 90, 94, 98, 99, 100};
    for (int i = 0; i < 12; ++i)
        if (value < thresholds[i])
            return static_cast<uint8_t>(i);
    return 11;
}

static inline std::array<uint8_t, 6> unpackIVs(uint16_t iv1, uint16_t iv2) {
    // HP, Atk, Def, SpA, SpD, Spe
    return {
        static_cast<uint8_t>(iv1 & 31),         static_cast<uint8_t>((iv1 >> 5) & 31),
        static_cast<uint8_t>((iv1 >> 10) & 31), static_cast<uint8_t>((iv2 >> 5) & 31),
        static_cast<uint8_t>((iv2 >> 10) & 31), static_cast<uint8_t>(iv2 & 31),
    };
}

std::vector<Gen4RNGResult> Gen4Generator::generate(uint32_t seed, uint32_t initialAdvances, uint32_t maxAdvances,
                                                   const Gen4Profile &profile, const Gen4StaticParams &params) const {
    switch (params.method) {
    case Gen4Method::Method1:
        return generateMethod1(seed, initialAdvances, maxAdvances, profile, params);
    case Gen4Method::MethodJ:
        return generateMethodJ_static(seed, initialAdvances, maxAdvances, profile, params);
    case Gen4Method::MethodK:
        return generateMethodK_static(seed, initialAdvances, maxAdvances, profile, params);
    }
    return {};
}

std::vector<Gen4RNGResult> Gen4Generator::generateWild(uint32_t seed, uint32_t initialAdvances, uint32_t maxAdvances,
                                                       const Gen4Profile &profile, const Gen4WildParams &params) const {
    switch (params.method) {
    case Gen4Method::MethodJ:
        return generateMethodJ_wild(seed, initialAdvances, maxAdvances, profile, params);
    case Gen4Method::MethodK:
        return generateMethodK_wild(seed, initialAdvances, maxAdvances, profile, params);
    default:
        return {};
    }
}

std::vector<Gen4RNGResult> Gen4Generator::generateMethod1(uint32_t seed, uint32_t initialAdvances, uint32_t maxAdvances,
                                                          const Gen4Profile &profile,
                                                          const Gen4StaticParams &params) const {
    std::vector<Gen4RNGResult> results;

    LCRNG4 rng(seed);
    rng.advance(initialAdvances);

    for (uint32_t cnt = 0; cnt <= maxAdvances; ++cnt) {
        LCRNG4 go = rng;

        uint16_t pidLow = go.nextUShort();
        uint16_t pidHigh = go.nextUShort();
        uint32_t pid = (static_cast<uint32_t>(pidHigh) << 16) | pidLow;
        uint16_t iv1 = go.nextUShort();
        uint16_t iv2 = go.nextUShort();

        auto ivs = unpackIVs(iv1, iv2);
        uint8_t nature = pid % 25;
        uint8_t ability = pid & 1;
        uint8_t gender = calcGender(pid, params.genderRatio, params.fixedGender);

        results.push_back({seed, initialAdvances + cnt, pid, iv1, iv2, ivs, nature, ability, gender, params.level, 255,
                           params.species});
        rng.next();
    }
    return results;
}

std::vector<Gen4RNGResult> Gen4Generator::generateMethodJ_static(uint32_t seed, uint32_t initialAdvances,
                                                                 uint32_t maxAdvances, const Gen4Profile &profile,
                                                                 const Gen4StaticParams &params) const {
    std::vector<Gen4RNGResult> results;

    bool cuteCharm =
        (params.lead == Gen4Lead::CuteCharmF || params.lead == Gen4Lead::CuteCharmM) && !params.fixedGender;
    uint8_t buffer = 0;
    if (params.lead == Gen4Lead::CuteCharmF)
        buffer = 25 * ((params.genderRatio / 25) + 1);

    LCRNG4 rng(seed);
    rng.advance(initialAdvances);

    for (uint32_t cnt = 0; cnt <= maxAdvances; ++cnt) {
        LCRNG4 go = rng;

        bool cuteCharmFlag = false;
        if (cuteCharm)
            cuteCharmFlag = go.nextUShortJ(3) != 0;

        uint8_t nature;
        if (params.lead == Gen4Lead::Synchronize) {
            nature = go.nextUShortJ(2) == 0 ? params.syncNature : go.nextUShortJ(25);
        } else {
            nature = go.nextUShortJ(25);
        }

        uint32_t pid;
        if (cuteCharmFlag) {
            pid = static_cast<uint32_t>(buffer + nature);
        } else {
            do {
                uint16_t low = go.nextUShort();
                uint16_t high = go.nextUShort();
                pid = (static_cast<uint32_t>(high) << 16) | low;
            } while (pid % 25 != nature);
        }

        uint16_t iv1 = go.nextUShort();
        uint16_t iv2 = go.nextUShort();

        auto ivs = unpackIVs(iv1, iv2);
        uint8_t ability = pid & 1;
        uint8_t gender = calcGender(pid, params.genderRatio, params.fixedGender);

        results.push_back({seed, initialAdvances + cnt, pid, iv1, iv2, ivs, nature, ability, gender, params.level, 255,
                           params.species});
        rng.next();
    }
    return results;
}

std::vector<Gen4RNGResult> Gen4Generator::generateMethodK_static(uint32_t seed, uint32_t initialAdvances,
                                                                 uint32_t maxAdvances, const Gen4Profile &profile,
                                                                 const Gen4StaticParams &params) const {
    std::vector<Gen4RNGResult> results;

    bool cuteCharm =
        (params.lead == Gen4Lead::CuteCharmF || params.lead == Gen4Lead::CuteCharmM) && !params.fixedGender;
    uint8_t buffer = 0;
    if (params.lead == Gen4Lead::CuteCharmF)
        buffer = 25 * ((params.genderRatio / 25) + 1);

    LCRNG4 rng(seed);
    rng.advance(initialAdvances);

    for (uint32_t cnt = 0; cnt <= maxAdvances; ++cnt) {
        LCRNG4 go = rng;

        bool cuteCharmFlag = false;
        if (cuteCharm)
            cuteCharmFlag = go.nextUShortK(3) != 0;

        uint8_t nature;
        if (params.lead == Gen4Lead::Synchronize) {
            nature = go.nextUShortK(2) == 0 ? params.syncNature : go.nextUShortK(25);
        } else {
            nature = go.nextUShortK(25);
        }

        uint32_t pid;
        if (cuteCharmFlag) {
            pid = static_cast<uint32_t>(buffer + nature);
        } else {
            do {
                uint16_t low = go.nextUShort();
                uint16_t high = go.nextUShort();
                pid = (static_cast<uint32_t>(high) << 16) | low;
            } while (pid % 25 != nature);
        }

        uint16_t iv1 = go.nextUShort();
        uint16_t iv2 = go.nextUShort();

        auto ivs = unpackIVs(iv1, iv2);
        uint8_t ability = pid & 1;
        uint8_t gender = calcGender(pid, params.genderRatio, params.fixedGender);

        results.push_back({seed, initialAdvances + cnt, pid, iv1, iv2, ivs, nature, ability, gender, params.level, 255,
                           params.species});
        rng.next();
    }
    return results;
}

std::vector<Gen4RNGResult> Gen4Generator::generateMethodJ_wild(uint32_t seed, uint32_t initialAdvances,
                                                               uint32_t maxAdvances, const Gen4Profile &profile,
                                                               const Gen4WildParams &params) const {
    std::vector<Gen4RNGResult> results;

    LCRNG4 rng(seed);
    rng.advance(initialAdvances);

    for (uint32_t cnt = 0; cnt <= maxAdvances; ++cnt) {
        LCRNG4 go = rng;

        // Encounter slot (grass uses division-based RNG)
        uint8_t slot = grassSlot(go.nextUShortJ(100));
        if (params.desiredSlot != 255 && slot != params.desiredSlot) {
            rng.next();
            continue;
        }

        const WildEncounterSlot &s = params.slots[slot];
        if (params.desiredSpecies != 0 && s.species != params.desiredSpecies) {
            rng.next();
            continue;
        }

        uint8_t level;
        if (params.lead == Gen4Lead::Pressure) {
            level = s.maxLevel;
        } else if (s.maxLevel > s.minLevel) {
            level = s.minLevel + go.nextUShortJ(s.maxLevel - s.minLevel + 1);
        } else {
            level = s.minLevel;
        }

        bool cuteCharm = (params.lead == Gen4Lead::CuteCharmF || params.lead == Gen4Lead::CuteCharmM) && !s.fixedGender;
        uint8_t buffer = 0;
        if (params.lead == Gen4Lead::CuteCharmF)
            buffer = 25 * ((s.genderRatio / 25) + 1);

        bool cuteCharmFlag = false;
        if (cuteCharm)
            cuteCharmFlag = go.nextUShortJ(3) != 0;

        uint8_t nature;
        if (params.lead == Gen4Lead::Synchronize) {
            nature = go.nextUShortJ(2) == 0 ? params.syncNature : go.nextUShortJ(25);
        } else {
            nature = go.nextUShortJ(25);
        }

        uint32_t pid;
        if (cuteCharmFlag) {
            pid = static_cast<uint32_t>(buffer + nature);
        } else {
            do {
                uint16_t low = go.nextUShort();
                uint16_t high = go.nextUShort();
                pid = (static_cast<uint32_t>(high) << 16) | low;
            } while (pid % 25 != nature);
        }

        uint16_t iv1 = go.nextUShort();
        uint16_t iv2 = go.nextUShort();

        auto ivs = unpackIVs(iv1, iv2);
        uint8_t ability = pid & 1;
        uint8_t gender = calcGender(pid, s.genderRatio, s.fixedGender);

        results.push_back(
            {seed, initialAdvances + cnt, pid, iv1, iv2, ivs, nature, ability, gender, level, slot, s.species});
        rng.next();
    }
    return results;
}

std::vector<Gen4RNGResult> Gen4Generator::generateMethodK_wild(uint32_t seed, uint32_t initialAdvances,
                                                               uint32_t maxAdvances, const Gen4Profile &profile,
                                                               const Gen4WildParams &params) const {
    std::vector<Gen4RNGResult> results;

    LCRNG4 rng(seed);
    rng.advance(initialAdvances);

    for (uint32_t cnt = 0; cnt <= maxAdvances; ++cnt) {
        LCRNG4 go = rng;

        // Encounter slot (HGSS uses mod-based RNG)
        uint8_t slot = grassSlot(go.nextUShortK(100));
        if (params.desiredSlot != 255 && slot != params.desiredSlot) {
            rng.next();
            continue;
        }

        const WildEncounterSlot &s = params.slots[slot];
        if (params.desiredSpecies != 0 && s.species != params.desiredSpecies) {
            rng.next();
            continue;
        }

        uint8_t level;
        if (params.lead == Gen4Lead::Pressure) {
            level = s.maxLevel;
        } else if (s.maxLevel > s.minLevel) {
            level = s.minLevel + go.nextUShortK(s.maxLevel - s.minLevel + 1);
        } else {
            level = s.minLevel;
        }

        bool cuteCharm = (params.lead == Gen4Lead::CuteCharmF || params.lead == Gen4Lead::CuteCharmM) && !s.fixedGender;
        uint8_t buffer = 0;
        if (params.lead == Gen4Lead::CuteCharmF)
            buffer = 25 * ((s.genderRatio / 25) + 1);

        bool cuteCharmFlag = false;
        if (cuteCharm)
            cuteCharmFlag = go.nextUShortK(3) != 0;

        // HGSS BCC/Safari have extra nature loops — skip for basic grass
        uint8_t nature;
        if (params.lead == Gen4Lead::Synchronize) {
            nature = go.nextUShortK(2) == 0 ? params.syncNature : go.nextUShortK(25);
        } else {
            nature = go.nextUShortK(25);
        }

        uint32_t pid;
        if (cuteCharmFlag) {
            pid = static_cast<uint32_t>(buffer + nature);
        } else {
            do {
                uint16_t low = go.nextUShort();
                uint16_t high = go.nextUShort();
                pid = (static_cast<uint32_t>(high) << 16) | low;
            } while (pid % 25 != nature);
        }

        uint16_t iv1 = go.nextUShort();
        uint16_t iv2 = go.nextUShort();

        auto ivs = unpackIVs(iv1, iv2);
        uint8_t ability = pid & 1;
        uint8_t gender = calcGender(pid, s.genderRatio, s.fixedGender);

        results.push_back(
            {seed, initialAdvances + cnt, pid, iv1, iv2, ivs, nature, ability, gender, level, slot, s.species});
        rng.next();
    }
    return results;
}

Pokemon Gen4Generator::buildPokemon(const Gen4RNGResult &result, const Gen4Profile &profile,
                                    const Gen4BuildParams &buildParams) const {
    Pokemon pokemon;
    memset(&pokemon, 0, sizeof(Pokemon));
    pokemon.pid = result.pid;

    BlockA *a = reinterpret_cast<BlockA *>(&pokemon.block_data[0]);
    BlockB *b = reinterpret_cast<BlockB *>(&pokemon.block_data[1]);
    BlockC *c = reinterpret_cast<BlockC *>(&pokemon.block_data[2]);
    BlockD *d = reinterpret_cast<BlockD *>(&pokemon.block_data[3]);

    a->speciesID = result.species;
    a->heldItem = buildParams.heldItem;
    a->otID = profile.tid;
    a->otSecretID = profile.sid;
    a->experiencePoints = buildParams.experience;
    a->friendship = buildParams.friendship;
    a->ability = result.ability;
    a->markings = buildParams.markings;
    a->languageOfOrigin = buildParams.languageOfOrigin;
    a->hpEVs = buildParams.evs[0];
    a->attackEVs = buildParams.evs[1];
    a->defenseEVs = buildParams.evs[2];
    a->speedEVs = buildParams.evs[3];
    a->spAtkEVs = buildParams.evs[4];
    a->spDefEVs = buildParams.evs[5];

    b->moveset[0] = buildParams.moves[0];
    b->moveset[1] = buildParams.moves[1];
    b->moveset[2] = buildParams.moves[2];
    b->moveset[3] = buildParams.moves[3];
    b->movePP[0] = buildParams.movePP[0];
    b->movePP[1] = buildParams.movePP[1];
    b->movePP[2] = buildParams.movePP[2];
    b->movePP[3] = buildParams.movePP[3];
    {
        uint32_t ivPacked = static_cast<uint32_t>(result.rawIV1) | (static_cast<uint32_t>(result.rawIV2) << 15);
        if (buildParams.isNicknamed)
            ivPacked |= (1u << 31);
        b->individualValues = ivPacked;
    }
    if (!isHGSS(profile.game))
        b->platinumMetAtLocation = buildParams.metLocation;

    c->gameOfOrigin = static_cast<uint8_t>(profile.game);
    if (buildParams.isNicknamed) {
        for (int i = 0; i < 11; ++i)
            c->nickname[i] = buildParams.nickname[i];
    } else {
        c->nickname[0] = 0xFFFF;
    }

    for (int i = 0; i < 8; ++i)
        d->otName[i] = profile.otName[i];
    d->metAtDate = buildParams.metDate;
    d->diamondPearlMetAtLocation = buildParams.metLocation;
    d->pokeBall = buildParams.pokeBall;
    d->metAtLevel = result.level & 0x7F;
    d->gender = profile.otGender & 1;
    d->encounterType = buildParams.encounterType;
    if (isHGSS(profile.game))
        d->hgssPokeBall = buildParams.pokeBall;

    m_em.setChecksum(pokemon);
    m_em.shuffleBlocks(pokemon);
    m_em.encryptPokemon(pokemon);

    return pokemon;
}

void Gen4Generator::finalizeWithChecksum(Pokemon &pokemon, uint16_t target) const {
    uint16_t current = m_em.calculateChecksum(pokemon);
    uint16_t delta = static_cast<uint16_t>(target - current);

    BlockA *a = reinterpret_cast<BlockA *>(&pokemon.block_data[0]);
    a->heldItem = static_cast<uint16_t>(a->heldItem + delta);

    pokemon.checksum = target;
    m_em.shuffleBlocks(pokemon);
    m_em.encryptPokemon(pokemon);
}

void Gen4Generator::searchSeeds(uint32_t seedMin, uint32_t seedMax, uint32_t maxAdvances, const Gen4Profile &profile,
                                const Gen4StaticParams &params,
                                const std::function<void(const Gen4RNGResult &)> &callback, uint32_t numThreads) const {
    if (numThreads == 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0)
            numThreads = 1;
    }

    uint64_t range = static_cast<uint64_t>(seedMax) - seedMin + 1;
    uint64_t perThread = (range + numThreads - 1) / numThreads;

    std::mutex mutex;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    for (uint32_t t = 0; t < numThreads; ++t) {
        uint64_t tStart = static_cast<uint64_t>(seedMin) + t * perThread;
        uint64_t tEnd = std::min(tStart + perThread - 1, static_cast<uint64_t>(seedMax));
        if (tStart > seedMax)
            break;

        threads.emplace_back([this, tStart, tEnd, maxAdvances, &profile, &params, &callback, &mutex]() {
            for (uint64_t s = tStart; s <= tEnd; ++s) {
                auto results = generate(static_cast<uint32_t>(s), 0, maxAdvances, profile, params);
                if (!results.empty()) {
                    std::lock_guard<std::mutex> lock(mutex);
                    for (const auto &r : results)
                        callback(r);
                }
            }
        });
    }

    for (auto &t : threads)
        t.join();
}
