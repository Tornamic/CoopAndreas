# CoopAndreas

Videos, pictures, news, suggestions, and communication can be found here:

[![Discord](https://img.shields.io/badge/Discord-5865F2?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/TwQsR4qxVx)
[![X](https://img.shields.io/badge/X-000000?style=for-the-badge&logo=x&logoColor=white)](https://x.com/CoopAndreas)
[![YouTube](https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white)](https://www.youtube.com/@CoopAndreas)
[![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=for-the-badge&logo=telegram&logoColor=white)](https://t.me/coopandreas)
[![VK](https://img.shields.io/badge/VK-0077FF?style=for-the-badge&logo=vk&logoColor=white)](https://vk.com/coopandreas)
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

## Donate
USDT TRC20: `TDGUyPCVZTXCgViGXkr2pmYQtwAPScgWJb`

TON: `UQAYxd6PNRBQ2O24vzTDp5NI6snN328y5YgiKIdHD2kHYyhq`

BTC: `bc1q7srx00mcxevz8j77xczett4cgmld8pd6sv8rda`

ETH: `0x9798744754f7c3b0a3370444026Db3DB96b7b459`

Contact information is current as of September 2024

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

### known bugs (highest priority)
- [ ] players AI is activated when you pause in a vehicle
- [ ] vehicle add-remove packet spam
- [ ] crashes when closing game caused by different things
- [ ] sometimes closing pause menu game cant load nametag weapon icon

### Current Tasks
- [ ] proper key sync
- [ ] vehicle sync
  - [X] spawn/delete
  - [X] enter/exit
  - [X] color(1/2) sync
  - [X] paintjob sync
  - [X] tuning sync
  - [ ] hydraulics
  - [X] hydra thrusters
  - [X] plane wheels
  - [ ] turret aim/shot
  - [X] horn
  - [X] keys (provides steer angles, brake, etc.)
  - [X] pos, speed, quat
  - [X] health
  - [X] damage status
  - [ ] trailer sync
  - [ ] door, engine states
  - [ ] passenger sync
    - [ ] radio sync
    - [ ] drive by shooting
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
- [ ] npc sync
  - [ ] pos, rot, speed
  - [ ] weapons
  - [ ] aim
  - [ ] shots
  - [ ] path
  - [ ] action
  - [ ] radar icon?
- [ ] fire sync
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
- [ ] cheat code sync
- [ ] pickup (collectables) sync
- [ ] jetpack sync
- [ ] idle anim sync

### ChatGPT Generated tasks n ideas:
- [ ] minigames sync
  - [ ] pool
  - [ ] bowling (chatgppt what????)
  - [ ] arm wrestling (wtf chatgpt???)
- [ ] casino sync
  - [ ] slot machines
  - [ ] roulette
  - [ ] horse betting
- [ ] territory wars sync
  - [ ] exact status synchronization of captured territories for all players
  - [ ] common signals for starting and ending gang wars
- [ ] animation sync
  - [ ] gestures/dances at parties
  - [ ] joint workouts at the gym (weightlifting, boxing)
- [ ] taxi sync
  - [ ] shared taxi ride with another player
  - [ ] proper fare splitting among all passengers
- [ ] shops sync
  - [ ] ammu-nation purchases sync
  - [ ] fast food purchases sync (Cluckin' Bell, Burger Shot)
  - [ ] clothing stores sync (Binco, SubUrban, etc.)
- [ ] driving schools sync
  - [ ] joint completion of driving tests at driving schools
- [ ] parachute training sync
  - [ ] parachute jump sync from tall buildings or planes
  
### Minor tasks and ideas
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
