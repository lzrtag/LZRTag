#!/usr/bin/ruby

require 'mqtt'
require 'json'
require 'base64'

raise "No file specified! :c" unless ARGV[0]

$mHost = "iot.eclipse.org"
$targetTag = "Xasin2"

$blockSize = 512;
$sendDelay = 1;

lastArg = nil;
inputData = Hash.new();
ARGV[1..-1].each { |str|
	if str[0] == "-" then
		lastArg = str[1..-1];
		inputData[lastArg] = true;
	elsif lastArg == nil then
		raise "Incorrect input arguments! :c"
	else
		inputData[lastArg] = str;
		lastArg = nil;
	end
}

$targetTag = inputData["t"] if inputData["t"];

$mPath = "Lasertag/Players/#{$targetTag}/Console/FileWrite";

print "Connecting ... "
MQTT::Client.connect($mHost) do |c|
	print "Connected!\n"

	$mqtt = c
	def esp(cmd)
		$mqtt.publish($mPath, cmd);
	end

	if inputData["R"] then
		print "Running!\n"
		$mPath = "Lasertag/Players/#{$targetTag}/Console/In";
		esp "dofile(\"#{ARGV[0]}\")";
	else
		dataSource = File.open(ARGV[0], "r");
		numBlocks = (dataSource.size / $blockSize).floor + 1;

		$dataBlob = Hash.new();
		$dataBlob[:targetFile] = ARGV[0];

		dataBlobBlock = String.new();

		begin
			dataSource.sysread($blockSize, dataBlobBlock);
		rescue EOFError
			$dataBlob[:eof] = true;
		end

		nBlock = 1;
		while true do
			$dataBlob[:data] = Base64.strict_encode64(dataBlobBlock);

			begin
				dataSource.sysread($blockSize, dataBlobBlock);
			rescue EOFError
				$dataBlob[:eof] = true;
			end

			print "Sending block #{nBlock}/#{numBlocks}\n"
			esp $dataBlob.to_json
			nBlock += 1;

			$dataBlob.delete(:targetFile);
			break if($dataBlob[:eof]);

			sleep $sendDelay;
		end

		if inputData["r"] then
			print "Running!\n"
			$mPath = "Lasertag/Players/#{$targetTag}/Console/In";
			esp "dofile(\"#{ARGV[0]}\")";
		end

		print "Done!\n"
	end
end
