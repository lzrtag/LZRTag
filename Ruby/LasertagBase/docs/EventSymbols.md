

This file will document all known internal Lasertag events and their purpose.
Users may add their own events via the standard system by simply sending a new event -
however, only the following event symbols and payloads will be used by the provided software.


## Game related events

###### :gameTick
Triggered in regular intervals, default 0.1s
Payload is [deltaTime since last tick].
It is only triggered when a game is present.

*Game class alternative:* Describe per-tick code using the
"phase" DSL. It allows easy and clean specification of tick code.

###### :gameStarting
Triggered when a new game is selected, either in Ruby or via MQTT.  
Payload is [newGame]

###### :gamePhaseEnds
Called immediately before the phase of a game is changed. Can be used to de-init some settings, or to finalize for example a player
team selection.  
The payload is [oldPhase, nextPhase]

*Game class alternative:* Descibe per-tick code using the "phase_end" DSL.

###### :gamePhaseStarts
Called immediately after the game's phase was changed to a new phase. Can, and should, be used to prepare a game's setting.  
The payload is [newPhase, oldPhase]

*Game class alternative:* Descibe per-tick code using the "phase_prep" DSL.

###### :playerEnteredGame
Called whenever a player is added to the list of game players.  
Payload is [thePlayer]

###### :playerLeftGame
Called whenever a player was removed from the list of game players.  
Payload is [thePlayer]

## Player config options
The following options will be triggered whenever some config of the player changes, and are mainly useful to trigger events or
keep track of the players etc.

###### :playerRegistered
Triggered when a brand new player is found on the MQTT network.
This will effectively only happen at the start of the server, or when a new player is turned on.  
Payload is [thePlayer]

###### :playerConnected
Triggered whenever a player reconnects to the server, either when restarting the server or mid-game after a disconnect.  
Payload is [thePlayer]

###### :playerDisconnected
Triggered whenever a player looses connection to the MQTT server (detected via the LWT topic).  
Payload is [thePlayer]

## Hardware player signals
These signals are sent in response to various player actions, as well as most of Hardware Player's
configuration changes.

###### :navSwitchPressed
Triggered when the player presses a navigation switch.  
Payload is [thePlayer, direction (0..2)]

###### :poseChanged
Triggered when the player changes the pose of the weapon. Possible poses are listed
under the Lasertag Hardware Player's "gyroPose" attribute
Payload is [thePlayer, poseSymbol]

###### :playerTeamChanged
Triggered when the team of a player is changed.  
Payload is [thePlayer, newTeam (0..7), oldTeam (0..7)]

###### :playerBrightnessChanged
Triggered when the brightness of a player is changed. For a list of brightness
symbols, see Hardware Player's "brightness" attribute.  
Payload is [thePlayer, newBrightness, oldBrightness]

###### :playerGunChanged
Triggered when the gun number of the player is changed.  
Payload is [thePlayer, newGun, oldGun]

*TODO:* The gun number should be replaced by an ID string, when the
gun configuration is moved to a Filesystem-based one (possibly by loading from JSON)

## Player life signals
These signals are sent whenever the player's HP count or life status is changed,
and let the application handle things like scoring damage and kills, and triggering
things like the heartbeat.

###### :playerRegenerated
Triggered whenever the player gains a bit of health, this can be by self-healing or
by a medic or similar providing health.  
Payload is [thePlayer, deltaLife]

###### :playerFullyRegenerated
Triggered similarly to :playerRegenerated, but only whenever the player has fully
regenerated and has reached maxLife again.  
Payload is [thePlayer]
