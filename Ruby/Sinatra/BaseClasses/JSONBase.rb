
require 'sinatra'
require 'json'

module Lasertag
	module HTTP
		class Basic < Sinatra::Application

			def find_template(views, name, engine, &block)
				[views, File.join(File.dirname(__FILE__), "../views")].flatten.each do |v|
					super(v, name, engine, &block);
				end
			end

			get '/stylesheet.css' do
				scss :stylesheet
			end
			get '/scripts/:script' do
				content_type :javascript
				send_file File.join(File.dirname(__FILE__), "../JScripts/#{params[:script]}")
			end

			get '/JSON/Game' do
				content_type :json
				settings.game.to_json
			end

			get '/JSON/Players' do
				content_type :json
				settings.game.player_hash.to_json
			end

			get '/JSON/Players/:player' do
				content_type :json
				if(player = settings.game[params[:player]]) then
					player.to_json
				else
					"{}"
				end
			end

			get '/Players' do

				haml :simplePlayerListing
			end

			get '/Players.XHR' do
				haml :_rawPlayerList, layout: false;
			end
		end
	end
end
