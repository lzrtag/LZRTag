
require 'sinatra'
require 'json'

module Lasertag
	module HTTP
		class Basic < Sinatra::Application

			def find_template(views, name, engine, &block)
				[views, File.join(File.dirname(__FILE__), "../Views")].flatten.each do |v|
					super(v, name, engine, &block);
				end
			end

			get '/JSON/Game' do
				game.to_json
			end

			get '/Players' do
				hamlString = 	"%h1 Currently connected players:\n";
				hamlString += 	"%ul\n"
				settings.game.each do |pName, player|
					hamlString += "	%li&= '#{player.inspect}'\n";
				end

				haml hamlString
			end
		end
	end
end
