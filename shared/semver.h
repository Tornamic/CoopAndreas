/*
(C) 2025 CoopAndreas
*/

// semver.h
#ifndef SEMVER_H
#define SEMVER_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Define stage constants
#define SEMVER_STAGE_RELEASE 0
#define SEMVER_STAGE_ALPHA   1
#define SEMVER_STAGE_BETA    2
#define SEMVER_STAGE_RC      3

// Struct for semver representation
typedef struct {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
    uint8_t stage;
} semver_t;

// Pack semver into uint32
static inline uint32_t semver_pack(const semver_t* version)
{
    return ((version->major & 0x1FF) << 23) |
        ((version->minor & 0x1FF) << 14) |
        ((version->patch & 0x3FF) << 4) |
        (version->stage & 0x3);
}

// Unpack uint32 into semver
static inline void semver_unpack(uint32_t packed, semver_t* version)
{
    if (version) {
        version->major = (packed >> 23) & 0x1FF;
        version->minor = (packed >> 14) & 0x1FF;
        version->patch = (packed >> 4) & 0x3FF;
        version->stage = packed & 0x3;
    }
}

// Parse stage string into stage constant
static inline uint8_t semver_parse_stage(const char* stage_str)
{
    if (strcmp(stage_str, "alpha") == 0) return SEMVER_STAGE_ALPHA;
    if (strcmp(stage_str, "beta") == 0) return SEMVER_STAGE_BETA;
    if (strcmp(stage_str, "release") == 0 || stage_str[0] == '\0') return SEMVER_STAGE_RELEASE;
    return SEMVER_STAGE_RC;
}

// Convert stage constant to string
static inline const char* semver_stage_to_string(uint8_t stage)
{
    switch (stage) {
    case SEMVER_STAGE_ALPHA: return "alpha";
    case SEMVER_STAGE_BETA: return "beta";
    case SEMVER_STAGE_RELEASE: return "release";
    default: return "rc";
    }
}

// Parse semver string into compressed uint32 and optionally into semver_t struct
static inline uint32_t semver_parse(const char* version_str, semver_t* version)
{
    char stage_str[16] = ""; // Buffer for stage string
    semver_t temp_version = { 0 };

    // Parse major, minor, patch, and optional stage
    int fields = sscanf(version_str, "%hu.%hu.%hu-%15s",
        &temp_version.major, &temp_version.minor, &temp_version.patch, stage_str);

    if (fields < 3) return 0; // At least major, minor, patch are required

    temp_version.stage = semver_parse_stage(stage_str);

    // Assign to version if not null
    if (version) {
        *version = temp_version;
    }

    // Return compressed uint32 version
    return semver_pack(&temp_version);
}

// Convert semver_t to string
static inline void semver_to_string(const semver_t* version, char* buffer, size_t buffer_size)
{
    const char* stage_str = semver_stage_to_string(version->stage);

    if (version->stage == SEMVER_STAGE_RELEASE) {
        snprintf(buffer, buffer_size, "%u.%u.%u",
            version->major, version->minor, version->patch);
    }
    else {
        snprintf(buffer, buffer_size, "%u.%u.%u-%s",
            version->major, version->minor, version->patch, stage_str);
    }
}

#endif // SEMVER_H