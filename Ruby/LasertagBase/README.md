
# LZRTag base library gem

This is the full documentation of the LZRTag ruby gem library.
It includes examples for the different elements of this code, and even a basic
deathmatch game to show off the mechanics more interactively.

## Prerequisites
Let's first start off by installing and setting up everything that's needed
to run a LZRTag game. We'll assume you have at least one, preferrably as many as possible,
LZRTag hardware sets with the latest firmware - consult the [HackADay page](bit.ly/XASLZR) as well as the [GitHub Wiki page](https://github.com/XasWorks/LZRTag/wiki/Assembly) on soldering them up.  
We will also need a WiFi network and a MQTT Server, either local or hosted, like
Flespi.io, with the LZRTag hardware set up to connect to the server. Successful
connection is indicated by a smooth, very slow waver of the blue connection LED
on the sets. If you want to double-check connection you can subscribe to the
Lasertag/# topic on your MQTT server and check if the sets are pinging properly.

Once we are sure that the sets are connected properly run `gem install lzrtag-base`.
The latest ruby is recommended for performance reasons, however, this code should work
with anything above 2.3.

## Testing on IRB
As a first experience, I'd recommend playing with the interface class on the interactive
ruby console. It's a great way to see the immediate effects of various commands
on the set, and is kinda fun, too.

Let's first start up the console and require the library.
```IRB
irb(main):001:0> require 'lzrtag.rb'
=> true
irb(main):002:0>
```
This will load all necessary files, and will also provide a few helper functions
to access the correct classes etc.  
Next, let's establish an MQTT connection and start the handler.

```IRB
irb(main):002:0> mqtt = MQTT::SubHandler.new('localhost'); # Or your address
MQTT: localhost trying reconnect...
MQTT: localhost connected!
irb(main):003:0> handler = LZRTag.Handler.new(mqtt);
I LZR::Handler init finished
```

Awesome! The handler should now be connected to your server, and should have registered
new players. If you look at your MQTT Server, for example with MQTT-Spy, you
should see that any connected players will now have an ID.

Let's see what players the system has already registered:
```irb
irb(main):004:0> handler.each do |player| puts player; end;
#<Player:DEVICE_ID#OFFLINE, Team=0, Battery=4.19, Ping=218ms>
```

Players have a nice inspect function so they don't clutter the console too much when printed. Neat, huh?

Alright, we need to fetch one single player to play with. If you have a device ID
(their MAC, used on the MQTT network to identify them) you can use that, otherwise
use the ShotID (number, 0..255). Let's do  that now - and while we're at it,
let's identify the player:
```irb
irb(main):005:0> player = handler["BC.DD.C2.D0.63.F8"]
=> #<Player:#OFFLINE, Team=0, Battery=4.19, Heap=0, Ping=218ms>
irb(main):006:0> player.hit
```

You should have seen and heard one of the sets flashing up - that's the one we
just retrieved. We can also colour it green and make it a little brighter!

```irb
irb(main):007:0> player.team = 2
=> 2
irb(main):08:0> player.brightness = :active
=> :active
```

Alright, awesome - let's not go too deep into what you can do with the player.
That's what the Ruby documentation is for, after all.

## And now?
Well, more complex behaviours can be added with so-called hooks. These are extensions
that perform automated tasks based on different events in the game, and use the
internal "event system".  
There is a list of currently used events available, as well as documentation on
the hooks class. You can make your own, or use the standard hooks provided in this
library - it's up to you!

Since a game is way more than just behaviours though, there are also "Games".
Each game is a collection of hooks, which provide the behaviour of the game. However,
they also include a description of different phases - stuff like "initializing",
"playing", "selecting teams", etc.  
The application can seamlessly switch between phases of a game, with each phase
defining which hooks to enable or disable and what to do every game tick.

It's best to look right into the documentation on those classes, since demonstrating
them bit by bit isn't really possible.
