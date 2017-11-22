
ping_notes = {
			1400,
	500, 	1400,
	500,  1400,
	290,  900,
	290,  1800
};

pingTimer = tmr.create();

currentNotePosition = 1;
function playNextNote()
	cNote = ping_notes[currentNotePosition*2 - 1];
	ping(cNote, cNote, 200);
	if(currentNotePosition*2 >= #ping_notes) then
		setVestBrightness(0);
		return;
	end
	pingTimer:alarm(ping_notes[currentNotePosition*2], tmr.ALARM_SEMI, playNextNote);
	currentNotePosition = currentNotePosition + 1;
end

setVestBrightness(5);
playNextNote();
