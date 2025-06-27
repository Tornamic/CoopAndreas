#pragma once

enum eScriptThingType : unsigned __int8
{
	SCRIPT_THING_SPHERE = 0x0,
	SCRIPT_THING_EFFECT_SYSTEM = 0x1,
	SCRIPT_THING_SEARCH_LIGHT = 0x2,
	SCRIPT_THING_CHECKPOINT = 0x3,
	SCRIPT_THING_SEQUENCE_TASK = 0x4,
	SCRIPT_THING_FIRE = 0x5,
	SCRIPT_THING_2D_EFFECT = 0x6,
	SCRIPT_THING_DECISION_MAKER = 0x7,
	SCRIPT_THING_PED_GROUP = 0x8,
};


class CTaskSequences
{
public:
	static constexpr auto NUM_SEQUENCES = 64;
	static constexpr auto NUM_TASKS = 8;

	static inline int& CTaskSequences::ms_iActiveSequence = *(int*)0x8D2E98;
	static inline bool(&CTaskSequences::ms_bIsOpened)[NUM_SEQUENCES] = *(bool(*)[NUM_SEQUENCES])0xC17898;
};

