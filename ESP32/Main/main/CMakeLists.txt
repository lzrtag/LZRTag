
set(COMPONENT_SRCS "main.cpp"
	"core/GunHandler.cpp" "core/gyro.cpp" "core/IR.cpp" "core/player.cpp" "core/setup.cpp"
	"fx/patterns/BasePattern.cpp" "fx/patterns/ShotFlicker.cpp" "fx/patterns/VestPattern.cpp"
	"fx/animatorThread.cpp" "fx/colorSets.cpp" "fx/ManeAnimator.cpp" "fx/patternModeHandling.cpp"
	"fx/sounds.cpp"
	"weapons/mylin.cpp" "weapons/wyre.cpp" "weapons/zinger.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
