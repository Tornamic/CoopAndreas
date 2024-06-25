-- Пример использования SL:NET в качестве клиента
-- Примечание: SLNetLoop рекомендуется выполнять асинхронно, если возможно
-- Можно также выполнять в основном цикле, но возможны некоторые задержки

-- Нужно для корректной работы примера, можно убрать если есть папка с библиотеками
package.path = package.path .. ';../?.lua;../?.luac;../../?.lua;../../?.luac'
package.cpath = package.cpath .. ';../?.dll;../../?.dll'
-- Нужно для корректной работы примера, можно убрать если есть папка с библиотеками

require 'SLNet' -- подключаем модуль SL:NET для дальнейшего использования

netHandle = SLNetInit() -- инициализируем модуль SL:Net, нужно делать при каждой смене сервера
-- то есть, если вы хотите подключиться к другому серверу, сначала инициализируйте SL:NET
netHandle:connect('127.0.0.1', 6666) -- подключаемся к серверу по набору IP:PORT
-- можно также использовать функцию SLNetConnect(netHandle, IP, Port)
netHandle:setPrefix('EXMPL') -- устанавливает проверочный префик в начало пакета
-- сервер и все клиенты должны иметь один и тот же префикс, иначе пакет не обработается
-- можно также использовать функцию SLNetSetPrefix(netHandle, Prefix)


-- в данной callback-функции мы будем обрабатывать приходящие пакеты
function onReceiveData(packetID, bitStream)
  print('New Incoming Message => ' .. bitStream:export())

  if packetID == 1 then -- если нам пришел первый PacketID
    -- собираем те параметры, которые нужно получить от сервера
    local a, b = bitStream:read(UINT8), bitStream:read(STRING, bitStream:read(UINT8))
    print('Message Content: ' .. a .. ', ' .. b)
  end

  local testString = 'Hello SL:NET World!'

  local BSNew = BitStream:new() -- создадим BitStream и запишем в него данные
  BSNew:write(UINT8, 128):write(UINT8, #testString):write(STRING, testString)

  SLNetSend(netHandle, 1, BSNew, 5) -- отправляем сообщение с PacketID 1 и приоритетом 5
  -- приоритет помогает увеличить шанс доставки пакета, по сути он будет отправляться до
  -- тех пор, пока не придет подтверждение получения, а количество повторов зависит от
  -- значения четвертого аргумента, не может быть больше 255 повторений - это лимит
  -- пакеты не имеющие сильного значения лучше отправлять с приоритетом ноль
end

netHandle:setHook(onReceiveData) -- ставим хук на входящие пакеты
-- можно также использовать функцию SLNetSetHook(netHandle, Callback)

SLNetSend(netHandle, 1, nil, 5) -- отправить можно и пустой пакет, зачем - не знаю
while true do
  netHandle:loop() -- обязательно вызывать в цикле, желательно в начале
  -- этот метод отвечает за прогон основных потоков получения и отправки данных
  -- вызывать только после того, как произошел bind/connect, иначе вернет FALSE
  -- метод также имеет зеркальную функцию: SLNetLoop(netHandle)
end