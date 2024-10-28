# CoopAndreas

Videos, pictures, news, suggestions, and communication can be found here:

[![Discord](https://img.shields.io/badge/Discord%20Server%20(most%20active)-5865F2?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/Z3ugSgFJMU)
[![X](https://img.shields.io/badge/X-000000?style=for-the-badge&logo=x&logoColor=white)](https://x.com/CoopAndreas)
[![YouTube](https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white)](https://www.youtube.com/@CoopAndreas)
[![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=for-the-badge&logo=telegram&logoColor=white)](https://t.me/coopandreas)
[![VK](https://img.shields.io/badge/VK-0077FF?style=for-the-badge&logo=vk&logoColor=white)](https://vk.com/coopandreas)
## Disclaimer
This mod is an unofficial modification for **Grand Theft Auto: San Andreas** and requires a legitimate copy of the game to function. No original game files or assets from Rockstar Games are included in this repository, and all content provided is independently developed. The project is not affiliated with Rockstar Games or Take-Two Interactive. All rights to the original game, its assets, and intellectual property belong to Rockstar Games and Take-Two Interactive. This mod is created solely for educational and non-commercial purposes. Users must comply with the terms of service and license agreements of Rockstar Games.


## Building

1. Verify that you have the C++ and C# packages installed in Visual Studio 2022.

2. Download [MY FORK of the plugin-sdk](https://github.com/Tornamic/plugin-sdk) and install it using [THIS INSTRUCTION](https://github.com/DK22Pac/plugin-sdk/wiki/Set-up-plugin-sdk). Set up your GTA-SA and plugin-sdk folders.

3. Install the enet library using the following instructions:

```ps
# Open PowerShell:

# Install vcpkg if you don't have it
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat

# Install enet
./vcpkg install enet
./vcpkg integrate install
./vcpkg install enet:x86-windows
./vcpkg integrate install
```

4. Open project_files/CoopAndreas.sln in Visual Studio 2022 and build every project (Ctrl+Shift+B).

## Donate
TODO

Contact information is current as of January 1970

## TODO list:
### Already Done ✓
- [X] setup C/C++ project
- [X] client - server connection
- [X] On foot sync -- in progress
  - [x] Walk/Run
  - [x] Jump
  - [x] Climb
  - [x] Crouch - (Fix Crouch Desync)
  - [x] Hit/Fight
- [X] Weapon sync
  - [X] Hold weapon
  - [X] Aim sync
  - [X] Lag shot sync - using keys, bad accuracy
  - [X] Shot sync - every bullet will be synced
- [X] Players info
  - [X] Health bar
  - [X] Armour bar
  - [X] Nickname
  - [X] weapon icon
- [X] define which player is the host
- [X] explosion sync
- [X] fix interior enter exit crash (fix CPlayerPed dctor)
- [X] players AI is activated when you pause in a vehicle
- [X] vehicle sync
  - [X] spawn/delete
  - [X] enter/exit
  - [X] color(1/2) sync
  - [X] paintjob sync
  - [X] tuning sync
  - [X] hydra thrusters
  - [X] plane wheels
  - [X] turret aim/shot
  - [X] horn
  - [X] keys (provides steer angles, brake, etc.)
  - [X] pos, speed, quat
  - [X] health
  - [X] damage status
  - [X] doors 
	- [X] locked state
  - [X] engine state
- [X] chat
- [X] time
- [X] weather
- [X] proper key sync
- [X] rendering
  - [X] text rendering (dx)
  - [X] sprite/txd rendering

### Current Tasks
- [ ] separate ped sync
- [ ] stream in/out players, peds, vehicles, etc.
- [ ] use original main.scm
- [ ] fix weapon selection
- [ ] force crouch sync using CTask
- [ ] refactor packet system (make abstract, more oop, etc.)
- [ ] vehicle sync
  - [ ] force hydraulics sync
  - [ ] trailer sync
- [ ] passenger sync
  - [ ] gamepad support
  - [ ] proper seat sync
  - [ ] radio sync
  - [ ] drive by shooting
- [ ] fixes
  - [X] mouse
  - [ ] widescreen
  - [X] fast load
    - [X] disable logos
    - [X] disable intro
- [ ] Fix models loading (green polygon)  --- related to stream it/out
- [ ] Players map sync
  - [ ] Areas aka GangZones
  - [ ] Mission icons
  - [X] Player map pin
    - [ ] fix proportion
  - [X] Player mark (waypoint)
- [ ] smooth interpolation
  - [ ] move
  - [ ] rotation
- [ ] wanted level
- [ ] stats sync
  - [ ] fat
  - [ ] muscle
  - [ ] weapon skills
  - [ ] fight styles
  - [ ] sync money
  - [ ] breath level bar
  - [ ] stamina sync
  - [ ] max hp sync
- [ ] style
  - [ ] tatoo
  - [ ] clothes
  - [ ] haircut
- [ ] npc sync
  - [X] pos, rot, speed
  - [X] weapons
  - [ ] aim
  - [ ] shots
  - [ ] task sync (good luck, warrior!)
  - [ ] radar icon?
  - [ ] speech sync (if not synced by tasks)
- [ ] fire sync
- [ ] cutscenes
  - [ ] objects
  - [ ] effects
  - [ ] peds
  - [ ] camera
- [ ] fix weapon desync
  - [X] fix grenade throw distance
  - [ ] fix rpg aim state
- [ ] cheat code sync
- [ ] pickups
  - [ ] graffities, horseshoes, snapshots, oysters
  - [ ] static weapons, armours
  - [ ] static bribes
  - [ ] drop
    - [ ] money
    - [ ] weapons
- [ ] jetpack sync
  - [X] flight 
  - [ ] pickup (related to task above) 
- [ ] anim sync
  - [ ] sprunk drinking
  - [ ] fast food eating
  - [ ] idle anims
  - [ ] funny TAB+NUM4 (or NUM6) anim sync (did you know about this?)
- [ ] gang groups sync
- [ ] player voice commands
- [ ] chat reactions (see LD_CHAT.txd)
- [ ] activities
  - [ ] pool 
  - [ ] basketball
  - [ ] casino sync (betting between players)
    - [ ] slot machines
    - [ ] roulette
    - [ ] horse betting
  - [ ] gym
    - [ ] exercises
    - [ ] fighting between players
  - [ ] dancing at parties
- [ ] gand wars sync
- [ ] parachute jump sync
- [ ] co-op vigilante sub-mission completing
### Minor tasks and ideas
- [ ] chat gamepad support with on-screen keyboard
- [ ] Sync laser sniper rifle red dot with all players
- [ ] Sync moon sniper rifle shot changing size easter egg with all players
- [ ] hot coffee implementation 😏
## TODO Launcher:
### Already Done ✓
- [X] setup project and create forms
- [X] inject all dll in the game (don't need ASI Loader anymore)
- [X] send params (nickname, ip/port) to client dll
- [X] implement translation
### Current Tasks
- [ ] start/control server directly from launcher
- [ ] save configs
