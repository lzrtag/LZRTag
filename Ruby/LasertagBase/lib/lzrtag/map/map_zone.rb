
module LZRTag
	module Map
		class Zone
			attr_accessor :tag

			attr_accessor :centerPoint, :radius
			attr_accessor :polygon
			attr_accessor :coordinatesAsGPS

			attr_reader    :style

			def initialize()
				@centerPoint = [0, 0];
				@radius = 0;
				@polygon = Array.new();
				@gpsCoordinates = false;

				@teamMask = 255;

				@style = {
					color: 		 "transparent",
					borderColor: "black"
				}
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

			def to_h()
				outHash = Hash.new();

				raise ArgumentError, "Tag needs to be set!" if(@tag.nil?);
				outHash[:tag] = @tag;

				if(@radius > 0.1)
					outHash[:centerPoint] = @centerPoint;
					outHash[:radius] = @radius;
				else
					outHash[:polygon] = @polygon;
				end
				outHash[:corrdinatesAsGPS] = @coordinatesAsGPS

				outHash[:style] = @style;
			end
		end
	end
end
