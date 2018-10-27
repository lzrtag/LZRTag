
require_relative 'effects_player.rb'

module LZRTag
	module Player
		class Life < Effects

			attr_reader :life
			attr_reader :maxLife

			def initialize(*data)
				super(*data);

				@life    = 100;
				@maxLife = 100;

				regenerate(@maxLife);
			end


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

				_pub_to("HP", @life.to_s, retain: true);
			end

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

				if(nLife != @life)
					@life = nLife;

					@handler.send_event :playerHurt, self, sourcePlayer, oLife - @life;
					kill_by(sourcePlayer) if(nLife <= 0);

					_pub_to("HP", @life.to_s, retain: true);
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
					_pub_to("HP", @life, retain: true);
				end
			end

			def clear_all_topics()
				super();
				_pub_to("HP", "", retain: true);
			end
		end
	end
end
