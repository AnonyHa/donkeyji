local user_var = {
	'cash',
	'exp',
}

local npc_var = {}

local item_var = {}

---------------------------------------------

function bind_var(cls, var_list, save_list)
	var_list = var_list or {}
	local parent = super(cls) or {}

	for _, var in ipairs(var_list) do
		local getor = 'get_'..var
		local setor = 'set_'..var
		local addor = 'add_'..var
		local subor = 'sub_'..var

		local my_getor = rawget(cls, getor)
		local parent_getor = rawget(cls, getor) 
		if my_getor == nil or parent_getor == my_getor then 
			cls[getor] = function()
				return cls[var]
			end
		end


		local my_setor = rawget(cls, setor)
		local parent_setor = rawget(cls, setor) 
		if my_setor == nil or parent_setor == my_setor then 
			cls[setor] = function(s)
				cls[var] = s
			end
		end

		local my_addor = rawget(cls, addor)
		local parent_addor = rawget(cls, addor) 
		if my_addor == nil or parent_addor == my_addor then 
			cls[addor] = function(s)
				cls[var] = (cls[var] or 0) + s
			end
		end

		local my_subor = rawget(cls, subor)
		local parent_subor = rawget(cls, subor) 
		if my_subor == nil or parent_subor == my_subor then 
			cls[subor] = function(s)
				cls[var] = (cls[var] or 0) - s
			end
		end
	end
end
