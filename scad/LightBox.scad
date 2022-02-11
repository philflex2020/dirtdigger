// NBTV Light Box
// Version 0.1 by Andrew Davie
// all measurements in millimeters

// 0.1  modified backplate and top shelf concept so that it's easy to get circuit board in.
// 0.0  initial version - prints OK but hard to get circuit board in!




// Note, everything built around it's centerpoint (x/y) but Z is always @0 and climbing, so adjust positions accordingly.

$fn=128;            // facets for round stuff

TOLERANCE = 0.1;        // adjusts for math errors in low-res view


// LED circuit board

LED_BOARD_X = 47;
LED_BOARD_Y = 72;
LED_BOARD_Z = 1.65;
LED_ASSEMBLY_Z = 12.25;         // height including LEDs

LED_RADIUS = 5;
COL_STAGGER = 1;                // 0 will not stagger the cols, 1 will
LED_COLS = 8;                   // # columns of LEDs
LED_NUMBER_PER_COL = 4;             
LED_SPACING = LED_RADIUS*0.75;
LED_STRIP_SPACING = LED_SPACING*2;

THICKNESS = 2;

BOX_X = LED_BOARD_X;
BOX_Y = LED_BOARD_Y;
BOX_Z = 25;


module LED(diameter,height) {
    
    hull() {
        cylinder(d=diameter,h=height-diameter/2);
        translate([0,0,height-diameter/2])
            sphere(d=diameter);
        cylinder(d=diameter+0.1,height=0.2);
    }
}

module LED_Strip(diameter, height, spacing, number) {
    for (i = [0:number]) {
        translate([0, spacing*i, 0])
            LED(diameter,height);
    }
}

module CircuitBoard(x,y,z) {

    // create a centered "base board"
    translate([-x/2,-y/2,0]) {
            cube([x,y,z]);
    }
    
    // Create strips of LEDs
    for (col= [1:LED_COLS]) {
                
         translate([-(LED_COLS+1)*LED_SPACING/2 + col*LED_SPACING,
                    -(LED_STRIP_SPACING*(col%2))/2-LED_STRIP_SPACING*LED_NUMBER_PER_COL/2,z])
            %LED_Strip(5,8,LED_STRIP_SPACING,4);
    }
}


// arc module from http://www.thingiverse.com/thing:1092611/#files
// THANKS!

module arc(radius, thick, angle){
	intersection(){
		union(){
			rights = floor(angle/90);
			remain = angle-rights*90;
			if(angle > 90){
				for(i = [0:rights-1]){
					rotate(i*90-(rights-1)*90/2){
						polygon([[0, 0], [radius+thick, (radius+thick)*tan(90/2)], [radius+thick, -(radius+thick)*tan(90/2)]]);
					}
				}
				rotate(-(rights)*90/2)
					polygon([[0, 0], [radius+thick, 0], [radius+thick, -(radius+thick)*tan(remain/2)]]);
				rotate((rights)*90/2)
					polygon([[0, 0], [radius+thick, (radius+thick)*tan(remain/2)], [radius+thick, 0]]);
			}else{
				polygon([[0, 0], [radius+thick, (radius+thick)*tan(angle/2)], [radius+thick, -(radius+thick)*tan(angle/2)]]);
			}
		}
		difference(){
			circle(radius+thick);
			circle(radius);
		}
	}
}


// Create a box centered on [0,0] and flat on z-plane
module hollowBox(x,y,z,thickness) {
    translate([-x/2-thickness,-y/2-thickness,0])
        difference() {
            cube([x+2*thickness,y+2*thickness,z+2*thickness]);
            translate([thickness,thickness,thickness])
                cube([x,y,z+thickness+TOLERANCE]);
        }
}



// We have a bunch of shelves; from the window back they are...
// first a sheet of diffusing material flush with the window
// then another sheet on FILTER1_SHELF
// then another sheet on FILTER2_SHELF
// then the circuit board with LEDS sits on LED_SHELF
// finally, the backplate sits on BACKPLATE_SHELF

FILTER1_SHELF = 3;
FILTER2_SHELF = 10;
LED_SHELF = 20;
BACKPLATE_SHELF = BOX_Z;

module shelf(height,modifier=0) {
    translate([-BOX_X/2,-BOX_Y/2,height])
        difference() {
            cube([BOX_X,BOX_Y,THICKNESS]);
            translate([THICKNESS*modifier,THICKNESS,-TOLERANCE])
                cube([BOX_X-THICKNESS*2*modifier,BOX_Y-THICKNESS*2,THICKNESS+2*TOLERANCE]);
        }
}

// Shelves are designed so objects below them can be inserted easily
// use a hot-glue gun to seat each item in position

shelf(FILTER1_SHELF);
shelf(FILTER2_SHELF);
shelf(LED_SHELF);
//shelf(BACKPLATE_SHELF);

WINDOW_TOLERANCE = 1.05;
HOLE1_RADIUS = 126.71;
HOLE2_RADIUS = 144.27;
HOLE_DIAMETER = 0.25;
ARC_THICK = (HOLE2_RADIUS-HOLE1_RADIUS+HOLE_DIAMETER)*WINDOW_TOLERANCE;


// The arced view window
module viewWindow(){
    translate([-(HOLE1_RADIUS+ARC_THICK/2),0,-TOLERANCE])
        linear_extrude(height=THICKNESS+2*TOLERANCE)
            arc(HOLE1_RADIUS,ARC_THICK,360/32*WINDOW_TOLERANCE,$fn=128);
}


// The backplate is a snap-fit, with a small slot to help get it off
// Note that the shelf stops light coming out of the slot :))

SLOT = 10;          // screwdriver slot for removing backplate
module backplate(){
    cube([BOX_X+2*THICKNESS,BOX_Y+2*THICKNESS,THICKNESS]);
    translate([THICKNESS,THICKNESS,THICKNESS]) {
        difference(){
            cube([BOX_X,BOX_Y,THICKNESS]);
            translate([THICKNESS,THICKNESS,-TOLERANCE])
                cube([BOX_X-2*THICKNESS,BOX_Y-2*THICKNESS,THICKNESS+TOLERANCE*2]);
        }
    }
    
//    difference(){
//        cube([BOX_X,BOX_Y,THICKNESS]);
//        translate([BOX_X/2-SLOT/2,-TOLERANCE,-TOLERANCE])
//            cube([SLOT,THICKNESS*0.75,THICKNESS+TOLERANCE*2]);
//    }
}



// Now do the actual rendering...



// Build the box and cut out the view window
difference(){
    hollowBox(BOX_X,BOX_Y,BOX_Z,THICKNESS);
    viewWindow();
}

// Now put in a circuit board to show how it looks
// this bit will NOT be printed
%translate([0,0,LED_SHELF+LED_BOARD_Z+THICKNESS])
    rotate([0,180,0])
        color([0.9,0.2,0.5],alpha=0.9)
            CircuitBoard(LED_BOARD_X,LED_BOARD_Y,LED_BOARD_Z);

// Finally we draw the backplate (on the z-plane so it can be printed!)
translate([BOX_X-BOX_X/2+THICKNESS+1,-BOX_Y/2-THICKNESS,0])
    backplate();
