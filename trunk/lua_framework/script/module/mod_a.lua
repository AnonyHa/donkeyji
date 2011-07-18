mod_cnt = 0

function huge()
	print('huge')
end

function __init__()
	SAVE.restore_module()
	SAVE.register_var('mod_cnt')
end
