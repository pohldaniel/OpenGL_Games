local color = {}

function color.new(r, g, b, a)
	local t = {
		r = r or 0,
		g = g or 0,
		b = b or 0,
        a = a or 0
	}
	setmetatable(t, color)
	return t
end

function color.iscolor(t)
	return getmetatable(t) == color
end

function color.__newindex(t, k, v, w)
	print("Can't create more fields in vcolors")
end

color.__index = color

function color.__tostring(t)
	return "(" .. t.r .. ", " .. t.g .. ", " .. t.b ..  ", " .. t.a .. ")"
end

return setmetatable(color, {
	__call = function(_, ...)
	return color.new(...)
end
})