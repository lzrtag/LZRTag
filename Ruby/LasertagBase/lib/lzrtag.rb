
module LZRTag
	def self.Handler()
		return LZRTag::Handler::Count;
	end
end

require_relative 'lzrtag/handler/count_handler.rb'
require_relative 'lzrtag/hooks/standard_hooks.rb'
