function huge()
	print('huge')
end

function __init__()
	print('___init___')
	local env = getfenv(1)
	for k, v in pairs(env) do
		print(k, v)
	end
end
