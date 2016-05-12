$fs = 0.8;

module RGB_Led() {
	difference() {
		union() {
			children();
			
			cylinder(d = 6, h = 10);
		}
		
		translate([0, 0, -0.1]) cylinder(d = 5, h = 11);
	}
}

module IR_Shield() {
	
	sHeight = 5;
	fDiam = 1;
	
	$fs = 0.1;
	
	difference() {
		hull() {
			cylinder(d = 4, h = 0.1);
			translate([0, 0, sHeight]) cylinder(d = fDiam + 1, h = 0.1);
		}
		hull() {
			translate([0, 0, -0.1]) cylinder(d = 3, h = 0.1);
			translate([0, 0, sHeight + 0.001]) cylinder(d = fDiam, h = 0.1);
		}
	}
}

module IR_Led() {
	difference() {
		union() {
			children();
			
			cylinder(d = 4, h = 5);
			
			translate([0, 0, 4.9]) IR_Shield();
		}
	translate([0, 0, -0.1]) cylinder(d = 3, h = 5.1001);
	}
}

module IR_Set() {
	RGB_Led()
	translate([8, 0, 0]) IR_Led() translate([-8, 0, 0]) 
	
	hull() {
		cylinder(d = 6, h = 1);
		translate([8, 0, 0]) cylinder(d = 4, h = 1);
	}
}

IR_Set();