
require_relative 'effects_player.rb'

module LZRTag
	module Player
		class Life < Effects

			# Return the amount of life a player currently has.
			# This value can not be set directly, but should instead be modified
			# via {regenerate} and {damage_by}.
			# It is a number from 0 to maxLife (default 100
			# Any change in a player's life is send to MQTT, as well as signalled via
			# :playerRegenerated and :playerHurt events
			attr_reader :life
			# Set the maximum amount of life a player has. Can be useful to
			# introduce tank classes with more life.
			attr_reader :maxLife

			# Returns the last time the player was damaged.
			# This can be useful to apply buffs and other data, as well as
			# "post-damage regenration delay"
			attr_reader :lastDamageTime

			def initialize(*data)
				super(*data);

				@life    = 0;
				@maxLife = 100;

				@lastDamageTime = Time.at(0);

				regenerate(@maxLife);
			end

			# Regenerate the player by a given amount.
			# This function will update the player's life, increasing it
			# by amount, and will send :playerRegenerated [player, deltaLife]
			# or :playerFullyRegenerated [player]
			# if a player's life was increased to the maximum value.
			# @param amount [Numeric] Amount of life to add
			def regenerate(amount)
				unless((amount.is_a? Numeric) && amount >= 0)
					raise ArgumentError, "Amount needs to be a positive number!"
				end

				nLife = @life + amount;
				nLife = @maxLife if(nLife > @maxLife)
				return if nLife == @life;

				oLife = @life;
				@life = nLife;

				@handler.send_event(:playerRegenerated, self, @life - oLife);
				if(@life == maxLife)
					@handler.send_event(:playerFullyRegenerated, self);
				end

				_pub_to("Stats/HP", @life.to_s, retain: true);
			end

			# Damage a player by a given amount with given source.
			# This function will damage a player by "amount", but will not
			# decrease a player's life below zero. Instead, it will kill the player.
			# This function will send out :playerHurt [player, sourcePlayer, deltaLife]
			# and potentially also :playerKilled
			# @param amount [Numeric] Amount to damage the player by
			# @param sourcePlayer [nil, Player::Base] The player this damage originated from - optional
			def damage_by(amount, sourcePlayer = nil)
				unless(amount.is_a? Numeric)
					raise ArgumentError, "Amount needs to be a number!";
				end
				unless (sourcePlayer.is_a? Base) or sourcePlayer.nil?
					raise ArgumentError, "sourcePlayer needs to be a Player::Base!"
				end

				oLife = @life;
				nLife = @life - amount;
				nLife = [0, nLife].max;

				@lastDamageTime = Time.now();

				if(nLife != @life)
					@life = nLife;

					@handler.send_event :playerHurt, self, sourcePlayer, oLife - @life;
					_pub_to("Stats/HP", @life.to_s, retain: true);

					kill_by(sourcePlayer) if(nLife <= 0);
				end

				return oLife - nLife;
			end

			def maxLife=(newVal)
				unless(newVal.is_a? Numeric && newVal >= 1)
					raise ArgumentError, "Amount needs to be a number >= 1!"
				end

				@maxLife = newVal;
				if(@life > @maxLife)
					@life = @maxLife;
					_pub_to("Stats/HP", @life, retain: true);
				end

				_pub_to("CFG/MaxLife", @maxLife, retain: true);
			end

			# @private
			def clear_all_topics()
				super();
				_pub_to("Stats/HP", "", retain: true);
				_pub_to("CFG/MaxLife", "", retain: true);
			end
		end
	end
end
