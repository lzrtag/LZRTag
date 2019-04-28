
reviveTimer = tmr.create();
shotTimer	= tmr.create();
reloadTimer = tmr.create();

function displayHit()
	vibrate(hitConf.hitVibration);
	overrideVest(hitConf.hitFlashDuration, hitConf.hitFlashBrightness);
end
function revivePlayer()
	if(not(player.dead)) then
		return;
	end
	player.dead = false;
	update_vest_brightness();
	homeQTT:publish(playerTopic .. "/Dead", "", 0, 1);
end
function killPlayer()
	if(player.dead) then
		return;
	end

	player.dead = true;

	vibrate(hitConf.deathVibration);
	overrideVest(hitConf.deathFlashDuration, hitConf.deathFlashBrightness);
	setVestBrightness(hitConf.deathBrightness);

	if(hitConf.deathDuration) then
		reviveTimer:alarm(hitConf.deathDuration, tmr.ALARM_SINGLE, revivePlayer);
	end
end

subscribeTo(playerTopic .. "/GunNo", 1,
	function(data)
		data = (tonumber(data) or 0);

		if(data > 0) then
			fireConf.shotLocked = false;
		else
			fireConf.shotLocked = true;
		end
	end);

subscribeTo(playerTopic .. "/Dead", 1,
	function(data)
		data = (data == "true");

		if((data) and (not player.dead)) then
			killPlayer();
		elseif(not(data) and (player.dead)) then
			reviveTimer:unregister();
			revivePlayer();
		end
	end);

subscribeTo(playerTopic .. "Dead/Timed", 0,
	function(data)
		data = tonumber(data);
		if(data) then
			hitConf.deathDuration = data;

			killPlayer();
		end
	end);

function canShoot()
	if(not(player.id)) then
		return false
	elseif(fireConf.shotLocked) then
		return false
	elseif(player.dead) then
		return false
	elseif((player.ammo == 0) and (fireConf.ammoCap ~= 0)) then
		return false
	elseif(player.shotCooldown) then
		return false
	end

	return player.button;
end
function updateAmmo(a)
	player.ammo = a;
	homeQTT:publish(playerTopic .. "/Ammo", struct.pack("L<L<", player.ammo, fireConf.ammoCap), 0, 0);
end
function reloadAmmo()
	if(fireConf.perReloadAmmo == 0) then
		return
	elseif(player.ammo >= fireConf.ammoCap) then
		return;
	elseif((player.ammo + fireConf.perReloadAmmo) >= fireConf.ammoCap) then
		updateAmmo(fireConf.ammoCap);
	else
		updateAmmo(player.ammo + fireConf.perReloadAmmo);

		reloadTimer:alarm(fireConf.reloadDelay, tmr.ALARM_SINGLE, reloadAmmo);
	end

	attemptShot();
end

function attemptShot()
	if(not(canShoot())) then
		return;
	end

	fireWeapon();
	player.shotCooldown = true;

	updateAmmo(player.ammo - 1);

	if(fireConf.flashDuration) then
		overrideVest(fireConf.flashDuration, fireConf.flashBrightness);
	end

	shotTimer:alarm(fireConf.perShotDelay, tmr.ALARM_SINGLE,
		function()
			player.shotCooldown = false;
			attemptShot();
		end);

	if(fireConf.perReloadAmmo > 0) then
		reloadTimer:alarm(fireConf.reloadDelay, tmr.ALARM_SINGLE, reloadAmmo);
	end
end

registerUARTCommand(0, 1,
	function(data)
		if((data:byte(1) == 0) == invertButton) then
			if(fireConf.reportButton) then
				eP = '{"type":"button","player":"' .. playerID .. '"}';
				homeQTT:publish(lasertagTopic .. "/Game/Events", eP, 0, 0);
			end

			player.button = true;
			attemptShot();
		else
			player.button = false;
		end
	end);

-- Handling of AVR-Detected shots
registerUARTCommand(1, 2,
	function(data)
		-- sec, usec, rate = rtctime.get()

		eP = '{"type":"hit","shooterID":' .. data:byte(1)
		eP = eP .. ',"target":"' .. playerID .. '","arbCode":' .. data:byte(2)
		eP = eP .. '}';

		homeQTT:publish(lasertagTopic .. "/Game/Events", eP, 0, 0);

		if(hitConf.dieOnHit and not(player.dead)) then
			killPlayer();
			homeQTT:publish(playerTopic .. "/Dead", "true", 0, 1);
		elseif(hitConf.flashOnHit) then
			displayHit();
		end
	end);
