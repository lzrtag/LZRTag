
module LZRTag
	def self.Handler()
		return LZRTag::Handler::Game;
	end
end

require_relative 'lzrtag/handler/game_handler.rb'
require_relative 'lzrtag/hooks/standard_hooks.rb'

require_relative 'lzrtag/game/base_game.rb'

require_relative 'lzrtag/map/map_set.rb'
