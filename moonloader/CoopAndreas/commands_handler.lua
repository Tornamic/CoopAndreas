function CommandsHandler(command, ishost)
    if command == '/q' then
        os.execute('taskkill /IM gta_sa.exe /F')
        return true
    elseif command == '/resp' or command == '/rr' then
        if ishost == true then
            if doesCharExist(friend) then
                deleteChar(friend)
            end 
            CreateFriend()
        else
            if doesCharExist(host) then
                deleteChar(host)
            end 
            CreateHost()
        end
        return true
    elseif command == '/rem' then
        local vehs = getAllVehicles()
        for i=1, table.getn(vehs) do
            if doesVehicleExist(vehs[i]) then
                deleteCar(vehs[i])    
            end
        end
        local chars = getAllChars()
        for i=1, table.getn(chars) do
            if doesCharExist(chars[i]) and chars[i] ~= PLAYER_PED and chars[i] ~= friend then
                deleteChar(chars[i])    
            end
        end

    end
    return false
end