
require_relative 'life_player.rb'

module LZRTag
	module Player
		class Statistics < Life
			attr_reader :kills
			attr_reader :deaths
			attr_reader :damage_done, :damage_received

			def initialize(*args)
				@kills = 0;
				@deaths = 0;

				@damage_done = 0;
				@damage_received = 0;
				@healed = 0;

				super(*args);
			end

			def reset()
				@kills = 0;
				@deaths = 0;

				@damage_done = 0;
				@damage_received = 0;
				@healed = 0;

				[	"Kills", "Deaths",
					"DamageDone", "DamageReceived",
					"Healed"].each do |k|
					_pub_to("Stats/#{k}", 0, retain: true);
				end

				# super(); // TO DO!
			end

			def add_damage(nDamage)
				@damage_done += nDamage;

				_pub_to("Stats/DamageDone", @damage_done, retain:  true);
			end
			def add_kill()
				@kills += 1;

				_pub_to("Stats/Kills", @kills, retain: true);
			end

			def regenerate(*args)
				healDone = super(*args);

				@healed += healDone;
				#_pub_to("Stats/Healed", @healed, retain: true);
			end
			def damage_by(*args)
				dmg_done = super(*args);

				@damage_received += dmg_done;
				args[1].add_damage(dmg_done) if(args[1])
				_pub_to("Stats/DamageReceived", @damage_received, retain: true);
			end

			def _set_dead(d, player = nil)
				return unless super(d, player);
				return unless d;

				player.add_kill() if(player);

				@deaths += 1;
				_pub_to("Stats/Deaths", @deaths, retain: true);
			end

			def ratio_kd()
				return @kills/@deaths
			end

			def clear_all_topics()
				[	"Kills", "Deaths",
					"DamageDone", "DamageReceived",
					"Healed"].each do |k|
					_pub_to("Stats/#{k}", "", retain: true);
				end
			end
		end
	end
end
