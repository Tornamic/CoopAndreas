-- Пример использования SL:NET в качестве сервера
-- Примечание: SLNetLoop рекомендуется выполнять асинхронно, если возможно
-- Можно также выполнять в основном цикле, но возможны некоторые задержки

-- Нужно для корректной работы примера, можно убрать если есть папка с библиотеками
package.path = package.path .. ';../?.lua;../?.luac;../../?.lua;../../?.luac'
package.cpath = package.cpath .. ';../?.dll;../../?.dll'
-- Нужно для корректной работы примера, можно убрать если есть папка с библиотеками

require 'SLNet'

netHandle = SLNetInit() -- инициализируем модуль SL:Net, вызывать при каждой смене IP:PORT
netHandle:bind('*', 6666) -- занимаем локальный IP и порт 6666, на него будут подключаться клиеты
-- можно также использовать функцию SLNetBind(netHandle, IP, Port)
netHandle:setPrefix('EXMPL') -- устанавливает проверочный префик в начало пакета
-- сервер и все клиенты должны иметь один и тот же префикс, иначе пакет не обработается
-- можно также использовать функцию SLNetSetPrefix(netHandle, Prefix)

-- в данной callback-функции мы будем обрабатывать приходящие пакеты
function onReceiveData(packetID, bitStream, addr, port)
  print('Message from ' .. addr .. ':' .. port)

  if packetID == 1 then -- если нам пришел первый PacketID
    -- собираем те параметры, которые нужно получить от сервера
    local a, b = bitStream:read(UINT8), bitStream:read(STRING, bitStream:read(UINT8))
    print('Message Content: ' .. a .. ', ' .. b)
  end

  local testString = 'Hello SL:NET World!'

  local BSNew = BitStream:new() -- создадим BitStream и запишем в него данные
  BSNew:write(UINT8, 128):write(UINT8, #testString):write(STRING, testString)

  SLNetSend(netHandle, 1, BSNew, addr, port, 5) -- отправляем сообщение с PacketID 1 и приоритетом 5
  -- приоритет помогает увеличить шанс доставки пакета, по сути он будет отправляться до
  -- тех пор, пока не придет подтверждение получения, а количество повторов зависит от
  -- значения четвертого аргумента, не может быть больше 255 повторений - это лимит
  -- пакеты не имеющие сильного значения лучше отправлять с приоритетом ноль
end

netHandle:setHook(onReceiveData) -- ставим хук на входящие пакеты
-- можно также использовать функцию SLNetSetHook(netHandle, Callback)

while true do
  netHandle:loop() -- обязательно вызывать в цикле, желательно в начале
  -- этот метод отвечает за прогон основных потоков получения и отправки данных
  -- вызывать только после того, как произошел bind/connect, иначе вернет FALSE
  -- метод также имеет зеркальную функцию: SLNetLoop(netHandle)
end