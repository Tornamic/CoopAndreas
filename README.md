# CoopAndreas

Videos, pictures, news, suggestions, and communication can be found here:

[Discord Server](https://discord.gg/TwQsR4qxVx)

## Building

1. Verify that you have the C++ and C# packages installed in Visual Studio 2022.

2. Download [THIS version of the plugin-sdk](https://github.com/DK22Pac/plugin-sdk/tree/380105ec5242cdb3a0cbc18505b151308dd85b53) and install it using [THIS INSTRUCTION](https://github.com/DK22Pac/plugin-sdk/wiki/Set-up-plugin-sdk). Set up your GTA-SA and plugin-sdk folders.

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
  
### Current Tasks
- [ ] vehicle sync
  - [X] spawn/delete
  - [X] enter/exit
  - [ ] color(1/2) sync
  - [ ] paintjob sync
  - [ ] tuning sync
  - [ ] hydraulics
  - [ ] plane wheels
  - [ ] turret aim/shot
  - [ ] horn
  - [X] keys (provides steer angles, brake, etc.)
  - [X] pos, speed, quat
  - [ ] health
  - [ ] damage status
  - [ ] trailer sync
  - [ ] door, engine states
- [ ] rendering
  - [X] text rendering (dx)
  - [ ] imgui integration
  - [X] sprite/txd rendering
- [ ] fixes
  - [X] mouse
  - [ ] widescreen
  - [ ] fast load
    - [ ] disable logos
    - [ ] disable intro
- [ ] Fix models loading (green polygon)
- [ ] Players map sync
  - [ ] Areas aka GangZones
  - [ ] Mission icons
  - [X] Player map pin
  - [X] Player mark (waypoint)
- [ ] chat
- [ ] smooth interpolation
  - [ ] move
  - [ ] rotation
- [ ] hud sync
  - [ ] popup messages
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
- [ ] passenger sync
- [ ] npc sync
  - [ ] pos, rot, speed
  - [ ] weapons
  - [ ] aim
  - [ ] shots
  - [ ] path
  - [ ] action
  - [ ] radar icon?
- [ ] fire sync
- [ ] explosion sync
- [ ] time
- [ ] weather
- [ ] cutscenes
  - [ ] objects
  - [ ] effects
  - [ ] peds
  - [ ] camera
- [ ] fix weapon desync
  - [ ] fix grenade throw distance
  - [ ] fix rpg aim state
- [ ] fix interior enter exit crash (fix CPlayerPed dctor)
- [ ] cheat code sync
- [ ] pickup (collectables) sync

### Minor tasks and ideas
- [ ] Sync laser sniper rifle red dot with all players
- [ ] Sync moon sniper rifle shot changing size easter egg with all players

## TODO Launcher:
### Already Done ✓
- [X] setup project and create forms
- [X] inject all dll in the game (don't need ASI Loader anymore)
- [X] send params (nickname, ip/port) to client dll
- [X] implement translation
### Current Tasks
- [ ] start/control server directly from launcher
- [ ] save configs
