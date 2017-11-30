
### Immediate commands
Number | Short Desc. | Data
:--- | --- | ---
A000 | Shot cfg | 0 = Disable, 1 = Enable, 99 = Immediate, >=100 Set consequent shots
E000 | Trigger status changed
E001 | Shot detected | Player, ShotID
A010 | Vibrate 	| Time (10ms)
A011 | Buzzer 	| Time (10ms), Start, End (60Hz)
A012 | Vest Blink Override | Level + time in 100ms

### Config commands
Number | Short Desc.
:--- | ---
A100 | Set Player ID
A101 | Set Player Team (0-3)
A200 | Set vest visibility level

###### A200
Visibilitiy levels:
- 0 = Absolutely no lights whatsoever, dimmed nozzle flash.
- 1 = Occasional dimmed blink, dimmed nozzle flash.
- 2 = Occasional bright blink, full nozzle flash.
- 3 = Dimmed with occasional blink, full nozzle flash.
- 4 = Full light, full nozzle flash.
- 5 = Special Mode - Full with white flashes, full nozzle flash with dim background.
