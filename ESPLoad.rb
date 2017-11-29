#!/usr/bin/ruby

require 'mqtt'
require 'json'
require 'base64'

require_relative 'Ruby/GameInterface/LTagGame.rb'

$mqtt = MQTT::SubHandler.new("iot.eclipse.org");

$game = Lasertag::Game.new($mqtt, id_assign: false);
$game.on_register() do |pName, player|
	player.data[:upload_queue] 	= Queue.new();
	player.data[:upload_thread] = Thread.new do
	loop do
		player.data[:current_upload] = player.data[:upload_queue].pop;
		player.data[:current_upload].transfer
	end
	end
end

lastArg = nil;
inputData = Hash.new() do |h,k|
	h[k] = Array.new();
end

ARGV[1..-1].each { |str|
	if str[0] == "-" then
		lastArg = str[1..-1];
		inputData[lastArg] ||= Array.new;
	elsif lastArg == nil then
		raise "Incorrect input arguments! :c"
	else
		inputData[lastArg] << str;
	end
}
