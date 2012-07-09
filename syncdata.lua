package.path = ";.\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?\\init.lua;C:\\Program Files (x86)\\Lua\\5.1\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\?\\init.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?.luac"
package.cpath = ".\\?.dll;.\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\?.dll;C:\\Program Files (x86)\\Lua\\5.1\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\?.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\loadall.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\loadall.dll"

require ("luacurl")
require ("luasql.sqlite3")
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


local function process_html(html)
	local env = luasql.sqlite3()
	local con = env:connect("local.db")
	local xfile = xml.eval(html)
	for k,v in ipairs(xfile) do
		local draw,redball1,redball2,redball3,redball4,redball5,redball6,blueball
		local position1,position2,position3,position4,position5,position6
		local sales,pools
		local winners1,bonus1,winners2,bonus2,winners3,bonus3,winners4,bonus4,winners5,bonus5,winners6,bonus6 
		if(v:tag() == "item") then
			draw		=	v[1][1]
			redball1	=	v[2][1]
			redball2	=	v[3][1]
			redball3	=	v[4][1]
			redball4	=	v[5][1]
			redball5	=	v[6][1]
			redball6	=	v[7][1]
			blueball	=	v[8][1]
			position1	=	v[9][1]
			position2	=	v[10][1]
			position3	=	v[11][1]
			position4	=	v[12][1]
			position5	=	v[13][1]
			position6	=	v[14][1]
			sales		=	v[15][1]
			pools		=	v[16][1]
			winners1	=	v[17][1]
			bonus1		=	v[18][1]
			winners2	=	v[19][1]
			bonus2		=	v[20][1]
			winners3	=	v[21][1]
			bonus3		=	v[22][1]
			winners4	=	v[23][1]
			bonus4		=	v[24][1]
			winners5	=	v[25][1]
			bonus5		=	v[26][1]
			winners6	=	v[27][1]
			bonus6		=	v[28][1]
			local sql = "insert or replace into think_shuangseqiu(draw,redball1,redball2,redball3,redball4,redball5,redball6,blueball,position1,position2,position3,position4,position5,position6,sales,pools,winners1,bonus1,winners2,bonus2,winners3,bonus3,winners4,bonus4,winners5,bonus5,winners6,bonus6) values(" .. draw ..",".. redball1 .. "," ..redball2 .. "," ..redball3 .. "," .. redball4 .. "," .. redball5 .. "," .. redball6 .. "," .. blueball .. "," .. position1 .. "," .. position2 .. "," .. position3 .. "," .. position4 .. "," .. position5 .. "," .. position6 .. "," .. sales .. "," .. pools .. "," .. winners1 .. "," .. bonus1 .. "," .. winners2 .. "," .. bonus2 .. "," .. winners3 .. "," .. bonus3 .. "," .. winners4 .. "," .. bonus4 .. "," .. winners5 .. "," .. bonus5 .. "," .. winners6 .. "," .. bonus6 .. ")"
			con:execute(sql)
		end
	end
	con:close()
	env:close()
end

local function get_last_draw()
	local env = luasql.sqlite3()
	local con = env:connect("local.db")
	local sql = "select draw from think_shuangseqiu order by draw desc limit 1 offset 5"
	local cur = con:execute(sql)
	local row = cur:fetch({},"n")
	cur:close()
	con:close()
	env:close()
	return row[1]
end

local draw = "00" .. get_last_draw()
draw = string.sub(draw, string.len(draw) - 5 + 1)
local url = "http://localhost/index.php/Index/list_draw/lastdraw/" .. draw
local f = io.open("log.txt","w")
f:write(url)
f:close()
local c, html = get_html(url)
process_html(html)