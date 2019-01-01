
module LZRTag
	module Map
		class Zone
			attr_accessor :tag

			attr_accessor :centerPoint, :radius
			attr_accessor :polygon
			attr_accessor :coordinatesAsGPS

			attr_accessor :teamMask

			attr_accessor :data

			attr_accessor :style

			def initialize()
				@centerPoint = [0, 0];
				@radius = 0;
				@polygon = Array.new();
				@coordinatesAsGPS = false;

				@teamMask = 255;

				@style = {
					color: 		 "transparent",
					borderColor: "black"
				}

				@data = Hash.new();
			end

			def affects_teams(teamList)
				@teamMask = 0;
				[teamList].flatten.each do |t|
					@teamMask += (2<<t);
				end
			end
			def ignores_teams(teamList)
				@teamMask = 255;
				[teamList].flatten.each do |t|
					@teamMask -= (2<<t);
				end
			end

			def self.from_raw_zone(rawZone)
				outZone = Zone.new();

				outZone.tag = rawZone[:arguments]["tag"] || rawZone[:name];

				outZone.polygon = rawZone[:polygon];
				outZone.coordinatesAsGPS = true;

				if(rawZone[:style])
					outZone.style = rawZone[:style];
				end

				if(tMask = rawZone[:arguments]["teamMask"])
					outZone.teamMask = tMask.to_i;
					rawZone[:arguments].delete "teamMask"
				end

				outZone.data = rawZone[:arguments];

				return outZone
			end

			def to_h()
				outHash = Hash.new();

				raise ArgumentError, "Tag needs to be set!" if(@tag.nil?);
				outHash[:tag] = @tag;
				outHash[:teamMask] = @teamMask;

				if(@radius > 0.1)
					outHash[:centerPoint] = @centerPoint;
					outHash[:radius] = @radius;
				else
					outHash[:polygon] = @polygon;
				end
				outHash[:coordinatesAsGPS] = @coordinatesAsGPS

				outHash[:style] = @style;

				outHash[:data] = @data;

				return outHash;
			end

			def inspect()
				return "#<Zone: #{to_h}>";
			end
		end
	end
end
