
require_relative 'life_player.rb'

module LZRTag
	module Player
		class Statistics < Life
			attr_reader :stats

			def initialize(*args)
				@stats = {
					"Kills" => 0,
					"Deaths" => 0,
					"DamageDone" => 0,
					"DamageReceived" => 0,
					"Healed" => 0,
				}

				super(*args);
			end

			def _update_stat(key, value)
				return unless(@stats.keys.include? key);
				return if(@stats[key] == value);

				@stats[key] = value;
				_pub_to("Stats/#{key}", value, retain: true);
			end
			def _increment_stat(key, value)
				_update_stat(key, @stats[key] + value);
			end

			def reset()
				@stats.keys.each do |k|
					@stats[k] = 0;
					_pub_to("Stats/#{k}", 0, retain: true);
				end

				# super(); // TO DO!
			end

			def regenerate(*args)
				healDone = super(*args);

				_increment_stat("Healed", healDone);
			end
			def damage_by(*args)
				dmg_done = super(*args);

				_increment_stat("DamageReceived", dmg_done);
				args[1]._increment_stat("DamageDone", dmg_done) if(args[1])
			end

			def _set_dead(d, player = nil)
				return unless super(d, player);
				return unless d;

				player._increment_stat("Kills", 1) if(player);

				_increment_stat("Deaths", 1);
			end

			def ratio_kd()
				return @stats["Kills"]/([1, @stats["Deaths"]].max);
			end

			def clear_all_topics()
				super();

				@stats.keys.each do |k|
					_pub_to("Stats/#{k}", "", retain: true);
				end
			end
		end
	end
end
