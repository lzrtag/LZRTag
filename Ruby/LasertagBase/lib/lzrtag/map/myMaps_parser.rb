
require 'xmlsimple'

require_relative 'map_zone.rb'

module LZRTag
	module Map
		class MyMapsParser
			attr_accessor :styles
			attr_accessor :polygons
			attr_accessor :points

			def initialize(filename)
				@xmlStructure = XmlSimple.xml_in(filename)["Document"][0];

				@styles = Hash.new();
				_fetch_styles();

				@polygons = Hash.new();
				@polygons[""] = _fetch_polygons();

				@points = Hash.new();
				@points[""] = _fetch_marks();

				if(folders = @xmlStructure["Folder"])
					folders.each do |folder|
						@polygons[folder["name"][0]] = _fetch_polygons(folder)
						@points[folder["name"][0]] = _fetch_marks(folder);
					end
				end
			end

			def _fetch_styles()
				@xmlStructure["Style"].each do |s|
					id = s["id"];
					if(id =~ /(.*)-normal$/)
						id = $1;
					else
						next;
					end

					next unless(s.has_key?("PolyStyle") && s.has_key?("LineStyle"))

					@styles["#" + id] = {
						color: s["PolyStyle"][0]["color"][0],
						borderColor: s["LineStyle"][0]["color"][0]
					};
				end
			end
			private :_fetch_styles

			def _fetch_polygons(folder = nil)
				folder ||= @xmlStructure;

				outZones = Array.new();

				return outZones unless(placemarks = folder["Placemark"])

				placemarks.each do |zone|
					next unless zone["Polygon"];

					outZone = Hash.new();
					outZone[:name]  = zone["name"][0];
					outZone[:description] = (zone["description"] || [""])[0];

					outZone[:arguments] = Hash.new();
					outZone[:description].split("<br>").each do |tag|
						if(tag =~ /([^:]*):([^:]*)/)
							outZone[:arguments][$1] = $2;
						end
					end

					outZone[:style] = @styles[zone["styleUrl"][0]];

					rawPolyData = zone["Polygon"][0]["outerBoundaryIs"][0]["LinearRing"][0]["coordinates"][0];
					rawPolyData.gsub!(" ", "");
					rawPolyArray = rawPolyData.split("\n");

					outZone[:polygon] = Array.new();
					rawPolyArray.each do |point|
						point = point.split(",");
						next if point.empty?
						outZone[:polygon] << point[0..1];
					end

					outZones << outZone;
				end

				return outZones;
			end
			private :_fetch_polygons

			def _fetch_marks(folder = nil)
				folder ||= @xmlStructure;

				outPoints = Array.new();
				return outPoints unless(placemarks = folder["Placemark"])

				placemarks.each do |pmark|
					next unless pmark["Point"];

					outPoint = Hash.new();
					outPoint[:name] = pmark["name"][0];
					outPoint[:description] = (pmark["description"] || [""])[0];

					outPoint[:description].split("<br>").each do |tag|
						if(tag =~ /([^:]*):([^:]*)/)
							outPoint[:arguments][$1] = $2;
						end
					end

					outPoint[:point] = pmark["Point"][0]["coordinates"][0].gsub(/\s/, "").split(",")[0..1];

					outPoints << outPoint
				end

				return outPoints
			end

			def generate_zones(zoneSet)
				zoneSet = [zoneSet].flatten;

				outZones = Array.new();
				zoneSet.each do |rawZone|
					outZones << Zone.from_raw_zone(rawZone);
				end

				return outZones;
			end
		end
	end
end
