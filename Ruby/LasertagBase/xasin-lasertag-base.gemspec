Gem::Specification.new do |s|
	s.name        = 'lzrtag-base'
	s.version     = '1.0.1'
	s.date        = '2019-10-19'
	s.summary     = "Base classes for Xasin's LZRTag system"
	s.authors     = ["Xasin"]
	s.files       = [ 	"lib/lzrtag.rb",
								"lib/lzrtag/handler/base_handler.rb",
								"lib/lzrtag/handler/hitArb_handler.rb",
								"lib/lzrtag/handler/count_handler.rb",
								"lib/lzrtag/handler/game_handler.rb",
								"lib/lzrtag/player/base_player.rb",
								"lib/lzrtag/player/hardware_player.rb",
								"lib/lzrtag/player/effects_player.rb",
								"lib/lzrtag/player/life_player.rb",
								"lib/lzrtag/hooks/base_hook.rb",
								"lib/lzrtag/hooks/standard_hooks.rb",
								"lib/lzrtag/game/base_game.rb",
								"lib/lzrtag/map/map_set.rb",
								"lib/lzrtag/map/map_zone.rb",
								"lib/lzrtag/map/myMaps_parser.rb",
								"README.md",
								"docs/EventSymbols.md",
								"docs/MQTTBranches.md"];
	s.homepage    =
	'https://github.com/XasWorks/LZRTag/'
	s.license     = 'GPL-3.0'

	s.add_runtime_dependency "mqtt-sub_handler", ">= 0.1.6"
	s.add_runtime_dependency "xml-simple"

	s.add_development_dependency "minitest"
	s.add_development_dependency "guard"
	s.add_development_dependency "guard-minitest"
end
