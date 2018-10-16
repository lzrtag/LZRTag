Gem::Specification.new do |s|
	s.name        = 'LZRTag_base'
	s.version     = '0.0.1'
	s.date        = '2018-10-16'
	s.summary     = "Base classes for Xasin's LZRTag system"
	s.authors     = ["Xasin"]
	s.files       = [ "lib/lzrtag/player/base_player.rb",
							"lib/lzrtag/handler.rb"]
	s.homepage    =
	'https://github.com/XasWorks/LZRTag/'
	s.license     = 'GPL-3.0'

	s.add_runtime_dependency "mqtt", ">= 0.5.0"
	s.add_runtime_dependency "json"

	s.add_development_dependency "minitest"
	s.add_development_dependency "guard"
	s.add_development_dependency "guard-minitest"
end
