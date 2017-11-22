
require 'json'
require 'base64'
require_relative '../Libs/MQTTSubscriber.rb'

module Lasertag
	class TransferManager
		def initialize(mqtt, blockSize: 512)
			@mqtt = mqtt;

			@blockSize = blockSize;

			@playerUpdatedTimes = Hash.new do |h, k|
				h[k] = Hash.new();
			end
		end

		def raw_send_file(filename, target)
			dataTopic 	= "Lasertag/Players/#{target}/Console/FileWrite"
			answerTopic = "Lasertag/Players/#{target}/Console/FileAnswer"
			@mqtt.publish_to "Lasertag/"
		end
	end

	class SingleTransfer
		attr_reader :state

		def initialize(mqtt, file, target, blocksize: 512)
			@state = :CONNECTING

			@mqtt = mqtt;

			@target = target;

			@filename 	= File.basename(file);
			@tmpFile 	= "tmp/#{@filename}.#{target}.upload";

			`cp #{file} #{@tmpFile}`
			@file = File.open(@tmpFile, "r");

			@blocksize		= blocksize
			@blockNumber	= 0;
			@numBlocks		= (@file.size.to_f / @blocksize).ceil;

			@transferThread = Thread.new do
				dataTopic 	= "Lasertag/Players/#{target}/Console/FileWrite"
				answerTopic = "Lasertag/Players/#{target}/Console/FileAnswer"

				for i=(0..@numBlocks) do

				end
			end
		end

		def join
			@transferThread.join
		end
	end
end
