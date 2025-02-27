#include "stdafx.h"
#include "COpCodeSync.h"
#include <Commands/CCustomCommandMgr.h>

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

// Keep sorted!
const SSyncedOpCode syncedOpcodes[] =
{
    // Messages 
    {0x00BA}, // print_big {key} [gxt_key] {time} [int] {style} [TextStyle]
    {0x00BE}, // clear_prints
    {0x01E3}, // print_with_number_big {key} [gxt_key] {num} [int] {duration} [int] {style}[TextStyle]
    {0x0318}, // register_mission_passed {key} [gxt_key]
    {0x03E5}, // print_help {key} [gxt_key]
    {0x054C}, // load_mission_text {tableName} [string]
    {0x0998}, // award_player_mission_respect {value} [int]

    // Population managment
    {0x01EB}, // set_car_density_multiplier {multiplier} [float]
    {0x0395}, // clear_area {x} [float] {y} [float] {z} [float] {radius} [float] {clearParticles} [bool]
    {0x03DE}, // set_ped_density_multiplier {multiplier} [float]
    {0x09D2}, // enable_ambient_crime {state} [bool]
    {0x06D0}, // switch_emergency_services {state} [bool]


    // Camera
    {0x015F}, // set_fixed_camera_position {x} [float] {y} [float] {z} [float] {xRotation} [float] {yRotation} [float] {zRotation} [float]
    {0x0160}, // point_camera_at_point {x} [float] {y} [float] {z} [float] {switchStyle} [SwitchType]
    {0x02EB}, // restore_camera_jumpcut
    {0x0373}, // set_camera_behind_player
    {0x0920}, // camera_set_vector_track {fromX} [float] {fromY} [float] {fromZ} [float] {toX} [float] {toY} [float] {toZ} [float] {time} [int] {ease} [bool]
    {0x0925}, // camera_reset_new_scriptables
    {0x092F}, // camera_persist_track {state} [bool]
    {0x0930}, // camera_persist_pos {state} [bool]
    {0x0936}, // camera_set_vector_move {fromX} [float] {fromY} [float] {fromZ} [float] {toX} [float] {toY} [float] {toZ} [float] {time} [int] {ease} [bool]
    {0x0A0B}, // load_scene_in_direction {x} [float] {y} [float] {z} [float] {heading} [float]


    // Scenes
    {0x016A}, // do_fade {time} [int] {direction} [Fade]
    {0x02A3}, // switch_widescreen {state} [bool]
    {0x02E4}, // load_cutscene {name} [string]
    {0x02E7}, // start_cutscene
    {0x02EA}, // clear_cutscene

    // World

    // Interiors
    {0x04BB}, // set_area_visible {areaId} [Interior]
    {0x07FB}, // switch_entry_exit {interiorName} [string] {state} [bool]

    // Weapons
    {0x06AB, true, {eSyncedParamType::PED}}, // hide_char_weapon_for_scripted_cutscene [Char] {state} [bool]
    {0x07A7, true, {eSyncedParamType::PED}}, // task_jetpack {handle} [Char]

    // Tasks
    {0x05BF, true, {eSyncedParamType::PED, eSyncedParamType::PED}}, // task_look_at_char {observer} [Char] {target} [Char] {time} [int]
    {0x05C0, true, {eSyncedParamType::PED, eSyncedParamType::VEHICLE}}, // task_look_at_vehicle {char} [Char] {vehicle} [Car] {time} [int]
    {0x05CA, true, {eSyncedParamType::PED, eSyncedParamType::VEHICLE}}, // task_enter_car_as_passenger {char} [Char] {vehicle} [Car] {time} [int] {seatId} [int]
    {0x05CB, true, {eSyncedParamType::PED, eSyncedParamType::VEHICLE}}, // task_enter_car_as_driver {char} [Char] {vehicle} [Car] {time} [int]
    {0x05D3, true, {eSyncedParamType::PED}}, // task_go_straight_to_coord {handle} [Char] {x} [float] {y} [float] {z} [float] {moveState} [MoveState] {time} [int]
    {0x0634, true, {eSyncedParamType::PED, eSyncedParamType::PED}}, // task_kill_char_on_foot_while_ducking {char} [Char] {target} [Char] {flags} [int] {actionDelay} [int] {actionChance} [int]
    {0x0647, true, {eSyncedParamType::PED}}, // clear_look_at [Char]
    {0x0673, true, {eSyncedParamType::PED}}, // task_dive_and_get_up {handle} [Char] {directionX} [float] {directionY} [float] {timeOnGround} [int]
    {0x0687, true, {eSyncedParamType::PED}}, // clear_char_tasks [Char]
    {0x0713, true, {eSyncedParamType::PED, eSyncedParamType::PED, eSyncedParamType::VEHICLE}}, // task_drive_by {handle} [Char] {targetChar} [Char] {targetVehicle} [Car] {x} [float] {y} [float] {z} [float] {radius} [float] {type} [DriveByType] {rightHandCarSeat} [bool] {fireRate} [int]
    {0x0792, true, {eSyncedParamType::PED}}, // clear_char_tasks_immediately [Char]
    {0x0967, true, {eSyncedParamType::PED}}, // start_char_facial_talk [Char] {duration} [int]
    {0x0968, true, {eSyncedParamType::PED}}, // stop_char_facial_talk [Char]

    // Actors
    {0x00A1, true, {eSyncedParamType::PED}}, // set_char_coordinates [Char] {x} [float] {y} [float] {z} [float]
    {0x0173, true, {eSyncedParamType::PED}}, // set_char_heading [Char] {heading} [float]
    {0x0350, true, {eSyncedParamType::PED}}, // set_char_stay_in_same_place [Char] {state} [bool]
    {0x07A1}, // set_next_desired_move_state {moveState} [MoveState]


    // Vehicles
    {0x00AB, true, {eSyncedParamType::VEHICLE}}, // set_car_coordinates [Car] {x} [float] {y} [float] {z} [float]
    {0x0175, true, {eSyncedParamType::VEHICLE}}, // set_car_heading [Car] {heading} [float]

    // Explosions
    {0x070C, true, {eSyncedParamType::VEHICLE}}, // explode_car_in_cutscene [Car]

    // Audio
    {0x097A}, // report_mission_audio_event_at_position {x} [float] {y} [float] {z} [float] {soundId} [int]

    // Animations
    {0x04ED}, // request_animation {animationFile} [string]
    {0x04EF}, // remove_animation {animationFile} [string]
    {0x0605, true, {eSyncedParamType::PED}}, // task_play_anim {handle} [Char] {animationName} [string] {animationFile} [string] {frameDelta} [float] {loop} [bool] {lockX} [bool] {lockY} [bool] {lockF} [bool] {time} [int]
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


static OpcodeParameter scriptParamsBuffer[10];
static uint8_t textLengthBuffer[10];
static char textParamBuffer[10][256];

static uint16_t scriptParamCount = 0;
static uint16_t textParamCount = 0;

static uint32_t lastOpCodeProcessed;
static CRunningScript* lastProcessedScript;

static uint8_t currentStringIdx = 0;

static uint16_t argCount = 0;
void __fastcall CRunningScript__CollectParameters_Hook_SwitchParametersContext(CRunningScript* script, int, uint16_t count)
{
    for (uint8_t i = argCount; i < count + argCount; i++)
    {
        ScriptParams[i - argCount] = scriptParamsBuffer[i].value;
    }
	argCount += count;
    /*memcpy(ScriptParams, scriptParamsBuffer, sizeof scriptParamsBuffer);*/
}

void __fastcall CRunningScript__ReadTextLabelFromScript_Hook_SwitchParametersContext(CRunningScript* script, int, char* ptr, uint8_t len)
{
    memset(ptr, 0, len);
    strncpy(ptr, textParamBuffer[currentStringIdx], textLengthBuffer[currentStringIdx]);
    currentStringIdx++;
}

/// <param name="opcodeIdx">syncedOpcodes index</param>
bool IsOpcodeSyncable(int* opcodeIdx = nullptr)
{
    if (CLocalPlayer::m_bIsHost
        && COpCodeSync::ms_bSyncingEnabled
        && std::find(COpCodeSync::ms_vSyncedScripts.begin(), COpCodeSync::ms_vSyncedScripts.end(), lastProcessedScript) != COpCodeSync::ms_vSyncedScripts.end())
    {
        for (int i = 0; i < ARRAY_SIZE(syncedOpcodes); i++)
        {
            if (syncedOpcodes[i].m_wOpCode == lastOpCodeProcessed)
            {
                if (opcodeIdx)
                {
                    *opcodeIdx = i;
                }

                return true;
            }
        }
    }
    return false;
}

void BuildAndSendOpcode()
{
    int idx = 0;
    if (!IsOpcodeSyncable(&idx))
        return;

    int dataSize = 
        sizeof(OpcodeSyncHeader) 
        + scriptParamCount * sizeof(int) 
        + textParamCount * sizeof(uint8_t);

    for (int i = 0; i < textParamCount; i++)
    {
        dataSize += textLengthBuffer[i];
    }

    std::vector<uint8_t> buffer(dataSize);
    uint8_t* current = buffer.data();

    OpcodeSyncHeader header;
    header.opcode = lastOpCodeProcessed;
    header.intParamCount = scriptParamCount;
    header.stringParamCount = textParamCount;

    /*if (header.opcode == 0x0605 || header.opcode == 0x04ed || header.opcode == 0x04ef)
    {
        CChat::AddMessage("0x%04x", header.opcode);
    }*/

    memcpy(current, &header, sizeof(header));
    current += sizeof(header);

    if (scriptParamCount)
    {
        for (int i = 0; i < scriptParamCount; i++)
        {
            if (syncedOpcodes[idx].m_bHasComplexParams && i < 4)
            {
                //CChat::AddMessage("Parsing complex opcode %04x parameter %d...", header.opcode, i);
                switch (syncedOpcodes[idx].m_aParamTypes[i])
                {
                case eSyncedParamType::PED:
                {
                    //CChat::AddMessage("Trying to parse a ped handle %d...", scriptParamsBuffer[i].value);
                    if (auto ped = CPools::GetPed(scriptParamsBuffer[i].value))
                    {
                        if (ped->m_nPedType > 3) // is a regular ped
                        {
                            if (auto networkPed = CNetworkPedManager::GetPed(ped))
                            {
                                //CChat::AddMessage("Network id %d...", networkPed->m_nPedId);
                                if (networkPed->m_bSyncing)
                                {
                                    scriptParamsBuffer[i].entityId = networkPed->m_nPedId;
                                    scriptParamsBuffer[i].entityType = eSyncedParamType::PED;
                                    //CChat::AddMessage("Parsed ped id %d...", networkPed->m_nPedId);
                                }
                                else scriptParamsBuffer[i].value = -1;
                            }
                            else scriptParamsBuffer[i].value = -1;
                        }
                        else // is a player ped
                        {
                            scriptParamsBuffer[i].entityType = eSyncedParamType::PLAYER;
                            if (ped == FindPlayerPed(0))
                            {
                                scriptParamsBuffer[i].entityId = CNetworkPlayerManager::m_nMyId;
                            }
                            else if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(ped))
                            {
                                scriptParamsBuffer[i].entityId = networkPlayer->m_iPlayerId;
                            }
                            else
                            {
                                scriptParamsBuffer[i].value = -1;
                            }
                        }

                    }
                    else scriptParamsBuffer[i].value = -1;
                    
                    break;
                }
                case eSyncedParamType::PLAYER:
                {
                    //CChat::AddMessage("Trying to parse a player...");
                    if (!scriptParamsBuffer[i].value)
                    {
                        scriptParamsBuffer[i].entityType = eSyncedParamType::PLAYER;
                        scriptParamsBuffer[i].entityId = CNetworkPlayerManager::m_nMyId;
                        //CChat::AddMessage("Parsed player id %d (me)...", CNetworkPlayerManager::m_nMyId);
                        break;
                    }
                    else scriptParamsBuffer[i].value = -1;

                    if (auto player = CWorld::Players[scriptParamsBuffer[i].value].m_pPed)
                    {
                        if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(player))
                        {
                            scriptParamsBuffer[i].entityType = eSyncedParamType::PLAYER;
                            scriptParamsBuffer[i].entityId = networkPlayer->m_iPlayerId;
                            //CChat::AddMessage("Parsed player id %d...", networkPlayer->m_iPlayerId);
                        }
                        else scriptParamsBuffer[i].value = -1;
                    }
                    else scriptParamsBuffer[i].value = -1;

                    break;
                }
                case eSyncedParamType::VEHICLE:
                {
                    //CChat::AddMessage("Trying to parse a vehicle...");
                    if (auto vehicle = CPools::GetVehicle(scriptParamsBuffer[i].value))
                    {
                        if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(vehicle))
                        {
                            scriptParamsBuffer[i].entityId = networkVehicle->m_nVehicleId;
                            scriptParamsBuffer[i].entityType = eSyncedParamType::VEHICLE;
                            //CChat::AddMessage("Parsed vehicle id %d...", networkVehicle->m_nVehicleId);
                        }
                        else scriptParamsBuffer[i].value = -1;
                    }
                    else scriptParamsBuffer[i].value = -1;

                    break;
                }
                default:
                    break;
                }
            }

            memcpy(current, &scriptParamsBuffer[i], sizeof(int));
            current += sizeof(int);
        }
    }

    if(textParamCount)
    {
        for (int i = 0; i < textParamCount; i++)
        {
            *current = static_cast<uint8_t>(textLengthBuffer[i]);
            current += sizeof(uint8_t);

            memcpy(current, textParamBuffer[i], textLengthBuffer[i]);
            current += textLengthBuffer[i];
        }
    }

    CNetwork::SendPacket(CPacketsID::OPCODE_SYNC, buffer.data(), dataSize, ENET_PACKET_FLAG_RELIABLE);

    memset(textParamBuffer, 0, sizeof textParamBuffer);
    memset(textLengthBuffer, 0, sizeof textLengthBuffer);
    scriptParamCount = 0;
    textParamCount = 0;
}

bool IsOpcodeRequiresStrictCheck(uint16_t opcode)
{
    if (opcode == 0x713) // task_drive_by
    {
        return false;
    }
    return true;
}

void COpCodeSync::HandlePacket(const uint8_t* buffer, int bufferSize)
{
    if (!buffer || bufferSize < sizeof(OpcodeSyncHeader))
    {
        return;
    }

    const uint8_t* current = buffer;

    OpcodeSyncHeader header;
    memcpy(&header, current, sizeof(header));
    current += sizeof(header);

    

    if (bufferSize < sizeof(header) + header.intParamCount * sizeof(int))
    {
        return;
    }

    memset(scriptParamsBuffer, 0, sizeof(scriptParamsBuffer));
    memset(textLengthBuffer, 0, sizeof(textLengthBuffer));
    memset(textParamBuffer, 0, sizeof(textParamBuffer));
    scriptParamCount = 0;
    textParamCount = 0;
    currentStringIdx = 0;

    scriptParamCount = header.intParamCount;
    if (scriptParamCount > 10) scriptParamCount = 10;

    textParamCount = header.stringParamCount;
    if (textParamCount > 10) textParamCount = 10;

    if (scriptParamCount)
    {
        memcpy(scriptParamsBuffer, current, scriptParamCount * sizeof(int));
        current += scriptParamCount * sizeof(int);

        int idx = 0;
        bool found = false;
        for (; idx < ARRAY_SIZE(syncedOpcodes); idx++)
        {
            if (syncedOpcodes[idx].m_wOpCode == header.opcode)
            {
                found = true;
                break;
            }
        }

        if(!found)
            return;

        if (syncedOpcodes[idx].m_bHasComplexParams)
        {
            /*if (header.opcode == 0x0605)
                CChat::AddMessage("Parsing complex opcode %04x...", header.opcode);*/

            int max = min(scriptParamCount, 4);
            for (int i = 0; i < max; i++)
            {
                switch (syncedOpcodes[idx].m_aParamTypes[i])
                {
                case eSyncedParamType::PED:
                {
                    /*if (header.opcode == 0x0605)    
                        CChat::AddMessage("Parsing ped...");*/
                    if (scriptParamsBuffer[i].entityType == eSyncedParamType::PED)
                    {
                        /*if (header.opcode == 0x0605)
                            CChat::AddMessage("Trying to parse as a regular ped...");*/
                        if (auto networkPed = CNetworkPedManager::GetPed(scriptParamsBuffer[i].entityId))
                        {
                            /*if (header.opcode == 0x0605)
                                CChat::AddMessage("Network instance found...");*/
                            if (auto ped = networkPed->m_pPed)
                            {
                                scriptParamsBuffer[i].value = CPools::GetPedRef(ped);
                                /*if (header.opcode == 0x0605)
                                    CChat::AddMessage("Parsed ped id %d...", networkPed->m_nPedId);*/
                            }
                            else if (!IsOpcodeRequiresStrictCheck(header.opcode))
                            {
                                scriptParamsBuffer[i].value = -1;
                            }
                            else return;
                        }
                        else if (!IsOpcodeRequiresStrictCheck(header.opcode))
                        {
                            scriptParamsBuffer[i].value = -1;
                        }
                        else return;
                    }
                    else
                    {
                        if (scriptParamsBuffer[i].entityId == CNetworkPlayerManager::m_nMyId)
                        {
                            if (auto playerPed = FindPlayerPed(0))
                            {
                                scriptParamsBuffer[i].value = CPools::GetPedRef(playerPed);
                            }
                        }
                        else
                        {
                            /*if (header.opcode == 0x0605)
                                CChat::AddMessage("Trying to parse as a player ped... %d", scriptParamsBuffer[i].entityId);*/
                            if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(scriptParamsBuffer[i].entityId))
                            {
                                /*if (header.opcode == 0x0605)
                                    CChat::AddMessage("Network instance found...");*/
                                if (auto ped = networkPlayer->m_pPed)
                                {
                                    scriptParamsBuffer[i].value = CPools::GetPedRef(ped);
                                    /*if (header.opcode == 0x0605)
                                        CChat::AddMessage("Parsed player id as ped %d...", networkPlayer->m_iPlayerId);*/
                                }
                                else if (!IsOpcodeRequiresStrictCheck(header.opcode))
                                {
                                    scriptParamsBuffer[i].value = -1;
                                }
                                else return;
                            }
                            else if (!IsOpcodeRequiresStrictCheck(header.opcode))
                            {
                                scriptParamsBuffer[i].value = -1;
                            }
                            else return;
                        }
                    }

                    break;
                }
                case eSyncedParamType::PLAYER:
                {
                    if (scriptParamsBuffer[i].entityId == CNetworkPlayerManager::m_nMyId)
                    {
                        scriptParamsBuffer[i].value = 0;
                        /*if (header.opcode == 0x0605)
                            CChat::AddMessage("Parsed player id %d (me)...", CNetworkPlayerManager::m_nMyId);*/
                        break;
                    }

                    if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(scriptParamsBuffer[i].entityId))
                    {
                        if (networkPlayer->m_pPed)
                        {
                            int playerNum = networkPlayer->GetInternalId();
                            if (playerNum)
                            {
                                scriptParamsBuffer[i].value = playerNum;
                                /*if (header.opcode == 0x0605)
                                    CChat::AddMessage("Parsed player id %d...", networkPlayer->m_iPlayerId);*/
                            }
                            else if (!IsOpcodeRequiresStrictCheck(header.opcode))
                            {
                                scriptParamsBuffer[i].value = -1;
                            }
                            else return;
                        }
                        else if (!IsOpcodeRequiresStrictCheck(header.opcode))
                        {
                            scriptParamsBuffer[i].value = -1;
                        }
                        else return;
                    }
                    else if (!IsOpcodeRequiresStrictCheck(header.opcode))
                    {
                        scriptParamsBuffer[i].value = -1;
                    }
                    else return;

                    break;
                }
                case eSyncedParamType::VEHICLE:
                {
                    if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(scriptParamsBuffer[i].entityId))
                    {
                        if (auto vehicle = networkVehicle->m_pVehicle)
                        {
                            scriptParamsBuffer[i].value = CPools::GetVehicleRef(vehicle);
                            /*if (header.opcode == 0x0605)
                                CChat::AddMessage("Parsed vehicle id %d...", networkVehicle->m_nVehicleId);*/
                        }
                        else if (!IsOpcodeRequiresStrictCheck(header.opcode))
                        {
                            scriptParamsBuffer[i].value = -1;
                        }
                        else return;
                    }
                    else if (!IsOpcodeRequiresStrictCheck(header.opcode))
                    {
                        scriptParamsBuffer[i].value = -1;
                    }
                    else return;

                    break;
                }
                default:
                    break;
                }
            }
            /*if (header.opcode == 0x0605)
                CChat::AddMessage("OK");*/
        }
    }

    if (textParamCount)
    {
        for (int i = 0; i < textParamCount; i++)
        {
            if (current >= buffer + bufferSize)
            {
                return;
            }

            textLengthBuffer[i] = *current;
            current += sizeof(uint8_t);

            if (current + textLengthBuffer[i] > buffer + bufferSize || textLengthBuffer[i] > 255)
            {
                return;
            }

            memcpy(textParamBuffer[i], current, textLengthBuffer[i]);
            textParamBuffer[i][textLengthBuffer[i]] = '\0';
            current += textLengthBuffer[i];
        }
    }


    static CRunningScript script;
    memset(&script, 0, sizeof(CRunningScript));
    script.Init();
    script.m_bIsMission = true;
    script.m_bUseMissionCleanup = false;
    strcpy(script.m_szName, "coopand");
    script.m_pBaseIP = script.m_pCurrentIP = (uint8_t*)&header.opcode;

    lastOpCodeProcessed = header.opcode;
    
    //if (header.opcode == 0x0605 || header.opcode == 0x04ed || header.opcode == 0x04ef)
    //{
    //    //CChat::AddMessage("0x%04x", header.opcode);
    //}
    
    // TODO: refactor
    if (lastOpCodeProcessed == 0x0701) // end_scene_skip
    {
        CHud::m_BigMessage[1][0] = 0;
    }
    else if (lastOpCodeProcessed == 0x02E7) // start_cutscene
    {
        if(CCutsceneMgr::ms_cutsceneLoadStatus != 2)
        {
            COpCodeSync::ms_bLoadingCutscene = true;
            return; // dont process opcode
        }
    }

	argCount = 0;
    patch::RedirectJump(0x464080, CRunningScript__CollectParameters_Hook_SwitchParametersContext, false);
    patch::RedirectJump(0x463D50, CRunningScript__ReadTextLabelFromScript_Hook_SwitchParametersContext, false);

    if ((header.opcode & 0x7FFF) < CCustomCommandMgr::MIN_CUSTOM_COMMAND)
    {
        CRunningScript::CommandHandlerTable[(header.opcode & 0x7FFF) / 100](&script, header.opcode & 0x7FFF);
    }
    else
    {
        CCustomCommandMgr::ProcessCommand((header.opcode & 0x7FFF), &script);
    }

    patch::SetRaw(0x464080, "\x66\x8B\x44\x24\x04", 5, false);
    patch::SetRaw(0x463D50, "\x8B\x41\x14\x83\xEC\x08", 6, false);
}

void __declspec(naked) OpcodeProcessingWellDone_Hook()
{
    __asm
    {
        mov lastOpCodeProcessed, ecx
        mov lastProcessedScript, esi

        mov ecx, esi
        call edx

        push eax
        call BuildAndSendOpcode
        pop eax

        test al, al

        push 0x469FF9
        ret
    }
}

static uint16_t argParamCount = 0;
static void CollectParamsProperly()
{
    for (uint8_t i = scriptParamCount; i < scriptParamCount + argParamCount; i++)
    {
        scriptParamsBuffer[i].value = ScriptParams[i - scriptParamCount];
    }

    scriptParamCount += argParamCount;
}

void __declspec(naked) CRunningScript__CollectParameters_Hook_GetSyncingParams()
{
    __asm
    {
        pop ebx
        mov eax, [esp + 0x4]
        mov argParamCount, ax

        pop eax
        add esp, 4

        pushad
    }

    if (lastProcessedScript 
        && IsOpcodeSyncable())
    {
        CollectParamsProperly();

        /*if (lastOpCodeProcessed == 0x0605)
        {
            CChat::AddMessage("arg0 %d", scriptParamsBuffer[0].value);
        }*/
    }
    else
    {
        scriptParamCount = 0;
        argParamCount = 0;
    }

    __asm
    {
        popad
        jmp eax
    }
}

static char* textPointer;
static uint8_t textLength;
void CollectTextParameters()
{
    if (textPointer
        && lastProcessedScript
        && IsOpcodeSyncable())
    {
        textLengthBuffer[textParamCount] = min(textLength, strlen(textPointer));
        strncpy_s(textParamBuffer[textParamCount], textPointer, textLengthBuffer[textParamCount]);
        textParamCount++;
    }
}

void __declspec(naked) CRunningScript__ReadTextLabelFromScript_Hook_GetSyncingParams()
{
    __asm
    {
        add esp, 8

        mov eax, [esp + 0x4]
        mov textPointer, eax

        mov eax, [esp + 0x8]
        mov textLength, al

        call CollectTextParameters

        retn 8
    }
}



void COpCodeSync::Init()
{
    DWORD temp;
    injector::UnprotectMemory(0x464080, 5, temp);
    injector::UnprotectMemory(0x463D50, 6, temp);

    patch::RedirectJump(0x469FF3, OpcodeProcessingWellDone_Hook);

    std::vector<int> hookPositions = { 0x463D92, 0x463DBF, 0x463E1A, 0x463E6B, 0x463EAC, 0x463F2A, 0x463F7F, 0x463FA2, 0x463FDA, 0x463FFB, 0x464034 };
    patch::RedirectJump(hookPositions, CRunningScript__ReadTextLabelFromScript_Hook_GetSyncingParams);
    patch::RedirectJump(0x46421D, CRunningScript__CollectParameters_Hook_GetSyncingParams);
}