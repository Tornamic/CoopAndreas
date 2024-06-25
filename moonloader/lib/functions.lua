--[[ Opcodes
shakeCam(int shake)  -- 0003
Player player = createPlayer(Model modelId, float atX, float atY, float atZ)  -- 0053
Ped ped = createChar(int pedtype, Model modelId, float atX, float atY, float atZ)  -- 009A
deleteChar(Ped ped)  -- 009B
float positionX, float positionY, float positionZ = getCharCoordinates(Ped ped)  -- 00A0
setCharCoordinates(Ped ped, float posX, float posY, float posZ)  -- 00A1
bool result = isCharInArea2d(Ped ped, float cornerAX, float cornerAY, float cornerBX, float cornerBY, bool sphere)  -- 00A3
bool result = isCharInArea3d(Ped ped, float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ, bool sphere)  -- 00A4
Vehicle car = createCar(Model modelId, float atX, float atY, float atZ)  -- 00A5
deleteCar(Vehicle car)  -- 00A6
carGotoCoordinates(Vehicle car, float driveToX, float driveToY, float driveToZ)  -- 00A7
carWanderRandomly(Vehicle car)  -- 00A8
carSetIdle(Vehicle car)  -- 00A9
float positionX, float positionY, float positionZ = getCarCoordinates(Vehicle car)  -- 00AA
setCarCoordinates(Vehicle car, float atX, float atY, float atZ)  -- 00AB
setCarCruiseSpeed(Vehicle car, float maxSpeed)  -- 00AD
setCarDrivingStyle(Vehicle car, int behaviour)  -- 00AE
setCarMission(Vehicle car, int driverBehaviour)  -- 00AF
bool result = isCarInArea2d(Vehicle car, float cornerAX, float cornerAY, float cornerBX, float cornerBY, bool sphere)  -- 00B0
bool result = isCarInArea3d(Vehicle car, float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ, bool sphere)  -- 00B1
printBig(GxtString gxtString, int time, int style)  -- 00BA
printText(GxtString gxtString, int time, int flag)  -- 00BB
printTextNow(GxtString gxtString, int time, int flag)  -- 00BC
clearPrints()  -- 00BE
int hours, int mins = getTimeOfDay()  -- 00BF
setTimeOfDay(int hours, int minutes)  -- 00C0
int minutes = getMinutesToTimeOfDay(int hours, int minutes)  -- 00C1
bool result = isPointOnScreen(float sphereX, float sphereY, float sphereZ, float radius)  -- 00C2
Vehicle car = storeCarCharIsIn(Ped ped)  -- 00D9
bool result = isCharInCar(Ped ped, Vehicle car)  -- 00DB
bool result = isCharInModel(Ped ped, Model carModel)  -- 00DD
bool result = isCharInAnyCar(Ped ped)  -- 00DF
bool result = isButtonPressed(Player player, int key)  -- 00E1
int state = getPadState(Player player, int key)  -- 00E2
bool result = locateCharAnyMeans2d(Ped ped, float pointX, float pointY, float radiusX, float radiusY, bool sphere)  -- 00EC
bool result = locateCharOnFoot2d(Ped ped, float pointX, float pointY, float radiusX, float radiusY, bool sphere)  -- 00ED
bool result = locateCharInCar2d(Ped ped, float pointX, float pointY, float radiusX, float radiusY, bool sphere)  -- 00EE
bool result = locateStoppedCharAnyMeans2d(Ped ped, float pointX, float pointY, float radiusX, float radiusY, bool sphere)  -- 00EF
bool result = locateStoppedCharOnFoot2d(Ped ped, float pointX, float pointY, float radiusX, float radiusY, bool sphere)  -- 00F0
bool result = locateStoppedCharInCar2d(Ped ped, float pointX, float pointY, float radiusX, float radiusY, bool sphere)  -- 00F1
bool result = locateCharAnyMeansChar2d(Ped ped, Ped nearPed, float radiusX, float radiusY, bool sphere)  -- 00F2
locateCharOnFootChar2d(Ped ped, Ped nearPed, float radiusX, float radiusY, bool sphere)  -- 00F3
bool result = locateCharInCarChar2d(Ped ped, Ped nearPed, float radiusX, float radiusY, bool sphere)  -- 00F4
bool result = locateCharAnyMeans3d(Ped ped, float sphereX, float sphereY, float sphereZ, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 00FE
bool result = locateCharOnFoot3d(Ped ped, float sphereX, float sphereY, float sphereZ, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 00FF
bool result = locateCharInCar3d(Ped ped, float sphereX, float sphereY, float sphereZ, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 0100
bool result = locateStoppedCharAnyMeans3d(Ped ped, float sphereX, float sphereY, float sphereZ, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 0101
bool result = locateStoppedCharOnFoot3d(Ped ped, float sphereX, float sphereY, float sphereZ, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 0102
bool result = locateStoppedCharInCar3d(Ped ped, float sphereX, float sphereY, float sphereZ, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 0103
bool result = locateCharAnyMeansChar3d(Ped ped, Ped nearPed, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 0104
bool result = locateCharOnFootChar3d(Ped ped, Ped nearPed, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 0105
bool result = locateCharInCarChar3d(Ped ped, Ped nearPed, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 0106
Object object = createObject(Model modelId, float atX, float atY, float atZ)  -- 0107
deleteObject(Object object)  -- 0108
givePlayerMoney(Player player, int money)  -- 0109
int money = getPlayerMoney(Player player)  -- 010B
giveRemoteControlledCarToPlayer(Player player, float float2, float float3, float float4)  -- 010C
alterWantedLevel(Player player, int wantedLevel)  -- 010D
alterWantedLevelNoDrop(Player player, int minimumWantedLevel)  -- 010E
bool result = isWantedLevelGreater(Player player, int level)  -- 010F
clearWantedLevel(Player player)  -- 0110
setDeatharrestState(bool value)  -- 0111
bool result = hasDeatharrestBeenExecuted()  -- 0112
addAmmoToChar(Ped ped, int weapon, int ammo)  -- 0114
bool result = isPlayerDead(Player player)  -- 0117
bool result = isCharDead(Ped ped)  -- 0118
bool result = isCarDead(Vehicle car)  -- 0119
bool result = isPlayerPressingHorn(Player player)  -- 0122
Ped ped = createCharInsideCar(Vehicle car, Model pedtype, int model)  -- 0129
bool result = isCarModel(Vehicle car, Model modelId)  -- 0137
int carGenerator = createCarGenerator(float atX, float atY, float atZ, float angle, Model modelId, int color1, int color2, bool forceSpawn, int alarm, int doorLock, int minDelay, int maxDelay)  -- 014B
switchCarGenerator(int carGenerator, int carsToGenerate)  -- 014C
displayOnscreenTimer(VarId var, bool countInDirection)  -- 014E
clearOnscreenTimer(VarId var)  -- 014F
clearOnscreenCounter(VarId var)  -- 0151
bool result = isCharInZone(Ped ped, GxtString zoneName)  -- 0154
pointCameraAtCar(Vehicle car, int mode, int switchstyle)  -- 0158
pointCameraAtChar(Ped ped, int mode, int switchstyle)  -- 0159
restoreCamera()  -- 015A
shakePad(Player player, int time, int intensity)  -- 015B
setTimeScale(float gamespeed)  -- 015D
setFixedCameraPosition(float positionX, float positionY, float positionZ, float rotationX, float rotationY, float rotationZ)  -- 015F
pointCameraAtPoint(float pointAtX, float pointAtY, float pointAtZ, int switchstyle)  -- 0160
Marker marker = addBlipForCarOld(Vehicle car, int unused, bool visibility)  -- 0161
Marker marker = addBlipForCharOld(Ped ped, int int2, int int3)  -- 0162
removeBlip(Marker marker)  -- 0164
changeBlipColour(Marker marker, int color)  -- 0165
Marker marker = addBlipForCoordOld(float atX, float atY, float atZ, int color, int flag)  -- 0167
changeBlipScale(Marker marker, int size)  -- 0168
setFadingColour(int r, int g, int b)  -- 0169
doFade(bool in, int time)  -- 016A
bool result = getFadingStatus()  -- 016B
addHospitalRestart(float atX, float atY, float atZ, float angle, int townNumber)  -- 016C
addPoliceRestart(float atX, float atY, float atZ, float angle, int townNumber)  -- 016D
overrideNextRestart(float atX, float atY, float atZ, float angle)  -- 016E
drawShadow(Particle particle, float atX, float atY, float atZ, float rotationFactor, float size, int intensity, int flags1, int flags2, int flags3)  -- 016F
float angle = getCharHeading(Ped ped)  -- 0172
setCharHeading(Ped ped, float angle)  -- 0173
float angle = getCarHeading(Vehicle car)  -- 0174
setCarHeading(Vehicle car, float angle)  -- 0175
float angle = getObjectHeading(Object object)  -- 0176
setObjectHeading(Object object, float angle)  -- 0177
bool result = isCharTouchingObject(Ped ped, Object object)  -- 0179
setCharAmmo(Ped ped, int weapon, int ammo)  -- 017B
declareMissionFlag(VarId flag)  -- 0180
Marker marker = addBlipForCar(Vehicle car)  -- 0186
Marker marker = addBlipForChar(Ped ped)  -- 0187
Marker marker = addBlipForObject(Object object)  -- 0188
Checkpoint checkpoint = addBlipForCoord(float atX, float atY, float atZ)  -- 018A
changeBlipDisplay(Marker marker, int mode)  -- 018B
addOneOffSound(float atX, float atY, float atZ, int sound)  -- 018C
int unk = addContinuousSound(float atX, float atY, float atZ, int sound)  -- 018D
removeSound(int sound)  -- 018E
bool result = isCarStuckOnRoof(Vehicle car)  -- 018F
addUpsidedownCarCheck(Vehicle car)  -- 0190
removeUpsidedownCarCheck(Vehicle car)  -- 0191
bool result = isCharInAreaOnFoot2d(Ped ped, float cornerAX, float cornerAY, float cornerBX, float cornerBY, bool sphere)  -- 01A1
bool result = isCharInAreaInCar2d(Ped ped, float cornerAX, float cornerAY, float cornerBX, float cornerBY, bool sphere)  -- 01A2
bool result = isCharStoppedInArea2d(Ped ped, float cornerAX, float cornerAY, float cornerBX, float cornerBY, bool sphere)  -- 01A3
bool result = isCharStoppedInAreaOnFoot2d(Ped ped, float cornerAX, float cornerAY, float cornerBX, float cornerBY, bool sphere)  -- 01A4
bool result = isCharStoppedInAreaInCar2d(Ped ped, float cornerAX, float cornerAY, float cornerBX, float cornerBY, bool sphere)  -- 01A5
bool result = isCharInAreaOnFoot3d(Ped ped, float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ, bool sphere)  -- 01A6
bool result = isCharInAreaInCar3d(Ped ped, float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ, bool sphere)  -- 01A7
bool result = isCharStoppedInArea3d(Ped ped, float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ, bool sphere)  -- 01A8
bool result = isCharStoppedInAreaOnFoot3d(Ped ped, float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ, bool sphere)  -- 01A9
bool result = isCharStoppedInAreaInCar3d(Ped ped, float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ, bool sphere)  -- 01AA
bool result = isCarStoppedInArea2d(Vehicle car, float cornerAX, float cornerAY, float cornerBX, float cornerBY, bool sphere)  -- 01AB
bool result = isCarStoppedInArea3d(Vehicle car, float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ, bool sphere)  -- 01AC
bool result = locateCar2d(Vehicle car, float pointX, float pointY, float radiusX, float radiusY, bool sphere)  -- 01AD
bool result = locateStoppedCar2d(Vehicle car, float pointX, float pointY, float radiusX, float radiusY, bool sphere)  -- 01AE
bool result = locateCar3d(Vehicle car, float sphereX, float sphereY, float sphereZ, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 01AF
bool result = locateStoppedCar3d(Vehicle car, float sphereX, float sphereY, float sphereZ, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 01B0
giveWeaponToChar(Ped ped, int weapon, int ammo)  -- 01B2
bool result = setPlayerControl(Player player, bool canMove)  -- 01B4
bool result = forceWeather(int weather)  -- 01B5
bool result = forceWeatherNow(int weather)  -- 01B6
releaseWeather()  -- 01B7
setCurrentCharWeapon(Ped ped, int weapon)  -- 01B9
bool result, float positionX, float positionY, float positionZ = getObjectCoordinates(Object object)  -- 01BB
bool result = setObjectCoordinates(Object object, float atX, float atY, float atZ)  -- 01BC
int timeMs = getGameTimer()  -- 01BD
bool result, int level = storeWantedLevel(Player player)  -- 01C0
bool result = isCarStopped(Vehicle car)  -- 01C1
markCharAsNoLongerNeeded(Ped ped)  -- 01C2
markCarAsNoLongerNeeded(Vehicle car)  -- 01C3
markObjectAsNoLongerNeeded(Object object)  -- 01C4
dontRemoveChar(Ped ped)  -- 01C5
dontRemoveObject(Object object)  -- 01C7
bool result, Ped ped = createCharAsPassenger(Vehicle car, Model pedtype, int model, int passengerSeat)  -- 01C8
bool result = printWithNumberBig(GxtString gxtString, int number, int time, int style)  -- 01E3
bool result = printWithNumber(GxtString gxtString, int number, int time, int flag)  -- 01E4
bool result = printWithNumberNow(GxtString gxtString, int number, int time, int flag)  -- 01E5
bool result = switchRoadsOn(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 01E7
switchRoadsOff(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 01E8
bool result, int passengers = getNumberOfPassengers(Vehicle car)  -- 01E9
int maxPassengers = getMaximumNumberOfPassengers(Vehicle car)  -- 01EA
bool result = setCarDensityMultiplier(float multiplier)  -- 01EB
bool result = setCarHeavy(Vehicle car, bool heavy)  -- 01EC
setMaxWantedLevel(int level)  -- 01F0
bool result = isCarInAirProper(Vehicle car)  -- 01F3
bool result = isCarUpsidedown(Vehicle car)  -- 01F4
bool result, Ped ped = getPlayerChar(Player player)  -- 01F5
bool result = cancelOverrideRestart()  -- 01F6
bool result = setPoliceIgnorePlayer(Player player, bool ignored)  -- 01F7
bool result = startKillFrenzy(GxtString gxtString, int weapon, int timeLimit, int targets, Model targetModels1, Model targetModels2, Model targetModels3, Model targetModels4, bool completedText)  -- 01F9
bool result, int status = readKillFrenzyStatus()  -- 01FA
bool result = locateCharAnyMeansCar2d(Ped ped, Vehicle car, float radiusX, float radiusY, bool sphere)  -- 0202
bool result = locateCharOnFootCar2d(Ped ped, Vehicle car, float radiusX, float radiusY, bool flag)  -- 0203
bool result = locateCharInCarCar2d(Ped ped, Vehicle car, float radiusX, float radiusY, bool sphere)  -- 0204
bool result = locateCharAnyMeansCar3d(Ped ped, Vehicle car, float radiusX, float radiusY, float radiusZ, bool flag)  -- 0205
bool result = locateCharOnFootCar3d(Ped ped, Vehicle car, float radiusX, float radiusY, float radiusZ, bool flag)  -- 0206
bool result = locateCharInCarCar3d(Ped ped, Vehicle car, float radiusX, float radiusY, float radiusZ, bool flag)  -- 0207
lockCarDoors(Vehicle car, int status)  -- 020A
bool result = explodeCar(Vehicle car)  -- 020B
bool result = addExplosion(float atX, float atY, float atZ, int radius)  -- 020C
bool result = isCarUpright(Vehicle car)  -- 020D
bool result, Pickup pickup = createPickup(Model modelId, int type, float atX, float atY, float atZ)  -- 0213
bool result = hasPickupBeenCollected(Pickup pickup)  -- 0214
bool result = removePickup(Pickup pickup)  -- 0215
bool result = setTaxiLights(Vehicle taxi, bool light)  -- 0216
bool result = printBigQ(GxtString gxtString, int time, int style)  -- 0217
bool result = setTargetCarForMissionGarage(GxtString garage, Vehicle car)  -- 021B
bool result = applyBrakesToPlayersCar(Player player, bool apply)  -- 0221
setCharHealth(Ped ped, int health)  -- 0223
setCarHealth(Vehicle car, int health)  -- 0224
int health = getCharHealth(Ped ped)  -- 0226
int health = getCarHealth(Vehicle car)  -- 0227
bool result = changeCarColour(Vehicle car, int primaryColor, int secondaryColor)  -- 0229
switchPedRoadsOn(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 022A
switchPedRoadsOff(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 022B
setGangWeapons(int gang, int weapons1, int weapons2, int weapons3)  -- 0237
bool result = isCharTouchingObjectOnFoot(Ped ped, Object object)  -- 023B
loadSpecialCharacter(GxtString gxtString, int id)  -- 023C
bool result = hasSpecialCharacterLoaded(int id)  -- 023D
bool result = isPlayerInRemoteMode(Player player)  -- 0241
setCutsceneOffset(float posX, float posY, float posZ)  -- 0244
setAnimGroupForChar(Ped ped, string style)  -- 0245
requestModel(Model modelId)  -- 0247
bool result = hasModelLoaded(Model modelId)  -- 0248
markModelAsNoLongerNeeded(Model modelId)  -- 0249
drawCorona(float atX, float atY, float atZ, float radius, int type, bool lensflares, int r, int g, int b)  -- 024F
storeClock()  -- 0253
restoreClock()  -- 0254
bool result = isPlayerPlaying(Player player)  -- 0256
int mode = getControllerMode()  -- 0293
setCanResprayCar(Vehicle car, bool sprayable)  -- 0294
unloadSpecialCharacter(int id)  -- 0296
resetNumOfModelsKilledByPlayer(Player player)  -- 0297
int quantity = getNumOfModelsKilledByPlayer(Player player, Model modelId)  -- 0298
activateGarage(GxtString garage)  -- 0299
Object object = createObjectNoOffset(Model modelId, float atX, float atY, float atZ)  -- 029B
bool result = isCharStopped(Ped ped)  -- 02A0
switchWidescreen(bool enable)  -- 02A3
Marker marker = addSpriteBlipForContactPoint(float atX, float atY, float atZ, int icon)  -- 02A7
Marker marker = addSpriteBlipForCoord(float atX, float atY, float atZ, int type)  -- 02A8
setCharOnlyDamagedByPlayer(Ped ped, bool enabled)  -- 02A9
setCarOnlyDamagedByPlayer(Vehicle car, bool enabled)  -- 02AA
setCharProofs(Ped ped, bool BP, bool FP, bool EP, bool CP, bool MP)  -- 02AB
setCarProofs(Vehicle car, bool BP, bool FP, bool EP, bool CP, bool MP)  -- 02AC
deactivateGarage(GxtString garage)  -- 02B9
bool result = isCarInWater(Vehicle car)  -- 02BF
float nodeX, float nodeY, float nodeZ = getClosestCharNode(float closestToX, float closestToY, float closestToZ)  -- 02C0
float nodeX, float nodeY, float nodeZ = getClosestCarNode(float closestToX, float closestToY, float closestToZ)  -- 02C1
carGotoCoordinatesAccurate(Vehicle car, float toX, float toY, float toZ)  -- 02C2
bool result = isCarOnScreen(Vehicle car)  -- 02CA
bool result = isCharOnScreen(Ped ped)  -- 02CB
bool result = isObjectOnScreen(Object object)  -- 02CC
float z = getGroundZFor3dCoord(float atX, float atY, float atZ)  -- 02CE
int fire = startScriptFire(float atX, float atY, float atZ, int propagation, int size)  -- 02CF
bool result = isScriptFireExtinguished(int fire)  -- 02D0
removeScriptFire(int fire)  -- 02D1
boatGotoCoords(Vehicle boat, float toX, float toY, float toZ)  -- 02D3
boatStop(Vehicle car)  -- 02D4
bool result = isCharShootingInArea(Ped ped, float cornerAX, float cornerAY, float cornerBX, float cornerBY, int weapon)  -- 02D6
bool result = isCurrentCharWeapon(Ped ped, int weapon)  -- 02D8
setBoatCruiseSpeed(Vehicle boat, float speed)  -- 02DB
Ped ped = getRandomCharInZone(GxtString zone, bool pedtype, bool gang, bool criminal_prostitute)  -- 02DD
bool result = isCharShooting(Ped ped)  -- 02E0
Pickup pickup = createMoneyPickup(float atX, float atY, float atZ, int cash, bool permanenceFlag)  -- 02E1
setCharAccuracy(Ped ped, int accuracy)  -- 02E2
float speed = getCarSpeed(Vehicle car)  -- 02E3
loadCutscene(GxtString cutscene)  -- 02E4
Object object = createCutsceneObject(Model modelId)  -- 02E5
setCutsceneAnim(int cutscene, GxtString anim)  -- 02E6
startCutscene()  -- 02E7
int time = getCutsceneTime()  -- 02E8
bool result = hasCutsceneFinished()  -- 02E9
clearCutscene()  -- 02EA
restoreCameraJumpcut()  -- 02EB
setCollectable1Total(int total)  -- 02ED
bool result = isProjectileInArea(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 02EE
bool result = isCharModel(Ped ped, Model modelId)  -- 02F2
loadSpecialModel(Model modelId, GxtString gxtString)  -- 02F3
float forwardX = getCarForwardX(Vehicle car)  -- 02F8
float forwardY = getCarForwardY(Vehicle car)  -- 02F9
changeGarageType(GxtString garage, int type)  -- 02FA
printWith2NumbersNow(GxtString gxtString, int numbers1, int numbers2, int time, int flag)  -- 02FD
printWith3Numbers(GxtString gxtString, int numbers1, int numbers2, int numbers3, int time, int flag)  -- 02FF
printWith4Numbers(GxtString gxtString, int numbers1, int numbers2, int numbers3, int numbers4, int time, int flag)  -- 0302
printWith4NumbersNow(GxtString gxtString, int numbers1, int numbers2, int numbers3, int numbers4, int time, int flag)  -- 0303
printWith6Numbers(GxtString gxtString, int numbers1, int numbers2, int numbers3, int numbers4, int numbers5, int numbers6, int time, int flag)  -- 0308
playerMadeProgress(int progress)  -- 030C
setProgressTotal(int maxProgress)  -- 030D
registerMissionGiven()  -- 0317
registerMissionPassed(GxtString mission)  -- 0318
removeAllScriptFires()  -- 031A
bool result = hasCharBeenDamagedByWeapon(Ped ped, int weapon)  -- 031D
bool result = hasCarBeenDamagedByWeapon(Vehicle car, int weapon)  -- 031E
explodeCharHead(Ped ped)  -- 0321
anchorBoat(Vehicle boat, bool anchor)  -- 0323
int fire = startCarFire(Vehicle car)  -- 0325
int fire = startCharFire(Ped ped)  -- 0326
Vehicle car = getRandomCarOfTypeInArea(float cornerAX, float cornerAY, float cornerBX, float cornerBY, Model modelId)  -- 0327
bool result = hasResprayHappened(Vehicle car)  -- 0329
setCameraZoom(int mode)  -- 032A
Pickup pickup = createPickupWithAmmo(Model modelId, int type, int ammo, float atX, float atY, float atZ)  -- 032B
setCarRamCar(Vehicle car, Vehicle car)  -- 032C
setPlayerNeverGetsTired(Player player, bool infiniteRun)  -- 0330
setPlayerFastReload(Player player, bool fastReload)  -- 0331
setCharBleeding(Ped ped, bool bleeding)  -- 0332
setFreeResprays(bool enable)  -- 0335
setCharVisible(Ped ped, bool visible)  -- 0337
setCarVisible(Vehicle car, bool visible)  -- 0338
bool result = isAreaOccupied(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ, bool solid, bool car, bool actor, bool object, bool particle)  -- 0339
displayText(float posX, float posY, GxtString gxtString)  -- 033E
setTextScale(float sizeX, float sizeY)  -- 033F
setTextColour(int r, int g, int b, int a)  -- 0340
setTextJustify(bool alignJustify)  -- 0341
setTextCentre(bool centered)  -- 0342
setTextWrapx(float linewidth)  -- 0343
setTextCentreSize(float linewidth)  -- 0344
setTextBackground(bool background)  -- 0345
setTextProportional(bool proportional)  -- 0348
setTextFont(int font)  -- 0349
bool result = rotateObject(Object object, float fromAngle, float toAngle, bool flag)  -- 034D
bool result = slideObject(Object object, float toX, float toY, float toZ, float speedX, float speedY, float speedZ, bool collisionCheck)  -- 034E
removeCharElegantly(Ped ped)  -- 034F
setCharStayInSamePlace(Ped ped, bool enabled)  -- 0350
bool result = isExplosionInArea(int explosionType, float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 0356
placeObjectRelativeToCar(Object object, Vehicle car, float offsetX, float offsetY, float offsetZ)  -- 035C
makeObjectTargettable(Object object, bool targetable)  -- 035D
addArmourToChar(Ped ped, int points)  -- 035F
openGarage(GxtString garage)  -- 0360
closeGarage(GxtString garage)  -- 0361
warpCharFromCarToCoord(Ped ped, float placeAtX, float placeAtY, float placeAtZ)  -- 0362
setVisibilityOfClosestObjectOfType(float atX, float atY, float atZ, float radius, Model modelId, bool visibility)  -- 0363
bool result = hasCharSpottedChar(Ped ped, Ped ped)  -- 0364
bool result = hasObjectBeenDamaged(Object object)  -- 0366
warpCharIntoCar(Ped ped, Vehicle car)  -- 036A
printWith2NumbersBig(GxtString gxtString, int numbers1, int numbers2, int time, int style)  -- 036D
setCameraBehindPlayer()  -- 0373
Ped ped = createRandomChar(float atX, float atY, float atZ)  -- 0376
bool result = isSniperBulletInArea(float float1, float float2, float float3, float float4, float float5, float float6)  -- 037E
setObjectVelocity(Object object, float velocityInDirectionX, float velocityInDirectionY, float velocityInDirectionZ)  -- 0381
setObjectCollision(Object object, bool collision)  -- 0382
printStringInStringNow(GxtString gxtString, GxtString string, int time1, int time2)  -- 0384
bool result = isPointObscuredByAMissionEntity(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 038A
loadAllModelsNow()  -- 038B
addToObjectVelocity(Object object, float velocityX, float velocityY, float velocityZ)  -- 038C
drawSprite(int texture, float positionX, float positionY, float width, float height, int r, int g, int b, int a)  -- 038D
drawRect(float positionX, float positionY, float width, float height, int r, int g, int b, int a)  -- 038E
int id = loadSprite(string name)  -- 038F
bool result = loadTextureDictionary(zstring txd)  -- 0390
removeTextureDictionary()  -- 0391
setObjectDynamic(Object object, bool moveable)  -- 0392
setCharAnimSpeed(Ped ped, string animation, float speed)  -- 0393
playMissionPassedTune(int music)  -- 0394
clearArea(float atX, float atY, float atZ, float radius, bool area)  -- 0395
freezeOnscreenTimer(bool timer)  -- 0396
switchCarSiren(Vehicle car, bool siren)  -- 0397
setCarWatertight(Vehicle car, bool watertight)  -- 039C
setCharCantBeDraggedOut(Ped ped, bool locked)  -- 039E
turnCarToFaceCoord(Vehicle car, float coordX, float coordY)  -- 039F
drawSphere(float atX, float atY, float atZ, float radius)  -- 03A1
setCarStatus(Vehicle car, int action)  -- 03A2
bool result = isCharMale(Ped ped)  -- 03A3
policeRadioMessage(float float1, float float2, float float3)  -- 03AA
setCarStrong(Vehicle car, bool strong)  -- 03AB
switchRubbish(bool int1)  -- 03AD
switchStreaming(bool streaming)  -- 03AF
bool result = isGarageOpen(GxtString garage)  -- 03B0
bool result = isGarageClosed(GxtString garage)  -- 03B1
swapNearestBuildingModel(float atX, float atY, float atZ, float radius, Model from, Model to)  -- 03B6
switchWorldProcessing(bool cutsceneOnly)  -- 03B7
clearAreaOfCars(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 03BA
int sphere = addSphere(float atX, float atY, float atZ, float radius)  -- 03BC
removeSphere(int sphere)  -- 03BD
setEveryoneIgnorePlayer(Player player, bool ignored)  -- 03BF
Vehicle car = storeCarCharIsInNoSave(Ped ped)  -- 03C0
displayOnscreenTimerWithString(VarId timer, int type, GxtString gxtString)  -- 03C3
displayOnscreenCounterWithString(VarId var, bool type, GxtString gxtString)  -- 03C4
createRandomCarForCarPark(float coordsX, float coordsY, float coordsZ, float zAngle)  -- 03C5
setWantedMultiplier(float sensitivity)  -- 03C7
setCameraInFrontOfPlayer()  -- 03C8
bool result = isCarVisiblyDamaged(Vehicle car)  -- 03C9
bool result = doesObjectExist(Object object)  -- 03CA
loadScene(float atX, float atY, float atZ)  -- 03CB
addStuckCarCheck(Vehicle car, float stuckCheckDistance, int time)  -- 03CC
removeStuckCarCheck(Vehicle car)  -- 03CD
bool result = isCarStuck(Vehicle car)  -- 03CE
loadMissionAudio(int asId, int name)  -- 03CF
bool result = hasMissionAudioLoaded(int id)  -- 03D0
playMissionAudio(int id)  -- 03D1
bool result = hasMissionAudioFinished(int id)  -- 03D2
float nodeX, float nodeY, float nodeZ, float angle = getClosestCarNodeWithHeading(float X, float Y, float Z)  -- 03D3
bool result = hasImportGarageSlotBeenFilled(int int1, int int2)  -- 03D4
clearThisPrint(GxtString text)  -- 03D5
clearThisBigPrint(GxtString text)  -- 03D6
setMissionAudioPosition(int id, float locationX, float locationY, float locationZ)  -- 03D7
activateSaveMenu()  -- 03D8
bool result = hasSaveGameFinished()  -- 03D9
noSpecialCameraForThisGarage(int int1)  -- 03DA
Marker marker = addBlipForPickup(Pickup pickup)  -- 03DC
setPedDensityMultiplier(float multiplier)  -- 03DE
setTextDrawBeforeFade(bool int1)  -- 03E0
int collected = getCollectable1sCollected()  -- 03E1
setSpritesDrawBeforeFade(bool antialiased)  -- 03E3
setTextRightJustify(bool alignRight)  -- 03E4
printHelp(GxtString gxtString)  -- 03E5
clearHelp()  -- 03E6
flashHudObject(int hudComponent)  -- 03E7
setGenerateCarsAroundCamera(bool int1)  -- 03EA
clearSmallPrints()  -- 03EB
setUpsidedownCarNotDamaged(Vehicle car, bool disableFlippedExplosion)  -- 03ED
bool result = isPlayerControllable(Player player)  -- 03EE
makePlayerSafe(Player player)  -- 03EF
int primaryColor, int secondaryColor = getCarColours(Vehicle car)  -- 03F3
setAllCarsCanBeDamaged(bool enable)  -- 03F4
setCarCanBeDamaged(Vehicle car, bool enable)  -- 03F5
setDrunkInputDelay(Player player, int handlingResponsiveness)  -- 03FD
setCharMoney(Ped ped, int money)  -- 03FE
float X, float Y, float Z = getOffsetFromObjectInWorldCoords(Object object, float offsetX, float offsetY, float offsetZ)  -- 0400
float X, float Y, float Z = getOffsetFromCarInWorldCoords(Vehicle car, float offsetX, float offsetY, float offsetZ)  -- 0407
clearMissionAudio(int id)  -- 040D
setFreeHealthCare(Player player, bool free)  -- 0414
loadAndLaunchMissionInternal(int mission)  -- 0417
setObjectDrawLast(Object object, bool drawLast)  -- 0418
int ammo = getAmmoInCharWeapon(Ped ped, int int)  -- 041A
setNearClip(float clip)  -- 041D
setRadioChannel(int radioStation)  -- 041E
setCarTraction(Vehicle car, float traction)  -- 0423
bool result = areMeasurementsInMetres()  -- 0424
float feet = convertMetresToFeet(float meters)  -- 0425
setCarAvoidLevelTransitions(Vehicle car, bool avoidLevelTransitions)  -- 0428
clearAreaOfChars(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 042B
setTotalNumberOfMissions(int totalMissions)  -- 042C
int imperial = convertMetresToFeetInt(int metric)  -- 042D
registerFastestTime(int stat, int to)  -- 042E
registerHighestScore(int int1, int int2)  -- 042F
warpCharIntoCarAsPassenger(Ped ped, Vehicle car, int passengerSeat)  -- 0430
bool result = isCarPassengerSeatFree(Vehicle car, int seat)  -- 0431
Ped ped = getCharInCarPassengerSeat(Vehicle car, int seat)  -- 0432
setCharIsChrisCriminal(Ped ped, bool flag)  -- 0433
startCredits()  -- 0434
stopCredits()  -- 0435
bool result = areCreditsFinished()  -- 0436
setMusicDoesFade(bool enable)  -- 043C
Model modelId = getCarModel(int int)  -- 0441
bool result = areAnyCarCheatsActivated()  -- 0445
setCharSuffersCriticalHits(Ped ped, bool enable)  -- 0446
bool result = isCharSittingInCar(Ped ped, Vehicle car)  -- 0448
bool result = isCharSittingInAnyCar(Ped ped)  -- 0449
bool result = isCharOnFoot(Ped ped)  -- 044B
loadSplashScreen(GxtString gxtString)  -- 044D
setJamesCarOnPathToPlayer(int int1)  -- 0450
setObjectRotation(Object object, float rotationX, float rotationY, float rotationZ)  -- 0453
float X, float Y, float Z = getDebugCameraCoordinates()  -- 0454
bool result = isPlayerTargettingChar(Player player, Ped ped)  -- 0457
bool result = isPlayerTargettingObject(Player player, Object object)  -- 0458
displayTextWithNumber(float x, float y, GxtString gxtString, int number)  -- 045A
displayTextWith2Numbers(float x, float y, GxtString gxtString, int numbersX, int numbersY)  -- 045B
failCurrentMission()  -- 045C
setInterpolationParameters(float delay, int time)  -- 0460
float X, float Y, float Z = getDebugCameraPointAt()  -- 0463
attachCharToCar(Ped ped, Vehicle car, float offsetX, float offsetY, float offsetZ, int position, float shootingAngleLimit, int weapon)  -- 0464
detachCharFromCar(Ped ped)  -- 0465
setCarStayInFastLane(Vehicle car, bool flag)  -- 0466
clearCharLastWeaponDamage(Ped ped)  -- 0467
clearCarLastWeaponDamage(Vehicle car)  -- 0468
int int10 = getRandomCopInArea(float float1, float float2, float float3, float float4, bool int5, bool int6, bool int7, bool int8, bool int9)  -- 0469
Ped ped = getDriverOfCar(Vehicle car)  -- 046C
int followers = getNumberOfFollowers(Ped ped)  -- 046D
giveRemoteControlledModelToPlayer(Player player, float atX, float atY, float atZ, float angle, Model RCModel)  -- 046E
int weapon = getCurrentCharWeapon(Ped ped)  -- 0470
bool result = locateCharAnyMeansObject2d(Ped ped, Object object, float radiusX, float radiusY, bool sphere)  -- 0471
bool result = locateCharOnFootObject2d(Ped ped, Object object, float radiusX, float radiusY, bool sphere)  -- 0472
bool result = locateCharInCarObject2d(Ped ped, Object object, float radiusX, float radiusY, bool sphere)  -- 0473
bool result = locateCharAnyMeansObject3d(Ped ped, Object object, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 0474
bool result = locateCharOnFootObject3d(Ped ped, Object object, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 0475
bool result = locateCharInCarObject3d(Ped ped, Object object, float radiusX, float radiusY, float radiusZ, bool sphere)  -- 0476
setCarTempAction(Vehicle car, int action, int time)  -- 0477
bool result = isCharOnAnyBike(Ped ped)  -- 047A
bool result = canCharSeeDeadChar(Ped ped, int pedtype)  -- 0480
setEnterCarRangeMultiplier(float float1)  -- 0481
Vehicle car = getRemoteControlledCar(Player player)  -- 0484
bool result = isPcVersion()  -- 0485
bool result = isModelAvailable(Model modelId)  -- 0488
shutCharUp(Ped ped, bool muted)  -- 0489
setEnableRcDetonate(bool detonation)  -- 048A
setCarRandomRouteSeed(Vehicle car, int routeSeed)  -- 048B
bool result = isAnyPickupAtCoords(float pickupX, float pickupY, float pickupZ)  -- 048C
removeAllCharWeapons(Ped ped)  -- 048F
bool result = hasCharGotWeapon(Ped ped, int weapon)  -- 0491
setTankDetonateCars(int tank, bool detonate)  -- 0493
int offset1, int offset2, int offset3, int offset4 = getPositionOfAnalogueSticks(int joystick)  -- 0494
bool result = isCarOnFire(Vehicle car)  -- 0495
bool result = isCarTireBurst(Vehicle car, int tire)  -- 0496
initialiseObjectPath(int int1, float float2)  -- 049C
setObjectPathSpeed(int int1, int int2)  -- 049E
setObjectPathPosition(int int1, float float2)  -- 049F
clearObjectPath(int int1)  -- 04A1
heliGotoCoords(Vehicle heli, float toX, float toY, float toZ, float altitudeMin, float altitudeMax)  -- 04A2
float coordsX, float coordsY, float coordsZ = getDeadCharPickupCoords(Ped ped)  -- 04A5
Pickup pickup = createProtectionPickup(float atX, float atY, float atZ, int int4, int int5)  -- 04A6
bool result = isCharInAnyBoat(Ped ped)  -- 04A7
bool result = isCharInAnyHeli(Ped ped)  -- 04A9
bool result = isCharInAnyPlane(Ped ped)  -- 04AB
bool result = isCharInWater(Ped ped)  -- 04AD
int weapon, int ammo, Model modelId = getCharWeaponInSlot(Ped ped, int slot)  -- 04B8
float float6, float float7, float float8, float float9, float float10, float float11, float float12 = getClosestStraightRoad(float atX, float atY, float atZ, float height, float radius)  -- 04B9
setCarForwardSpeed(Vehicle car, float speed)  -- 04BA
setInteriorVisible(int interior)  -- 04BB
markCarAsConvoyCar(Vehicle car, bool convoy)  -- 04BD
resetHavocCausedByPlayer(int int1)  -- 04BE
int int2 = getHavocCausedByPlayer(int int1)  -- 04BF
createScriptRoadblock(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ, int type)  -- 04C0
clearAllScriptRoadblocks()  -- 04C1
float X, float Y, float Z = getOffsetFromCharInWorldCoords(Ped ped, float offsetX, float offsetY, float offsetZ)  -- 04C4
bool result = hasCharBeenPhotographed(Ped ped)  -- 04C5
switchSecurityCamera(bool int1)  -- 04C7
bool result = isCharInFlyingVehicle(Ped ped)  -- 04C8
Marker marker = addShortRangeSpriteBlipForCoord(float atX, float atY, float atZ, int icon)  -- 04CE
setHeliOrientation(Vehicle heli, float angle)  -- 04D0
clearHeliOrientation(Vehicle heli)  -- 04D1
planeGotoCoords(int plane, float X, float Y, float Z, float z1, float z2)  -- 04D2
float X, float Y, float Z = getNthClosestCarNode(float X, float Y, float Z, int type)  -- 04D3
drawWeaponshopCorona(float X, float Y, float Z, float radius, int type, int flare, int r, int g, int b)  -- 04D5
setEnableRcDetonateOnContact(bool enable)  -- 04D6
freezeCharPosition(Ped ped, bool locked)  -- 04D7
setCharDrownsInWater(Ped ped, bool drowns)  -- 04D8
setObjectRecordsCollisions(Object object, bool set)  -- 04D9
bool result = hasObjectCollidedWithAnything(Object object)  -- 04DA
removeRcBuggy()  -- 04DB
int armour = getCharArmour(Ped ped)  -- 04DD
setHeliStabiliser(Vehicle heli, bool limiter)  -- 04DF
setCarStraightLineDistance(Vehicle car, int radius)  -- 04E0
popCarBoot(Vehicle car)  -- 04E1
shutPlayerUp(Player player, bool shut)  -- 04E2
setPlayerMood(Player player, int flag, int time)  -- 04E3
requestCollision(float X, float Y)  -- 04E4
bool result = locateObject2d(Object object, float X, float Y, float radiusX, float radiusY, bool sphere)  -- 04E5
bool result = locateObject3d(Object object, float X, float Y, float Z, float radiusX, float radiusY, float radiusZ, bool flag)  -- 04E6
bool result = isObjectInWater(Object object)  -- 04E7
bool result = isObjectInArea2d(Object object, float cornerAX, float cornerAY, float cornerBX, float cornerBY, bool sphere)  -- 04E9
bool result = isObjectInArea3d(Object object, float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ, bool flag)  -- 04EA
taskToggleDuck(Ped ped, bool crouch)  -- 04EB
requestAnimation(string animation)  -- 04ED
bool result = hasAnimationLoaded(string animation)  -- 04EE
removeAnimation(string animation)  -- 04EF
bool result = isCharWaitingForWorldCollision(Ped ped)  -- 04F0
bool result = isCarWaitingForWorldCollision(Vehicle car)  -- 04F1
attachCharToObject(Ped ped, Object object, float offsetX, float offsetY, float offsetZ, int orientation, float angle, int lockWeapon)  -- 04F4
displayNthOnscreenCounterWithString(VarId text, int type, int line, GxtString gxtString)  -- 04F7
addSetPiece(int type, float rectX1, float rectY1, float rectX2, float rectY2, float spawnAX, float spawnAY, float headedTowards1X, float headedTowards1Y, float spawnBX, float spawnBY, float headedTowards2X, float headedTowards2Y)  -- 04F8
setExtraColours(int color, bool fade)  -- 04F9
clearExtraColours(bool fade)  -- 04FA
int twowheelstime, float twowheelsdistance, int wheelietime, float wheelieDistance, int stoppieTime, float stoppieDistance = getWheelieStats(Player player)  -- 04FC
burstCarTire(Vehicle car, int tire)  -- 04FE
bool result = isPlayerWearing(Player player, string bodypart, int skin)  -- 0500
setPlayerCanDoDriveBy(Player player, bool mode)  -- 0501
int handleAs = createSwatRope(int pedtype, Model modelId, float X, float Y, float Z)  -- 0503
setCarModelComponents(Model car, int variation1, int variation2)  -- 0506
closeAllCarDoors(Vehicle car)  -- 0508
float distance = getDistanceBetweenCoords2d(float x1, float y1, float x2, float y2)  -- 0509
float distance = getDistanceBetweenCoords3d(float x1, float y1, float z1, float x2, float y2, float z2)  -- 050A
sortOutObjectCollisionWithCar(Object object, Vehicle car)  -- 050E
int level = getMaxWantedLevel()  -- 050F
printHelpForever(GxtString text)  -- 0512
printHelpForeverWithNumber(GxtString text, int number)  -- 0513
Pickup pickup = createLockedPropertyPickup(float pX, float pY, float pZ, GxtString gxtString)  -- 0517
Pickup pickup = createForsalePropertyPickup(float pX, float pY, float pZ, int price, GxtString gxtString)  -- 0518
freezeCarPosition(Vehicle car, bool locked)  -- 0519
bool result = hasCharBeenDamagedByChar(Ped ped, Ped byActor)  -- 051A
bool result = hasCharBeenDamagedByCar(Ped ped, Vehicle byCar)  -- 051B
bool result = hasCarBeenDamagedByChar(Vehicle car, Ped byActor)  -- 051C
bool result = hasCarBeenDamagedByCar(Vehicle car, Vehicle byCar)  -- 051D
int radio = getRadioChannel()  -- 051E
setCharStayInCarWhenJacked(Ped ped, bool stay)  -- 0526
setPlayerDrunkenness(Player player, int drunk)  -- 052C
Vehicle car = getRandomCarOfTypeInAreaNoSave(float x1, float y1, float x2, float y2, Model modelId)  -- 053E
setCanBurstCarTires(Vehicle car, bool vulnerability)  -- 053F
fireHunterGun(Vehicle car)  -- 0541
bool result = isCharTouchingVehicle(Ped ped, Vehicle car)  -- 0547
setCharCanBeShotInVehicle(Ped ped, bool can)  -- 054A
loadMissionText(GxtString table)  -- 054C
clearCharLastDamageEntity(Ped ped)  -- 054E
clearCarLastDamageEntity(Vehicle car)  -- 054F
freezeObjectPosition(Object object, bool freeze)  -- 0550
removeWeaponFromChar(Ped ped, int weapon)  -- 0555
makePlayerFireProof(Player player, bool fireproof)  -- 055D
increasePlayerMaxHealth(Player player, int increase)  -- 055E
increasePlayerMaxArmour(Player player, int increase)  -- 055F
Ped ped = createRandomCharAsDriver(Vehicle car)  -- 0560
Ped ped = createRandomCharAsPassenger(Vehicle car, int seat)  -- 0561
ensurePlayerHasDriveByWeapon(Player player, int ammo)  -- 0563
makeHeliComeCrashingDown(Vehicle heli)  -- 0564
addExplosionNoSound(float pX, float pY, float pZ, int type)  -- 0565
linkObjectToInterior(Object object, int interior)  -- 0566
setCharNeverTargetted(Ped ped, bool untargetable)  -- 0568
bool result = wasCutsceneSkipped()  -- 056A
bool result = isCharInAnyPoliceVehicle(Ped ped)  -- 056C
bool result = doesCharExist(Ped ped)  -- 056D
bool result = doesVehicleExist(Vehicle car)  -- 056E
Marker blip = addShortRangeSpriteBlipForContactPoint(float pX, float pY, float pZ, int icon)  -- 0570
setAllTaxisHaveNitro(bool toggle)  -- 0572
freezeCarPositionAndDontLoadCollision(Vehicle car, bool keep)  -- 0574
freezeCharPositionAndDontLoadCollision(Ped ped, bool keep)  -- 0575
setPlayerIsInStadium(bool set)  -- 057E
displayRadar(bool enable)  -- 0581
registerBestPosition(int stat, float float)  -- 0582
bool result = isPlayerInInfoZone(Player player, GxtString zone)  -- 0583
setLoadCollisionForCarFlag(Vehicle car, bool enable)  -- 0587
setLoadCollisionForCharFlag(Ped ped, bool enable)  -- 0588
addBigGunFlash(float fromX, float fromY, float fromZ, float toX, float toY, float toZ)  -- 058A
float progress = getProgressPercentage()  -- 058C
setVehicleToFadeIn(Vehicle car, int flag)  -- 0594
registerOddjobMissionPassed()  -- 0595
bool result = isPlayerInShortcutTaxi(Player player)  -- 0596
bool result = isCharDucking(Ped ped)  -- 0597
setOnscreenCounterFlashWhenFirstDisplayed(VarId text, bool flashing)  -- 059C
shuffleCardDecks(bool shuffle)  -- 059D
int card = fetchNextCard()  -- 059E
float vecX, float vecY, float vecZ = getObjectVelocity(Object object)  -- 059F
bool result = isDebugCameraOn()  -- 05A0
addToObjectRotationVelocity(Object object, float vecX, float vecY, float vecZ)  -- 05A1
setObjectRotationVelocity(Object object, float vecX, float vecY, float vecZ)  -- 05A2
bool result = isObjectStatic(Object object)  -- 05A3
float angle = getAngleBetween2dVectors(float vecX, float vecY, float vecX, float vecY)  -- 05A4
bool result = do2dRectanglesCollide(float areaX, float areaY, float scaleX, float scaleY, float overlapareaX, float overlapareaY, float overlapscaleX, float overlapscaleY)  -- 05A5
float axisX, float axisY, float axisZ = getObjectRotationVelocity(Object object)  -- 05A6
addVelocityRelativeToObjectVelocity(Object object, float vecX, float vecY, float vecZ)  -- 05A7
float speed = getObjectSpeed(Object object)  -- 05A8
bool result, float X, float Y = get2dLinesIntersectPoint(float l1x1, float l1y1, float l1x2, float l1y2, float l2x1, float l2y1, float l2x2, float l2y2)  -- 05B0
taskPause(Ped ped, int timeMS)  -- 05B9
taskStandStill(Ped ped, int timeMS)  -- 05BA
taskFallAndGetUp(Ped ped, bool int2, int time)  -- 05BB
taskJump(Ped ped, bool jump)  -- 05BC
taskTired(Ped ped, int timeMS)  -- 05BD
taskDie(Ped ped)  -- 05BE
taskLookAtChar(Ped ped, int lookAt, int timeMS)  -- 05BF
taskLookAtVehicle(Ped ped, int lookAt, int timeMS)  -- 05C0
taskSay(Ped ped, int audio)  -- 05C1
taskShakeFist(Ped ped)  -- 05C2
taskCower(Ped ped)  -- 05C3
taskHandsUp(Ped ped, int timeMS)  -- 05C4
taskDuck(Ped ped, int timeMS)  -- 05C5
taskUseAtm(Ped ped)  -- 05C7
taskScratchHead(Ped ped)  -- 05C8
taskLookAbout(Ped ped, int timeMS)  -- 05C9
taskEnterCarAsPassenger(Ped ped, Vehicle car, int time, int passengerSeat)  -- 05CA
taskEnterCarAsDriver(Ped ped, Vehicle car, int timeMS)  -- 05CB
taskLeaveCar(Ped ped, Vehicle car)  -- 05CD
taskLeaveCarAndFlee(Ped ped, Vehicle car, float X, float Y, float Z)  -- 05CF
taskCarDriveToCoord(Ped ped, Vehicle car, float toX, float toY, float toZ, float speed, int int7, int model, int int9)  -- 05D1
taskCarDriveWander(Ped ped, Vehicle hijackCar, float searchRadius, int trafficBehavior)  -- 05D2
taskGoStraightToCoord(Ped ped, float toX, float toY, float toZ, int mode, int time)  -- 05D3
taskAchieveHeading(Ped ped, float angle)  -- 05D4
flushRoute()  -- 05D6
extendRoute(float pointX, float pointY, float pointZ)  -- 05D7
taskFollowPointRoute(Ped ped, int flags1, int flags2)  -- 05D8
taskGotoChar(Ped ped, Ped toActor, int timelimit, float stopWithinRadius)  -- 05D9
taskFleePoint(Ped ped, float fromX, float fromY, float fromZ, float awayRadius, int timelimit)  -- 05DA
taskFleeChar(Ped ped, Ped fromActor, float radius, int timelimit)  -- 05DB
taskSmartFleePoint(Ped ped, float fromX, float fromY, float fromZ, float stopAtRadius, int timelimit)  -- 05DC
taskSmartFleeChar(Ped ped, Ped fromActor, float originRadius, int timelimit)  -- 05DD
taskWanderStandard(Ped ped)  -- 05DE
taskKillCharOnFoot(Ped ped, Ped killActor)  -- 05E2
startPlaybackRecordedCar(Vehicle car, int path)  -- 05EB
stopPlaybackRecordedCar(Vehicle car)  -- 05EC
pausePlaybackRecordedCar(Vehicle car)  -- 05ED
unpausePlaybackRecordedCar(Vehicle car)  -- 05EE
setCarEscortCarLeft(Vehicle car, Vehicle followCar)  -- 05F1
setCarEscortCarRight(Vehicle car, Vehicle followCar)  -- 05F2
setCarEscortCarRear(Vehicle car, Vehicle followCar)  -- 05F3
setCarEscortCarFront(Vehicle car, Vehicle followCar)  -- 05F4
taskFollowPathNodesToCoord(Ped ped, float pathX, float pathY, float pathZ, int mode, int time)  -- 05F5
bool result = isCharInAngledArea2d(Ped ped, float x1, float y1, float x2, float y2, float angle, bool sphere)  -- 05F6
bool result = isCharInAngledAreaOnFoot2d(Ped ped, float x1, float y1, float x2, float y2, float angle, bool sphere)  -- 05F7
bool result = isCharInAngledAreaInCar2d(Ped ped, float x1, float y1, float x2, float y2, float angle, bool sphere)  -- 05F8
bool result = isCharStoppedInAngledArea2d(Ped ped, float x1, float y1, float x2, float y2, float height, bool flag)  -- 05F9
bool result = isCharStoppedInAngledAreaOnFoot2d(Ped ped, float x1, float y1, float x2, float y2, float angle, bool sphere)  -- 05FA
bool result = isCharStoppedInAngledAreaInCar2d(Ped ped, float x1, float y1, float x2, float y2, float height, bool flag)  -- 05FB
bool result = isCharInAngledArea3d(Ped ped, float x1, float y1, float z1, float x2, float y2, float z2, float angle, bool sphere)  -- 05FC
bool result = isCharInAngledAreaOnFoot3d(Ped ped, float x1, float y1, float z1, float x2, float y2, float z2, float angle, bool sphere)  -- 05FD
bool result = isCharInAngledAreaInCar3d(Ped ped, float x1, float y1, float z1, float x2, float y2, float z2, float depth, bool flag)  -- 05FE
bool result = isCharStoppedInAngledArea3d(Ped ped, float x1, float y1, float z1, float x2, float y2, float z2, float depth, bool flag)  -- 05FF
bool result = isCharStoppedInAngledAreaOnFoot3d(Ped ped, float x1, float y1, float z1, float x2, float y2, float z2, float depth, bool flag)  -- 0600
bool result = isCharStoppedInAngledAreaInCar3d(Ped ped, float x1, float y1, float z1, float x2, float y2, float z2, float depth, bool flag)  -- 0601
bool result = isCharInTaxi(Ped ped)  -- 0602
taskGoToCoordAnyMeans(Ped ped, float toX, float toY, float toZ, int mode, Vehicle useCar)  -- 0603
float zAngle = getHeadingFromVector2d(float pX, float pY)  -- 0604
taskPlayAnim(Ped ped, string animation, string IFP, float framedelta, bool loop, bool lockX, bool lockY, bool lockF, int time)  -- 0605
loadPathNodesInArea(float x1, float y1, float x2, float y2)  -- 0606
releasePathNodes()  -- 0607
int maker = loadCharDecisionMaker(int type)  -- 060A
setCharDecisionMaker(Ped ped, int maker)  -- 060B
setTextDropshadow(int shadow, int r, int g, int b, int a)  -- 060D
bool result = isPlaybackGoingOnForCar(Vehicle car)  -- 060E
setSenseRange(Ped ped, float accuracy)  -- 060F
bool result = isCharPlayingAnim(Ped ped, string animation)  -- 0611
setCharAnimPlayingFlag(Ped ped, string animation, bool flag)  -- 0612
float time = getCharAnimCurrentTime(Ped ped, string animation)  -- 0613
setCharAnimCurrentTime(Ped ped, string animation, float time)  -- 0614
int task = openSequenceTask()  -- 0615
closeSequenceTask(int task)  -- 0616
performSequenceTask(Ped ped, int task)  -- 0618
setCharCollision(Ped ped, bool enable)  -- 0619
float totalTime = getCharAnimTotalTime(Ped ped, string animation)  -- 061A
clearSequenceTask(int task)  -- 061B
int handle = addAttractor(float originX, float originY, float originZ, float zAngle, float unknownAngle, int taskSequence)  -- 061D
clearAttractor(int handle)  -- 061E
Ped ped = createCharAtAttractor(int pedtype, Model modelId, int ASOrigin, int task)  -- 0621
taskLeaveCarImmediately(Ped ped, Vehicle car)  -- 0622
incrementIntStat(int stat, int add)  -- 0623
incrementFloatStat(int stat, float add)  -- 0624
decrementIntStat(int stat, int int)  -- 0625
decrementFloatStat(int stat, float float)  -- 0626
registerIntStat(int stat, int int)  -- 0627
registerFloatStat(int stat, float value)  -- 0628
setIntStat(int stat, int int)  -- 0629
setFloatStat(int stat, float float)  -- 062A
int status = getScriptTaskStatus(Ped ped, int task)  -- 062E
int group = createGroup(int type)  -- 062F
setGroupLeader(int group, Ped ped)  -- 0630
setGroupMember(int group, Ped ped)  -- 0631
removeGroup(int group)  -- 0632
taskLeaveAnyCar(Ped ped)  -- 0633
taskKillCharOnFootWhileDucking(Ped ped, int weapon, int flags, int time, int chance)  -- 0634
taskAimGunAtChar(Ped ped, int aimAt, int timeMS)  -- 0635
taskGoToCoordWhileShooting(Ped ped, float toX, float toY, float toZ, int mode, float turnRadius, float stopRadius, int lookAtActor)  -- 0637
taskStayInSamePlace(Ped ped, bool stay)  -- 0638
taskTurnCharToFaceChar(Ped ped, int rotateTo)  -- 0639
bool result = isCharAtScriptedAttractor(Ped ped, int origin)  -- 0642
setSequenceToRepeat(int pack, bool loop)  -- 0643
int progess = getSequenceProgress(Ped ped)  -- 0646
clearLookAt(Ped ped)  -- 0647
setFollowNodeThresholdDistance(Ped ped, float dist)  -- 0648
Particle particle = createFxSystem(string particle, float pX, float pY, float pZ, int type)  -- 064B
playFxSystem(Particle particle)  -- 064C
stopFxSystem(Particle particle)  -- 064E
playAndKillFxSystem(Particle particle)  -- 064F
killFxSystem(Particle particle)  -- 0650
int stat = getIntStat(int stat)  -- 0652
float stat = getFloatStat(int stat)  -- 0653
setObjectRenderScorched(Object object, bool fireproof)  -- 0654
taskLookAtObject(Ped ped, int lookAt, int timeMS)  -- 0655
float float = limitAngle(float angle)  -- 0656
openCarDoor(Vehicle car, int door)  -- 0657
float X, float Y, float Z = getPickupCoordinates(Pickup pickup)  -- 065B
removeDecisionMaker(int maker)  -- 065C
Model modelId = getCharModel(Ped ped)  -- 0665
taskAimGunAtCoord(Ped ped, float atX, float atY, float atZ, int timeMS)  -- 0667
taskShootAtCoord(Ped ped, float atX, float atY, float atZ, int timeMS)  -- 0668
Particle particle = createFxSystemOnChar(string particle, Ped ped, float offsetX, float offsetY, float offsetZ, int type)  -- 0669
Particle particle = createFxSystemOnCharWithDirection(string particle, Ped ped, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ, int type)  -- 066A
Particle particle = createFxSystemOnCar(string particle, Vehicle car, float offsetX, float offsetY, float offsetZ, int type)  -- 066B
Particle particle = createFxSystemOnCarWithDirection(string particle, Vehicle car, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ, int type)  -- 066C
Particle particle = createFxSystemOnObject(string particle, Object object, float offsetX, float offsetY, float offsetZ, int type)  -- 066D
Particle particle = createFxSystemOnObjectWithDirection(string particle, Object object, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ, int flag)  -- 066E
taskDestroyCar(Ped ped, Vehicle car)  -- 0672
taskDiveAndGetUp(Ped ped, float toOffsetX, float toOffsetY, int time)  -- 0673
customPlateForNextCar(Model modelId, string numberplate)  -- 0674
taskShuffleToNextCarSeat(Ped ped, Vehicle car)  -- 0676
taskChatWithChar(Ped ped, int withActor, bool flag, int unknownFlag)  -- 0677
attachCameraToVehicle(Vehicle car, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ, float tilt, int switchstyle)  -- 0679
attachCameraToVehicleLookAtVehicle(Vehicle car, float offsetX, float offsetY, float offsetZ, int toCar, float tilt, int switchstyle)  -- 067A
attachCameraToVehicleLookAtChar(Vehicle car, float offsetX, float offsetY, float offsetZ, Ped ped, float tilt, int switchstyle)  -- 067B
attachCameraToChar(Ped ped, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ, float tilt, int switchstyle)  -- 067C
attachCameraToCharLookAtChar(Ped ped, float offsetX, float offsetY, float offsetZ, int targetActor, float tilt, int switchstyle)  -- 067E
forceCarLights(Vehicle car, int lights)  -- 067F
addPedtypeAsAttractorUser(int ASOrigin, int pedtype)  -- 0680
attachObjectToCar(Object object, Vehicle car, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ)  -- 0681
detachObject(Object object, float X, float Y, float Z, bool collisionDetection)  -- 0682
attachCarToCar(Vehicle car, int toCar, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ)  -- 0683
detachCar(Vehicle car, float X, float Y, float Z, bool collisionDetection)  -- 0684
bool result = isObjectAttached(Object object)  -- 0685
bool result = isVehicleAttached(Vehicle car)  -- 0686
clearCharTasks(Ped ped)  -- 0687
taskTogglePedThreatScanner(Ped ped, bool unknownFlag1, bool unknownFlag2, bool unknownFlag3)  -- 0688
popCarDoor(Vehicle car, int door, bool visible)  -- 0689
fixCarDoor(Vehicle car, int door)  -- 068A
taskEveryoneLeaveCar(Vehicle car)  -- 068B
bool result = isPlayerTargettingAnything(Player player)  -- 068C
float X, float Y, float Z = getActiveCameraCoordinates()  -- 068D
float X, float Y, float Z = getActiveCameraPointAt()  -- 068E
popCarPanel(Vehicle car, int component, bool effectFlag)  -- 0697
fixCarPanel(Vehicle car, int componentB)  -- 0698
fixCarTire(Vehicle car, int tire)  -- 0699
attachObjectToObject(Object object, int toObject, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ)  -- 069A
attachObjectToChar(Object object, Ped ped, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ)  -- 069B
float vecX, float vecY, float vecZ = getCarSpeedVector(Vehicle car)  -- 06A2
float mass = getCarMass(Vehicle car)  -- 06A3
taskDiveFromAttachmentAndGetUp(Ped ped, int timeMS)  -- 06A5
attachCharToBike(Ped ped, Vehicle car, float offsetX, float offsetY, float offsetZ, int position, float shootingAngle1, float shootingAngle2, int weapon)  -- 06A7
taskGotoCharOffset(Ped ped, int toActor, int timelimit, float approachDistance, float approachAngle)  -- 06A8
taskLookAtCoord(Ped ped, float toX, float toY, float toZ, int timeMS)  -- 06A9
hideCharWeaponForScriptedCutscene(Ped ped, bool hide)  -- 06AB
float speed = getCharSpeed(Ped ped)  -- 06AC
setGroupDecisionMaker(int group, int maker)  -- 06AD
int maker = loadGroupDecisionMaker(int type)  -- 06AE
disablePlayerSprint(Player player, bool mode)  -- 06AF
taskSitDown(Ped ped, int timeMS)  -- 06B0
Searchlight searchlight = createSearchlight(float atX, float atY, float atZ, float targetX, float targetY, float targetZ, float radius1, float radius2)  -- 06B1
deleteSearchlight(Searchlight searchlight)  -- 06B2
bool result = doesSearchlightExist(Searchlight searchlight)  -- 06B3
moveSearchlightBetweenCoords(Searchlight searchlight, float fromX, float fromY, float fromZ, float toX, float toY, float toZ, float speed)  -- 06B4
pointSearchlightAtCoord(Searchlight searchlight, float toX, float toY, float toZ, float speed)  -- 06B5
pointSearchlightAtChar(Searchlight searchlight, Ped ped, float speed)  -- 06B6
bool result = isCharInSearchlight(Searchlight searchlight, Ped ped)  -- 06B7
bool result = hasCutsceneLoaded()  -- 06B9
taskTurnCharToFaceCoord(Ped ped, float atX, float atY, float atZ)  -- 06BA
taskDrivePointRoute(Ped ped, Vehicle car, float speed)  -- 06BB
fireSingleBullet(float fromX, float fromY, float fromZ, float targetX, float targetY, float targetZ, int energy)  -- 06BC
bool result = isLineOfSightClear(float fromX, float fromY, float fromZ, float toX, float toY, float toZ, bool checkBuildings, bool checkVehicles, bool checkActors, bool checkObjects, bool checkParticles)  -- 06BD
float roll = getCarRoll(Vehicle car)  -- 06BE
pointSearchlightAtVehicle(Searchlight searchlight, Vehicle car, float speed)  -- 06BF
bool result = isVehicleInSearchlight(int int, Vehicle car)  -- 06C0
Searchlight searchlight = createSearchlightOnVehicle(Vehicle car, float offsetX, float offsetY, float offsetZ, float targetX, float targetY, float targetZ, float radius, float radius)  -- 06C1
taskGoToCoordWhileAiming(Ped ped, float toX, float toY, float toZ, int mode, float turnRadius, float stopRadius, Ped ped, float offsetX, float offsetY, float offsetZ)  -- 06C2
int num = getNumberOfFiresInRange(float atX, float atY, float atZ, float radius)  -- 06C3
Marker marker = addBlipForSearchlight(Searchlight searchlight)  -- 06C4
skipToEndAndStopPlaybackRecordedCar(Vehicle car)  -- 06C5
taskCarTempAction(Ped ped, Vehicle car, int performAction, int timelimit)  -- 06C7
setLaRiots(bool enable)  -- 06C8
removeCharFromGroup(Ped ped)  -- 06C9
attachSearchlightToSearchlightObject(Searchlight searchlight, int tower, int housing, int bulb, float offsetX, float offsetY, float offsetZ)  -- 06CA
switchEmergencyServices(bool enable)  -- 06D0
Checkpoint checkpoint = createCheckpoint(int type, float atX, float atY, float atZ, float pointX, float pointY, float pointZ, float radius)  -- 06D5
deleteCheckpoint(Checkpoint checkpoint)  -- 06D6
switchRandomTrains(bool enable)  -- 06D7
Vehicle train = createMissionTrain(int type, float atX, float atY, float atZ, bool direction)  -- 06D8
deleteMissionTrains()  -- 06D9
markMissionTrainsAsNoLongerNeeded()  -- 06DA
deleteAllTrains()  -- 06DB
setTrainSpeed(Vehicle train, float speed)  -- 06DC
setTrainCruiseSpeed(Vehicle train, float speed)  -- 06DD
int caboose = getTrainCaboose(Vehicle train)  -- 06DE
deletePlayer(Player player)  -- 06DF
setTwoPlayerCameraMode(bool mode)  -- 06E0
taskCarMission(Ped ped, Vehicle car, int targetCar, int order, float maxSpeed, int trafficFlag)  -- 06E1
taskGoToObject(Ped ped, int toObject, int timelimit, float stopWithinRadius)  -- 06E2
taskWeaponRoll(Ped ped, bool roll)  -- 06E3
taskCharArrestChar(Ped ped, int bustActor)  -- 06E4
Model itemID = getAvailableVehicleMod(Vehicle car, int poolIndex)  -- 06E5
int type = getVehicleModType(Model component)  -- 06E6
int componentId = addVehicleMod(Vehicle car, Model component)  -- 06E7
removeVehicleMod(Vehicle car, int componentId)  -- 06E8
requestVehicleMod(Model component)  -- 06E9
bool result = hasVehicleModLoaded(Model component)  -- 06EA
markVehicleModAsNoLongerNeeded(Model component)  -- 06EB
int num = getNumAvailablePaintjobs(Vehicle car)  -- 06EC
giveVehiclePaintjob(int set, int paintjob)  -- 06ED
bool result = isGroupMember(Ped ped, int group)  -- 06EE
bool result = isGroupLeader(Ped ped, int group)  -- 06EF
setGroupSeparationRange(int group, float range)  -- 06F0
limitTwoPlayerDistance(float distance)  -- 06F1
releaseTwoPlayerDistance()  -- 06F2
setPlayerPlayerTargetting(bool can)  -- 06F3
float X, float Y, float Z = getScriptFireCoords(int fire)  -- 06F5
float X, float Y, float Z, float ZAngle = getNthClosestCarNodeWithHeading(float forX, float forY, float forZ, int direction)  -- 06F8
setPlayersCanBeInSeparateCars(bool allow)  -- 06FA
bool result = doesCarHaveStuckCarCheck(Vehicle car)  -- 06FC
setPlaybackSpeed(Vehicle car, float speed)  -- 06FD
bool result = areAnyCharsNearChar(Ped ped, float range)  -- 06FF
skipCutsceneEnd()  -- 0701
int percentage = getPercentageTaggedInArea(float x1, float y1, float x2, float y2)  -- 0702
setTagStatusInArea(float x1, float y1, float x2, float y2, bool value)  -- 0703
carGotoCoordinatesRacing(Vehicle car, float toX, float toY, float toZ)  -- 0704
startPlaybackRecordedCarUsingAi(Vehicle car, int path)  -- 0705
skipInPlaybackRecordedCar(Vehicle car, float path)  -- 0706
clearCharDecisionMakerEventResponse(int maker, int event)  -- 0708
addCharDecisionMakerEventResponse(int maker, int event, int taskID, float respect, float hate, float like, float dislike, bool inCar, bool onFoot)  -- 0709
taskPickUpObject(Ped ped, Object object, float offsetX, float offsetY, float offsetZ, int boneId1, int boneId2, string performAnimation, int IFPFile, int time)  -- 070A
dropObject(Ped ped, bool object)  -- 070B
explodeCarInCutscene(Vehicle car)  -- 070C
buildPlayerModel(Player player)  -- 070D
planeAttackPlayer(int hydra, Vehicle car, float radius)  -- 070E
planeFlyInDirection(int plane, float direction, float altitudemin, float altitudemax)  -- 070F
planeFollowEntity(int plane, Ped ped, Vehicle car, float radius)  -- 0710
taskDriveBy(Ped ped, int drivebyActor, Vehicle car, float pX, float pY, float pZ, float radiusX, int radiusY, bool radiusZ, int firingRate)  -- 0713
setCarStayInSlowLane(Vehicle car, bool stay)  -- 0714
takeRemoteControlOfCar(Player player, Vehicle car)  -- 0715
bool result = isClosestObjectOfTypeSmashedOrDamaged(Model object, float atX, float atY, float atZ, float radius, bool smashed, bool damaged)  -- 0716
startSettingUpConversation(Ped ped)  -- 0717
finishSettingUpConversation()  -- 0719
bool result = isConversationAtNode(Ped ped, GxtString gxtString)  -- 071A
int health = getObjectHealth(Object object)  -- 071E
setObjectHealth(Object object, int health)  -- 071F
breakObject(Object object, int intensity)  -- 0723
heliAttackPlayer(Vehicle heli, Player player, float radius)  -- 0724
heliFollowEntity(Vehicle heli, Ped ped, Vehicle car, float radius)  -- 0726
policeHeliChaseEntity(Vehicle heli, Ped ped, Vehicle car, float radius)  -- 0727
taskUseMobilePhone(Ped ped, bool hold)  -- 0729
taskWarpCharIntoCarAsDriver(Ped ped, Vehicle car)  -- 072A
taskWarpCharIntoCarAsPassenger(Ped ped, Vehicle car, int passengerseat)  -- 072B
switchCopsOnBikes(bool generate)  -- 072C
bool result = isFlameInAngledArea2d(float x1, float y1, float x2, float y2, float angle, bool sphere)  -- 072D
bool result = isFlameInAngledArea3d(float x1, float y1, float z1, float x2, float y2, float z2, float angle, bool sphere)  -- 072E
addStuckCarCheckWithWarp(Vehicle car, float checkDistance, int time, bool stuck, bool flipped, bool warp, int path)  -- 072F
damageCarPanel(Vehicle car, int door)  -- 0730
setCarRoll(Vehicle car, float roll)  -- 0731
bool result = suppressCarModel(Model modelId)  -- 0732
dontSuppressCarModel(Model modelId)  -- 0733
dontSuppressAnyCarModels()  -- 0734
bool result = isPs2KeyboardKeyPressed(int key)  -- 0735
bool result = isPs2KeyboardKeyJustPressed(int key)  -- 0736
bool result = isCharHoldingObject(Ped ped, int liftingObject)  -- 0737
setCarCanGoAgainstTraffic(Vehicle car, bool can)  -- 073B
damageCarDoor(Vehicle car, int door)  -- 073C
Vehicle car = getRandomCarInSphereNoSave(float X, float Y, float Z, float radius, int model)  -- 073E
Ped ped = getRandomCharInSphere(float X, float Y, float Z, float radius, bool pedtypeCivilian, bool gang, bool prostitute)  -- 073F
bool result = hasCharBeenArrested(Ped ped)  -- 0741
setPlaneThrottle(int plane, float throttle)  -- 0742
heliLandAtCoords(Vehicle heli, float X, float Y, float Z, float minaltitude, float maxaltitude)  -- 0743
planeStartsInAir(int hydra)  -- 0745
setRelationship(int acquaintance, int pedtype, int toPedtype)  -- 0746
clearRelationship(int acquaintance, int pedtype, int toPedtype)  -- 0747
clearGroupDecisionMakerEventResponse(int maker, int event)  -- 0749
addGroupDecisionMakerEventResponse(int maker, int event, int taskID, float respect, float hate, float like, float dislike, bool inCar, bool onFoot)  -- 074A
drawSpriteWithRotation(int texture, float x, float y, float scaleX, float scaleY, float angle, int r, int g, int b, int a)  -- 074B
taskUseAttractor(Ped ped, int attractor)  -- 074C
taskShootAtChar(Ped ped, int atActor, int timelimit)  -- 074D
setInformRespectedFriends(int flags, float radius, int pedsToScan)  -- 074E
bool result = isCharRespondingToEvent(Ped ped, int event)  -- 074F
setObjectVisible(Object object, bool visibility)  -- 0750
taskFleeCharAnyMeans(Ped ped, int fleeFrom, float runDistance, int time, bool changeCourse, int unkTime1, int unkTime2, float awayRadius)  -- 0751
flushPatrolRoute()  -- 0754
extendPatrolRoute(float X, float Y, float Z, string animation, string IFPFile)  -- 0755
bool result = playObjectAnim(Object object, string animation, string IFPFile, float framedelta, bool lockF, bool loop)  -- 075A
setRadarZoom(int zoom)  -- 075B
bool result = doesBlipExist(Marker marker)  -- 075C
loadPrices(GxtString shopping)  -- 075D
loadShop(GxtString shopping)  -- 075E
int num = getNumberOfItemsInShop()  -- 075F
int item = getItemInShop(int index)  -- 0760
int price = getPriceOfItem(int item)  -- 0761
taskDead(Ped ped)  -- 0762
setCarAsMissionCar(Vehicle car)  -- 0763
setZonePopulationType(GxtString zone, int popcycle)  -- 0767
setZoneDealerStrength(GxtString zone, int density)  -- 076A
int strength = getZoneDealerStrength(GxtString zone)  -- 076B
setZoneGangStrength(GxtString zone, int gang, int density)  -- 076C
int density = getZoneGangStrength(GxtString zone, int gang)  -- 076D
bool result = isMessageBeingDisplayed()  -- 076F
setCharIsTargetPriority(Ped ped, bool targetPriority)  -- 0770
customPlateDesignForNextCar(Model modelNumplate, int townTexture)  -- 0771
taskGotoCar(Ped ped, Vehicle car, int timeMS, float stopAtDistance)  -- 0772
requestIpl(string group)  -- 0776
removeIpl(string group)  -- 0777
removeIplDiscreetly(string group)  -- 0778
setCharRelationship(Ped ped, int acquaintance, int pedtype)  -- 077A
clearCharRelationship(Ped ped, int acquaintance, int pedtype)  -- 077B
clearAllCharRelationships(Ped ped, int acquaintance)  -- 077C
float pitch = getCarPitch(Vehicle car)  -- 077D
int interior = getActiveInterior()  -- 077E
heliKeepEntityInView(Vehicle heli, Ped ped, Vehicle car, float minaltitude, float maxaltitude)  -- 0780
int model = getWeapontypeModel(int id)  -- 0781
int slot = getWeapontypeSlot(int id)  -- 0782
int info = getShoppingExtraInfo(int item, int flag)  -- 0783
givePlayerClothes(Player player, int texture, int model, int bodypart)  -- 0784
int num = getNumberOfFiresInArea(float x1, float y1, float z1, float x2, float y2, float z2)  -- 0786
attachWinchToHeli(Vehicle heli, bool magnet)  -- 0788
releaseEntityFromWinch(Vehicle heli)  -- 0789
int carriage = getTrainCarriage(Vehicle train, int handle)  -- 078A
Vehicle carHandle, Ped pedHandle, Object objectHandle = grabEntityOnWinch(Vehicle heli)  -- 078B
GxtString name = getNameOfItem(int item)  -- 078C
taskClimb(Ped ped, bool climb)  -- 078F
buyItem(int item)  -- 0790
clearCharTasksImmediately(Ped ped)  -- 0792
storeClothesState()  -- 0793
restoreClothesState()  -- 0794
float length = getRopeHeightForObject(int magnet)  -- 0796
setRopeHeightForObject(int magnet, float length)  -- 0797
Vehicle carHandle, Ped pedHandle, Object objectHandle = grabEntityOnRopeForObject(int magnet)  -- 0798
releaseEntityFromRopeForObject(int magnet)  -- 0799
playerEnteredDockCrane()  -- 079D
playerEnteredBuildingsiteCrane()  -- 079E
playerLeftCrane()  -- 079F
performSequenceTaskFromProgress(Ped ped, int sequence, int unkProgress1, int unkProgress2)  -- 07A0
setNextDesiredMoveState(int speed)  -- 07A1
taskGotoCharAiming(Ped ped, int followActor, float minradius, float maxradius)  -- 07A3
int unkProgress1, int unkProgress2 = getSequenceProgressRecursive(Ped ped)  -- 07A4
taskKillCharOnFootTimed(Ped ped, int attackActor, int time)  -- 07A5
float X, float Y, float Z = getNearestTagPosition(float X, float Y, float Z)  -- 07A6
taskJetpack(Ped ped)  -- 07A7
setArea51SamSite(bool enable)  -- 07A8
bool result, Searchlight searchlight = isCharInAnySearchlight(Ped ped)  -- 07A9
bool result = isTrailerAttachedToCab(Vehicle trailer, Vehicle car)  -- 07AB
detachTrailerFromCab(Vehicle trailer, Vehicle cab)  -- 07AC
int group = getPlayerGroup(Player player)  -- 07AF
GxtString shop = getLoadedShop()  -- 07B0
int int2, int int3, int int4 = getBeatProximity(int track)  -- 07B1
setGroupDefaultTaskAllocator(int group, int command)  -- 07B3
setPlayerGroupRecruitment(Player player, bool enabled)  -- 07B4
activateHeliSpeedCheat(Vehicle heli, int power)  -- 07BB
taskSetCharDecisionMaker(Ped ped, int maker)  -- 07BC
deleteMissionTrain(Vehicle train)  -- 07BD
markMissionTrainAsNoLongerNeeded(Vehicle train)  -- 07BE
setBlipAlwaysDisplayOnZoomedRadar(Marker marker, bool displayAlways)  -- 07BF
requestCarRecording(int path)  -- 07C0
bool result = hasCarRecordingBeenLoaded(int path)  -- 07C1
setMissionTrainCoordinates(Vehicle train, float X, float Y, float Z)  -- 07C7
taskComplexPickupObject(Ped ped, Object object)  -- 07C9
listenToPlayerGroupCommands(Ped ped, bool listen)  -- 07CB
setPlayerEnterCarButton(Player player, bool can)  -- 07CC
taskCharSlideToCoord(Ped ped, float toX, float toY, float toZ, float angle, float withinRadius)  -- 07CD
int weekday = getCurrentDayOfWeek()  -- 07D0
registerScriptBrainForCodeUse(int id, GxtString gxtString)  -- 07D3
applyForceToCar(Vehicle car, float vecX, float vecY, float vecZ, float rotationX, float rotationY, float rotationZ)  -- 07D5
addToCarRotationVelocity(Vehicle car, float vecX, float vecY, float vecZ)  -- 07DA
setCarRotationVelocity(Vehicle car, float vecX, float vecY, float vecZ)  -- 07DB
setCharShootRate(Ped ped, int rate)  -- 07DD
bool result = isModelInCdimage(Model modelId)  -- 07DE
removeOilPuddlesInArea(float x1, float y1, float x2, float y2)  -- 07DF
setBlipAsFriendly(Marker marker, bool type)  -- 07E0
taskSwimToCoord(Ped ped, float toX, float toY, float toZ)  -- 07E1
float x1, float y1, float z1, float x2, float y2, float z2 = getModelDimensions(Model modelId)  -- 07E4
int maker = copyCharDecisionMaker(Ped ped)  -- 07E5
int maker = copyGroupDecisionMaker(int group)  -- 07E6
taskDrivePointRouteAdvanced(Ped ped, Vehicle car, float speed, int flag1, int flag2, int flag3)  -- 07E7
bool result = isRelationshipSet(int acquaintance, int ofActors, int toActors)  -- 07E8
setCarAlwaysCreateSkids(Vehicle car, bool enable)  -- 07EE
int city = getCityFromCoords(float X, float Y, float Z)  -- 07EF
bool result = hasObjectOfTypeBeenSmashed(float X, float Y, float Z, float radius, Model modelId)  -- 07F0
bool result = isPlayerPerformingWheelie(Player player)  -- 07F1
bool result = isPlayerPerformingStoppie(Player player)  -- 07F2
setCheckpointCoords(Checkpoint checkpoint, float X, float Y, float Z)  -- 07F3
controlCarHydraulics(Vehicle car, float f1, float f2, float f3, float f4)  -- 07F5
int numberOfLeaders, int numberOfMembers = getGroupSize(int group)  -- 07F6
setObjectCollisionDamageEffect(Object object, bool destructible)  -- 07F7
setCarFollowCar(Vehicle car, int followCar, float radius)  -- 07F8
playerEnteredQuarryCrane()  -- 07F9
playerEnteredLasVegasCrane()  -- 07FA
switchEntryExit(GxtString interior, bool access)  -- 07FB
displayTextWithFloat(float X, float Y, GxtString GXT, float value, int flag)  -- 07FC
bool result = doesGroupExist(int group)  -- 07FD
giveMeleeAttackToChar(Ped ped, int fightingStyle, int moves)  -- 07FE
setCarHydraulics(Vehicle car, bool hydraulics)  -- 07FF
bool result = is2playerGameGoingOn()  -- 0800
float fov = getCameraFov()  -- 0801
bool result = doesCarHaveHydraulics(Vehicle car)  -- 0803
taskCharSlideToCoordAndPlayAnim(Ped ped, float toX, float toY, float toZ, float angle, float radius, string animation, int ifp1, float ifp2, bool LA, bool LX, bool LY, bool LF, int LT)  -- 0804
int number = getTotalNumberOfPedsKilledByPlayer(Player player)  -- 0806
float X, float Y, float Z = getLevelDesignCoordsForObject(Object object, int spoot)  -- 080A
int event = getCharHighestPriorityEvent(Ped ped)  -- 080E
float X, float Y, float Z = getParkingNodeInArea(float x1, float y1, float z1, float x2, float y2, float z2)  -- 0810
Vehicle car = getCarCharIsUsing(Ped ped)  -- 0811
taskPlayAnimNonInterruptable(Ped ped, string animation, string IFP, float framedelta, bool loopA, bool lockX, bool lockY, bool lockF, int time)  -- 0812
addStuntJump(float startX, float startY, float startZ, float radiusX, float radiusY, float radiusZ, float goalX, float goalY, float goalZ, float radius2X, float radius2Y, float radius2Z, float cameraX, float cameraY, float cameraZ, int reward)  -- 0814
setObjectCoordinatesAndVelocity(Object object, float X, float Y, float Z)  -- 0815
setCharKindaStayInSamePlace(Ped ped, bool stay)  -- 0816
taskFollowPatrolRoute(Ped ped, int walkMode, int routeMode)  -- 0817
bool result = isCharInAir(Ped ped)  -- 0818
float height = getCharHeightAboveGround(Ped ped)  -- 0819
setCharWeaponSkill(Ped ped, int skill)  -- 081A
setTextEdge(int size, int r, int g, int b, int a)  -- 081C
setCarEngineBroken(Vehicle car, bool broken)  -- 081D
bool result = isThisModelABoat(Model modelId)  -- 081E
bool result = isThisModelAPlane(Model modelId)  -- 081F
bool result = isThisModelAHeli(Model modelId)  -- 0820
setFirstPersonInCarCameraMode(bool enable)  -- 0822
taskGreetPartner(Ped ped, Ped ped2, float unk1, int unk2)  -- 0823
setHeliBladesFullSpeed(Vehicle heli)  -- 0825
displayHud(bool enable)  -- 0826
connectLods(Object object, int lod)  -- 0827
setMaxFireGenerations(int max)  -- 0828
taskDieNamedAnim(Ped ped, string animation, string ifp1, float ifp2, int time)  -- 0829
setPlayerDuckButton(Player player, bool able)  -- 082A
setPoolTableCoords(float x1, float y1, float z1, float x2, float y2, float z2)  -- 0830
bool result = hasObjectBeenPhotographed(Object object)  -- 0833
doCameraBump(float rotationZ, float rotationY)  -- 0834
int day, int month = getCurrentDate()  -- 0835
setObjectAnimSpeed(Object object, string animation, float speed)  -- 0836
bool result = isObjectPlayingAnim(Object object, string anim)  -- 0837
float progress = getObjectAnimCurrentTime(Object object, string animation)  -- 0839
setObjectAnimCurrentTime(Object object, string animation, float progress)  -- 083A
setCharVelocity(Ped ped, float vecX, float vecY, float vecZ)  -- 083C
float vecX, float vecY, float vecZ = getCharVelocity(Ped ped)  -- 083D
setCharRotation(Ped ped, float vecX, float vecY, float vecZ)  -- 083E
float value = getCarUprightValue(Vehicle car)  -- 083F
setVehicleInterior(Vehicle car, int interior)  -- 0840
selectWeaponsForVehicle(Vehicle car, bool gun)  -- 0841
int city = getCityPlayerIsIn(Player player)  -- 0842
GxtString name = getNameOfZone(float X, float Y, float Z)  -- 0843
activateInteriorPeds(bool activate)  -- 084D
setVehicleCanBeTargetted(Vehicle car, bool unk)  -- 084E
taskFollowFootsteps(Ped ped, int followActor)  -- 0850
damageChar(Ped ped, int health, bool affectArmour)  -- 0851
setCarCanBeVisiblyDamaged(Vehicle car, bool can)  -- 0852
setHeliReachedTargetDistance(Vehicle heli, int dist)  -- 0853
float level = getSoundLevelAtCoords(Ped ped, float X, float Y, float Z)  -- 0855
setCharAllowedToDuck(Ped ped, bool enable)  -- 0856
setHeadingForAttachedPlayer(Player player, float toAngle, float rotationSpeed)  -- 0858
taskWalkAlongsideChar(Ped ped, int alongisdeActor)  -- 0859
createEmergencyServicesCar(Model car, float X, float Y, float Z)  -- 085A
taskKindaStayInSamePlace(Ped ped, bool stay)  -- 085B
startPlaybackRecordedCarLooped(Vehicle car, int path)  -- 085E
setCharInterior(Ped ped, int interior)  -- 0860
bool result = isAttachedPlayerHeadingAchieved(Player player)  -- 0861
enableEntryExitPlayerGroupWarping(float X, float Y, float radius, bool access)  -- 0864
Object object = getClosestStealableObject(float X, float Y, float Z, float radius)  -- 0866
bool result = isProceduralInteriorActive(int interior)  -- 0867
removeCarRecording(int path)  -- 0873
setZonePopulationRace(GxtString zone, int popcycle)  -- 0874
setObjectOnlyDamagedByPlayer(Object object, bool player)  -- 0875
createBirds(float x1, float y1, float z1, float x2, float y2, float z2, int flag1, int flag2)  -- 0876
setVehicleDirtLevel(Vehicle car, float level)  -- 0878
setGangWarsActive(bool enable)  -- 0879
bool result = isGangWarGoingOn()  -- 087A
givePlayerClothesOutsideShop(Player player, string clothes, string model, int bodyPart)  -- 087B
clearLoadedShop()  -- 087C
setGroupSequence(int group, int Aspack)  -- 087D
setCharDropsWeaponsWhenDead(Ped ped, bool droppable)  -- 087E
setCharNeverLeavesGroup(Ped ped, bool set)  -- 087F
setPlayerFireButton(Player player, bool able)  -- 0881
attachFxSystemToCharBone(Particle particle, Ped ped, int mode)  -- 0883
registerAttractorScriptBrainForCodeUse(int handle, GxtString script)  -- 0884
setHeadingLimitForAttachedChar(Ped ped, int orientation, float limit)  -- 0887
Marker blip = addBlipForDeadChar(Ped ped)  -- 0888
float X, float Y, float Z = getDeadCharCoordinates(Ped ped)  -- 0889
taskPlayAnimWithFlags(Ped ped, string animation, string ifp, float framedelta, bool loopA, bool lockX, bool lockY, bool lockF, int time, bool force, bool lockZ)  -- 088A
setVehicleAirResistanceMultiplier(Vehicle car, float multiplier)  -- 088B
setCarCoordinatesNoOffset(Vehicle car, float X, float Y, float Z)  -- 088C
setUsesCollisionOfClosestObjectOfType(float X, float Y, float Z, float radius, Model modelId, bool collisionDetection)  -- 088D
setTimeOneDayForward()  -- 088E
setTimerBeepCountdownTime(VarId timer, int reach)  -- 0890
attachTrailerToCab(int trailer, int cab)  -- 0893
bool result = isVehicleTouchingObject(Vehicle car, Object object)  -- 0897
enableCraneControls(bool UP, bool DOWN, bool RELEASE)  -- 0898
bool result = isPlayerInPositionForConversation(Ped ped)  -- 089B
enableConversation(Ped ped, bool enable)  -- 089C
Ped ped = getRandomCharInSphereOnlyDrugsBuyers(float X, float Y, float Z, float radius)  -- 089E
int pedtype = getPedType(Ped ped)  -- 089F
bool result = taskUseClosestMapAttractor(Ped ped, float radius, Model nearModel, float offsetX, float offsetY, float offsetZ, string scriptNamed)  -- 08A0
planeAttackPlayerUsingDogFight(int hydra, Player player, float radius)  -- 08A2
canTriggerGangWarWhenOnAMission(bool can)  -- 08A3
controlMovableVehiclePart(Vehicle car, float angle)  -- 08A4
winchCanPickVehicleUp(Vehicle car, bool attractive)  -- 08A5
openCarDoorABit(Vehicle car, int door, float rotation)  -- 08A6
bool result = isCarDoorFullyOpen(Vehicle car, int door)  -- 08A7
setAlwaysDraw3dMarkers(bool set)  -- 08A8
streamScript(int script)  -- 08A9
bool result = hasStreamedScriptLoaded(int script)  -- 08AB
setGangWarsTrainingMission(bool set)  -- 08AC
setCharHasUsedEntryExit(Ped ped, float X, float Y, float radius)  -- 08AD
setCharMaxHealth(Ped ped, int health)  -- 08AF
setNightVision(bool enable)  -- 08B1
setInfraredVision(bool enable)  -- 08B2
setZoneForGangWarsTraining(GxtString zone)  -- 08B3
setCharCanBeKnockedOffBike(Ped ped, bool can)  -- 08C6
setCharCoordinatesDontWarpGang(Ped ped, float X, float Y, float Z)  -- 08C7
addPriceModifier(int item, int price)  -- 08C8
removePriceModifier(int item)  -- 08C9
initZonePopulationSettings()  -- 08CA
explodeCarInCutsceneShakeAndBits(Vehicle car, bool shake, bool effect, bool sound)  -- 08CB
bool result = isSkipCutsceneButtonPressed()  -- 08D0
bool result, float X, float Y, float Z = getCutsceneOffset()  -- 08D1
setObjectScale(Object object, float scale)  -- 08D2
int popcycle = getCurrentPopulationZoneType()  -- 08D3
int menu = createMenu(GxtString title, float posX, float posY, float width, int columns, bool interactive, bool background, int alignment)  -- 08D4
setMenuColumnOrientation(int menu, int column, int alignment)  -- 08D6
int item = getMenuItemSelected(int menu)  -- 08D7
int item = getMenuItemAccepted(int menu)  -- 08D8
activateMenuItem(int menu, int row, bool enable)  -- 08D9
deleteMenu(int menu)  -- 08DA
setMenuColumn(int menu, int column, GxtString header, GxtString data1, GxtString data2, GxtString data3, GxtString data4, GxtString data5, GxtString data6, GxtString data7, GxtString data8, GxtString data9, GxtString data10, GxtString data11, GxtString data12)  -- 08DB
setBlipEntryExit(Marker marker, float X, float Y, float radius)  -- 08DC
switchDeathPenalties(bool lose)  -- 08DD
switchArrestPenalties(bool lose)  -- 08DE
setExtraHospitalRestartPoint(float X, float Y, float Z, float radius, float angle)  -- 08DF
setExtraPoliceStationRestartPoint(float X, float Y, float Z, float radius, float angle)  -- 08E0
int num = findNumberTagsTagged()  -- 08E1
int percentage = getTerritoryUnderControlPercentage()  -- 08E2
bool result = isObjectInAngledArea2d(Object object, float x1, float y1, float x2, float y2, float radius, bool sphere)  -- 08E3
bool result = isObjectInAngledArea3d(Object object, float x1, float y1, float z1, float x2, float y2, float z2, float depth, bool flag)  -- 08E4
Ped ped = getRandomCharInSphereNoBrain(float X, float Y, float Z, float radius)  -- 08E5
setPlaneUndercarriageUp(int plane, bool set)  -- 08E6
disableAllEntryExits(bool disable)  -- 08E7
attachAnimsToModel(Model modelId, GxtString externalScript)  -- 08E8
setObjectAsStealable(Object object, bool liftable)  -- 08E9
setCreateRandomGangMembers(bool enable)  -- 08EA
addSparks(float posX, float posY, float posZ, float vecX, float vecY, float vecZ, int density)  -- 08EB
int class = getVehicleClass(Vehicle car)  -- 08EC
clearConversationForChar(Ped ped)  -- 08ED
setMenuItemWithNumber(int panel, int column, int row, GxtString gxtString, int number)  -- 08EE
setMenuItemWith2Numbers(int panel, int column, int row, GxtString gxtString, int numbers1, int numbers2)  -- 08EF
setCutsceneModelTexture(GxtString cutsceneModel, GxtString textureName)  -- 08F0
GxtString nameB = getNameOfInfoZone(float atX, float atY, float atZ)  -- 08F1
vehicleCanBeTargettedByHsMissile(Vehicle car, bool targetable)  -- 08F2
setFreebiesInVehicle(Vehicle car, bool containsGoodies)  -- 08F3
setScriptLimitToGangSize(bool max)  -- 08F4
makePlayerGangDisappear()  -- 08F5
makePlayerGangReappear()  -- 08F6
int textureCRC, int modelCRC = getClothesItem(Player player, int bodypart)  -- 08F7
showUpdateStats(bool display)  -- 08F8
setCoordBlipAppearance(Checkpoint checkpoint, int type)  -- 08FB
setHeathazeEffect(bool enable)  -- 08FD
bool result = isHelpMessageBeingDisplayed()  -- 08FE
bool result = hasObjectBeenDamagedByWeapon(Object object, int type)  -- 08FF
clearObjectLastWeaponDamage(Object object)  -- 0900
setPlayerJumpButton(Player player, bool enable)  -- 0901
int r, int g, int b, int a = getHudColour(int interface)  -- 0904
lockDoor(int door, bool lock)  -- 0905
setObjectMass(Object object, float mass)  -- 0906
float mass = getObjectMass(int int)  -- 0907
setObjectTurnMass(Object object, float turnMass)  -- 0908
float turnMass = getObjectTurnMass(Object object)  -- 0909
setSpecificZoneToTriggerGangWar(GxtString zone)  -- 090C
clearSpecificZonesToTriggerGangWar()  -- 090D
setActiveMenuItem(int panel, int activeRow)  -- 090E
markStreamedScriptAsNoLongerNeeded(int externalScript)  -- 090F
removeStreamedScript(int externalScript)  -- 0910
setMessageFormatting(bool priority, int leftmargin, int maxwidth)  -- 0912
startNewStreamedScript(int externalScript, table args)  -- 0913
setWeatherToAppropriateTypeNow()  -- 0915
winchCanPickObjectUp(Object object, bool enable)  -- 0916
switchAudioZone(GxtString zone, bool enableSound)  -- 0917
setCarEngineOn(Vehicle car, bool on)  -- 0918
setCarLightsOn(Vehicle car, bool lights)  -- 0919
Ped ped = getUserOfClosestMapAttractor(float sphereX, float sphereY, float sphereZ, float radius, Model modelId, string externalScriptNamed)  -- 091C
switchRoadsBackToOriginal(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 091D
switchPedRoadsBackToOriginal(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 091E
int landingGearStatus = getPlaneUndercarriagePosition(int plane)  -- 091F
cameraSetVectorTrack(float pointX, float pointY, float pointZ, float transverseX, float transverseY, float transverseZ, int time, bool smooth)  -- 0920
cameraSetLerpFov(float from, float to, int timelimit, bool smoothTransition)  -- 0922
switchAmbientPlanes(bool enable)  -- 0923
setDarknessEffect(bool enable, int value)  -- 0924
cameraResetNewScriptables()  -- 0925
int value = getNumberOfInstancesOfStreamedScript(int externalScript)  -- 0926
allocateStreamedScriptToRandomPed(int externalScript, Model actorModel, int priority)  -- 0928
allocateStreamedScriptToObject(int externalScript, Model objectModel, int priority, float radius, int type)  -- 0929
int handle = getGroupMember(int group, int member)  -- 092B
float height = getWaterHeightAtCoords(float atX, float atY, bool ignoreWaves)  -- 092E
cameraPersistTrack(bool lock)  -- 092F
cameraPersistPos(bool lock)  -- 0930
cameraPersistFov(bool lock)  -- 0931
bool result = cameraIsVectorMoveRunning()  -- 0933
bool result = cameraIsVectorTrackRunning()  -- 0934
cameraSetVectorMove(float cameraX, float cameraY, float cameraZ, float positionX, float positionY, float positionZ, int time, bool smoothTransition)  -- 0936
drawWindow(float cornerAX, float cornerAY, float cornerBX, float cornerBY, GxtString gxtString, int style)  -- 0937
attachCarToObject(Vehicle car, Object object, float offsetX, float offsetY, float offsetZ, float rotationX, float rotationY, float rotationZ)  -- 0939
setGarageResprayFree(GxtString garage, bool free)  -- 093A
setCharBulletproofVest(Ped ped, bool enable)  -- 093B
setCinemaCamera(bool lock)  -- 093D
setCharFireDamageMultiplier(Ped ped, float multiplier)  -- 093E
setGroupFollowStatus(int group, bool status)  -- 0940
setSearchlightClipIfColliding(Searchlight searchlight, bool flag)  -- 0941
bool result = hasPlayerBoughtItem(int item)  -- 0942
setCameraInFrontOfChar(Ped ped)  -- 0944
int maxArmour = getPlayerMaxArmour(Player player)  -- 0945
setCharUsesUpperbodyDamageAnimsOnly(Ped ped, bool uninterupted)  -- 0946
int spokenPhrase = setCharSayContext(Ped ped, int speech)  -- 0947
addExplosionVariableShake(float atX, float atY, float atZ, int type, float cameraShake)  -- 0948
attachMissionAudioToChar(int id, Ped ped)  -- 0949
updatePickupMoneyPerDay(Pickup pickup, int cash)  -- 094A
GxtString interiorName = getNameOfEntryExitCharUsed(Ped ped)  -- 094B
float coordX, float coordY, float coordZ, int number = getPositionOfEntryExitCharUsed(Ped ped)  -- 094C
bool result = isCharTalking(Ped ped)  -- 094D
disableCharSpeech(Ped ped, bool disable)  -- 094E
enableCharSpeech(Ped ped)  -- 094F
setUpSkip(float posX, float posY, float posZ, float angle)  -- 0950
clearSkip()  -- 0951
preloadBeatTrack(int soundtrack)  -- 0952
int status = getBeatTrackStatus()  -- 0953
playBeatTrack()  -- 0954
stopBeatTrack()  -- 0955
int max = findMaxNumberOfGroupMembers()  -- 0956
vehicleDoesProvideCover(Vehicle car, bool providesCover)  -- 0957
Pickup pickup = createSnapshotPickup(float atX, float atY, float atZ)  -- 0958
Pickup pickup = createHorseshoePickup(float atX, float atY, float atZ)  -- 0959
Pickup pickup = createOysterPickup(float atX, float atY, float atZ)  -- 095A
bool result = hasObjectBeenUprooted(Object object)  -- 095B
addSmokeParticle(float atX, float atY, float atZ, float velocityX, float velocityY, float velocityZ, int r, int g, int b, int a, float size, float factor)  -- 095C
bool result = isCharStuckUnderCar(Ped ped)  -- 095D
controlCarDoor(Vehicle car, int door, int unlatch, float angle)  -- 095E
float angle = getDoorAngleRatio(Vehicle car, int door)  -- 095F
setPlayerDisplayVitalStatsButton(Player player, bool display)  -- 0960
setCharKeepTask(Ped ped, bool keepTasks)  -- 0961
int id = createMenuGrid(GxtString gxtString, int positionX, int positionY, float width, int columns, bool interactive, bool background, int alignment)  -- 0964
bool result = isCharSwimming(Ped ped)  -- 0965
int status = getCharSwimState(Ped ped)  -- 0966
startCharFacialTalk(Ped ped, int time)  -- 0967
stopCharFacialTalk(Ped ped)  -- 0968
bool result = isBigVehicle(Vehicle car)  -- 0969
switchPoliceHelis(bool enable)  -- 096A
storeCarModState()  -- 096B
restoreCarModState()  -- 096C
Model modelId = getCurrentCarMod(Vehicle car, int slot)  -- 096D
bool result = isCarLowRider(Vehicle car)  -- 096E
bool result = isCarStreetRacer(Vehicle car)  -- 096F
forceDeathRestart()  -- 0970
syncWater()  -- 0971
setCharCoordinatesNoOffset(Ped ped, float atX, float atY, float atZ)  -- 0972
bool result = doesScriptFireExist(int fire)  -- 0973
resetStuffUponResurrection()  -- 0974
bool result = isEmergencyServicesVehicle(Vehicle car)  -- 0975
killFxSystemNow(Particle particle)  -- 0976
bool result = isObjectWithinBrainActivationRange(Player player)  -- 0977
int to = copySharedCharDecisionMaker(int from)  -- 0978
reportMissionAudioEventAtPosition(float atX, float atY, float atZ, int event)  -- 097A
reportMissionAudioEventAtObject(int at, int event)  -- 097B
attachMissionAudioToObject(int id, Object object)  -- 097C
int colours = getNumCarColours(Vehicle car)  -- 097D
extinguishFireAtPoint(float atX, float atY, float atZ, float radius)  -- 0980
bool result = hasTrainDerailed(Vehicle train)  -- 0981
setCharForceDieInCar(Ped ped, bool stayInCarWhenDead)  -- 0982
setOnlyCreateGangMembers(bool enable)  -- 0983
Model modelId = getObjectModel(Object object)  -- 0984
setCharUsesCollisionClosestObjectOfType(float sphereX, float sphereY, float sphereZ, float radius, Model modelId, bool solid, int forActor)  -- 0985
clearAllScriptFireFlags()  -- 0986
int blockingCar = getCarBlockingCar(Vehicle car)  -- 0987
int paintjob = getCurrentVehiclePaintjob(Vehicle car)  -- 0988
setHelpMessageBoxSize(int width)  -- 0989
setGunshotSenseRangeForRiot2(float range)  -- 098A
float angle = getCarMovingComponentOffset(Vehicle car)  -- 098D
setNamedEntryExitFlag(GxtString interior, int bitmask, bool flag)  -- 098E
pauseCurrentBeatTrack(bool paused)  -- 0991
setPlayerWeaponsScrollable(Player player, bool scrollable)  -- 0992
markRoadNodeAsDontWander(float atX, float atY, float atZ)  -- 0994
unmarkAllRoadNodesAsDontWander()  -- 0995
setCheckpointHeading(Checkpoint checkpoint, float angle)  -- 0996
setMissionRespectTotal(int respect)  -- 0997
awardPlayerMissionRespect(int respect)  -- 0998
setCarCollision(Vehicle car, bool collision)  -- 099A
changePlaybackToUseAi(Vehicle car)  -- 099B
cameraSetShakeSimulationSimple(int type, float timelimit, float intensity)  -- 099C
bool result = isNightVisionActive()  -- 099D
setCreateRandomCops(bool enable)  -- 099E
taskSetIgnoreWeaponRangeFlag(Ped ped, bool ignore)  -- 099F
taskPickUpSecondObject(Ped ped, Object object, float offsetX, float offsetY, float offsetZ, int bone, int int7, string animation, string file, int time)  -- 09A0
dropSecondObject(Ped ped, bool to)  -- 09A1
removeObjectElegantly(Object object)  -- 09A2
drawCrosshair(bool draw)  -- 09A3
setUpConversationNodeWithSpeech(GxtString question, GxtString answerY, GxtString answerN, int questionWav, int answerYWav, int answerNWav)  -- 09A4
showBlipsOnAllLevels(bool enable)  -- 09A6
setCharDruggedUp(Ped ped, bool druggedUp)  -- 09A7
bool result = isCharHeadMissing(Ped ped)  -- 09A8
int CRC32 = getHashKey(string string)  -- 09A9
setUpConversationEndNodeWithSpeech(GxtString gxtString, int speech)  -- 09AA
randomPassengerSay(int passengers, int audioTable)  -- 09AB
hideAllFrontendBlips(bool hide)  -- 09AC
setPlayerInCarCameraMode(int mode)  -- 09AD
bool result = isCharInAnyTrain(Ped ped)  -- 09AE
setUpSkipAfterMission(float posX, float posY, float posZ, float angle)  -- 09AF
setVehicleIsConsideredByPlayer(Vehicle car, bool accessible)  -- 09B0
Model modelId, int class = getRandomCarModelInMemory(bool unk)  -- 09B2
int doorStatus = getCarDoorLockStatus(Vehicle car)  -- 09B3
setClosestEntryExitFlag(float atX, float atY, float radius, int bitmask, bool flag)  -- 09B4
setCharSignalAfterKill(Ped ped, bool signal)  -- 09B5
setCharWantedByPolice(Ped ped, bool wanted)  -- 09B6
setZoneNoCops(GxtString zone, bool disableCops)  -- 09B7
addBlood(float atX, float atY, float atZ, float offsetX, float offsetY, float offsetZ, int density, int onActor)  -- 09B8
displayCarNames(bool show)  -- 09B9
displayZoneNames(bool show)  -- 09BA
bool result = isCarDoorDamaged(Vehicle car, int door)  -- 09BB
setCharCoordinatesDontWarpGangNoOffset(Ped ped, float atX, float atY, float atZ)  -- 09BC
setMinigameInProgress(bool enable)  -- 09BD
bool result = isMinigameInProgress()  -- 09BE
setForceRandomCarModel(Model modelId)  -- 09BF
Vehicle car = getRandomCarOfTypeInAngledAreaNoSave(float x1, float y1, float x2, float y2, float angle, int int6)  -- 09C0
addNextMessageToPreviousBriefs(bool int1)  -- 09C1
failKillFrenzy()  -- 09C2
bool result = isCopVehicleInArea3dNoSave(float cornerAX, float cornerAY, float cornerAZ, float cornerBX, float cornerBY, float cornerBZ)  -- 09C3
setPetrolTankWeakpoint(Vehicle car, bool enabled)  -- 09C4
bool result = isCharUsingMapAttractor(Ped ped)  -- 09C5
setPlayerModel(Player player, Model modelId)  -- 09C7
bool result = areSubtitlesSwitchedOn()  -- 09C8
removeCharFromCarMaintainPosition(Ped ped, Vehicle car)  -- 09C9
setObjectProofs(Object object, bool BP, bool FP, bool EP, bool CP, bool MP)  -- 09CA
bool result = isCarTouchingCar(Vehicle car1, Vehicle car2)  -- 09CB
bool result = doesObjectHaveThisModel(Object object, Model modelId)  -- 09CC
setTrainForcedToSlowDown(Vehicle train, bool forced)  -- 09CF
bool result = isVehicleOnAllWheels(Vehicle car)  -- 09D0
bool result = doesPickupExist(Pickup pickup)  -- 09D1
enableAmbientCrime(bool enable)  -- 09D2
clearWantedLevelInGarage()  -- 09D4
int unk = setCharSayContextImportant(Ped ped, int soundslot, bool flags1, bool flags2, bool flags3)  -- 09D5
setCharSayScript(Ped ped, int sound, bool flags1, bool flags2, bool flags3)  -- 09D6
forceInteriorLightingForPlayer(Player player, bool force)  -- 09D7
useDetonator()  -- 09D9
bool result = isMoneyPickupAtCoords(float atX, float atY, float atZ)  -- 09DA
setMenuColumnWidth(int panel, int column, int width)  -- 09DB
makeRoomInPlayerGangForMissionPeds(int group)  -- 09DD
bool result = isCharGettingInToACar(Ped ped)  -- 09DE
setUpSkipForSpecificVehicle(float posX, float posY, float posZ, float angle, Vehicle car)  -- 09E0
int price = getCarModelValue(Model modelId)  -- 09E1
int generator = createCarGeneratorWithPlate(float atX, float atY, float atZ, float angle, Model modelId, int color1, int color2, bool forceSpawn, int alarm, int doorLock, int minDelay, int maxDelay, string plate)  -- 09E2
bool result = findTrainDirection(Vehicle train)  -- 09E3
setAircraftCarrierSamSite(bool enable)  -- 09E4
drawLightWithRange(float atX, float atY, float atZ, int r, int g, int b, float radius)  -- 09E5
enableBurglaryHouses(bool enable)  -- 09E6
bool result = isPlayerControlOn(Player player)  -- 09E7
int interior = getCharActiveInterior(Ped ped)  -- 09E8
giveNonPlayerCarNitro(Vehicle car)  -- 09E9
playerTakeOffGoggles(Player player, bool useAnim)  -- 09EB
allowFixedCameraCollision(bool allow)  -- 09EC
bool result = hasCharSpottedCharInFront(Ped ped, Ped ped2)  -- 09ED
forceBigMessageAndCounter(bool stayOnScreen)  -- 09EE
setVehicleCameraTweak(Model carModel, float distance, float altitudeMultiplier, float angleX)  -- 09EF
resetVehicleCameraTweak()  -- 09F0
reportMissionAudioEventAtChar(Ped ped, int event)  -- 09F1
bool result = doesDecisionMakerExist(int maker)  -- 09F2
ignoreHeightDifferenceFollowingNodes(Ped ped, bool ignore)  -- 09F4
shutAllCharsUp(bool enable)  -- 09F5
setCharGetOutUpsideDownCar(Ped ped, bool canGetOut)  -- 09F6
reportMissionAudioEventAtCar(Vehicle car, int event)  -- 09F7
doWeaponStuffAtStartOf2pGame()  -- 09F8
bool result = hasGameJustReturnedFromFrontend()  -- 09FA
int language = getCurrentLanguage()  -- 09FB
bool result = isObjectIntersectingWorld(Object object)  -- 09FC
int width = getStringWidth(GxtString gxtString)  -- 09FD
resetVehicleHydraulics(Vehicle car)  -- 09FE
setRespawnPointForDurationOfMission(float posX, float posY, float posZ)  -- 09FF
bool result = isThisModelACar(Model modelId)  -- 0A01
switchOnGroundSearchlight(Searchlight searchlight, bool lightsThroughObstacles)  -- 0A02
bool result = isGangWarFightingGoingOn()  -- 0A03
bool result = isNextStationAllowed(Vehicle train)  -- 0A06
skipToNextAllowedStation(Vehicle train)  -- 0A07
int width = getStringWidthWithNumber(GxtString gxtString, int number)  -- 0A08
shutCharUpForScriptedSpeech(Ped ped, bool muted)  -- 0A09
enableDisabledAttractorsOnObject(Object object, bool enable)  -- 0A0A
loadSceneInDirection(float coordX, float coordY, float coordZ, float angle)  -- 0A0B
bool result = isPlayerUsingJetpack(Player player)  -- 0A0C
clearThisPrintBigNow(int style)  -- 0A0E
bool result = hasLanguageChanged()  -- 0A0F
incrementIntStatNoMessage(int stat, int value)  -- 0A10
setExtraCarColours(Vehicle car, int tertiaryColor, int quaternaryColor)  -- 0A11
int tertiaryColor, int quaternaryColor = getExtraCarColours(Vehicle car)  -- 0A12
manageAllPopulation()  -- 0A13
setNoResprays(bool enable)  -- 0A14
bool result = hasCarBeenResprayed(Vehicle car)  -- 0A15
attachMissionAudioToCar(int audioId, Vehicle car)  -- 0A16
setHasBeenOwnedForCarGenerator(int generator, bool owned)  -- 0A17
setUpConversationNodeWithScriptedSpeech(GxtString questionGXT, GxtString answerYesGXT, GxtString answerNoGXT, int questionWAV, int answerYesWAV, int answerNoWAV)  -- 0A18
setAreaName(GxtString gxtString)  -- 0A19
taskPlayAnimSecondary(Ped ped, string animation, string IFP, float framedelta, bool loopA, bool lockX, bool lockY, bool lockF, int time)  -- 0A1A
bool result = isCharTouchingChar(Ped ped, Ped ped2)  -- 0A1B
disableHeliAudio(Vehicle helicopter, bool disable)  -- 0A1C
taskHandGesture(Ped ped, Ped ped2)  -- 0A1D
takePhoto(bool unk)  -- 0A1E
incrementFloatStatNoMessage(int stat, float value)  -- 0A1F
setPlayerGroupToFollowAlways(Player player, bool followAlways)  -- 0A20
improveCarByCheating(Vehicle car, bool affectedByCheats)  -- 0A21
changeCarColourFromMenu(int panelID, Vehicle car, int colorslot, int activeRow)  -- 0A22
highlightMenuItem(int panel, int row, bool highlight)  -- 0A23
setDisableMilitaryZones(bool disable)  -- 0A24
setCameraPositionUnfixed(float xAngle, float zAngle)  -- 0A25
setRadioToPlayersFavouriteStation()  -- 0A26
setDeathWeaponsPersist(Ped ped, bool persist)  -- 0A27
setCharSwimSpeed(Ped ped, float speed)  -- 0A28
bool result = isPlayerClimbing(Player player)  -- 0A29
bool result = isThisHelpMessageBeingDisplayed(GxtString gxtString)  -- 0A2A
bool result = isWidescreenOnInOptions()  -- 0A2B
drawSubtitlesBeforeFade(bool flag)  -- 0A2C
drawOddjobTitleBeforeFade(bool flag)  -- 0A2D
taskFollowPathNodesToCoordWithRadius(Ped ped, float toX, float toY, float toZ, int mode, int time, float stopRadius)  -- 0A2E
setPhotoCameraEffect(bool firstPersonView)  -- 0A2F
fixCar(Vehicle car)  -- 0A30
setPlayerGroupToFollowNever(Player player, bool neverFollow)  -- 0A31
bool result = isCharAttachedToAnyCar(Ped ped)  -- 0A32
Ped ped = storeCarCharIsAttachedToNoSave(Vehicle car)  -- 0A33
setUpSkipForVehicleFinishedByScript(float posX, float posY, float posZ, float angle, Vehicle car)  -- 0A35
bool result = isSkipWaitingForScriptToFadeIn()  -- 0A36
forceAllVehicleLightsOff(bool off)  -- 0A37
int mode = getPlayerInCarCameraMode()  -- 0A39
bool result = isLastBuildingModelShotByPlayer(Player player, Model modelId)  -- 0A3A
clearLastBuildingModelShotByPlayer(Player player)  -- 0A3B
setUpConversationEndNodeWithScriptedSpeech(GxtString dialogueGxt, int wav)  -- 0A3C
activatePimpCheat(bool enable)  -- 0A3D
Ped ped = getRandomCharInAreaOffsetNoSave(float sphereX, float sphereY, float sphereZ, float radiusX, float radiusY, float radiusZ)  -- 0A3E
setScriptCoopGame(bool enable)  -- 0A3F
Marker marker = createUser3dMarker(float atX, float atY, float atZ, int color)  -- 0A40
removeUser3dMarker(Marker marker)  -- 0A41
getRidOfPlayerProstitute()  -- 0A43
displayNonMinigameHelpMessages(bool display)  -- 0A44
setRailtrackResistanceMult(float tracksFriction)  -- 0A45
switchObjectBrains(int externalScript, bool canBeStreamedIn)  -- 0A46
finishSettingUpConversationNoSubtitles()  -- 0A47
allowPauseInWidescreen(bool enable)  -- 0A48
float x, float y = getPcMouseMovement()  -- 0A4A
bool result = isPcUsingJoypad()  -- 0A4B
bool result = isMouseUsingVerticalInversion()  -- 0A4C
bool result = startNewCustomScript(zstring filepath, table args)  -- 0A92
launchCustomMission(zstring filepath, table args)  -- 0A94
int handle = getScmThreadStructNamed(GxtString thread)  -- 0AAA
setCleoSharedVar(int var, int value)  -- 0AB3
int value = getCleoSharedVar(int var)  -- 0AB4
sampSpawnPlayer()  -- 0AF6
uint handle = sampGetBase()  -- 0AF7
sampAddChatMessage(zstring text, uint color)  -- 0AF8
sampSendChat(zstring text)  -- 0AF9
bool result = isSampAvailable()  -- 0AFA
sampRequestClass(int class)  -- 0AFB
sampSendScmEvent(int event, int id, int param1, int param2)  -- 0AFC
sampSetSpecialAction(int action)  -- 0AFD
sampSendDeathByPlayer(int playerId, int reason)  -- 0AFE
bool result, Vehicle car = sampGetCarHandleBySampVehicleId(int id)  -- 0AFF
bool result, Ped ped = sampGetCharHandleBySampPlayerId(int id)  -- 0B20
bool result = sampIsChatInputActive()  -- 0B21
sampSetSendrate(int type, int rate)  -- 0B22
bool result = sampIsPlayerConnected(int id)  -- 0B23
uint structPtr = sampGetPlayerStructPtr(int id)  -- 0B24
int health = sampGetPlayerHealth(int id)  -- 0B25
int armor = sampGetPlayerArmor(int id)  -- 0B26
sampSetGamestate(int gamestate)  -- 0B27
sampDisconnectWithReason(bool timeout)  -- 0B28
sampSetLocalPlayerName(zstring name)  -- 0B29
int ping = sampGetPlayerPing(int id)  -- 0B2A
bool result, int id = sampGetPlayerIdByCharHandle(Ped handle)  -- 0B2B
bool result, int id = sampGetVehicleIdByCarHandle(Vehicle car)  -- 0B2C
bool result, float posX, float posY, float posZ = sampGetStreamedOutPlayerPos(int id)  -- 0B2F
sampSendEnterVehicle(int id, bool passenger)  -- 0B30
sampSendExitVehicle(int id)  -- 0B31
sampSendSpawn()  -- 0B32
sampSendDamageVehicle(Vehicle car, int panel, int doors, int lights, int tires)  -- 0B33
bool result = sampRegisterChatCommand(zstring cmd, function func)  -- 0B34
zstring name = sampGetPlayerNickname(int id)  -- 0B36
uint color = sampGetPlayerColor(int id)  -- 0B37
sampConnectToServer(zstring ip, uint port)  -- 0B38
zstring ip, uint port = sampGetCurrentServerAddress()  -- 0B39
zstring name = sampGetCurrentServerName()  -- 0B3A
sampShowDialog(int id, zstring caption, zstring text, zstring button1, zstring button2, int style)  -- 0B3B
bool result, int button, int list, zstring input = sampHasDialogRespond(int id)  -- 0B3C
Bitstream bs = raknetNewBitStream()  -- 0B3D
raknetDeleteBitStream(Bitstream bs)  -- 0B3E
raknetResetBitStream(Bitstream bs)  -- 0B3F
raknetBitStreamWriteBool(Bitstream bs, bool value)  -- 0B40
raknetBitStreamWriteInt8(Bitstream bs, int value)  -- 0B40
raknetBitStreamWriteInt16(Bitstream bs, int value)  -- 0B40
raknetBitStreamWriteInt32(Bitstream bs, int value)  -- 0B40
raknetBitStreamWriteFloat(Bitstream bs, float value)  -- 0B40
raknetBitStreamWriteBuffer(Bitstream bs, uint dest, uint size)  -- 0B40
raknetBitStreamWriteBitStream(Bitstream bs, Bitstream bitStream)  -- 0B40
raknetBitStreamWriteString(Bitstream bs, string str)  -- 0B40
raknetSendRpcEx(int rpc, Bitstream bs, int priority, int reliability, int channel, bool timestamp)  -- 0B41
raknetSendBitStreamEx(Bitstream bs, int priority, int reliability, int channel)  -- 0B42
int textlabel = sampCreate3dText(zstring text, uint color, float posX, float posY, float posZ, float distance, bool ignoreWalls, int playerId, int vehicleId)  -- 0B44
sampDestroy3dText(int textlabel)  -- 0B45
bool result = sampIs3dTextDefined(int 3dText)  -- 0B46
sampCloseCurrentDialogWithButton(int button)  -- 0B47
int list = sampGetCurrentDialogListItem()  -- 0B48
sampSetCurrentDialogListItem(int list)  -- 0B49
zstring text = sampGetCurrentDialogEditboxText()  -- 0B4A
sampSetCurrentDialogEditboxText(zstring text)  -- 0B4B
bool result = sampIsDialogActive()  -- 0B4C
int type = sampGetCurrentDialogType()  -- 0B4D
int id = sampGetCurrentDialogId()  -- 0B4E
int gamestate = sampGetGamestate()  -- 0B4F
Object object = sampGetObjectHandleBySampId(int id)  -- 0B50
Pickup pickup = sampGetPickupHandleBySampId(int id)  -- 0B51
int objectId = sampGetObjectSampIdByHandle(Object object)  -- 0B52
int pickupId = sampGetPickupSampIdByHandle(Pickup pickup)  -- 0B53
int count = sampGetListboxItemsCount()  -- 0B54
int animid = sampGetPlayerAnimationId(int playerId)  -- 0B57
zstring name, zstring file = sampGetAnimationNameAndFile(int animid)  -- 0B58
int id = sampFindAnimationIdByNameAndFile(zstring name, zstring file)  -- 0B59
int resX, int resY = getScreenResolution()  -- 0B5A
zstring text = sampGetListboxItemText(int item)  -- 0B5B
bool result = sampIsPlayerPaused(int id)  -- 0B5C
sampToggleCursor(bool show)  -- 0B5D
bool result = sampIsLocalPlayerSpawned()  -- 0B61
int action = sampGetPlayerSpecialAction(int id)  -- 0B62
bool result = sampUnregisterChatCommand(zstring cmd)  -- 0B63
bool result = sampIsPlayerNpc(int id)  -- 0B64
int score = sampGetPlayerScore(int id)  -- 0B65
sampSetChatString(int id, zstring text, zstring prefix, uint color, uint pcolor)  -- 0B74
zstring text, zstring prefix, uint color, uint pcolor = sampGetChatString(int id)  -- 0B75
sampSetChatInputText(zstring text)  -- 0B76
zstring text = sampGetChatInputText()  -- 0B77
sampfuncsLog(zstring msg)  -- 0B78
sampSetChatInputEnabled(bool enabled)  -- 0B79
uint rakclientPtr = sampGetRakclientInterface()  -- 0B7A
uint rakpeer = sampGetRakpeer()  -- 0B7B
uint address = sampGetRakclientFuncAddressByIndex(int index)  -- 0B7C
uint callbackAddress = sampGetRpcCallbackByRpcId(int index)  -- 0B7D
uint node = sampGetRpcNodeByRpcId(int index)  -- 0B7E
uint sampPtr = sampGetSampInfoPtr()  -- 0B7F
DxutDialog dialog = dxutCreateDialog(zstring name)  -- 0B80
bool result, int event, int id = dxutPopEvent(DxutDialog dialog)  -- 0B81
dxutAddButton(DxutDialog dialog, int id, zstring text, int posX, int posY, int sizeX, int sizeY)  -- 0B82
dxutAddCheckbox(DxutDialog dialog, int id, zstring text, int posX, int posY, int sizeX, int sizeY)  -- 0B83
dxutSetDialogPos(DxutDialog dialog, int posX, int posY, int sizeX, int sizeY)  -- 0B84
int posX, int posY, int sizeX, int sizeY = dxutGetDialogPosAndSize(DxutDialog dialog)  -- 0B85
dxutSetDialogVisible(DxutDialog dialog, bool visible)  -- 0B86
bool result = dxutIsDialogVisible(DxutDialog dialog)  -- 0B87
dxutAddEditbox(DxutDialog dialog, int id, zstring text, int posX, int posY, int sizeX, int sizeY)  -- 0B88
zstring text = dxutGetControlText(DxutDialog dialog, int id)  -- 0B89
raknetSendRpc(int rpc, Bitstream bs)  -- 0B8A
raknetSendBitStream(Bitstream bs)  -- 0B8B
bool result = sampIsCursorActive()  -- 0B8C
sampSetCursorMode(int mode)  -- 0B8D
int mode = sampGetCursorMode()  -- 0B8E
dxutSetControlVisible(DxutDialog dialog, int id, bool visible)  -- 0B90
dxutAddStatic(DxutDialog dialog, int id, zstring text, int posX, int posY, int sizeX, int sizeY)  -- 0B91
bool result = dxutIsCheckboxChecked(DxutDialog dialog, int id)  -- 0B92
dxutSetDialogBackgroundColor(DxutDialog dialog, uint color)  -- 0B93
dxutSetControlText(DxutDialog dialog, int id, zstring text)  -- 0B94
bool result = dxutControlIsVisible(DxutDialog dialog, int id)  -- 0B95
dxutAddSlider(DxutDialog dialog, int id, int posX, int posY, int sizeX, int sizeY, int max)  -- 0B96
int value = dxutGetSliderValue(DxutDialog dialog, int id)  -- 0B97
dxutSetSliderValue(DxutDialog dialog, int id, int value)  -- 0B98
dxutAddListbox(DxutDialog dialog, int id, int posX, int posY, int sizeX, int sizeY)  -- 0B99
dxutListboxInsertItem(DxutDialog dialog, int id, zstring element, uint data, int after)  -- 0B9A
int element, int count = dxutGetListboxSelectedItemAndCount(DxutDialog dialog, int id)  -- 0B9B
dxutListboxDeleteItem(DxutDialog dialog, int id, int element)  -- 0B9C
zstring text, uint data = dxutGetListboxItemTextAndData(DxutDialog dialog, int id, int element)  -- 0B9D
dxutCheckboxSetChecked(DxutDialog dialog, int id, bool checked)  -- 0B9E
dxutEnableDialogCaption(DxutDialog dialog, bool enable)  -- 0B9F
bool result = dxutIsDialogCaptionEnabled(DxutDialog dialog)  -- 0BA0
dxutSetDialogMinimized(DxutDialog dialog, bool minimized)  -- 0BA1
bool result = dxutIsDialogMinimized(DxutDialog dialog)  -- 0BA2
dxutDeleteControl(DxutDialog dialog, int id)  -- 0BA3
dxutDeleteDialog(DxutDialog dialog)  -- 0BA4
dxutSetFocusOnControl(DxutDialog dialog, int id)  -- 0BA5
dxutSetControlSize(DxutDialog dialog, int id, int sizeX, int sizeY)  -- 0BA6
int sizeX, int sizeY = dxutGetControlSize(DxutDialog dialog, int id)  -- 0BA7
dxutSetControlPos(DxutDialog dialog, int id, int posX, int posY)  -- 0BA8
int posX, int posY = dxutGetControlPos(DxutDialog dialog, int id)  -- 0BA9
dxutSetCheckboxColor(DxutDialog dialog, int id, uint color)  -- 0BAA
bool result = dxutIsDialogExists(DxutDialog dialog)  -- 0BAB
uint settingsPtr = sampGetServerSettingsPtr()  -- 0BAC
uint poolsPtr = sampGetSampPoolsPtr()  -- 0BAD
uint chatPtr = sampGetChatInfoPtr()  -- 0BAE
uint inputPtr = sampGetInputInfoPtr()  -- 0BAF
uint dialogPtr = sampGetDialogInfoPtr()  -- 0BB0
uint kill = sampGetKillInfoPtr()  -- 0BB1
uint miscPtr = sampGetMiscInfoPtr()  -- 0BB2
uint tdpoolPtr = sampGetTextdrawPoolPtr()  -- 0BB3
int objpoolPtr = sampGetObjectPoolPtr()  -- 0BB4
uint gzpoolPtr = sampGetGangzonePoolPtr()  -- 0BB5
uint tlabelpoolPtr = sampGetTextlabelPoolPtr()  -- 0BB6
uint playerpoolPtr = sampGetPlayerPoolPtr()  -- 0BB7
uint vehpoolPtr = sampGetVehiclePoolPtr()  -- 0BB8
uint pickuppoolPtr = sampGetPickupPoolPtr()  -- 0BB9
sampStorePlayerOnfootData(int id, uint dstBuffer)  -- 0BBA
sampStorePlayerIncarData(int id, uint dstBuffer)  -- 0BBB
sampStorePlayerPassengerData(int id, uint dstBuffer)  -- 0BBC
sampStorePlayerTrailerData(int id, uint dstBuffer)  -- 0BBD
sampStorePlayerAimData(int id, uint dstBuffer)  -- 0BBE
sampSendRconCommand(zstring cmd)  -- 0BBF
sampSendOnfootData(uint dataPtr)  -- 0BC0
sampSendIncarData(uint dataPtr)  -- 0BC1
sampSendPassengerData(uint dataPtr)  -- 0BC2
sampSendAimData(uint dataPtr)  -- 0BC3
sampSendBulletData(uint dataPtr)  -- 0BC4
sampSendTrailerData(uint dataPtr)  -- 0BC5
sampSendUnoccupiedData(uint dataPtr)  -- 0BC6
sampSendSpectatorData(uint dataPtr)  -- 0BC7
sampSendClickPlayer(int id, int source)  -- 0BC8
sampSendDialogResponse(int id, int button, int listitem, zstring input)  -- 0BC9
sampSendClickTextdraw(int id)  -- 0BCA
sampSendGiveDamage(int id, float damage, int weapon, int bodypart)  -- 0BCB
sampSendTakeDamage(int id, float damage, int weapon, int bodypart)  -- 0BCC
sampSendEditObject(bool playerObject, int objectId, int response, float posX, float posY, float posZ, float rotX, float rotY, float rotZ)  -- 0BCD
sampSendEditAttachedObject(int response, int index, int model, int bone, float offsetX, float offsetY, float offsetZ, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ)  -- 0BCE
sampSendInteriorChange(int id)  -- 0BCF
sampSendRequestSpawn()  -- 0BD0
sampSendPickedUpPickup(int id)  -- 0BD1
sampSendMenuSelectRow(int id)  -- 0BD2
sampSendMenuQuit()  -- 0BD3
sampSendVehicleDestroyed(int id)  -- 0BD4
bool result = sampIsScoreboardOpen()  -- 0BD5
sampToggleScoreboard(bool show)  -- 0BD6
zstring text = sampGetDialogText()  -- 0BD7
zstring caption = sampGetDialogCaption()  -- 0BD8
sampSetDialogClientside(bool clientside)  -- 0BD9
bool result = sampIsDialogClientside()  -- 0BDA
bool result = sampIsChatVisible()  -- 0BDB
int mode = sampGetChatDisplayMode()  -- 0BDC
sampSetChatDisplayMode(int mode)  -- 0BDD
pauseScmThread(uint thread)  -- 0BDE
resumeScmThread(uint thread)  -- 0BDF
bool value = raknetBitStreamReadBool(Bitstream bs)  -- 0BE7
int value = raknetBitStreamReadInt8(Bitstream bs)  -- 0BE7
int value = raknetBitStreamReadInt16(Bitstream bs)  -- 0BE7
int value = raknetBitStreamReadInt32(Bitstream bs)  -- 0BE7
float value = raknetBitStreamReadFloat(Bitstream bs)  -- 0BE7
raknetBitStreamReadBuffer(Bitstream bs, uint dest, uint size)  -- 0BE8
string value = raknetBitStreamReadString(Bitstream bs, uint size)  -- 0BE8
raknetBitStreamResetReadPointer(Bitstream bs)  -- 0BE9
raknetBitStreamResetWritePointer(Bitstream bs)  -- 0BEA
raknetBitStreamIgnoreBits(Bitstream bs, int amount)  -- 0BEB
raknetBitStreamSetWriteOffset(Bitstream bs, int offset)  -- 0BEC
raknetBitStreamSetReadOffset(Bitstream bs, int offset)  -- 0BED
uint value = raknetBitStreamGetNumberOfBitsUsed(Bitstream bs)  -- 0BEE
uint value = raknetBitStreamGetNumberOfBytesUsed(Bitstream bs)  -- 0BEF
uint value = raknetBitStreamGetNumberOfUnreadBits(Bitstream bs)  -- 0BF0
int value = raknetBitStreamGetWriteOffset(Bitstream bs)  -- 0BF1
int value = raknetBitStreamGetReadOffset(Bitstream bs)  -- 0BF2
uint value = raknetBitStreamGetDataPtr(Bitstream bs)  -- 0BF3
zstring string = raknetBitStreamDecodeString(Bitstream bs, int size)  -- 0BF4
raknetBitStreamEncodeString(Bitstream bs, zstring string)  -- 0BF5
raknetEmulRpcReceiveBitStream(int rpc, Bitstream bs)  -- 0BF6
raknetEmulPacketReceiveBitStream(int packet, Bitstream bs)  -- 0BF7
zstring name = raknetGetRpcName(int rpc)  -- 0BF8
zstring name = raknetGetPacketName(int packet)  -- 0BF9
bool result = setSampfuncsGlobalVar(zstring var, int value)  -- 0BFC
bool result, int value = getSampfuncsGlobalVar(zstring var)  -- 0BFD
sampCreate3dTextEx(int id, zstring text, uint color, float posX, float posY, float posZ, float distance, bool ignoreWalls, int playerId, int vehicleId)  -- 0C45
zstring string, uint color, float posX, float posY, float posZ, float distance, bool ignoreWalls, int playerId, int vehicleId = sampGet3dTextInfoById(int id)  -- 0C46
sampSet3dTextString(int id, zstring text)  -- 0C47
sampTextdrawCreate(int id, zstring text, float posX, float posY)  -- 0C48
sampTextdrawSetBoxColorAndSize(int id, int box, uint color, float sizeX, float sizeY)  -- 0C49
sampTextdrawSetAlign(int id, int align)  -- 0C4A
sampTextdrawSetProportional(int id, int proportional)  -- 0C4B
sampTextdrawSetStyle(int id, int style)  -- 0C4C
sampTextdrawSetShadow(int id, int shadow, uint color)  -- 0C4D
sampTextdrawSetOutlineColor(int id, int outline, uint color)  -- 0C4E
sampTextdrawSetModelRotationZoomVehColor(int id, int model, float rotX, float rotY, float rotZ, float zoom, int clr1, int clr2)  -- 0C4F
sampTextdrawSetString(int id, zstring text)  -- 0C50
sampTextdrawSetPos(int id, float posX, float posY)  -- 0C51
sampTextdrawSetLetterSizeAndColor(int id, float letSizeX, float letSizeY, uint color)  -- 0C52
int box, uint color, float sizeX, float sizeY = sampTextdrawGetBoxEnabledColorAndSize(int id)  -- 0C53
int align = sampTextdrawGetAlign(int id)  -- 0C54
int prop = sampTextdrawGetProportional(int id)  -- 0C55
int style = sampTextdrawGetStyle(int id)  -- 0C56
int shadow, uint color = sampTextdrawGetShadowColor(int id)  -- 0C57
int outline, uint color = sampTextdrawGetOutlineColor(int id)  -- 0C58
int model, float rotX, float rotY, float rotZ, float zoom, int clr1, int clr2 = sampTextdrawGetModelRotationZoomVehColor(int id)  -- 0C59
zstring text = sampTextdrawGetString(int id)  -- 0C5A
float posX, float posY = sampTextdrawGetPos(int id)  -- 0C5B
float letSizeX, float letSizeY, uint color = sampTextdrawGetLetterSizeAndColor(int id)  -- 0C5C
bool result = sampTextdrawIsExists(int id)  -- 0C5D
sampTextdrawDelete(int id)  -- 0C5E
bool result = isSampfuncsGlobalVarDefined(zstring var)  -- 0C5F
bool read, bool write = getSampfuncsGlobalVarAccessForThread(zstring var, uint thread)  -- 0C61
runSampfuncsConsoleCommand(zstring cmd)  -- 0C62
bool result = sampfuncsRegisterConsoleCommand(zstring cmd, function func)  -- 0C63
bool result = sampfuncsUnregisterConsoleCommand(zstring cmd)  -- 0C64
uint thread = createScmThreadAtPointer(uint pointer, table args)  -- 0C6B
setScmThreadLocalVar(uint thread, int var, any value)  -- 0C6C
int value = getScmThreadLocalVar(uint thread, int var)  -- 0C6D
destroyScmThread(uint thread)  -- 0C6E
restartScmThread(uint thread, table args)  -- 0C6F
bool result = isSampfuncsConsoleActive()  -- 0C7E
sampSetClientCommandDescription(zstring cmd, zstring text)  -- 0C7F
setSampfuncsConsoleCommandDescription(zstring cmd, zstring text)  -- 0C80
sampForceVehicleSync(int id)  -- 0C81
sampForceUnoccupiedSyncSeatId(int id, int seatId)  -- 0C82
sampForceOnfootSync()  -- 0C83
sampForceAimSync()  -- 0C84
sampForceTrailerSync(int id)  -- 0C85
sampForcePassengerSyncSeatId(int id, int seatId)  -- 0C86
sampForceStatsSync()  -- 0C87
sampForceWeaponsSync()  -- 0C88
int id = sampGetMaxPlayerId(bool streamed)  -- 0C8A
int count = sampGetPlayerCount(bool streamed)  -- 0C8B
sampProcessChatInput(zstring text)  -- 0C8F
bool result = sampIsChatCommandDefined(zstring cmd)  -- 0C90
bool result = isSampfuncsConsoleCommandDefined(zstring cmd)  -- 0C91
int version = getCleoLibraryVersion()  -- 0C92
]]

--[[ Functions
float x, float y, float z, float w = getObjectQuaternion(Object object)
setObjectQuaternion(Object object, float x, float y, float z, float w)
float x, float y, float z, float w = getVehicleQuaternion(Vehicle car)
setVehicleQuaternion(Vehicle car, float x, float y, float z, float w)
float x, float y, float z, float w = getCharQuaternion(Ped ped)
setCharQuaternion(Ped ped, float x, float y, float z, float w)
AudioStream handle = loadAudioStream(zstring audio)
setAudioStreamState(AudioStream handle, int state)
releaseAudioStream(AudioStream handle)
double length = getAudioStreamLength(AudioStream handle)
int state = getAudioStreamState(AudioStream handle)
float volume = getAudioStreamVolume(AudioStream audio)
setAudioStreamVolume(AudioStream audio, float volume)
setAudioStreamLooped(AudioStream audio, bool loop)
AudioStream handle = load3dAudioStream(zstring audio)
setPlay3dAudioStreamAtCoordinates(AudioStream handle, float posX, float posY, float posZ)
setPlay3dAudioStreamAtObject(AudioStream audio, Object object)
setPlay3dAudioStreamAtChar(AudioStream audio, Ped ped)
setPlay3dAudioStreamAtCar(AudioStream audio, Vehicle car)
AudioStream handle = loadAudioStreamFromMemory(uint address, uint size)
AudioStream handle = load3dAudioStreamFromMemory(uint address, uint size)
renderDrawLine(float pos1X, float pos1Y, float pos2X, float pos2Y, float width, uint color)
renderDrawBox(float posX, float posY, float sizeX, float sizeY, uint color)
renderDrawBoxWithBorder(float posX, float posY, float sizeX, float sizeY, uint color, float bsize, uint bcolor)
float length = renderGetFontDrawTextLength(DxFont font, zstring text, [bool ignoreColorTags=false])
float height = renderGetFontDrawHeight(DxFont font)
uint index = renderGetFontCharIndexAt(DxFont font, string text, float x, [bool ignoreColorTags=false])
float width = renderGetFontCharWidth(DxFont font, string char)
float width = renderGetFontCharWidth(DxFont font, uint char)
DxFont font = renderCreateFont(zstring font, int height, uint flags, [uint charset])
renderReleaseFont(DxFont font)
renderFontDrawText(DxFont font, zstring text, float posX, float posY, uint color, [bool ignoreColorTags=false])
renderDrawPolygon(float posX, float posY, float sizeX, float sizeY, int corners, float rotation, uint color)
DxTexture texture = renderLoadTextureFromFile(zstring file)
renderReleaseTexture(DxTexture texture)
renderDrawTexture(DxTexture texture, float posX, float posY, float sizeX, float sizeY, float rotation, uint color)
renderBegin(int type)
renderEnd()
renderColor(uint color)
renderVertex(float vX, float vY)
renderSetTexCoord(float posX, float posY)
renderBindTexture(DxTexture texture)
uint struct = renderGetTextureStruct(DxTexture texture)
uint sprite = renderGetTextureSprite(DxTexture texture)
uint sizeX, uint sizeY = renderGetTextureSize(DxTexture texture)
renderSetRenderState(int state, uint value)
DxTexture texture = renderLoadTextureFromFileInMemory(uint pointer, uint size)
script_version_number(int version)  -- (задаёт числовую версию скрипта, предназначен преимущественно для системы проверки обновлений)
script_version(string version)  -- (задаёт текстовую версию скрипта)
script_name(string name)  -- (задаёт название скрипта)
script_description(string description)  -- (задаёт описание скрипта)
script_authors(string author, ...)  -- (задаёт список авторов (разработчиков) скрипта)
script_author(string author)  -- (псевдоним функции 'script_authors', отличается только более подходящим названием для указания одного автора)
script_dependencies(string name, ...)  -- (задаёт зависимости скрипта, предназначен для вывода. в будущем получит дополнительную функциональность)
script_moonloader(int version)  -- (задаёт минимальную требуемую версию MoonLoader, выводит сообщение об ошибке, если версия не соответствует (не прекращает работу скрипта))
LuaScript s = thisScript()  -- (возвращает объект скрипта из которого была вызвана функция)
wait(int time)  -- (приостанавливает выполнение основного потока скрипта на заданное время в миллисекундах)
print(any value, ...)  -- (перезаписанная стандартная функция 'print', выводит текст в файл moonloader.log и вызывает событие 'onScriptMessage')
int value = getGameGlobal(int index)  -- (возвращает значение глобальной переменной игры по её индексу)
setGameGlobal(int index, int value)  -- (задаёт значение глобальной переменной игры)
uint ptr = getGameGlobalPtr(int index)  -- (возвращает адрес глобальной переменной игры по индексу)
bool loaded = isSampfuncsLoaded()  -- (проверяет плагин SAMPFUNCS на подключенность к игре)
bool loaded = isCleoLoaded()  -- (проверяет плагин CLEO на подключенность к игре)
bool loaded = isSampLoaded()  -- (проверяет, запущен ли SA:MP)
bool state = isKeyDown(int keyId)  -- (возвращает статус клавиши по её виртуальному идентификатору - нажата/не нажата)
reloadScripts()  -- (выгружает все загруженные скрипты и загружает заново из рабочей директории, учитывая добавленные и убранные)
bool status = isOpcodesAvailable()  -- (проверяет, доступны ли функции опкодов для использования. на этапе выполнения подпрограммы 'main' они гарантированно доступны, так что проверка не требуется)
int i = representFloatAsInt(float f)  -- (представляет значение дробного числа как целое. важно знать, что эта функция не предназначена для преобразования типа и используется исключительно в функциях, производящих непосредственные манипуляции с памятью процесса)
float i = representIntAsFloat(int i)  -- (представляет значение целого числа как дробное. ^)
setGxtEntry(string key, string text)  -- (создаёт или перезаписывает существующую GXT-запись для последующего использования во многих функциях игры, выводящих текстовую информацию)
string key = setFreeGxtEntry(string text)  -- (создаёт новую (!) GXT-запись с уникальным ключём и возвращает его. для изменения используйте функцию 'setGxtEntry')
string key = getFreeGxtKey()  -- (генерирует и возвращает новый (!) уникальный ключ для создания GXT-записи)
string text = getGxtText(string key)  -- (возвращает текст GXT-записи по ключу, поиск производится включая записи игры. возвращает пустую строку, если запись не существует)
clearGxtEntry(string key)  -- (удаляет GXT-запись по ключу, не затрагивает игровые записи)
bool active = isPauseMenuActive()  -- (проверяет, открыто ли игровое меню паузы)
bool foreground = isGameWindowForeground()  -- (проверяет, находится ли окно игры на переднем плане)
int major, int minor, int majorRev, int minorRev, int game, int region, bool steam, bool cracked = getGameVersion()  -- (возвращает информацию о версии игры)
int version = getMoonloaderVersion()  -- (возвращает версию MoonLoader)
double time = localClock()  -- (возвращает значение внутреннего таймера в формате 'секунды.миллисекунды'. в отличие от системного таймера (os.clock()), этот таймер работает только если игра активна (не свёрнута и находится на переднем плане))
freeTextures()  -- (освобождает все загруженные функцией 'loadSprite' текстуры, принадлежащие вызвавшему скрипту)
string path = getWorkingDirectory()  -- (возвращает полный путь к рабочей директории MoonLoader'а)
string path = getGameDirectory()  -- (возвращает полный путь к директории игры)
useRenderCommands(bool enable)  -- (аналог опкода 03F0. отличается тем, что затрагивает только рендер lua-скриптами и не влияет на рендер самой игры)
writeMemory(uint address, uint size, int value, bool virtualProtect)  -- (записывает значение в память процесса по указанному адресу. для записи дробного нужно предварительно конвертировать его в целое функцией 'representFloatAsInt'. аналог опкода CLEO 0A8C)
int value = readMemory(uint address, uint size, bool virtualProtect)  -- (читает значение из памяти процесса по указанному адресу. для чтения дробного нужно конвертировать результирующее значение функцией 'representIntAsFloat'. аналог опкода CLEO 0A8D)
bool result, uint handle = loadDynamicLibrary(string library)  -- (загружает динамическую библиотеку по названию и возвращает её хендл (адрес). аналог опкода CLEO 0AA2)
freeDynamicLibrary(uint handle)  -- (выгружает динамическую библиотеку по её хендлу. аналог опкода CLEO 0AA3)
bool result, uint proc = getDynamicLibraryProcedure(string proc, uint handle)  -- (получает адрес экспортируемой функции по названию и хендлу библиотеки. аналог опкода CLEO 0AA4)
bool result = doesFileExist(string file)  -- (проверяет файл на существование. аналог опкода CLEO 0AAB)
bool result = doesDirectoryExist(string directory)  -- (проверяет директорию на существование. аналог опкода CLEO 0AE4)
bool result = createDirectory(string directory)  -- (создаёт иерархию директорий для указанного пути. аналог опкода CLEO 0AE5)
float val = popFloat()  -- (извлекает верхнее значение из стека FPU и возвращает его. аналог опкода CLEO 0AE9)
bool result = isGameVersionOriginal()
uint memory = allocateMemory(uint size)
freeMemory(uint memory)
Filesearch handle, string name = findFirstFile(string mask)
string file = findNextFile(Filesearch handle)
findClose(Filesearch handle)
bool result, Ped ped = findAllRandomCharsInSphere(float posX, float posY, float posZ, float radius, bool findNext, bool skipDead)
bool result, Vehicle car = findAllRandomVehiclesInSphere(float posX, float posY, float posZ, float radius, bool findNext, bool skipWrecked)
bool result, Object object = findAllRandomObjectsInSphere(float posX, float posY, float posZ, float radius, bool findNext)
uint ptr = getCharPointer(Ped ped)
uint ptr = getCarPointer(Vehicle car)
uint struct = getObjectPointer(Object object)
int returnValue = callFunction(uint address, int params, int pop, ...)
int returnValue = callMethod(uint address, int struct, int params, int pop, ...)
Vehicle car, Ped ped = storeClosestEntities(Ped ped)
switchCarEngine(Vehicle car, bool state)
bool result, float posX, float posY, float posZ = getTargetBlipCoordinates()
int gears = getCarNumberOfGears(Vehicle car)
int gear = getCarCurrentGear(Vehicle car)
bool state = isCarSirenOn(Vehicle car)
bool state = isCarEngineOn(Vehicle car)
printHelpString(string text)
printStyledString(string text, int time, int style)
printString(string text, int time)
printStringNow(string text, int time)
bool result, Ped ped = getCharPlayerIsTargeting(Player player)
GxtString name = getNameOfVehicleModel(Model modelId)
bool result = testCheat(string text)
bool result = spawnVehicleByCheating(Model modelId)
Ped handle = getCharPointerHandle(uint ptr)
Vehicle handle = getVehiclePointerHandle(uint ptr)
Object handle = getObjectPointerHandle(uint ptr)
bool result, table colPoint = processLineOfSight(float originX, float originY, float originZ, float targetX, float targetY, float targetZ, [bool checkSolid=true], [bool car=false], [bool ped=false], [bool object=false], [bool particle=false], [bool seeThrough=false], [bool ignoreSomeObjects=false], [bool shootThrough=false])
bool result = setClipboardText(string text)
string text = getClipboardText()
int value = getStructElement(uint struct, int offset, uint size, [bool unprotect=false])
setStructElement(uint struct, int offset, uint size, int value, [bool unprotect=false])
float w, float x, float y, float z = convertMatrixToQuaternion(float rightX, float rightY, float rightZ, float frontX, float frontY, float frontZ, float upX, float upY, float upZ)
float rightX, float rightY, float rightZ, float frontX, float frontY, float frontZ, float upX, float upY, float upZ = convertQuaternionToMatrix(float w, float x, float y, float z)
float wposX, float wposY = convert3DCoordsToScreen(float posX, float posY, float posZ)
setGameKeyState(int key, int state)
int posX, int posY = getCursorPos()
float gposX, float gposY = convertWindowScreenCoordsToGameScreenCoords(float wposX, float wposY)
float wposX, float wposY = convertGameScreenCoordsToWindowScreenCoords(float gposX, float gposY)
float posX, float posY, float posZ = convertScreenCoordsToWorld3D(float posX, float posY, float depth)
uint handle = getModuleHandle(string module)
uint address = getModuleProcAddress(string module, string proc)
setVirtualKeyDown(int vkey, bool down)
setCharKeyDown(int ckey, bool down)
int index = downloadUrlToFile(string url, string file, function statusCallback)
bool state = isKeyJustPressed(int key)
bool result, float x, float y, float z, float w, float h = convert3DCoordsToScreenEx(float posX, float posY, float posZ, [bool checkMin=false], [bool checkMax=false])
float value = getStructFloatElement(uint struct, int offset, [bool unprotect=false])
setStructFloatElement(uint struct, int offset, float value, [bool unprotect=false])
bool state = wasKeyPressed(int key)
bool state = wasKeyReleased(int key)
int delta = getMousewheelDelta()
consumeWindowMessage([bool game=true], [bool scripts=true])
addEventHandler(string eventName, function callback)
bool paused = isGamePaused()
double time = gameClock()
script_properties(string property, ...)
script_url(string url)
any imports = import(string filename)
string json = encodeJson(table data)
table data = decodeJson(string json)
showCursor(bool show, [bool lockControls])
lockPlayerControl(bool lock)
bool locked = isPlayerControlLocked()
bool result = setBlipCoordinates(Marker blip, float x, float y, float z)
bool result = setTargetBlipCoordinates(float x, float y, float z)
bool result = placeWaypoint(float x, float y, float z)
bool result = removeWaypoint()
string path = getFolderPath(int csidl)
float value = getTimeStepValue()
uint devicePtr = getD3DDevicePtr()
table objects = getAllObjects()
table peds = getAllChars()
table vehicles = getAllVehicles()
float value = getGameGlobalFloat(int index)
setGameGlobalFloat(int index, float value)
LuaScript s = script.load(string file)  -- (загружает скрипт из файла и возвращает объект скрипта. возвращает nil, если скрипт не был загружен)
LuaScript s = script.find(string name)  -- (ищет загруженный скрипт по имени. возвращает nil, если скрипт не найден)
table list = script.list()  -- (возвращает массив загруженных скриптов)
LuaScript script = script.get(int scriptId)  -- (возвращает объект скрипта по идентификатору. если скрипта с указанным идентификатором не существует, то возвращает nil)
script.this
table data = inicfg.load([table default], [string file])
bool result = inicfg.save(table data, [string file])
int value = memory.read(uint address, uint size, [bool unprotect=false])
memory.write(uint address, int value, uint size, [bool unprotect=false])
int value = memory.getint8(uint address, [bool unprotect=false])
bool result = memory.setint8(uint address, int byte, [bool unprotect=false])
int value = memory.getint16(uint address, [bool unprotect=false])
bool result = memory.setint16(uint address, int word, [bool unprotect=false])
int value = memory.getint32(uint address, [bool unprotect=false])
bool result = memory.setint32(uint address, int dword, [bool unprotect=false])
double value = memory.getint64(uint address, [bool unprotect=false])
bool result = memory.setint64(uint address, double qword, [bool unprotect=false])
int value = memory.getuint8(uint address, [bool unprotect=false])
bool result = memory.setuint8(uint address, int byte, [bool unprotect=false])
int value = memory.getuint16(uint address, [bool unprotect=false])
bool result = memory.setuint16(uint address, int word, [bool unprotect=false])
int value = memory.getuint32(uint address, [bool unprotect=false])
bool result = memory.setuint32(uint address, int dword, [bool unprotect=false])
double value = memory.getuint64(uint address, [bool unprotect=false])
bool result = memory.setuint64(uint address, double qword, [bool unprotect=false])
float value = memory.getfloat(uint address, [bool unprotect=false])
bool result = memory.setfloat(uint address, float value, [bool unprotect=false])
double value = memory.getdouble(uint address, [bool unprotect=false])
bool result = memory.setdouble(uint address, double value, [bool unprotect=false])
int oldProtection = memory.unprotect(uint address, uint size)
int oldProtection = memory.protect(uint address, uint size, int newProtection)
memory.copy(uint destAddress, uint srcAddress, uint size, [bool unprotect=false])
bool result = memory.compare(uint address1, uint address2, uint size, [bool unprotect=false])
string str = memory.tostring(uint address, [uint size], [bool unprotect=false])
string hexstr = memory.tohex(uint address, uint size, [bool unprotect=false])
bool result = memory.hex2bin(string hex, uint dstAddress, uint size)
string bin = memory.hex2bin(string hex)
memory.fill(uint address, int value, uint size, [bool unprotect=false])
uint address = memory.strptr(string str)
LuaThread thread = lua_thread.create(function func, ...)
LuaThread thread = lua_thread.create_suspended(function func)
]]

--[[ Events
main()  -- (основная подпрограмма, запускается когда игра загружена. пока что только в ней и во вложенных функциях можно использовать задержку 'wait')
onExitScript(bool quitGame)  -- (выполняется в скрипте перед его нормальным завершением)
onScriptMessage(string msg, LuaScript script)  -- (вызывается когда любой скрипт добавляет новое сообщение в лог)
onSystemMessage(string msg, int type, LuaScript script)  -- (вызывается когда MoonLoader добавляет новое сообщение в лог)
onQuitGame()  -- (выполняется перед выходом из игры)
onSystemInitialized()  -- (выполняется после загрузки всех скриптов, но перед входом в основной цикл)
onScriptLoad(LuaScript script)  -- (выполняется при загрузке нового скрипта)
onScriptTerminate(LuaScript script, bool quitGame)  -- (вызывается перед выгрузкой любого скрипта. выполняется раньше, чем 'onExitScript')
bool process, int id, Bitstream bitStream = onReceiveRpc(int id, Bitstream bitStream)  -- (получение RPC)
bool process, int id, Bitstream bitStream = onReceivePacket(int id, Bitstream bitStream)  -- (получение пакета)
bool process, int id, Bitstream bitStream, int priority, int reliability, int orderingChannel, bool shiftTs = onSendRpc(int id, Bitstream bitStream, int priority, int reliability, int orderingChannel, bool shiftTs)  -- (отправка RPC)
bool process, int id, Bitstream bitStream, int priority, int reliability, int orderingChannel = onSendPacket(int id, Bitstream bitStream, int priority, int reliability, int orderingChannel)  -- (отправка пакета)
onWindowMessage(uint msg, uint wparam, int lparam)
onStartNewGame(int missionPackNumber)
table newSaveData = onSaveGame(table saveData)
onLoadGame(table saveData)
onD3DDeviceLost()
onD3DDeviceReset()
onD3DPresent()
]]