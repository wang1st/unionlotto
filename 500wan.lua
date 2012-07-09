require ("luacurl")
local MYSQL = false
require ("luasql.mysql")
require ("luasql.sqlite3")

function if_not_digital_then_zero(str)
	if(tonumber(str) == nil) then
		return "0"
	else
		return str
	end
end

function get_html(url)
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


function process_html(html)
	--<font class="cfont2"><strong>03001</strong></font>
	draw = string.match(html, "<font class=\"cfont2\"><strong>(%d+)</strong></font>")
	print(draw)

	--[[								<ul>
										<li class="ball_red">10</li>
										<li class="ball_red">11</li>
										<li class="ball_red">12</li>
										<li class="ball_red">13</li>
										<li class="ball_red">26</li>
										<li class="ball_red">28</li>
										<li class="ball_blue">11</li>
									</ul>
	]]

	redball1,redball2,redball3,redball4,redball5,redball6,blueball = string.match(html, "<ul>%s-<li class=\"ball_red\">(%d+)</li>%s-<li class=\"ball_red\">(%d+)</li>%s-<li class=\"ball_red\">(%d+)</li>%s-<li class=\"ball_red\">(%d+)</li>%s-<li class=\"ball_red\">(%d+)</li>%s-<li class=\"ball_red\">(%d+)</li>%s-<li class=\"ball_blue\">(%d+)</li>%s-</ul>")
	print(redball1,redball2,redball3,redball4,redball5,redball6,blueball)

	--[[<td>
								26 28 11 13 10 12</td>
								]]
	position1,position2,position3,position4,position5,position6 = string.match(html, "<td>%s-(%d+)%s-(%d+)%s-(%d+)%s-(%d+)%s-(%d+)%s-(%d+)</td>")
	print(position1,position2,position3,position4,position5,position6)


	--[[
				<td>
					本期销量：<span class="cfont1 ">10,307,806元</span>&nbsp;&nbsp;&nbsp;&nbsp;奖池滚存：<span
						class="cfont1 ">2,097,070元</span></td>

			]]
	sales,pools = string.match(html, "<span class=\"cfont1 \">([%d%p]+)%W+</span>[^<]+<span%s+class=\"cfont1 \">([%d%p]+)%W+</span>") --中文不能写入匹配字符串内, %p是标点符号, %W是非数字和字母
	sales = string.gsub(sales,",","")
	sales = if_not_digital_then_zero(sales)
	pools = string.gsub(pools,",","")
	pools = if_not_digital_then_zero(pools)
	print(sales,pools)



	--[[		<tr align="center">
				<td>
					一等奖</td>
				<td>
					0</td>
				<td>
					0</td>
			</tr>
			<tr align="center">
				<td>
					二等奖</td>
				<td>
					1</td>
				<td>
					898,744</td>
			</tr>
			<tr align="center">
				<td>
					三等奖</td>
				<td>
					26</td>
				<td>
					3,000</td>
			</tr>
			<tr align="center">
				<td>
					四等奖</td>
				<td>
					1458</td>
				<td>
					200</td>
			</tr>
			<tr align="center">
				<td>
					五等奖</td>
				<td>
					30231</td>
				<td>
					10</td>
			</tr>
			<tr align="center">
				<td>
					六等奖</td>
				<td>
					276620</td>
				<td>
					5</td>
			</tr>
	]]


	winners1,bonus1,winners2,bonus2,winners3,bonus3,winners4,bonus4,winners5,bonus5,winners6,bonus6 = string.match(html, "<tr align=\"center\">%s+<td>[^<]+</td>[^<]+<td>%s+([%d%p]+)</td>%s+<td>%s+([%d%p]+)</td>%s+</tr>%s+<tr align=\"center\">%s+<td>[^<]+</td>[^<]+<td>%s+([%d%p]+)</td>%s+<td>%s+([%d%p]+)</td>%s+</tr>%s+<tr align=\"center\">%s+<td>[^<]+</td>[^<]+<td>%s+([%d%p]+)</td>%s+<td>%s+([%d%p]+)</td>%s+</tr>%s+<tr align=\"center\">%s+<td>[^<]+</td>[^<]+<td>%s+([%d%p]+)</td>%s+<td>%s+([%d%p]+)</td>%s+</tr>%s+<tr align=\"center\">%s+<td>[^<]+</td>[^<]+<td>%s+([%d%p]+)</td>%s+<td>%s+([%d%p]+)</td>%s+</tr>%s+<tr align=\"center\">%s+<td>[^<]+</td>[^<]+<td>%s+([%d%p]+)</td>%s+<td>%s+([%d%p]+)</td>%s+</tr>")
	winners1 = string.gsub(winners1, ",", "")
	winners1 = if_not_digital_then_zero(winners1)
	bonus1 = string.gsub(bonus1, ",", "")
	bonus1 = if_not_digital_then_zero(bonus1)
	winners2 = string.gsub(winners2, ",", "")
	winners2 = if_not_digital_then_zero(winners2)
	bonus2 = string.gsub(bonus2, ",", "")
	bonus2 = if_not_digital_then_zero(bonus2)
	winners3 = string.gsub(winners3, ",", "")
	winners3 = if_not_digital_then_zero(winners3)
	bonus3 = string.gsub(bonus3, ",", "")
	bonus3 = if_not_digital_then_zero(bonus3)
	winners4 = string.gsub(winners4, ",", "")
	winners4 = if_not_digital_then_zero(winners4)
	bonus4 = string.gsub(bonus4, ",", "")
	bonus4 = if_not_digital_then_zero(bonus4)
	winners5 = string.gsub(winners5, ",", "")
	winners5 = if_not_digital_then_zero(winners5)
	bonus5 = string.gsub(bonus5, ",", "")
	bonus5 = if_not_digital_then_zero(bonus5)
	winners6 = string.gsub(winners6, ",", "")
	winners6 = if_not_digital_then_zero(winners6)
	bonus6 = string.gsub(bonus6, ",", "")
	bonus6 = if_not_digital_then_zero(bonus6)
	print(winners1,bonus1,winners2,bonus2,winners3,bonus3,winners4,bonus4,winners5,bonus5,winners6,bonus6)


--	sql = "insert into think_shuangseqiu(draw,redball1,redball2,redball3,redball4,redball5,redball6,blueball,position1,position2,position3,position4,position5,position6,sales,pools,winners1,bonus1,winners2,bonus2,winners3,bonus3,winners4,bonus4,winners5,bonus5,winners6,bonus6) values(" .. draw ..",".. redball1 .. "," ..redball2 .. "," ..redball3 .. "," .. redball4 .. "," .. redball5 .. "," .. redball6 .. "," .. blueball .. "," .. position1 .. "," .. position2 .. "," .. position3 .. "," .. position4 .. "," .. position5 .. "," .. position6 .. "," .. sales .. "," .. pools .. "," .. winners1 .. "," .. bonus1 .. "," .. winners2 .. "," .. bonus2 .. "," .. winners3 .. "," .. bonus3 .. "," .. winners4 .. "," .. bonus4 .. "," .. winners5 .. "," .. bonus5 .. "," .. winners6 .. "," .. bonus6 .. ")"
--	print(sql)
	sql = "insert into think_shuangseqiu(draw,redball1,redball2,redball3,redball4,redball5,redball6,blueball) values(" .. draw ..",".. redball1 .. "," ..redball2 .. "," ..redball3 .. "," .. redball4 .. "," .. redball5 .. "," .. redball6 .. "," .. blueball .. ")"
	
if MYSQL == true then
	env = luasql.mysql()
	con = env:connect("test","root","","localhost",3306)
else
	env = luasql.sqlite3()
	con = env:connect("local.db")
end

	row = con:execute(sql)
	print(row)

	sql = "update think_shuangseqiu set position1=" .. position1 .. ",position2=" .. position2 .. ",position3=" .. position3 .. ",position4=" .. position4 .. ",position5=" .. position5 .. ",position6=" .. position6 .. " where draw=" .. draw
	print(sql)
	row = con:execute(sql)
	print(row)

	sql = "update think_shuangseqiu set sales=" .. sales .. ",pools=" .. pools .. ",winners1=" .. winners1 .. ",bonus1=" .. bonus1 .. ",winners2=" .. winners2 .. ",bonus2=" .. bonus2 .. ",winners3=" .. winners3 .. ",bonus3=" .. bonus3 .. ",winners4=" .. winners4 .. ",bonus4=" .. bonus4 .. ",winners5=" .. winners5 .. ",bonus5=" .. bonus5 .. "," .. "winners6=" .. winners6 .. "," .. "bonus6=" .. bonus6 .." where draw=" .. draw
	print(sql)
	row = con:execute(sql)
	print(row)

	con:close()
	env:close()
	return row
end


if MYSQL ~= true then
	env = luasql.sqlite3()
	con = env:connect("local.db")
	sql = [=[
	CREATE TABLE `think_shuangseqiu` (
  `draw` int(11) NOT NULL,
  `redball1` int(11) NOT NULL,
  `redball2` int(11) NOT NULL,
  `redball3` int(11) NOT NULL,
  `redball4` int(11) NOT NULL,
  `redball5` int(11) NOT NULL,
  `redball6` int(11) NOT NULL,
  `blueball` int(11) NOT NULL,
  `position1` int(11) DEFAULT NULL,
  `position2` int(11) DEFAULT NULL,
  `position3` int(11) DEFAULT NULL,
  `position4` int(11) DEFAULT NULL,
  `position5` int(11) DEFAULT NULL,
  `position6` int(11) DEFAULT NULL,
  `sales` int(11) DEFAULT NULL,
  `pools` int(11) DEFAULT NULL,
  `winners1` int(11) DEFAULT NULL,
  `bonus1` int(11) DEFAULT NULL,
  `winners2` int(11) DEFAULT NULL,
  `bonus2` int(11) DEFAULT NULL,
  `winners3` int(11) DEFAULT NULL,
  `bonus3` int(11) DEFAULT NULL,
  `winners4` int(11) DEFAULT NULL,
  `bonus4` int(11) DEFAULT NULL,
  `winners5` int(11) DEFAULT NULL,
  `bonus5` int(11) DEFAULT NULL,
  `winners6` int(11) DEFAULT NULL,
  `bonus6` int(11) DEFAULT NULL,
  PRIMARY KEY (`draw`)
);
]=]
	con:execute(sql)
	con:close()
	env:close()
end

for i=12,12 do
	for j=60,200 do
		url = string.format("http://kaijiang.500wan.com/shtml/ssq/%02d%03d.shtml", i, j)
		print(url)
		code, html = get_html(url)
		--print(html)
		if(string.len(html) < 1000) then break end
		process_html(html)
	end
end

