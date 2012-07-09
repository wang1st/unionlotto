package.path = ";.\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?\\init.lua;C:\\Program Files (x86)\\Lua\\5.1\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\?\\init.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?.luac"
package.cpath = ".\\?.dll;.\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\?.dll;C:\\Program Files (x86)\\Lua\\5.1\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\?.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\loadall.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\loadall.dll"

require ("luasql.sqlite3")

local env = luasql.sqlite3()
local con = env:connect("local.db")
local sql = "select * from think_shuangseqiu order by draw asc"
local cur = con:execute(sql)
local row = cur:fetch({},"n")
local D = Draws()
while row ~= nil do
	D:add_new_draw({tonumber(row[1]),tonumber(row[2]),tonumber(row[3]),tonumber(row[4]),tonumber(row[5]),tonumber(row[6]),tonumber(row[7]),tonumber(row[8]),tonumber(row[9]),tonumber(row[10]),tonumber(row[11]),tonumber(row[12]),tonumber(row[13]),tonumber(row[14]),tonumber(row[15]),tonumber(row[16]),	tonumber(row[17]),tonumber(row[18]),tonumber(row[19]),tonumber(row[20]),tonumber(row[21]),tonumber(row[22]),tonumber(row[23]),tonumber(row[24]),tonumber(row[25]),tonumber(row[26]),tonumber(row[27]),tonumber(row[28])})
--	print(tonumber(row[1]))
	row = cur:fetch({},"n")
end
cur:close()
con:close()
env:close()