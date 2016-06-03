$fs = 0.8;

$wallThickness = 0.6;

casingBottomRadius 	= 26;
casingTopRadius 		= 15;
casingHeight 			= 22 - 7;
	

screwHead = 6;
screwRadius = casingBottomRadius - 5;

module casing_shell_inner() {
	$fn = 6;
	cylinder(r1 = casingBottomRadius - $wallThickness,
					r2 = casingTopRadius - $wallThickness,
					h  = casingHeight - $wallThickness);
}

module casing_shell() {
	difference() {
		$fn = 6;
		cylinder(r1 = casingBottomRadius, r2 = casingTopRadius, h = casingHeight);
		casing_shell_inner();
	}
}
		

module casing_screw_negative(position) {
	translate(position + [0, 0, 3]) cylinder(d = screwHead, h = $wallThickness + 10.001);
	translate(position - [0, 0, 10]) cylinder(d = 3.1, h = 100);
}

module casing_screw_positive(position) {
	intersection() {
		translate(position) cylinder(d = screwHead + $wallThickness*2, h = 100);
		casing_shell_inner();
	}
}

module casing_with_screws() {
	difference() {
		union() {
			casing_shell();
			for(i=[0:120:360]) rotate([0, 0, i]) casing_screw_positive([0, -1.5 - $wallThickness + screwRadius, 0]);
		}
		for(i=[0:120:360]) rotate([0, 0, i])
		casing_screw_negative([0, -1.5 - $wallThickness + screwRadius, 0]);
	}
}

module electronics_holder() {
	elX = 24.36; 
	elY = 19;

	zUp = 2 + $wallThickness;
	
	$wallThickness = 0.6;
	
	translate([elX + $wallThickness *2, elY + $wallThickness *2, 0] /-2)
	difference() {
		cube([elX + $wallThickness *2, elY + $wallThickness *2, zUp + $wallThickness]);
		translate([$wallThickness*2, $wallThickness*2, 0]) cube([elX - $wallThickness*2, elY - $wallThickness*2, zUp + $wallThickness]);
		translate([$wallThickness, $wallThickness, zUp]) cube([elX, elY, 100]);
	}
}

module casing_bottom() {
	difference() {
		union() {
			$fn = 6;
			cylinder(r = casingBottomRadius, h = $wallThickness);
			
			difference() {
				cylinder(r = casingBottomRadius, h = 7);
				cylinder(r = casingBottomRadius - $wallThickness, h = 8);
			}
			
			for(i=[0:120:360]) rotate([0, 0, i]) translate([0, -1.5 - $wallThickness + screwRadius]) 			cylinder(d = screwHead, h = 7, $fn = 13);
		}
		for(i=[0:120:360]) rotate([0, 0, i]) translate([0, -1.5 - $wallThickness + screwRadius])
			cylinder(d = 3.1, h = 7);
	}
}


translate([0, 0, 7]) casing_with_screws();
%rotate([0, 0, 30]) electronics_holder();
%casing_bottom();