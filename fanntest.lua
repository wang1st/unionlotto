package.path = ";.\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?\\init.lua;C:\\Program Files (x86)\\Lua\\5.1\\?.lua;C:\\Program Files (x86)\\Lua\\5.1\\?\\init.lua;C:\\Program Files (x86)\\Lua\\5.1\\lua\\?.luac"
package.cpath = ".\\?.dll;.\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\?.dll;C:\\Program Files (x86)\\Lua\\5.1\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\?.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\?51.dll;C:\\Program Files (x86)\\Lua\\5.1\\loadall.dll;C:\\Program Files (x86)\\Lua\\5.1\\clibs\\loadall.dll"

require ("luasql.sqlite3")
require("luafann")

local function split_params(row, redball)
	local t = {}
	local j = 1
	if(redball == true) then
		for i = 1, 33 do
			if( i == tonumber(row[j])) then
				t[i] = 1
				if(j < 6) then
					j = j + 1
				end
			else
				t[i] = 0
			end
		end
	else
		for i = 1, 16 do
			if(i == tonumber(row[7])) then
				t[i] = 1
			else
				t[i] = 0
			end
		end
	end
	
	if(redball == true) then
		return t[1],t[2],t[3],t[4],t[5],t[6],t[7],t[8],t[9],t[10],t[11],t[12],t[13],t[14],t[15],t[16],t[17],t[18],t[19],t[20],t[21],t[22],t[23],t[24],t[25],t[26],t[27],t[28],t[29],t[30],t[31],t[32],t[33]
	else
		return t[1],t[2],t[3],t[4],t[5],t[6],t[7],t[8],t[9],t[10],t[11],t[12],t[13],t[14],t[15],t[16]
	end
end

local function pick_numbers(row)
	local t = {}
	local n = #row
	for i = 1, n do
		t[i] = {k = i, v = row[i]}
	end
	table.sort(t, function(a,b)	if( a.v > b.v) then return true else return false end end)
	if(n == 16) then 
		return t[1].k
	else
		return {t[1].k,t[2].k,t[3].k,t[4].k,t[5].k,t[6].k}
	end
end


local env = luasql.sqlite3()
local con = env:connect("local.db")
local sql = "select redball1, redball2, redball3, redball4, redball5, redball6, blueball from think_shuangseqiu order by draw desc limit 2"
local cur1 = con:execute(sql)
local rows = {}
rows[1] = cur1:fetch({},"n")
rows[2] = cur1:fetch({},"n")

local last_bonus = {}
sql = "select draw, bonus1, bonus2, bonus3, bonus4, bonus5, bonus6 from think_shuangseqiu order by draw desc limit 1"
local cur2 = con:execute(sql)
last_bonus = cur2:fetch({},"a")

cur1:close()
cur2:close()
con:close()
env:close()

local times = {"2y","3y","5y","8y","all"}
local blueball = {}
local redballs = {}

for i = 1, 5 do
	redballs[i] = {}
	local ann =  fann.create_from_file("red_train_" .. times[i] .. ".net")
	for j = 1, 2 do
		local t = {}
		t[1],t[2],t[3],t[4],t[5],t[6],t[7],t[8],t[9],t[10],t[11],t[12],t[13],t[14],t[15],t[16],t[17],t[18],t[19],t[20],t[21],t[22],t[23],t[24],t[25],t[26],t[27],t[28],t[29],t[30],t[31],t[32],t[33] = ann:run(split_params(rows[j], true))
		redballs[i][j] = pick_numbers(t)
	end
	local ann =  fann.create_from_file("blue_train_" .. times[i] .. ".net")
	blueball[i] = {}
	for j = 1, 2 do
		local t = {}
		t[1],t[2],t[3],t[4],t[5],t[6],t[7],t[8],t[9],t[10],t[11],t[12],t[13],t[14],t[15],t[16] = ann:run(split_params(rows[j], false))
		blueball[i][j] = pick_numbers(t)
	end
end


local D = Draws()
local balls = {}
balls[1] = {} --下期预测
for i = 1, 5 do
	balls[1][i] = {}
	table.sort(redballs[i][1])
	for j = 1, 6 do
		io.write(redballs[i][1][j],",")
		balls[1][i][j] = redballs[i][1][j]
	end
	io.write(blueball[i][1],"\n")
	balls[1][i][7] = blueball[i][1]
	
	D:set_predition({}, balls[1][i])
end
	io.write("\n")

balls[2] = {} --上期预测
for i = 1, 5 do
	balls[2][i] = {}
	table.sort(redballs[i][2])
	local redshot = 0
	local blueshot = 0
	for j = 1, 6 do
		io.write(redballs[i][2][j],",")
		balls[2][i][j] = redballs[i][2][j]
		for k = 1, 6 do
--			print(rows[1][k])
			if(balls[2][i][j] == tonumber(rows[1][k])) then
				redshot = redshot + 1
			end
		end
	end
	balls[2][i][7] = blueball[i][2]
	if(balls[2][i][7] == tonumber(rows[1][7])) then
		blueshot = 1
	end
	io.write(blueball[i][2])

	local level = 0
	local bonus = 0
	if(redshot == 6 and blueshot == 1) then
		level = 1
		bonus = last_bonus["bonus1"]
	elseif(redshot == 6 and blueshot == 0) then
		level = 2
		bonus = last_bonus["bonus2"]
	elseif(redshot == 5 and blueshot == 1) then
		level = 3
		bonus = 3000
	elseif(redshot == 5 and blueshot == 0) then
		level = 4
		bonus = 200
	elseif(redshot == 4 and blueshot == 1) then
		level = 4
		bonus = 200
	elseif(redshot == 4 and blueshot == 0) then
		level = 5
		bonus = 10
	elseif(redshot == 3 and blueshot == 1) then
		level = 5
		bonus = 10
	elseif(blueshot == 1) then
		level = 6
		bonus = 5
	else
		level = 0
		bonus = 0
	end
	
	local params = {}
	params[1] = tonumber(last_bonus["draw"])
	params[2] = redshot
	params[3] = blueshot
	params[4] = level
	params[5] = bonus
	
	D:set_predition(params, balls[2][i])
end

