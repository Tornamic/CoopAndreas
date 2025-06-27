#pragma once

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
    eSyncedParamType m_aParamTypes[4] = { eSyncedParamType::NONE, eSyncedParamType::NONE, eSyncedParamType::NONE, eSyncedParamType::NONE };
};

struct OpcodeSyncHeader
{
    uint16_t opcode;
    uint8_t intParamCount : 4;
    uint8_t stringParamCount : 4;
};

struct OpcodeParameter
{
    union
    {
        struct
        {
            eSyncedParamType entityType : 4;
            int entityId : 28;
        };
        int value;
    };
};

class COpCodeSync
{
public:
	static inline bool ms_bSyncingEnabled = true;
	static inline bool ms_bLoadingCutscene = true;
	static inline bool ms_abLoadingMissionAudio[4];
	static inline std::vector<CRunningScript*> ms_vSyncedScripts;
    static inline OpcodeParameter scriptParamsBuffer[10];
    static inline bool ms_bProcessingTaskSequence = false;

	static void Init();
	static void HandlePacket(const uint8_t* buffer, int bufferSize);
	static std::vector<uint8_t> COpCodeSync::SerializeOpcode(int idx, int& outSize);
	static bool COpCodeSync::IsOpcodeSyncable(int opcode, int* opcodeIdx = nullptr, bool ignoreOpCodeSync = false);
};