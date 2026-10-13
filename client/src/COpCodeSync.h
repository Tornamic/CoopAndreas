#pragma once

#define NUM_SYNCED_PARAMS (15)

// only the first parameters can hold an entity, see `SSyncedOpCode::m_aParamTypes`
#define NUM_ENTITY_PARAMS (4)

enum class eSyncedParamType
{
    NONE,

    // any `CPed` (aka actor, character), examples: `$ACTOR_SMOKE`
    PED,

    // `$PLAYER_CHAR` (internal playerId for the `CWorld::Players` array)
    // also used as `CPed` in cases where it is necessary to handle 
    // a player as a ped rather than a player (very often, e.g. `$PLAYER_ACTOR`)
    PLAYER,

    // any `CVehicle` (aka car)
    VEHICLE,

    // not in use yet
    OBJECT,

    MAX_PARAMS
};

struct SSyncedOpCode
{
    uint16_t m_wOpCode;

    // means the opcode has parameters that are hard to sync, 
    // like entities from the `eSyncedParamType`
    bool m_bHasComplexParams = false;

    // yes, only the first 4 parameters contain entity handles, 
    // i checked and did not find any opcode where a handle is in 
    // the 5th or later parameters
    eSyncedParamType m_aParamTypes[NUM_ENTITY_PARAMS] = { eSyncedParamType::NONE, eSyncedParamType::NONE, eSyncedParamType::NONE, eSyncedParamType::NONE };
};

struct OpcodeSyncHeader
{
    uint16_t opcode;
    uint8_t intParamCount : 4;
    uint8_t stringParamCount : 4;
};

// Serialized as a plain `int`, unless it's an entity parameter (see `SSyncedOpCode::m_aParamTypes`),
// those are serialized as `uint8_t entityType` followed by:
//   PLAYER  - playerid_t (4 bytes)
//   PED     - packed pedid_t (8 bytes)
//   VEHICLE - packed vehicleid_t (8 bytes)
//   NONE    - `value` (4 bytes), the entity couldn't be resolved, usually -1
struct OpcodeParameter
{
    // the script parameter, on receiving entity parameters are replaced with the local handles
    int value = 0;

    // NONE unless `value` has been resolved to a network entity
    eSyncedParamType entityType = eSyncedParamType::NONE;
    playerid_t playerid{};
    pedid_t pedid{};
    vehicleid_t vehicleid{};

    void SetValue(int newValue)
    {
        *this = OpcodeParameter();
        value = newValue;
    }
};

class COpCodeSync
{
public:
	static inline bool ms_bSyncingEnabled = true;
	static inline bool ms_bLoadingCutscene = true;
	static inline bool ms_abLoadingMissionAudio[4];
	
    static inline uint32_t ms_iFreeSyncedScript = 0;
    static inline char ms_aszSyncedScripts[256][8];

    static inline OpcodeParameter scriptParamsBuffer[NUM_SYNCED_PARAMS];
    static inline bool ms_bProcessingTaskSequence = false;
    static inline bool bProcessingNetworkOpcode = false;

	static void Init();
	static void HandlePacket(const uint8_t* buffer, int bufferSize);
	static std::vector<uint8_t> COpCodeSync::SerializeOpcode(int idx, int& outSize);
	static bool COpCodeSync::IsOpcodeSyncable(int opcode, int* opcodeIdx = nullptr, bool ignoreOpCodeSync = false);
};
