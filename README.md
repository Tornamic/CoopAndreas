# CoopAndreas
TODO list:
- [X] setup C/C++ project
- [X] client - server connection
- [ ] on foot sync   —   in progress
- [ ] vehicle sync
- [ ] chat
- [ ] trailer sync
- [ ] npc sync
- [ ] other syncs (like fire, birds, etc.)
- [ ] mission adaptation

## Notes for me

- [ ] написать спавн игроков
функции, добавление игроков в список есть
при подключении буду отправлять пакет всем 
игрокам пакет какой нить по типу PACKET_CONNECTED

- [ ] определить что синхронизировать
легкий способ кидать сразу весь класс CPlayerPed игрока 
но тут могут быть всякие конфликты с локальным игроком

или

отправлять примерно как в сампе
позицию, скорость, поворот, клавиши (в сампе они там пакуються в один байт вроде)
попробовать отправлять весь CControllerState

