#pragma once

#include "EncryptoMon.hpp"
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

enum class Game { DP, Platinum, HGSS };
enum class Language { JP, International };

struct VersionConfig {
    Game game = Game::DP;
    Language language = Language::International;

    // JP: both fields capped at 5 real chars; International: nickname 10, OT name 7.
    uint8_t nicknameMaxLen() const { return language == Language::JP ? 5 : 10; }
    uint8_t otNameMaxLen() const { return language == Language::JP ? 5 : 7; }
};

// 0xFFFF is always accepted as the string terminator.
struct CharRangeConfig {
    uint16_t minCharID = 0x0001;
    uint16_t maxCharID = 0x01B5;
    uint8_t maxLen = 10; // max real characters before the 0xFFFF terminator
};

// The flash write stopped at the 256-byte page boundary that falls
// within the first THIRD_BLOCK_OFFSET bytes of this pokemon slot,
// so 'overwriteLen' bytes at the slot start come from new data and
// the rest come from the base slot.
struct CollisionEntry {
    uint32_t pokemonIndex; // linear 0-based index across all 18*30 slots
    uint8_t overwriteLen;  // bytes from slot start overwritten (always < 72)
};

struct TweakFields {
    bool nickname = true;
    bool otName = true;
    bool metDate = true;
    bool currentBoxID = true; // try all box IDs 0-17 when sweeping for a CRC match
    // Disabled until (TID, SID) is validated against reachable RNG seeds.
    bool tidSid = false;
};

struct SaveCorruptionConfig {
    VersionConfig version;
    CharRangeConfig nickname;
    CharRangeConfig otName;
    uint16_t targetChecksum = 0xED5A;
    Pokemon basePokemon = {};
    TweakFields tweakFields;
};

struct ChimeraSolution {
    uint32_t pid;
    uint8_t blockOrder;
    uint8_t overwriteLen;
    uint32_t pokemonIndex;
    uint32_t boxID = 0; // currentBoxID value that makes the box CRC match
    Pokemon source;     // modified source in save-file format (shuffled + encrypted)
    Pokemon chimera;
};

struct Gen4Frame {
    uint32_t pid;
    uint16_t iv1; // hp|(atk<<5)|(def<<10)
    uint16_t iv2; // spe|(spa<<5)|(spd<<10)
};

class SaveCorruptionSolver {
  public:
    using Config = SaveCorruptionConfig;

    // A FrameEnumerator is called repeatedly: it fills 'frame' and returns true,
    // or returns false when the sequence is exhausted.
    using FrameEnumerator = std::function<bool(Gen4Frame &frame)>;

    // Layout constants derived from struct sizes.
    // Verified by static_asserts in SaveCorruptionSolver.cpp.
    static constexpr uint32_t FLASH_PAGE_SIZE = 256;
    static constexpr uint32_t BOX_HEADER_BYTES = 4;  // sizeof(BoxData::currentBoxID)
    static constexpr uint8_t POKEMON_BYTES = 136;    // sizeof(Pokemon)
    static constexpr uint8_t HEADER_BYTES = 8;       // offsetof(Pokemon, block_data)
    static constexpr uint8_t BLOCK_BYTES = 32;       // sizeof(Block)
    static constexpr uint8_t TOTAL_BLOCK_WORDS = 64; // block_data / sizeof(uint16_t)
    // Third block starts here; valid collisions only overwrite bytes [0, THIRD_BLOCK_OFFSET)
    static constexpr uint8_t THIRD_BLOCK_OFFSET = HEADER_BYTES + 2 * BLOCK_BYTES; // 72

    SaveCorruptionSolver(EncryptoMon &em, Config cfg = {});

    // Find every slot in BoxData whose 256-byte page boundary lands
    // within the first THIRD_BLOCK_OFFSET bytes of that slot.
    std::vector<CollisionEntry> findValidCollisions() const;

    // For each frame yielded by nextFrame, try every (pokemon file × collision entry)
    // pair. For each chimera candidate, sweep char pairs in the non-OW region to find
    // a value split that also makes the BoxDataSave CRC match targetBoxCRC.
    // boxData must be the empty BoxDataSave whose footer.checksum == targetBoxCRC.
    void solve(const std::vector<std::string> &pokemonFiles, FrameEnumerator nextFrame, BoxDataSave &boxData,
               uint16_t targetBoxCRC);

    // CRC16-CCITT as used by Gen 4 save blocks (init=0xFFFF).
    // NOT the same as the per-pokemon checksum (which is a simple 16-bit word sum).
    // For DP storage block: cover all bytes except the last 0x14 (footer); result
    // goes at footer+0x12 (little-endian u16, i.e. 2 bytes before end of footer).
    struct CRCState {
        uint8_t top = 0xFF;
        uint8_t bot = 0xFF;
    };
    static CRCState crc16CCITTUpdate(CRCState state, const uint8_t *data, size_t length);
    static uint16_t crc16CCITTFinalize(CRCState state);
    static uint16_t crc16CCITT(const uint8_t *data, size_t length);

    // Build a zero-initialised BoxDataSave (all 18×30 slots are encrypted empty pokemon,
    // i.e. zero bytes, since pid=0 makes the XOR stream all-zero).
    // boxNames and boxBackgrounds are memcpy'd from boxMiscPath
    // (layout: BoxName[18] then BoxBackground[18], 738 bytes total).
    // Returns false and writes nothing on I/O error or wrong file size.
    static bool buildBoxData(EncryptoMon &em, uint32_t currentBoxID, const std::string &boxMiscPath, BoxDataSave &out);

    // Returns a FrameEnumerator that scans LCRNG seeds in [seedStart, seedEnd] via
    // Method 1, yielding only frames where blockA is at shuffled position 2.
    // Default range covers the full 32-bit space.
    static FrameEnumerator makeBlockAPos2Enumerator(uint32_t seedStart = 0, uint32_t seedEnd = 0xFFFFFFFFu);

    // Same as solve() but splits [0, 0xFFFFFFFF] into numThreads seed ranges and
    // runs each on its own thread. Pass numThreads=0 to use hardware_concurrency().
    void solveParallel(const std::vector<std::string> &pokemonFiles, BoxDataSave &boxData, uint16_t targetBoxCRC,
                       unsigned numThreads = 0);

    // Build a chimera for a source pokemon (unshuffled + decrypted canonical form),
    // a collision entry, and a Gen4 Method 1 frame (pid + iv1 + iv2).
    // Applies the frame IVs to the source before shuffling.
    // Returns false if the PID's block order does not place blockA at position 2,
    // or if OW headroom is insufficient.
    // If shuffledPlaintextOut is non-null it receives the shuffled + checksum-set but
    // NOT yet encrypted pokemon, which is needed by sweepBoxCRC.
    bool buildChimera(Pokemon source, const CollisionEntry &entry, const Gen4Frame &frame, ChimeraSolution &out,
                      Pokemon *shuffledPlaintextOut = nullptr) const;

    // TODO: filter results by minimum IV thresholds
    // TODO: multithreaded search over all 32-bit seeds
    // TODO: after finding chimera, verify the game would accept the pokemon (bad-egg flag etc.)
    // TODO: add box name and background data structs to BoxData and account for them in collision math

  private:
    EncryptoMon &m_em;
    Config m_cfg;
    mutable std::mutex m_printMutex;

    // Base Pokemon in save-file format (canonical → shuffled → encrypted).
    Pokemon m_baseEncrypted;
    // Prefix sums of (base_encrypted[i] XOR mask[targetCS][i]) — what each word
    // of the non-OW region decrypts to when the chimera is read with targetCS.
    uint32_t m_baseDecryptedSum[TOTAL_BLOCK_WORDS + 1];

    void printSolution(const std::string &srcFile, const ChimeraSolution &sol) const;

    // Sweep one char pair in the non-OW region of shuffledPlaintext, keeping
    // their uint16 sum fixed (pokemon checksum invariant), to find a split whose
    // re-encrypted chimera makes the full BoxDataSave CRC equal targetBoxCRC.
    // prefixState  = CRC state after processing bytes before the slot.
    // suffix/suffixLen = bytes after the slot up to (but not including) the footer.
    // On success fills out with the updated source+chimera and returns true.
    bool sweepBoxCRC(const Pokemon &shuffledPlaintext, const ChimeraSolution &baseSol, CRCState prefixState,
                     const uint8_t *suffix, size_t suffixLen, uint16_t targetBoxCRC, ChimeraSolution &out) const;

    // Satisfies sum constraints for both the source Pokemon and the chimera
    // using the fields enabled in cfg.tweakFields. Returns false if either
    // region cannot reach its target sum with the available headroom.
    bool satisfyChimeraChecksums(Pokemon &shuffled, uint8_t k) const;

    // Map canonical word index [0..63] to its shuffled word index for a given block order.
    static uint8_t canonicalToShuffled(uint8_t canonWord, uint8_t order);
};
