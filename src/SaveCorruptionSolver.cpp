#include "SaveCorruptionSolver.hpp"
#include "Gen4RNG.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

static_assert(sizeof(Pokemon) == SaveCorruptionSolver::POKEMON_BYTES, "Pokemon size mismatch");
static_assert(offsetof(Pokemon, block_data) == SaveCorruptionSolver::HEADER_BYTES, "Pokemon header size mismatch");
static_assert(sizeof(Block) == SaveCorruptionSolver::BLOCK_BYTES, "Block size mismatch");
static_assert(sizeof(BoxData::currentBoxID) == SaveCorruptionSolver::BOX_HEADER_BYTES, "BoxData header size mismatch");

SaveCorruptionSolver::SaveCorruptionSolver(EncryptoMon &em, Config cfg) : m_em(em), m_cfg(cfg) {
    m_em.generateXORMasks();

    Pokemon base = m_cfg.basePokemon;
    m_em.setChecksum(base);
    m_em.shuffleBlocks(base);
    m_em.encryptPokemon(base);
    m_baseEncrypted = base;

    uint16_t targetMask[TOTAL_BLOCK_WORDS];
    m_em.generateXORMask(m_cfg.targetChecksum, targetMask, TOTAL_BLOCK_WORDS);
    const uint16_t *baseData = reinterpret_cast<const uint16_t *>(m_baseEncrypted.block_data);
    m_baseDecryptedSum[0] = 0;
    for (int i = 0; i < TOTAL_BLOCK_WORDS; ++i)
        m_baseDecryptedSum[i + 1] = m_baseDecryptedSum[i] + (uint32_t)(baseData[i] ^ targetMask[i]);
}

uint16_t SaveCorruptionSolver::crc16CCITT(const uint8_t *data, size_t length) {
    uint8_t top = 0xFF;
    uint8_t bot = 0xFF;
    for (size_t i = 0; i < length; i++) {
        uint32_t x = data[i] ^ top;
        x ^= (x >> 4);
        top = static_cast<uint8_t>(bot ^ (x >> 3) ^ (x << 4));
        bot = static_cast<uint8_t>(x ^ (x << 5));
    }
    return static_cast<uint16_t>((static_cast<uint16_t>(top) << 8) | bot);
}

bool SaveCorruptionSolver::buildBoxData(EncryptoMon &em, uint32_t currentBoxID, const std::string &boxMiscPath, BoxDataSave &out) {
    static_assert(sizeof(BoxName) == 40, "BoxName size mismatch");
    static_assert(sizeof(BoxBackground) == 1, "BoxBackground size mismatch");
    constexpr size_t MISC_SIZE = sizeof(BoxName) * 18 + sizeof(BoxBackground) * 18;

    if (currentBoxID > 17) {
        std::cerr << "buildBoxData: currentBoxID " << currentBoxID << " out of range [0,17]\n";
        return false;
    }

    std::ifstream f(boxMiscPath, std::ios::binary);
    if (!f) {
        std::cerr << "buildBoxData: cannot open " << boxMiscPath << "\n";
        return false;
    }
    uint8_t misc[MISC_SIZE];
    f.read(reinterpret_cast<char *>(misc), MISC_SIZE);
    if (static_cast<size_t>(f.gcount()) != MISC_SIZE) {
        std::cerr << "buildBoxData: expected " << MISC_SIZE << " bytes, got " << f.gcount() << "\n";
        return false;
    }

    // Build a properly encrypted empty pokemon: all block fields zero, then
    // shuffled (block order for pid=0) and XOR-encrypted with the stream
    // derived from checksum=0. This is what the game writes for unused slots.
    em.generateXORMasks();
    Pokemon emptySlot = {};
    em.setChecksum(emptySlot);
    em.shuffleBlocks(emptySlot);
    em.encryptPokemon(emptySlot);

    out = {};
    out.data.currentBoxID = currentBoxID;
    for (auto &box : out.data.boxes)
        for (auto &slot : box.pokemon)
            slot = emptySlot;
    std::memcpy(out.data.boxNames, misc, sizeof(BoxName) * 18);
    std::memcpy(out.data.boxBackgrounds, misc + sizeof(BoxName) * 18, sizeof(BoxBackground) * 18);
    return true;
}

std::vector<CollisionEntry> SaveCorruptionSolver::findValidCollisions() const {
    std::vector<CollisionEntry> entries;

    constexpr uint32_t totalPokemon = 18 * 30;

    for (uint32_t i = 0; i < totalPokemon; i++) {
        uint32_t P = BOX_HEADER_BYTES + i * POKEMON_BYTES;

        // First 256-byte page boundary at or after P
        uint32_t N = (P + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
        uint32_t collisionPoint = N * FLASH_PAGE_SIZE;

        if (collisionPoint < P + THIRD_BLOCK_OFFSET) {
            uint8_t overwriteLen = static_cast<uint8_t>(collisionPoint - P);
            entries.push_back({i, overwriteLen});
        }
    }

    return entries;
}

// Canonical word index → shuffled word index for the given block order.
// The canonical layout is A=words 0-15, B=16-31, C=32-47, D=48-63.
// After shuffling, block X lands at physical position blockXPositions[order],
// so canonical word i lives at shuffled word blockXPositions[i/16][order]*16 + (i%16).
uint8_t SaveCorruptionSolver::canonicalToShuffled(uint8_t canonWord, uint8_t order) {
    const uint8_t *posTable[4] = {blockAPositions, blockBPositions, blockCPositions, blockDPositions};
    return posTable[canonWord / 16][order] * 16 + (canonWord % 16);
}

// Satisfies two checksum constraints on an already-shuffled Pokemon:
//
// Non-OW: sum(source_shuffled[k..63]) == zeroContrib  → source checksum == targetCS
// OW:     sum(source_shuffled[0..k-1]) == neededSum   → chimera checksum == targetCS
//
// Non-OW is closed by adjusting otID (canon 2, sw=34 for blockA at pos 2, always >= k)
// via modular addition.  Currently any uint16_t is accepted; once RNG-based PID
// generation is integrated the caller must separately verify the resulting (TID, SID)
// is reachable from a valid seed.
// OW is satisfied greedily via OT name chars (canon 48-55), nickname chars
// (canon 32-42), and metAtDate bytes (canon 57-58).
// Returns false if the OW region has insufficient headroom.
bool SaveCorruptionSolver::satisfyChimeraChecksums(Pokemon &shuffled, uint8_t k) const {
    uint16_t *bd = reinterpret_cast<uint16_t *>(shuffled.block_data);
    uint8_t order = static_cast<uint8_t>(((shuffled.pid & 0x3E000) >> 13) % 24);

    uint16_t zeroContrib =
        static_cast<uint16_t>((m_baseDecryptedSum[TOTAL_BLOCK_WORDS] - m_baseDecryptedSum[k]) & 0xFFFF);
    uint16_t neededSum = static_cast<uint16_t>((m_cfg.targetChecksum - zeroContrib) & 0xFFFF);

    uint32_t sumOW = 0, sumNonOW = 0;
    for (int i = 0; i < k; ++i)
        sumOW += bd[i];
    for (int i = k; i < TOTAL_BLOCK_WORDS; ++i)
        sumNonOW += bd[i];

    int32_t deltaNonOW =
        static_cast<int32_t>(static_cast<int16_t>((zeroContrib - static_cast<uint16_t>(sumNonOW & 0xFFFF)) & 0xFFFF));
    int32_t deltaOW =
        static_cast<int32_t>(static_cast<int16_t>((neededSum - static_cast<uint16_t>(sumOW & 0xFFFF)) & 0xFFFF));

    auto adjustChars = [&](int canonStart, int canonEnd, uint16_t minChar, uint16_t maxChar, bool nonOW, int32_t &rem) {
        for (int c = canonStart; c <= canonEnd && rem != 0; ++c) {
            uint8_t sw = canonicalToShuffled(static_cast<uint8_t>(c), order);
            if ((sw >= k) != nonOW)
                continue;
            uint16_t cur = bd[sw];
            if (cur < minChar || cur > maxChar)
                continue;
            if (rem > 0) {
                int32_t add = std::min(rem, static_cast<int32_t>(maxChar - cur));
                bd[sw] = static_cast<uint16_t>(cur + add);
                rem -= add;
            } else {
                int32_t sub = std::min(-rem, static_cast<int32_t>(cur - minChar));
                bd[sw] = static_cast<uint16_t>(cur - sub);
                rem += sub;
            }
        }
    };

    // metAtDate bytes packed into canonical words 57-58.
    // Word 57 = dateEggReceived.day | (metAtDate.year << 8),  year in [0, 99].
    // Word 58 = metAtDate.month | (metAtDate.day << 8),  month [1,12], day [1,31].
    auto adjustMetDate = [&](bool nonOW, int32_t &rem) {
        uint8_t sw57 = canonicalToShuffled(57, order);
        if ((sw57 >= k) == nonOW && rem != 0) {
            uint8_t curYear = static_cast<uint8_t>(bd[sw57] >> 8);
            if (rem > 0) {
                int32_t add = std::min(rem / 256, static_cast<int32_t>(99 - curYear));
                if (add > 0) {
                    bd[sw57] =
                        static_cast<uint16_t>((bd[sw57] & 0x00FFu) | (static_cast<uint16_t>(curYear + add) << 8));
                    rem -= add * 256;
                }
            } else {
                int32_t sub = std::min((-rem) / 256, static_cast<int32_t>(curYear));
                if (sub > 0) {
                    bd[sw57] =
                        static_cast<uint16_t>((bd[sw57] & 0x00FFu) | (static_cast<uint16_t>(curYear - sub) << 8));
                    rem += sub * 256;
                }
            }
        }
        uint8_t sw58 = canonicalToShuffled(58, order);
        if ((sw58 >= k) == nonOW && rem != 0) {
            uint8_t curMonth = static_cast<uint8_t>(bd[sw58] & 0xFF);
            uint8_t curDay = static_cast<uint8_t>(bd[sw58] >> 8);
            if (curMonth < 1 || curDay < 1)
                return;
            if (rem > 0) {
                int32_t dayAdd = std::min(rem / 256, static_cast<int32_t>(31 - curDay));
                curDay += static_cast<uint8_t>(dayAdd);
                rem -= dayAdd * 256;
                int32_t monthAdd = std::min(rem, static_cast<int32_t>(12 - curMonth));
                curMonth += static_cast<uint8_t>(monthAdd);
                rem -= monthAdd;
            } else {
                int32_t daySub = std::min((-rem) / 256, static_cast<int32_t>(curDay - 1));
                curDay -= static_cast<uint8_t>(daySub);
                rem += daySub * 256;
                int32_t monthSub = std::min(-rem, static_cast<int32_t>(curMonth - 1));
                curMonth -= static_cast<uint8_t>(monthSub);
                rem += monthSub;
            }
            bd[sw58] = static_cast<uint16_t>(curMonth | (static_cast<uint16_t>(curDay) << 8));
        }
    };

    const TweakFields &tf = m_cfg.tweakFields;

    // Non-OW region
    if (tf.tidSid && deltaNonOW != 0) {
        uint8_t sw = canonicalToShuffled(2, order);
        bd[sw] = static_cast<uint16_t>((static_cast<int32_t>(bd[sw]) + deltaNonOW) & 0xFFFF);
        deltaNonOW = 0;
    }
    if (tf.otName)
        adjustChars(48, 48 + m_cfg.otName.maxLen - 1, m_cfg.otName.minCharID, m_cfg.otName.maxCharID, true, deltaNonOW);
    if (tf.nickname)
        adjustChars(32, 32 + m_cfg.nickname.maxLen - 1, m_cfg.nickname.minCharID, m_cfg.nickname.maxCharID, true, deltaNonOW);
    if (tf.metDate)
        adjustMetDate(true, deltaNonOW);
    if (deltaNonOW != 0)
        return false;

    // OW region
    if (tf.otName)
        adjustChars(48, 48 + m_cfg.otName.maxLen - 1, m_cfg.otName.minCharID, m_cfg.otName.maxCharID, false, deltaOW);
    if (tf.nickname)
        adjustChars(32, 32 + m_cfg.nickname.maxLen - 1, m_cfg.nickname.minCharID, m_cfg.nickname.maxCharID, false, deltaOW);
    if (tf.metDate)
        adjustMetDate(false, deltaOW);
    if (deltaOW != 0)
        return false;

    return true;
}

bool SaveCorruptionSolver::buildChimera(Pokemon source, const CollisionEntry &entry, const Gen4Frame &frame,
                                        ChimeraSolution &out) const {
    uint8_t order = static_cast<uint8_t>(((frame.pid & 0x3E000) >> 13) % 24);
    if (blockAPositions[order] != 2)
        return false;

    // Apply Method 1 IVs; preserve isEgg/hasNickname flags in bits 30-31.
    {
        BlockB *bb = reinterpret_cast<BlockB *>(&source.block_data[1]);
        uint32_t flags = bb->individualValues & 0xC0000000u;
        bb->individualValues = (static_cast<uint32_t>(frame.iv1 & 0x7FFFu) | (static_cast<uint32_t>(frame.iv2 & 0x7FFFu) << 15)) | flags;
    }

    source.pid = frame.pid;
    m_em.shuffleBlocks(source);

    uint8_t L = entry.overwriteLen;
    int k = (L > HEADER_BYTES) ? (L - HEADER_BYTES) / 2 : 0;
    if (k == 0)
        return false;

    if (!satisfyChimeraChecksums(source, static_cast<uint8_t>(k)))
        return false;

    source.checksum = m_cfg.targetChecksum;
    m_em.encryptPokemon(source);

    Pokemon chimera = m_baseEncrypted;
    memcpy(&chimera, &source, L);

    out.pid = frame.pid;
    out.blockOrder = order;
    out.overwriteLen = L;
    out.pokemonIndex = entry.pokemonIndex;
    out.source = source;
    out.chimera = chimera;

    return true;
}

SaveCorruptionSolver::FrameEnumerator SaveCorruptionSolver::makeBlockAPos2Enumerator() {
    struct State {
        uint32_t seed = 0;
        bool exhausted = false;
    };
    return [s = State{}](Gen4Frame &out) mutable -> bool {
        while (!s.exhausted) {
            uint32_t s0 = s.seed;
            if (s.seed == 0xFFFFFFFFu)
                s.exhausted = true;
            else
                ++s.seed;

            LCRNG4 rng(s0);
            uint16_t pidLow = rng.nextUShort();
            uint16_t pidHigh = rng.nextUShort();
            uint32_t pid = static_cast<uint32_t>(pidLow) | (static_cast<uint32_t>(pidHigh) << 16);

            uint8_t order = static_cast<uint8_t>(((pid & 0x3E000) >> 13) % 24);
            if (blockAPositions[order] != 2)
                continue;

            out = {pid, rng.nextUShort(), rng.nextUShort()};
            return true;
        }
        return false;
    };
}

void SaveCorruptionSolver::solve(const std::vector<std::string> &pokemonFiles, FrameEnumerator nextFrame) {
    auto collisions = findValidCollisions();

    // Load and validate all source pokemon upfront so we only read files once.
    struct Source {
        std::string file;
        Pokemon src;
    };
    std::vector<Source> sources;
    for (const auto &file : pokemonFiles) {
        ExtendedPokemon ext;
        if (!m_em.loadBinaryExtendedPokemon(file, ext)) {
            std::cerr << "Could not load: " << file << "\n";
            continue;
        }
        Pokemon &src = ext.pokemon;
        m_em.decryptPokemon(src);
        m_em.unshuffleBlocks(src);

        if (!m_em.isNicknameValid(src, m_cfg.nickname.minCharID, m_cfg.nickname.maxCharID, m_cfg.nickname.maxLen)) {
            std::cout << "Skipping " << file << ": nickname out of char range\n";
            continue;
        }
        if (!m_em.isOTNameValid(src, m_cfg.otName.minCharID, m_cfg.otName.maxCharID, m_cfg.otName.maxLen)) {
            std::cout << "Skipping " << file << ": OT name out of char range\n";
            continue;
        }
        sources.push_back({file, src});
    }
    if (sources.empty())
        return;

    Gen4Frame frame;
    while (nextFrame(frame)) {
        for (const auto &s : sources) {
            for (const auto &collision : collisions) {
                ChimeraSolution sol;
                if (buildChimera(s.src, collision, frame, sol))
                    printSolution(s.file, sol);
            }
        }
    }
}

void SaveCorruptionSolver::printSolution(const std::string &srcFile, const ChimeraSolution &sol) const {
    uint32_t box = sol.pokemonIndex / 30;
    uint32_t slot = sol.pokemonIndex % 30;

    auto printHex = [](const Pokemon &p) {
        const uint8_t *b = reinterpret_cast<const uint8_t *>(&p);
        for (size_t i = 0; i < sizeof(Pokemon); ++i) {
            if (i % 16 == 0)
                std::cout << "    ";
            else if (i % 8 == 0)
                std::cout << " ";
            std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(b[i]) << " ";
            if (i % 16 == 15)
                std::cout << "\n";
        }
        if (sizeof(Pokemon) % 16 != 0)
            std::cout << "\n";
    };

    std::cout << "=== Chimera solution ===\n"
              << "  Source:       " << srcFile << "\n"
              << "  Slot:         box " << box << ", slot " << slot << " (index " << sol.pokemonIndex << ")\n"
              << "  Overwrite:    " << static_cast<int>(sol.overwriteLen) << " bytes\n"
              << "  PID:          0x" << std::hex << std::setw(8) << std::setfill('0') << sol.pid << std::dec << "\n"
              << "  Block order:  " << static_cast<int>(sol.blockOrder) << "\n"
              << "  Source hex:\n";
    printHex(sol.source);
    std::cout << "  Chimera hex:\n";
    printHex(sol.chimera);
    std::cout << std::dec << "\n";
}
