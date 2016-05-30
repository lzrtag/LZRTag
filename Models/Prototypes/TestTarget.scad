$fs = 0.8;

$wallThickness = 1;

casingBottomRadius 	= 50;
casingTopRadius 		= 30;
casingHeight 			= 20;
	

screwHead = 6;
screwRadius = casingBottomRadius - 10;

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
	translate(position + [0, 0, $wallThickness]) cylinder(d = screwHead, h = 100);
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

module casing_bottom() {
	difference() {
		union() {
			$fn = 6;
			cylinder(r = casingBottomRadius, h = $wallThickness);
		}
		for(i=[0:120:360]) rotate([0, 0, i])
		casing_screw_negative([0, -1.5 - $wallThickness + screwRadius, 0]);
	}
}


casing_bottom();