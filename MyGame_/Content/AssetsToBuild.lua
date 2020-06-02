--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/vertexInputLayout_3dObject.shader", arguments = { "vertex" } },
	},
	geometries =
	{
		{ path = "Geometries/Cone.lua"},
		{ path = "Geometries/HalfRCircle.lua"},
		{ path = "Geometries/QuarterRCircle.lua"},
		{ path = "Geometries/2x2Square.lua"},
		{ path = "Geometries/1RCircle.lua"},
		{ path = "Geometries/1x1Square.lua"},
	},
	effects =
	{
		{ path = "Effects/standardeffect.lua"},
		{ path = "Effects/customeffect.lua"},
		{ path = "Effects/yelloweffect.lua"},
		{ path = "Effects/browneffect.lua"},
	},
	colliders =
	{
		{ path = "Colliders/1rCircle.lua"},
		{ path = "Colliders/QuarterRCircle.lua"},
		{ path = "Colliders/HalfRCircle.lua"},
		{ path = "Colliders/2x2Square.lua"},
		{ path = "Colliders/1x1Square.lua"},
	},
	audiosources = 
    {
        { path = "Audio/Shoot.wav" },
        { path = "Audio/Explosion.wav" },
        { path = "Audio/BGMusic.wav" },
		 { path = "Audio/PowerUp.wav" },
        { path = "Audio/GameOver.wav" },
        { path = "Audio/PowerDown.wav" },
		 { path = "Audio/Start.wav" },
    }
}
