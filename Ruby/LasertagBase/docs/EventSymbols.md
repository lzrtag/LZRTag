

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
Called immediately after the game's phase was changed to a new phase. Can, and should, be used to prepare a game's setting
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
