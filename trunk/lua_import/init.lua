--先dofile，将import模块放入_G
dofile('./import.lua')

--import mod_a.lua
A = import('./mod_a.lua')

--call interface in mod_a
A.huge()
