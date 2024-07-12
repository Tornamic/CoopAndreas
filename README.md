# CoopAndreas
TODO list:
- [X] setup C/C++ project
- [X] client - server connection
- [ ] on foot sync   —   in progress
- [ ] define which player is the host
- [ ] vehicle sync
- [ ] chat
- [ ] trailer sync
- [ ] npc sync
- [ ] other syncs (like fire, birds, etc.)
- [ ] mission adaptation

TODO Launcher:
- [X] setup project and create forms
- [X] inject all dll in the game (don't need ASI Loader anymore)
- [ ] send params (nickname, ip/port) to client dll
- [ ] start/control server directly from launcher
- [ ] implement translation
- [ ] save configs

## Notes for me

- [X] написать спавн игроков

функции, добавление игроков в список есть
при подключении буду отправлять пакет всем 
игрокам пакет какой нить по типу PACKET_CONNECTED

- [X] определить что синхронизировать

легкий способ кидать сразу весь класс CPlayerPed игрока 
но тут могут быть всякие конфликты с локальным игроком

или

отправлять примерно как в сампе
позицию, скорость, поворот, клавиши (в сампе они там пакуються в один байт вроде)
попробовать отправлять весь CControllerState

- [ ] написать синхру пешком

сейчас написал спавн, получение и кривую отправку которую буду завтра фиксить, крч при получении чего либо из FindPlayerped или при вызове его происходит краш, определить от чего крашит и пофиксить, написать базовое передвижение

