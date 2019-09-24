

require_relative 'lzrtag.rb'

require 'midilib/io/seqreader'
require 'midilib/sequence.rb'

$mqtt = MQTT::SubHandler.new("localhost");
$handler = LZRTag.Handler.new($mqtt);

seq = MIDI::Sequence.new();
File.open("Portal2Cara.mid", 'rb') { | file |
	seq.read(file);
}

trackID = 0;
timecode_note_events = Array.new();
seq.each do |track|
	puts "Processing new track!";

	trackNotes = Hash.new();
	track.each do |event|
		next unless event.is_a? MIDI::NoteEvent;
		lastNote = trackNotes[event.note];

		unless(lastNote.nil? || lastNote.velocity == 0)
			timecode_note_events <<
			{	startTime:	lastNote.time_from_start,
				pitch:		lastNote.note,
				volume:		lastNote.velocity,
				duration:	event.time_from_start - lastNote.time_from_start,
				track:		trackID};
		end

		trackNotes[event.note] = event;
	end
	trackID += 1;
end

timecode_note_events.sort_by! {|n| n[:startTime] }

puts("Notes processed: #{timecode_note_events.length} - Beginning play");

lastPlayer = 0;
players = Array.new();

sleep 2

$handler.each do |pl|
	puts "Checking player #{pl}"
	players << pl if pl.connected?
end

puts "Active players: #{players}";

beginningTime = Time.now();
timecode_note_events.each do |note|
	sTime = note[:startTime]/2000.0 - (Time.now() - beginningTime);
	sleep sTime if sTime > 0;

	puts "Playing #{note}!"

	if(note[:track] == 1)
		players[0].play_midi(note[:pitch], note[:volume]*70, note[:duration]/2, 360 + 60*(note[:pitch]-70)/5.0)
	else
		players[lastPlayer+1].play_midi(note[:pitch], note[:volume]*50, note[:duration]/2, 360 + 60*(note[:pitch]-65)/5.0);
		lastPlayer = (lastPlayer+1) % (players.length-1)
	end
end
