function test()
  local myPlayer = getMyPlayer()
  if ( myPlayer:Alive() ) then
    return true
  end
  return false
end

function getMyPlayer()
    local player = {}
    function player:Alive()
        return 1
    end
    return player
end

assert(test())

print '58 Ok'