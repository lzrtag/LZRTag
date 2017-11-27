
require 'json'
require 'base64'
require_relative '../Libs/MQTTSubscriber.rb'

module Lasertag
	class RawTransfer
		attr_reader :state
		attr_reader :Target

		attr_reader :Filepath
		attr_reader :Filename
		attr_reader :Blocksize
		attr_reader :BlockNum
		attr_reader :currentBlock

		def initialize(mqtt, target, filepath, **options)
			@state = :IDLE;

			@mqtt 	= mqtt;
			@Target 	= target;
			if options[:playertopic] then
				@TransferTopic = options[:playertopic] + "#{target}/"
			else
				@TransferTopic = "Lasertag/Players/#{target}/"
			end
			@retryAttempts = 0;
			@MaxRetries 	= (options[:MaxRetries] or 2);

			@Filepath		= filepath;
			@Filename 		= (options[:filename] or File.basename(filepath));
			@file 			= File.open(@Filepath, "r");

			@Blocksize 		= (options[:blocksize] or 256);
			@BlockNum		= (@file.size.to_f / @Blocksize).ceil;
			@currentBlock  = 0;
		end

		def transfer()
			return true if @state == :COMPLETED;

			txTopic = @TransferTopic + "Console/FileWrite";
			rxTopic = @TransferTopic + "Console/FileAnswer";

			@state = :CONNECTING;

			# Send a "prep" message to the Lasertag
			# On success, continue on to the next stage, else fail
			@retryAttempts = 0;
			@currentBlock = 0;
			loop do
				@mqtt.publish_to txTopic, {
					file: @Filename,
					block: 0
				}.to_json, qos: 2

				@mqtt.wait_for rxTopic, qos: 2, timeout: 10 do |tList, data|
					if(data == "READY: #{@Filename}") then
						@state = :TRANSFER
						true
					end
				end

				if @state == :TRANSFER then
					break;
				else
					@retryAttempts += 1;
					if @retryAttempts >= @MaxRetries then
						@state = :FAILED;
						return false;
					end
				end
			end

			# Send out "n" blocks of data
			# On a failure, retry
			# Break if transfer fails too often.
			loop do
				@currentBlock += 1;

				packet = {
					file: 	@Filename,
					block: 	@currentBlock,
				}

				packet[:close] = true if @currentBlock == @BlockNum;

				dataSegment = String.new
				begin
					@file.sysread(@Blocksize, dataSegment);
				rescue EOFError
				end
				packet[:data] = Base64.strict_encode64(dataSegment);

				@retryAttempts = 0;
				loop do
					@mqtt.publish_to txTopic, packet.to_json, qos: 2;

					packetACKed = false;
					@mqtt.wait_for rxTopic, qos: 2, timeout: 10 do |tList, data|
						packetACKed = (data == "OK: #{@Filename}, #{@currentBlock}");
					end

					break if packetACKed;

					@retryAttempts += 1;
					if @retryAttempts >= @MaxRetries then
						@state = :FAILED;
						return false;
					end
				end

				if(@currentBlock == @BlockNum) then
					@state = :COMPLETED;
					return true;
				end
			end
		end
	end
end
