# MQTT Branch setup description

## Weapon Branches
The weapons are the main unit of the Lasertag system. As such, they use a specific
branch setup to minimize traffic to them, while making code maintaining easier.

The basic branch setup is:
> Lasertag/Players/**ID**/**Keys...**

- **ID** is the MAC of the Player, allowing unique identification of
	each set out of the box.
- **Key** is a one- or two-layer key of the attribute to change.
	Attributes are grouped, making it easier to subscribe to one group of attributes
	without having to worry about the others.

In the following, the different groups of keys will be outlined, followed by
a description of each attribute per group.

### Non-Grouped attributes

|Keys|Data|Descrition|Retained|
|--|--|--|--|
|Connection|String|LWT Topic of the sets. "OK" if connected.|Yes|

### Config attributes

Any attributes under this category can be found under "/CFG/**KEY**".
They are slow-changing configuration parameters for the weapons.

|Key|Data|Descrition|Retained|
|--|--|--|--|
|Team|STR Number|Team number, 0 to 7|Yes|
|Brightness|STR Number|Brightness preset number. In Ruby, handled by symbols|Yes|
|Name|String|User-set name of the weapon|Yes|
|Dead|"1" or "0"|Sets the alive-ness of the gun.|Yes|
|Dead/Timed|STR Number|Number, in seconds, to kill the weapon for. Auto-revives|No|
|GunNo|STR Number|Gun preset number. 0 or "" disable shooting|Yes|
|Marked|STR Number|Color to mark the set with. 1-7 use team colors, custom colors can be used|Yes|
|Heartbeat|"1" or "0"|Enables/Disables heartbeat of the weapon|Yes|
|Hit|STR Number|Number, in seconds, to make the gun show that it's hit|No|
|Vibrate|STR Number|Number, in seconds, for which to make the gun vibrate|No|

### Sound attributes

Any attributes under this category are found under "/Sound/**KEY**"
They all relate to noises the gun can make.

|Key|Data|Descrition|Retained|
|--|--|--|--|
|Note|uint32_t[3]|Raw note play data. Frequency(Hz), Volume (0..2^16), Duration(ms)|No|
|File|String|Name of a raw audio file to play.|No|

### Hardware attributes

Any attributes under this category are found under "/HW/**KEY**"
They all relate to the hardware of the system, such as battery etc.

|Key|Data|Descrition|Retained|
|--|--|--|--|
|Ping|uint32_t[3]|Battery, free-heap and ping data|No|
|NSwitch|STR Number|Navigation switch press info (0-3)|No|
|Gyro|String|Clear string of the pose the gyroscope is reporting|Yes|
|BeaconDetect|STR Number|Number of the beacon signal that was detected (0-255)|No|

### Statistics Attributes

Any attributes under this category are found under "/Stats/**KEY**"
They all relate to various game statistics that do not directly influence the hardware, but are still relevant to the game and how it plays out.


|Key|Data|Descrition|Retained|
|--|--|--|--|
|HP|STR Number|Float number, 0  to 100 (or more with shields)|Yes|
|Ammo|uint32_t[2]|Current and Max ammo of the weapon|Yes|
|Ammo/Set|STR Number|Give the player some ammo (*WIP*)|No|
