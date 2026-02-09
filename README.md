# CoopAndreas
[![Made in Ukraine](https://img.shields.io/badge/made_in-ukraine-ffd700.svg?labelColor=0057b7)](https://stand-with-ukraine.pp.ua)

Videos, pictures, news, suggestions, and communication can be found here:

[![Discord](https://img.shields.io/badge/Discord%20Server%20(most%20active)-5865F2?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/Z3ugSgFJMU)
[![YouTube](https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white)](https://www.youtube.com/@CoopAndreas)
[![X](https://img.shields.io/badge/X-000000?style=for-the-badge&logo=x&logoColor=white)](https://x.com/CoopAndreas)
[![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=for-the-badge&logo=telegram&logoColor=white)](https://t.me/coopandreas)
## Disclaimer
This mod is an unofficial modification for **Grand Theft Auto: San Andreas** and requires a legitimate copy of the game to function. No original game files or assets from Rockstar Games are included in this repository, and all content provided is independently developed. The project is not affiliated with Rockstar Games or Take-Two Interactive. All rights to the original game, its assets, and intellectual property belong to Rockstar Games and Take-Two Interactive. This mod is created solely for educational and non-commercial purposes. Users must comply with the terms of service and license agreements of Rockstar Games.


## Building (Windows)

### Client & Server

1. Make sure you have the **C++ package** installed in **Visual Studio 2022** and **[xmake](https://xmake.io/)** (needed to build the project).

2. Download **[this version of plugin-sdk](https://github.com/DK22Pac/plugin-sdk/tree/050d18b6e1770477deab81a40028a40277583d97)** and install it using **[this instruction](https://github.com/DK22Pac/plugin-sdk/wiki/Set-up-plugin-sdk)**.  
   Set up your GTA-SA and plugin-sdk folders, and make sure `GTA_SA_DIR` and `PLUGIN_SDK_DIR` environment variables are set.

3. Open a terminal in the repository root and build the projects:

```bash
# Build client DLL
xmake --build client

# Build server binary
xmake --build server
```
If `GTA_SA_DIR` is set, the client DLL (`CoopAndreasSA.dll`) will automatically be placed in your game directory.

---

### Proxy (DLL Loader)

The proxy is required to load (inject) the main DLL (`CoopAndreasSA.dll`) into the game executable.

1. Go to your game directory and rename `eax.dll` to `eax_orig.dll`.

2. Build the **Proxy** project:

```bash
xmake --build proxy
```

3. Rename the output DLL to `eax.dll` and copy it to your game folder.

---

### `main.scm`

1. Download and install **[Sanny Builder 4](https://github.com/sannybuilder/dev/releases)**.

2. Copy all files from the `sdk/Sanny Builder 4/` directory to the **Sanny Builder 4 installation folder**.  
   This will add **CoopAndreas opcodes** to the compiler.

3. Open `scm/main.txt` in **Sanny Builder 4**, compile it, and copy all generated files to  
   `${GTA_SA_DIR}/CoopAndreas/`.

---

### Running

1. Run `server.exe` it will start accepting client connections.
2. Run `gta_sa.exe`, press **Start Game**, enter your nickname, then provide the server IP and port.
   - If the server is running on the same machine, use `127.0.0.1`
   - Default port: `6767`


## Building (GNU/Linux)

### Server

TODO

## Donate
https://send.monobank.ua/jar/8wPrs73MBa

USDT TRC20: `TNdTwiy9JM2zUe8qgBoMJoAExKf4gs5vGA`

BTC: `bc1qwsl8jv2gyvry75j727qkktr5vgcmqm5e69qt2t`

ETH: `0xE7aE0448A147844208C9D51b0Ac673Bafbe2a35c`

PayPal `kirilltymoshchenko59@gmail.com`

*If you need another way to donate, please dm me on discord: `@tornamic`*

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
- [X] separate ped sync
- [X] EnEx markers sync
- [X] mission markers sync
  
### Current Tasks
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
- [ ] cutscenes
  - [X] cutscenes
  - [ ] vote to skip
- [ ] Players map sync
  - [ ] Areas aka GangZones
  - [X] Mission icons
  - [X] Player map pin
    - [ ] fix proportion
  - [X] Player mark (waypoint)
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
- [ ] fire sync
- [ ] cheat code sync
- [ ] anim sync
  - [X] sprunk drinking
  - [X] fast food eating
  - [ ] idle anims
  - [ ] funny TAB+NUM4 (or NUM6) anim sync (did you know about this?)
- [ ] gang groups sync
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
- [ ] smooth interpolation
  - [ ] move
  - [ ] rotation
  - [X] weapon aim interpolation
- [ ] npc sync
  - [X] pos, rot, speed
  - [X] weapons
  - [ ] in vehicle sync
    - [ ] driver
      - [X] position velocity rotation  
      - [X] gas/break lights
      - [X] wheel movement
      - [ ] horn
      - [ ] siren
      - [X] current path, target entity, mission
    - [X] passenger
  - [ ] aim
  - [ ] shots
  - [ ] task sync (good luck, warrior!)
  - [X] radar icon?
  - [X] speech sync
- [ ] player voice commands
- [ ] chat reactions (see LD_CHAT.txd)
- [ ] gang wars sync
- [ ] parachute jump sync
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

## TODO Missions
### Already Done ✓
### Current Tasks
- [X] Big Smoke
- [X] Ryder
- [X] Tagging Up Turf
- [ ] Cleaning The Hood
- [ ] Drive-Thru
- [ ] Nines And AK's
- [ ] Drive-By
- [ ] Sweet's Girl
- [ ] Cesar Vialpando
- [ ] OG Loc
- [ ] Running Dog
- [ ] Wrong Side Of The Tracks
- [ ] Just Business
- [ ] Home Invasion
- [ ] Catalyst
- [ ] Robbing Uncle Sam
- [ ] Life's A Beach
- [ ] Madd Dogg's Rhymes
- [ ] Management Issues
- [ ] House Party
- [ ] High Stakes, Low Rider
- [ ] Burning Desire
- [ ] Gray Imports
- [ ] Doberman
- [ ] Los Sepulcros
- [ ] Reuniting The Families
- [ ] The Green Sabre
- [ ] Badlands
- [ ] Tanker Commander
- [ ] Body Harvest
- [ ] Local Liquor Store
- [ ] Against All Odds
- [ ] Small Town Bank
- [ ] Wu Zi Mu
- [ ] Farewell, My Love...
- [ ] Are You Going To San Fierro?
- [ ] Wear Flowers In Your Hair
- [ ] 555 WE TIP
- [ ] Deconstruction
- [ ] Air Raid
- [ ] Supply Lines...
- [ ] New Model Army
- [ ] Photo Opportunity
- [ ] Jizzy
- [ ] T-Bone Mendez
- [ ] Mike Toreno
- [ ] Outrider
- [ ] Snail Trail
- [ ] Ice Cold Killa
- [ ] Pier 69
- [ ] Toreno's Last Flight
- [ ] Mountain Cloud Boys
- [ ] Ran Fa Li
- [ ] Lure
- [ ] Amphibious Assault
- [ ] The Da Nang Thang
- [ ] Yay Ka-Boom-Boom
- [ ] Zeroing In
- [ ] Test Drive
- [ ] Customs Fast Track
- [ ] Puncture Wounds
- [ ] Monster
- [ ] Highjack
- [ ] Interdiction
- [ ] Verdant Meadows
- [ ] N.O.E.
- [ ] Stowaway
- [ ] Black Project
- [ ] Green Goo
- [ ] Fender Ketchup
- [ ] Explosive Situation
- [ ] You've Had Your Chips
- [ ] Don Peyote
- [ ] Architectural Espionage
- [ ] Key To Her Heart
- [ ] Dam And Blast
- [ ] Cop Wheels
- [ ] Up, Up And Away!
- [ ] Intensive Care
- [ ] The Meat Business
- [ ] Fish In A Barrel
- [ ] Misappropriation
- [ ] Madd Dogg
- [ ] Freefall
- [ ] High Noon
- [ ] Saint Mark's Bistro
- [ ] A Home In The Hills
- [ ] Vertical Bird
- [ ] Home Coming
- [ ] Beat Down On B Dup
- [ ] Grove 4 Life
- [ ] Cut Throat Business
- [ ] Riot
- [ ] Los Desperados
- [ ] End Of The Line

## TODO Other Scripts
### Already done ✓
### Current Tasks
- [ ] Property purchase sync
- [ ] Submissions
  - [ ] Taxi driver
  - [ ] Firefighter
  - [ ] Vigilante
  - [ ] Paramedic
  - [ ] Pimp
  - [ ] Freight Train Missions
- [ ] Hiden races
  - [ ] BMX
  - [ ] NRG-500
  - [ ] The Chiliad Challenge 
- [ ] Stadions
  - [ ] 8-Track
  - [ ] Blood Bowl
  - [ ] Dirt Track
  - [ ] Kick Start
- [ ] Ammu-nation challenge
- [ ] Schools
  - [ ] Driving school
  - [ ] Flight school
  - [ ] Bike school
  - [ ] Boat school
- [ ] Asset Missions
  - [ ] Trucker (8 missions)
  - [ ] Valet (5 missions)
  - [ ] Career (7 missions)
- [ ] Courier
  - [ ] Los Santos - Roboi's Food Mart
  - [ ] San Fierro - Hippy Shopper
  - [ ] Las Venturas - Burger Shot
- [ ] Street Racing (22)
  - [ ] Little Loop
  - [ ] Backroad Wanderer
  - [ ] City Circuit
  - [ ] Vinewood
  - [ ] Freeway
  - [ ] Into The Country
  - [ ] Dirtbike Danger
  - [ ] Bandito Country
  - [ ] Go-Go Karting
  - [ ] San Fierro Fastlane
  - [ ] San Fierro Hills
  - [ ] Country Endurance
  - [ ] SF To LV
  - [ ] Dam Rider
  - [ ] Desert Tricks
  - [ ] LV Ringroad
  - [ ] World War Ace
  - [ ] Barnstorming
  - [ ] Win Military Service
  - [ ] Chopper Checkpoint
  - [ ] Whirly Bird Waypoint
  - [ ] Heli Hell
- [ ] Import / Export
