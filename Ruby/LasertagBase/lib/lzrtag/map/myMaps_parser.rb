
require 'xmlsimple'

module LZRTag
	module Maps
		class MyMapsParser
			def initialize(filename)
				@xmlStructure = XmlSimple.xml_in(filename)["Document"][0];

				@styles = Hash.new();
				_fetch_styles();

				@polygons = Array.new();
				_fetch_polygons();
			end

			def _fetch_styles()
				@xmlStructure["Style"].each do |s|
					id = s["id"];
					if(id =~ /(.*)-normal$/)
						id = $1;
					else
						next;
					end

					@styles[id] = {
						color: s["PolyStyle"][0]["Color"][0],
						borderColor: s["LineStyle"][0]["Color"][0]
					};
				end
			end
			private _fetch_styles

			def _fetch_polygons()
				@xmlStructure["Placemark"].each do |zone|
					outZone = Hash.new();
					outZone[:name]  = zone["Name"][0];
					outZone[:style] = @styles[zone["styleUrl"][0]];

					rawPolyData = zone["Polygon"][0]["outerBoundaryIs"][0]["LinearRing"][0]["coordinates"][0];
					rawPolyData.gsub!(" ", "");
					rawPolyArray = rawPolyData.split("\n");

					outZone[:polygon] = Array.new();
					rawPolyArray.each do |point|
						point = point.split(",");
						outZone[:polgon] << point[0..1];
					end
				end
			end
		end
	end
end
