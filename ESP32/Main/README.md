# The Lasertag ESP32 Core code

This code is what drives the newest version of the Lasertag.
Written in C++, this code here handles graphics, shot sounds, as well as communication with the Ruby-Side server code, via a MQTT Channel.

All hardware-related systems can be found under main/core/setup.cpp
Graphics are executed in main/fx/animatorThread.cpp, while gun profiles are found under main/weapons/

If you want to add your own weapon, animation feature, communication code or similar, it's best to open an issue and contact the team.
Documenting this work is a bit tough, but explaining specifics should be easy!
