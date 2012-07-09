package.path = ";.\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?\\init.lua;C:\\Program Files (x86)\\Lua\\5.1\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\?\\init.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?.luac"
package.cpath = ".\\?.dll;.\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\?.dll;C:\\Program Files (x86)\\Lua\\5.1\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\?.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\loadall.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\loadall.dll"

require ("luacurl")
require ("LuaXml")

local function get_html(url)
    local result = { }
    local c = curl.new()
	c:setopt(curl.OPT_URL, url)
	c:setopt(curl.OPT_WRITEDATA, result)
	c:setopt(curl.OPT_WRITEFUNCTION, function(tab, buffer)
			table.insert(tab, buffer)
			return #buffer
		end)
    local ok = c:perform()
    c:close()
    c = nil
    return ok, table.concat(result)
end


local function get_expire_date(html) --process_html
	local xfile = xml.eval(html)
	local expire_date = "2012-01-01"
	if(#xfile > 0) then
		for k,v in ipairs(xfile[1]) do
			if(v:tag() == "expire_date") then
				expire_date = v[1]
				break
			end
		end
	end
	print(expire_date)
	return expire_date
end

local D = Draws()
local user_code = D:get_user_code()
local url = "http://twocolorball.sinaapp.com/index.php/Verify/logon/user_code/" .. user_code
--local f = io.open("log.txt","w")
--f:write(url)
local c, html = get_html(url)
local expire_date = get_expire_date(html)
expire_date = string.gsub(expire_date, "-", "")
--f:write(expire_date)
--f:close()
D:set_expire_date(tonumber(expire_date))

