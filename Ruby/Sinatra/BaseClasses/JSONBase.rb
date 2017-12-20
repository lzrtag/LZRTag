
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

			get '/JSON/Game' do
				game.to_json
			end

			get '/Players' do
				haml :simplePlayerListing
			end
		end
	end
end
