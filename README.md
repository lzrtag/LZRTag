# LZRTag
### ESP32-Based Lasertag

Ever wanted to set up your own system of Lasertag?
Interested in having an easily modifiable system to create your own gamemodes and styles with?
Or just looking for a cool project to try out?
Well, why not take a spin at this!

*Note:* This project is not in serious development. I (Xasin) take no responsibility for what is done with this code, and I can't guarantee it'll do exactly what I tell it to (even if I try to give it treats >:c)
However, it is a fully functional system, if you know how to use it, and it has proven itself
to be pretty reliable and useful!

## The components
In order to have a functional game, you will need at least the following:
- Two or more soldered Lasertag Sets
- A WiFi access point or a newtwork of points throughout the arena.
- A MQTT Server, preferrably in the local network, to reduce ping.
- And a system running Ruby 2.4 or better, to run the Lasertag Backend and your gamemodes

## The Electronics
*Attention:* The electronics aren't necessarily beginner-friendly. I've designed them with components no smaller than a TSOP package or a 0805 resistor, but the newer versions of the hardware do include two QFN Packages - one for a optional IMU, so it can be left out, and one for a standard I2S audio chip, for which you can use any breakout board that supports 44100Hz, 16 bit playback. If you have a hot air gun,
definitely make use of it!

The electronics are based around the ESP32, a fairly cheap, powerful, WiFi and Bluetooth capable chip. The PCB includes all necessary support structures, including a battery charge circuit and a USB to UART converter to reprogram and debug the ESP.
It comes with the following hardware:
- The ESP32 plus beefy voltage regulation, ESD protection, etc. It ain't gonna break!
- A MAX I2S decoder + amplifier, for on-board, high quality audio.
- A LSM6D IMU, to detect movement, collisions, gestures, etc.
- Peripherals to receive and send IR Signals (documented here: TODO)
- Feedback in the shape of WS2812 LEDs and a vibration motor
- Trigger, Gun Select and Reload buttons.

The vest connection is exposed with a D-SUB connector, and I2C lines are available to make
more complex add-ons easier to achieve. There's also a I2C breakout connection on the PCB itself!

The Vest itself can be as simple as a couple of Vishay 40kHz IR receivers and WS2812 LEDs, but
there is a connection for vest-specific haptic feedback, and via the I2C connection it's easy to add
more complex features.

To solder one, check out the Aisler project here: https://aisler.net/p/QLILZEMV
Or head to the "Electronics" folder and generate your own Gerber files!

## The Casing
The 3D printed casing I designed is totally optional - the electronics can be easily fitted into any other casing, provided it has space for a switch, a speaker and a couple of LEDs!
However, if you do want to print it out, there's the Fusion360 page here: https://a360.co/2Lbm2zJ
