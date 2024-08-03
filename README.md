# CoopAndreas

Videos, pictures, news, suggestions, and communication can be found here:

[Discord Server](https://discord.gg/TwQsR4qxVx)

used plugin-sdk commit https://github.com/DK22Pac/plugin-sdk/tree/380105ec5242cdb3a0cbc18505b151308dd85b53

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
  
### Current Tasks
- [ ] define which player is the host
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
- [ ] style
  - [ ] tatoo
  - [ ] clothes
  - [ ] haircut
- [ ] vehicle sync
  - [ ] pos, speed, quat
  - [ ] health
  - [ ] damage status
  - [ ] trailer sync
  - [ ] door, engine states
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
