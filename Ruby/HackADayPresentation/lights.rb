

def say(text)
	`espeak -s 150 "#{text}"`
end


require_relative '../LasertagBase/lib/lzrtag.rb'

$mqtt = MQTT::SubHandler.new("192.168.6.111");
$game = LZRTag.Handler.new($mqtt);

sleep 5

$pl = $game["RevThree"]

$pl.brightness = 1;
$pl.team = 1;
$pl.gunNo = 2;

$pl.brightness = 0;
$pl.team = 0;

say "Hello HackADay"
say "Lasertag development is continuing at an acceptable pace"

say "Example: Light system rewrite, resolved."
say "Observe. "

$pl.brightness = 1;
sleep 1

say "A new team-selection mode was added, to signal that players may switch teams"
Thread.new do
	5.times do |i| $pl.team = i+1; sleep 4; end
end
say "This mode relies on a much more flexible library of effects, making such patterns easy to add"

$pl.brightness = 3;
say "A few other things have been optimized, such as shot effects:"
sleep 4
say "Being hit"
$pl.hit(1.5);
sleep 3;
say "Or killed"
$pl.dead = true;
$pl.hit(2);
sleep 5
$pl.dead = false;
say "Or being marked"
$pl.marked = 0xBB6666;

sleep 5

say "Thank you for watching"
