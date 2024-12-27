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

### Client, Server, Launcher (Windows)
1. Verify that you have the C++ and C# packages installed in Visual Studio 2022.

2. Download [THIS version of the plugin-sdk](https://github.com/DK22Pac/plugin-sdk/tree/050d18b6e1770477deab81a40028a40277583d97) and install it using [THIS INSTRUCTION](https://github.com/DK22Pac/plugin-sdk/wiki/Set-up-plugin-sdk). Set up your GTA-SA and plugin-sdk folders.

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


### Server (GNU/Linux)
#### Requirements 
* GCC or LLVM Compilers 
* GNU Make or Ninja  
* Enet library (no need because coopandreas uses latest custom enet library in repo , check ``build`` and ``thirdparty-libraries`` folder 

#### Building using CMake 
copy and paste the commands below in your terminal :  
```bash
mkdir build
cd build
cmake -S .. -B .
make # or ninja if you use -G "Ninja" parameter in cmake
./CoopAndreasServer
``` 

### main.scm

1. Download and install [Sanny Builder 3](https://libertycity.net/files/gta-san-andreas/140792-sanny-builder-v3.3.3.html) (**SB4** compatibility is uncertain)

2. Move all files from the `sdk/Sanny Builder 3/` folder to the SB3 installation folder. This will add CoopAndreas's opcodes to the compiler

3. Open `scm/main.txt` with **Sanny Builder 3**, compile it, and then move all output files to the `${GTA_SA_DIR}/CoopAndreas/`

## Donate
https://send.monobank.ua/jar/8wPrs73MBa

*If you need another way to donate, please dm me on discord: `@tornamic`*

Contact information is current as of December 2024

## TODO list:
### Already Done ✓
- [X] setup C/C++ project
- [X] client - server connection
- [X] On foot sync
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
- [X] explosion sync
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
- [X] style
  - [X] tatoo
  - [X] clothes
  - [X] haircut
  
### Current Tasks
- [ ] separate ped sync
- [ ] stream in/out players, peds, vehicles, etc.
- [ ] vehicle sync
  - [ ] force hydraulics sync
  - [ ] trailer sync
- [ ] passenger sync
  - [ ] gamepad support
  - [X] proper seat sync
  - [ ] radio sync
  - [X] drive by shooting
- [ ] fixes
  - [X] mouse
  - [ ] widescreen
  - [X] fast load
- [ ] Fix models loading (green polygon)  --- related to stream it/out
- [ ] Players map sync
  - [ ] Areas aka GangZones
  - [ ] Mission icons
  - [X] Player map pin
    - [ ] fix proportion
  - [X] Player mark (waypoint)
- [ ] smooth interpolation
  - [ ] move
  - [X] rotation
  - [X] weapon aim interpolation
- [ ] wanted level
- [ ] stats sync
  - [X] fat
  - [X] muscle
  - [X] weapon skills
  - [X] fight styles
  - [ ] sync money
  - [ ] breath level bar
  - [X] stamina sync
  - [ ] max hp sync
- [ ] npc sync
  - [X] pos, rot, speed
  - [X] weapons
  - [ ] in vehicle sync
    - [ ] driver
      - [X] position velocity rotation  
      - [X] gas/break lights
      - [ ] wheel movement
      - [ ] horn
      - [ ] siren
      - [ ] current path, target entity, mission
    - [X] passenger
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
- [ ] stunt
  - [ ] collecting
  - [ ] for-player slow motion
- [ ] chat gamepad support with on-screen keyboard
### Minor tasks and ideas
- [ ] Sync laser sniper rifle red dot with all players
- [ ] Sync moon sniper rifle shot changing size easter egg with all players
## TODO Launcher:
### Already Done ✓
- [X] setup project and create forms
- [X] inject all dll in the game (don't need ASI Loader anymore)
- [X] send params (nickname, ip/port) to client dll
- [X] implement translation
- [X] save configs
### Current Tasks
- [ ] start/control server directly from launcher
