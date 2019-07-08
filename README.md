# LZRTag
### ESP32-Based Lasertag

Ever wanted to set up your own system of Lasertag?
Interested in having an easily modifiable system to create your own gamemodes and styles with?
Or just looking for a cool project to try out?
Well, why not take a spin at this!

*Note:* This project is not in serious development. I (Xasin) take no responsibility for what is done with this code, and I can't guarantee it'll do exactly what I tell it to (even if I try to give it treats >:c)
However, it is a fully functional system, if you know how to use it.

## The components
In order to have a functional game, you will need at least the following:
- Two or more soldered Lasertag Sets
- A WiFi access point with SSID "Lasertag", PW "Lasertag" (though this can be changed in Firmware ^^)
- A MQTT Server, preferrably in the local network, to reduce ping.
- And a system running Ruby 2.4 or better, to run the Lasertag Ruby server code

## The Electronics
*Attention:* The electronics aren't necessarily beginner-friendly. I've designed them with components no smaller than a TSOP package or a 0805 resistor, but the newer versions of the hardware do include two QFN Packages - one for a optional IMU, so it can be left out, and one for a standard I2S audio chip, for which you can use any breakout board that supports 44100Hz, 16 bit playback.

The electronics are based around the ESP32, a fairly cheap, powerful, WiFi and Bluetooth capable chip. The PCB includes all necessary support structures, including a battery charge circuit and a USB to UART converter to reprogram and debug the ESP. Additionally, it has built-in WS2812's, making it easy to create good looking casings with illumination!

To solder one, check out the Aisler project here: https://aisler.net/p/QLILZEMV
Or head to the "Electronics" folder and generate your own Gerber files!
### Caution: The PCB needs a redesign due to design flaws!

In addition to the PCB, you will also need a couple of WS2812's to make your vests and such look extra fancy, as well as a few 40kHz IR Receivers, preferrably the good ones from Vishay, marked as 45m range. The more the better, but they are a bit costly!

## The Casing
The 3D printed casing I designed is totally optional - the electronics can be easily fitted into any other casing, provided it has space for a switch, a speaker and a couple of LEDs!
However, if you do want to print it out, there's the Fusion360 page here: https://a360.co/2Lbm2zJ

