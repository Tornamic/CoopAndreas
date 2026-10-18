#pragma once

// traffic lights in the same phase for every player (the server time instead of the local game time), so the traffic
// all of them see stops and goes on the same lights
class CTrafficLightsSync
{
public:
    static constexpr uint8_t LIGHT_GREEN = 0;
    static constexpr uint8_t LIGHT_YELLOW = 1;
    static constexpr uint8_t LIGHT_RED = 2;

    static void InjectHooks();
};
