
include_files = { "*.lua" }

stds.nodemcu = {
	read_globals = {
		"tmr",
		"mqtt",
		"uart",
		"wifi",
		"adc",
		"node",
		"sjson",
		"file",
		"encoder",
		"sntp",
		"rtctime",
	}
}

stds.playerdata = {
	globals = {
		"serverURL",
		"playerTopic",
		"lasertagTopic",
		"playerID",
		"invertButton",
	}
}

stds.lasertag = {
	globals = {
		"setVestBrightness",
		"setVestColor",
		"overrideVest",
		"ping",
		"vibrate",
		"setVibratePattern",
		"fireWeapon",

		"gameRunning",
		"game",

		"SAFEMODE",
		"systemIsSetUp",

		"subscribeTo",
		"registerUARTCommand",
		"homeQTT",
		"homeQTT_connected",
		"onMQTTConnect",
	}
}

stds.serverconf = {
	globals = {
		"hitConf",
		"fireConf",
		"player"
	}
}

std="min+nodemcu+lasertag+playerdata+serverconf"

allow_defined 			= true

ignore = {
	"131", -- Unused global variables
	"212", -- Unused function argument
	"213", -- Unused loop variables
}
