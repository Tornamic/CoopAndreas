#pragma once
#include <Windows.h>

#define MAX_SCRIPT_VARS	16	// Size of our variable saving array
#define MAX_SCRIPT_SIZE	255	// Size of ScriptBuf - Max is really (2+(13*5))

// Super Simplified SA Version
struct GAME_SCRIPT_THREAD	// 0xE0 bytes total.
{
	BYTE Pad1[0x14];		// 0x00 [0x14]
	DWORD dwScriptIP;		// 0x14 [0x04]
	BYTE Pad2[0x24];		// 0x18 [0x24]
	DWORD dwLocalVar[18];	// 0x3C [0x48]
	BYTE Pad3[0x41];		// 0x84 [0x41]
	DWORD dwIfFlag;			// 0xC5 [0x04]
	BYTE Pad4[0xF];			// 0xC9 [0x0F]
	DWORD dwIndexSomething;	// 0xD8 [0x04]	(indexes into 0xA49960)
	BYTE bHasLocalCopy;		// 0xDC [0x01]	(has local copy of something, if 1, then uses .data:00A48960 CTheScripts_LocalCopy instead of var0)
	BYTE Pad5[0x3];			// 0xDD [0x03]
};

struct SCRIPT_COMMAND		//	Params
{							//		i = integer
	WORD OpCode;			//		f = float
	char Params[MAX_SCRIPT_VARS]; //	v = variable
};							//		s = string

void InitScripting();
extern "C" int __declspec(dllexport) ScriptCommand(WORD opcode, const char args[], ...);	// The main scripting function. See notes.


/*	Below are some opcodes i've defined, but it's still missing
	about 95% of them ;) See http://sa-db.webtools4you.net/		*/
const SCRIPT_COMMAND create_arrow_above_actor = { 0x0187, "iv" };
const SCRIPT_COMMAND set_player_skin = { 0x09C7, "ii" };
const SCRIPT_COMMAND request_model = { 0x0247, "i" };		// (CAR_*|BIKE_*|BOAT_*|WEAPON_*|OBJECT_*)
const SCRIPT_COMMAND load_requested_models = { 0x038B, "" };		// -/-
const SCRIPT_COMMAND create_car = { 0x00A5, "ifffv" };	// (CAR_*|BIKE_*|BOAT_*), x, y, z, var_car
const SCRIPT_COMMAND load_special_actor = { 0x023C, "is" };		// SPECIAL_*, MODEL_*
const SCRIPT_COMMAND create_actor = { 0x009A, "iifffv" };	// PEDTYPE_*, #MODEL, x, y, z, var_actor
const SCRIPT_COMMAND destroy_actor_fading = { 0x034F, "i" };		// var_actor
const SCRIPT_COMMAND destroy_actor = { 0x009b, "i" };		// var_actor
const SCRIPT_COMMAND set_weather = { 0x01B6, "i" };		// WEATHER_*
const SCRIPT_COMMAND set_fade_color = { 0x0169, "iii" };	// Red(0-255), Green(0-255), Blue(0-255)
const SCRIPT_COMMAND fade = { 0x016A, "ii" };		// (time in ms), FADE_*
const SCRIPT_COMMAND is_fading = { 0x016B, "" };		// -/-
const SCRIPT_COMMAND freeze_player = { 0x01B4, "ii" };		// PLAYER_CHAR, Freeze(1/0)
const SCRIPT_COMMAND name_thread = { 0x03A4, "s" };		// "MAIN"
const SCRIPT_COMMAND set_max_wanted_level = { 0x01F0, "i" };		// MaxLevel
const SCRIPT_COMMAND set_wasted_busted_check = { 0x0111, "i" };		// Check(1/0)
const SCRIPT_COMMAND set_current_time = { 0x00C0, "ii" };		// Hours, Minutes
const SCRIPT_COMMAND get_current_time = { 0x00C0, "vv" };		// Hours, Minutes
const SCRIPT_COMMAND refresh_screen = { 0x04E4, "ff" };		// x, y
const SCRIPT_COMMAND set_camera = { 0x03CB, "fff" };	// x, y, z
const SCRIPT_COMMAND create_player = { 0x0053, "vfffv" };	// 0, x, y, z, PLAYER_CHAR
const SCRIPT_COMMAND create_actor_from_player = { 0x01F5, "vv" };		// PLAYER_CHAR, PLAYER_ACTOR
const SCRIPT_COMMAND set_actor_skin = { 0x0352, "vs" };		// var_actor, MODEL_*
const SCRIPT_COMMAND refresh_actor_skin = { 0x0353, "v" };		// var_actor
const SCRIPT_COMMAND select_interior = { 0x04BB, "i" };		// INTERIOR_*
const SCRIPT_COMMAND make_actor_leave_vehicle = { 0x03E2, "v" };		// var_actor
const SCRIPT_COMMAND clear_actor_objective = { 0x011C, "v" };		// var_actor
const SCRIPT_COMMAND set_car_color = { 0x0229, "iii" };	// var_car col1 col2
const SCRIPT_COMMAND wait = { 0x0001, "i" };		// time_in_ms
const SCRIPT_COMMAND create_thread = { 0x00D7, "iz" };		// StartIP (4F)
const SCRIPT_COMMAND end_thread = { 0x004E, "" };		// -/-
const SCRIPT_COMMAND destroy_car = { 0x00A6, "i" };		// var_car
const SCRIPT_COMMAND set_car_z_angle = { 0x0175, "if" };		// var_car, angle
const SCRIPT_COMMAND car_relative_coordinates = { 0x0407, "vfffvvv" };// var_car, x, y, z, var_x, var_y, var_z
const SCRIPT_COMMAND set_car_max_speed = { 0x00AD, "if" };		// var_car, speed
const SCRIPT_COMMAND is_model_available = { 0x0248, "i" };		// #MODEL
const SCRIPT_COMMAND create_actor_in_driverseat = { 0x0129, "viiv" };	// var_car, pedtype, #MODEL, var_actor
const SCRIPT_COMMAND create_actor_in_passenger = { 0x01c8, "viiiv" };	// var_car, pedtype, #MODEL, seat, var_actor
const SCRIPT_COMMAND drive_car_to_point1 = { 0x02c2, "ifff" };	// var_car, x, y, z
const SCRIPT_COMMAND car_ignore_traffic = { 0x00AE, "vi" };		// var_car, flag
const SCRIPT_COMMAND set_car_immune_to_nonplayer = { 0x02aa, "vi" };		// var_car, immune(1/0)
const SCRIPT_COMMAND give_actor_weapon = { 0x01B2, "iii" };	// var_actor, weapon, ammo
const SCRIPT_COMMAND reset_actor_flags = { 0x01ED, "v" };		// var_actor
const SCRIPT_COMMAND set_actor_ped_stats = { 0x0243, "vi" };		// var_actor, pedstats
const SCRIPT_COMMAND toggle_actor_wander = { 0x0319, "vi" };		// var_actor, wander(1/0)
const SCRIPT_COMMAND play_music = { 0x0394, "i" };		// music
const SCRIPT_COMMAND clear_player_wanted_level = { 0x0110, "v" };		// PLAYER_CHAR
const SCRIPT_COMMAND set_car_door_status = { 0x020A, "ii" };		// var_car, status
const SCRIPT_COMMAND remove_references_to_actor = { 0x01C2, "v" };		// var_actor
const SCRIPT_COMMAND remove_references_to_car = { 0x01C3, "v" };		// var_car
const SCRIPT_COMMAND set_player_wanted_level = { 0x010D, "vi" };		// PLAYER_CHAR, level
const SCRIPT_COMMAND set_player_health = { 0x0222, "vi" };		// PLAYER_CHAR, health
const SCRIPT_COMMAND get_car_health = { 0x0227, "vv" };		// var_car, var_health
const SCRIPT_COMMAND text_styled = { 0x00BA, "sii" };	// TEXT, time, style
const SCRIPT_COMMAND text_now = { 0x00BC, "sii" };	// TEXT, time, style
const SCRIPT_COMMAND text_1number_styled = { 0x01E3, "siii" };	// TEXT, number, time, style
const SCRIPT_COMMAND toggle_car_siren = { 0x0397, "vi" };		// var_car, siren(1/0)
const SCRIPT_COMMAND toggle_widescreen = { 0x02A3, "i" };		// widescreen(1/0)
const SCRIPT_COMMAND set_camera_behind_player = { 0x0373, "" };		// -/-
const SCRIPT_COMMAND camera_on_player = { 0x0157, "vii" };	// var_player, ukn, ukn
const SCRIPT_COMMAND camera_on_vehicle = { 0x0158, "iii" };	// var_car, ukn, ukn
const SCRIPT_COMMAND camera_on_actor = { 0x0159, "iii" };	// var_actor, ukn, ukn
const SCRIPT_COMMAND restore_camera = { 0x015A, "" };		// -/-
const SCRIPT_COMMAND point_camera = { 0x0160, "fffi" };	// x, y, z, type
const SCRIPT_COMMAND restore_camera_jumpcut = { 0x02EB, "" };		// -/-
const SCRIPT_COMMAND set_camera_position = { 0x015F, "ffffff" }; // x, y, z, vx, vy, vz
const SCRIPT_COMMAND tie_actor_to_player = { 0x01DF, "vv" };		// var_actor, PLAYER_CHAR
const SCRIPT_COMMAND tie_marker_to_car = { 0x0161, "iiiv" };	// var_car, ukn, ukn, var_marker
const SCRIPT_COMMAND tie_marker_to_actor = { 0x0162, "iiiv" };	// var_actor, ukn, ukn, var_marker
const SCRIPT_COMMAND disable_marker = { 0x0164, "i" };		// var_marker
const SCRIPT_COMMAND set_marker_color = { 0x0165, "ii" };		// var_marker, color
const SCRIPT_COMMAND set_marker_brightness = { 0x0166, "ii" };		// var_marker, brightness
const SCRIPT_COMMAND create_marker = { 0x0167, "fffiiv" };	// x, y, z, ukn, ukn, var_marker
const SCRIPT_COMMAND create_radar_marker_without_sphere = { 0x04CE, "fffiv" };	// x, y, z, marker_type, var_marker
const SCRIPT_COMMAND show_on_radar = { 0x0168, "ii" };		// var_marker, size
const SCRIPT_COMMAND set_car_driver_behaviour = { 0x00AF, "vi" };		// var_car, behaviour
const SCRIPT_COMMAND set_actor_to_kill_actor = { 0x01C9, "vv" };		// var_actor, var_actor
const SCRIPT_COMMAND set_actor_to_kill_player = { 0x01CA, "vv" };		// var_actor, PLAYER_CHAR
const SCRIPT_COMMAND is_actor_dead = { 0x0118, "v" };		// var_actor
const SCRIPT_COMMAND player_near_point_on_foot = { 0x00F6, "vffffffi" };//	PLAYER_CHAR, x, y, z, rx, ry, rz, b
const SCRIPT_COMMAND create_icon_marker_sphere = { 0x02A7, "fffiv" };
const SCRIPT_COMMAND is_player_near_point_3d = { 0x00F5, "iffffffi" };//	PLAYER_CHAR, x, y, z, rx, ry, rz, b
const SCRIPT_COMMAND is_actor_near_point_3d = { 0x00FE, "iffffffi" };//	var_actor, x, y, z, rx, ry, rz, b
const SCRIPT_COMMAND is_car_near_point_3d = { 0x01AF, "iffffffi" };//	var_car, x, y, z, rx, ry, rz, b
const SCRIPT_COMMAND set_player_z_angle = { 0x0171, "vf" };
const SCRIPT_COMMAND give_player_weapon = { 0x01B1, "iii" };	// PLAYER_CHAR, weapon, ammo
const SCRIPT_COMMAND get_line_of_sight = { 0x06BD, "ffffffiiiii" }; // x1, y1, z1, x2, y2, z2, solid, vehicle, actor, obj, fx

const SCRIPT_COMMAND add_to_player_money = { 0x0109, "ii" };		// PLAYER_CHAR, money
const SCRIPT_COMMAND get_player_money = { 0x010b, "iv" };    // PlayerNum, VarInt

const SCRIPT_COMMAND restart_if_busted_at = { 0x016D, "ffff" };	// x, y, z, a
const SCRIPT_COMMAND is_marker_enabled = { 0x075c, "i" };		// markerID
const SCRIPT_COMMAND has_car_sunk = { 0x02bf, "i" };	// CarID
const SCRIPT_COMMAND is_car_wrecked = { 0x0119, "i" }; // CarID
const SCRIPT_COMMAND get_ground_z = { 0x02ce, "fffv" };	// x, y, z, var_ground_z

const SCRIPT_COMMAND create_checkpoint1 = { 0x0189, "fffv" };	// x, y, z, var_checkpoint
const SCRIPT_COMMAND create_checkpoint2 = { 0x018a, "fffv" };	// x, y, z, var_checkpoint
//const SCRIPT_COMMAND is_actor_near_point_3d		= {0x00FE,	"iffffffi"};
const SCRIPT_COMMAND create_racing_checkpoint = { 0x06d5,	"ifffffffv" };	// type, x, y, z, nx, ny. nz, size, $id (n=next checkpoint)
/* 0       = Street Checkpoint with arrow to the next checkpoint
   1 and 2 = Street Checkpoint
   3 and 4 = Flight Checkpoint (circle) */
const SCRIPT_COMMAND destroy_racing_checkpoint = { 0x06d6,	"i" };

// added by kyeman. (I don't use correct conventions so don't copy elsewhere
// without double checking they're how you want them, yalls.

const SCRIPT_COMMAND restart_if_wasted_at = { 0x016C, "ffffi" };
const SCRIPT_COMMAND toggle_player_infinite_run = { 0x0330, "ii" };
const SCRIPT_COMMAND toggle_player_controllable = { 0x01B4, "ii" };
const SCRIPT_COMMAND toggle_player_fast_reload = { 0x0331, "ii" };
const SCRIPT_COMMAND set_actor_armed_weapon = { 0x01b9, "ii" };
const SCRIPT_COMMAND get_actor_armed_weapon = { 0x0470, "iv" };
const SCRIPT_COMMAND remove_player_weapons = { 0x03b8, "i" };
const SCRIPT_COMMAND put_actor_in_car = { 0x036A, "ii" };
const SCRIPT_COMMAND put_player_at_and_remove_from_car = { 0x012a, "ifff" };
const SCRIPT_COMMAND set_actor_immunities = { 0x02ab, "iiiiii" };
const SCRIPT_COMMAND set_car_immunities = { 0x02ac, "iiiiii" };
const SCRIPT_COMMAND set_actor_can_be_decapitated = { 0x0446, "ii" };
const SCRIPT_COMMAND create_object = { 0x0107, "ifffv" };
const SCRIPT_COMMAND set_object_z_angle = { 0x0177, "if" };
const SCRIPT_COMMAND set_object_rotation = { 0x0453, "ifff" };
const SCRIPT_COMMAND set_object_collision = { 0x0382, "ii" };
const SCRIPT_COMMAND put_object_at = { 0x01Bc, "ifff" };
const SCRIPT_COMMAND move_object = { 0x034e, "iffffffi" };
const SCRIPT_COMMAND make_object_moveable = { 0x0392, "ii" };
const SCRIPT_COMMAND toggle_object_collision = { 0x0382, "ii" };
const SCRIPT_COMMAND play_sound = { 0x018c, "fffi" };
const SCRIPT_COMMAND create_sound = { 0x018d,	"fffiv" };
const SCRIPT_COMMAND remove_actor_from_car_and_put_at = { 0x0362, "ifff" };
const SCRIPT_COMMAND send_actor_to_car_driverseat = { 0x05CB, "iii" };
const SCRIPT_COMMAND send_actor_to_car_passenger = { 0x05CA, "iiii" };
const SCRIPT_COMMAND make_actor_leave_car = { 0x05CD, "ii" };
const SCRIPT_COMMAND get_car_z_angle = { 0x0174, "iv" };
const SCRIPT_COMMAND get_player_z_angle = { 0x0170, "iv" };
const SCRIPT_COMMAND create_train = { 0x06D8, "ifffiv" };
const SCRIPT_COMMAND destroy_train = { 0x07bd, "i" };
const SCRIPT_COMMAND set_train_acceleration = { 0x06DC, "if" };
const SCRIPT_COMMAND set_train_speed = { 0x06DD, "if" };
const SCRIPT_COMMAND set_vehicle_speed = { 0x04BA, "if" };
const SCRIPT_COMMAND is_train_wrecked = { 0x0981, "i" };
const SCRIPT_COMMAND actor_driving_train = { 0x09AE, "i" };
const SCRIPT_COMMAND enable_train_traffic = { 0x06d7, "i" };
const SCRIPT_COMMAND unknown_train_attr = { 0x0A07, "i" }; // vehicle discardable?
const SCRIPT_COMMAND car_gas_tank_explosion = { 0x09C4, "ii" };
const SCRIPT_COMMAND lock_car = { 0x0519, "ii" };
const SCRIPT_COMMAND put_train_at = { 0x07c7, "ifff" };
const SCRIPT_COMMAND turn_off_car_engine = { 0x02d4,	"i" };
const SCRIPT_COMMAND release_model = { 0x0249,	"i" };
const SCRIPT_COMMAND get_active_interior = { 0x077e, "v" };
const SCRIPT_COMMAND get_actor_z_angle = { 0x0172, "iv" };
const SCRIPT_COMMAND set_actor_z_angle = { 0x0173, "if" };
const SCRIPT_COMMAND actor_set_collision = { 0x0619, "ii" };
const SCRIPT_COMMAND actor_unknown1 = { 0x094f, "i" };
const SCRIPT_COMMAND toggle_player_ignored_by_all = { 0x03bf, "ii" };
const SCRIPT_COMMAND enter_passenger_driveby = { 0x0713, "iiiffffiii" };
const SCRIPT_COMMAND lock_actor = { 0x04d7, "ii" };
const SCRIPT_COMMAND refresh_streaming_at = { 0x04E4, "ff" };
const SCRIPT_COMMAND put_actor_in_car2 = { 0x0430, "iii" };
const SCRIPT_COMMAND toggle_vehicle_headlights = { 0x067F, "ii" };
const SCRIPT_COMMAND drive_car_to_point = { 0x05D1, "iiffffiii" };

const SCRIPT_COMMAND toggle_goggles = { 0x09EB, "ii" };
const SCRIPT_COMMAND kill_actor = { 0x0321, "i" };

//const SCRIPT_COMMAND kill_actor					= { 0x5BE, "i" };
const SCRIPT_COMMAND set_actor_animation_set = { 0x0245, "is" };
const SCRIPT_COMMAND request_animation = { 0x04ED, "s" };
const SCRIPT_COMMAND is_animation_loaded = { 0x04EE, "s" };
const SCRIPT_COMMAND release_animation = { 0x04EF, "s" };
const SCRIPT_COMMAND apply_animation = { 0x0812, "issfiiiii" }; // actor,animation,library,floatunk,bool,bool,bool,bool,int
const SCRIPT_COMMAND is_actor_performing_anim = { 0x0611, "is" };
const SCRIPT_COMMAND set_actor_weapon_droppable = { 0x087e, "ii" };
const SCRIPT_COMMAND set_actor_money = { 0x03fe, "ii" };
const SCRIPT_COMMAND toggle_radar_blank = { 0x0581, "i" };
const SCRIPT_COMMAND text_clear_all = { 0x00be, "" };
const SCRIPT_COMMAND link_actor_to_interior = { 0x0860, "ii" };
const SCRIPT_COMMAND repair_car = { 0x0A30, "i" };
const SCRIPT_COMMAND toggle_player_trapped_incar = { 0x07cc, "ii" };
const SCRIPT_COMMAND set_car_hydraulics = { 0x07FF,	"ii" }; //iCar, i (0=off/1=on)

const SCRIPT_COMMAND toggle_player_weapons_changable = { 0x0992, "ii" }; // iPlayer,yes/no
const SCRIPT_COMMAND apply_object_animation = { 0x075a, "issfii" }; // object,szAnim,szIFP,fUnk,iUnk,iUnk2
const SCRIPT_COMMAND remove_weapon_from_actor = { 0x0555, "ii" }; // iActor,iWeapon
const SCRIPT_COMMAND get_actor_move_velocity = { 0x083d, "ivvv" }; // iActor,fRetZ,fRetY,fRetZ
const SCRIPT_COMMAND get_actor_distance_from_ground = { 0x0819, "iv" }; // iActor,fRet
const SCRIPT_COMMAND is_actor_falling_think = { 0x0818, "i" }; // iActor

const SCRIPT_COMMAND attach_object_to_actor = { 0x069b, "iiffffff" };
const SCRIPT_COMMAND set_object_visible = { 0x0750, "ii" };
const SCRIPT_COMMAND destroy_object_with_fade = { 0x09A2, "i" };
const SCRIPT_COMMAND disassociate_object = { 0x0682, "ifffi" };
const SCRIPT_COMMAND set_object_scaling = { 0x08d2, "if" };
const SCRIPT_COMMAND set_actor_rotation_angles = { 0x083e, "ifff" };
const SCRIPT_COMMAND is_actor_in_the_water = { 0x04ad, "i" };

const SCRIPT_COMMAND create_pickup = { 0x0213, "iifffv" };
const SCRIPT_COMMAND create_pickup_with_ammo = { 0x032b, "iiifffv" };
const SCRIPT_COMMAND is_pickup_picked_up = { 0x0214, "i" };
const SCRIPT_COMMAND is_pickup_created = { 0x09D1, "i" };
const SCRIPT_COMMAND destroy_pickup = { 0x0215, "i" };

const SCRIPT_COMMAND set_event = { 0x09C6, "iiii" };
const SCRIPT_COMMAND change_car_skin = { 0x06ED,	"ii" };
const SCRIPT_COMMAND add_car_component = { 0x06E7, "iiv" };	// CAR, COMPONENT, COMPONENT VAR NAME
const SCRIPT_COMMAND is_component_available = { 0x06EA, "i" };
const SCRIPT_COMMAND request_car_component = { 0x06E9, "i" };
const SCRIPT_COMMAND remove_component = { 0x06E8, "ii" };

const SCRIPT_COMMAND unknown_arrow = { 0x07e0, "ii" };
const SCRIPT_COMMAND show_on_radar2 = { 0x018b, "ii" };

//[21:14] <@jax> 0360 open_garage
//[21:14] <@jax> 0361 close_garage
const SCRIPT_COMMAND open_garage = { 0x0360, "s" };
const SCRIPT_COMMAND close_garage = { 0x0361, "s" };

const SCRIPT_COMMAND find_ground_z = { 0x02CE, "fffv" };
const SCRIPT_COMMAND put_car_at = { 0x00AB, "vfff" };
const SCRIPT_COMMAND is_actor_crouching = { 0x0597, "i" };
const SCRIPT_COMMAND is_ped_bounding_sphere_visible = { 0x02CB, "i" };
const SCRIPT_COMMAND is_player_in_a_car = { 0x0443, "i" };
const SCRIPT_COMMAND set_camera_pos = { 0x03cb, "fff" };
const SCRIPT_COMMAND set_actor_collision_detection = { 0x0619, "ii" };

const SCRIPT_COMMAND toggle_actor_cellphone = { 0x0729, "ii" };
const SCRIPT_COMMAND actor_task_sit = { 0x06b0, "ii" };
const SCRIPT_COMMAND actor_task_handsup = { 0x5c4, "ii" };
const SCRIPT_COMMAND actor_task_use_atm = { 0x5c7, "ii" };
const SCRIPT_COMMAND change_stat = { 0x0629, "ii" };
const SCRIPT_COMMAND unk_gang_war1 = { 0x08A3, "i" };
const SCRIPT_COMMAND unk_gang_war2 = { 0x0879, "i" };
const SCRIPT_COMMAND set_zone_owned_by = { 0x076C, "sii" };

const SCRIPT_COMMAND toggle_car_tires_vulnerable = { 0x053f, "ii" };
const SCRIPT_COMMAND set_fighting_style = { 0x07fe, "iii" };

const SCRIPT_COMMAND link_vehicle_to_interior = { 0x0840, "ii" };
const SCRIPT_COMMAND destroy_object = { 0x0108, "i" };
const SCRIPT_COMMAND create_radar_marker_icon = { 0x0570, "fffii" };
const SCRIPT_COMMAND put_trailer_on_cab = { 0x893, "ii" };
const SCRIPT_COMMAND detach_trailer_from_cab = { 0x7AC, "ii" };
const SCRIPT_COMMAND is_trailer_on_cab = { 0x7AB, "ii" };
const SCRIPT_COMMAND enable_heli_magnet = { 0x0788, "ii" };
//const SCRIPT_COMMAND create_icon_marker_without_sphere	= { 0x04CE, "fffii" };

const SCRIPT_COMMAND get_player_weapon_ammo = { 0x0419, "iiv" };
const SCRIPT_COMMAND set_player_weapon_ammo = { 0x017A, "iii" };

const SCRIPT_COMMAND set_train_flag = { 0x09CF, "ii" };
const SCRIPT_COMMAND put_camera_on_vehicle = { 0x0679, "ifffffffi" };
const SCRIPT_COMMAND put_camera_on_actor = { 0x067C, "ifffffffi" };
const SCRIPT_COMMAND create_explosion_with_radius = { 0x0948, "fffii" };
const SCRIPT_COMMAND set_car_numberplate = { 0x0674, "is" };
const SCRIPT_COMMAND enable_zone_names = { 0x09BA, "i" };

const SCRIPT_COMMAND remove_panel = { 0x08DA, "i" };
const SCRIPT_COMMAND create_panel = { 0x08D4, "sfffiiiiv" };
const SCRIPT_COMMAND set_panel_column_data = { 0x08DB, "iisssssssssssss" };
const SCRIPT_COMMAND set_panel_column_width = { 0x09DB, "iii" };
const SCRIPT_COMMAND set_panel_row_enable = { 0x08D9, "iii" };
const SCRIPT_COMMAND get_panel_selected_row = { 0x08D8, "iv" };
const SCRIPT_COMMAND get_panel_active_row = { 0x08D7, "iv" };

const SCRIPT_COMMAND destroy_particle = { 0x650, "i" };
//opcode_066a('PETROLCAN', lhActor47, 0.0, 0.116, 0.048, lf05f, lf060, lf061, 1, l0089);
const SCRIPT_COMMAND attach_particle_to_actor2 = { 0x066a, "siffffffiv" };
const SCRIPT_COMMAND make_particle_visible = { 0x64c, "i" };

const SCRIPT_COMMAND create_property_pickup = { 0x518, "i" };
const SCRIPT_COMMAND set_actor_not_react_to_gun_point = { 0x350, "ii" };
const SCRIPT_COMMAND set_actor_targetable = { 0x568, "ii" };
const SCRIPT_COMMAND set_actor_behaviour = { 0x11a, "ii" };
const SCRIPT_COMMAND set_actor_unk5 = { 0x446, "ii" };

const SCRIPT_COMMAND set_actor_dicision = { 0x60B, "ii" };
const SCRIPT_COMMAND walk_actor_to_point = { 0x0211, "vff" }; // iActor, fX, fY

const SCRIPT_COMMAND damage_car = { 0x730, "ii" }; // iVehicle, component ID
//opcode_077a(ghActor93, 0, 0);
//opcode_0446(ghActor93, 0);

const SCRIPT_COMMAND set_helicopter_fly_to = { 0x04A2, "ifffff" }; // iVehicle, fX, fY, fZ, fAltitudeMin, fAltitudeMax

const SCRIPT_COMMAND set_player_drunk_visuals = { 0x052c, "ii" }; // player, severity (0-255)
const SCRIPT_COMMAND handling_responsiveness = { 0x03fd, "ii" }; // player, severity (0-255)


