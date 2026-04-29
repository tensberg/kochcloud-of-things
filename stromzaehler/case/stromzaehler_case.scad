// alle Angaben in mm

$fn = 50;

// Entwicklungs-Parameter
render_bottom = true;
render_top_1 = true;
render_top_2 = true;
render_exploded = true;

//
// vorgegebene Parameter
//

case_wall_thickness = 1.8;
case_wall_inset = 0.4;

circuit_board_length = 60;
circuit_board_length_top_1 = 29;
circuit_board_width = 40;
circuit_board_height = 1.53;
circuit_height_above_board_1 = 10;
circuit_height_above_board_2 = 17;
circuit_height_below_board = 2; // TODO: messen

corner_offset = 0; // Rundung der Gehäuseecken

upper_case_connector_inset = 0.3;

// die 4 Löcher an den Seiten der Platine für die Befestigung
circuit_board_hole_diameter = 2.5;
circuit_board_hole_offset = 0.8;
circuit_board_holder_height = 3;

// Bauelemente

// Druckknopf
button_diameter = 13.6;
button_offset_front = 1;
button_offset_right = 7;

// ESP
// soviel steht der ESP über die Platine hinaus
board_overhang = 2;

// Sensor-Anschlüsse
// Abstand vom Platinenrand
connector_offset_left = 7;
connector_offset_right = 2;
connector_offset_front = 21;
connector_width = 6;

// LEDs
led_diameter = 5;
led_offset_front = 15.5;
led_offset_right_1 = 16;
led_offset_right_2 = 32;

// Gehäuseöffner (Platz für Schraubenzieher zum aufhebeln)
case_opener_width = 4;
case_opener_height = 1.4;

// USB-Anschluss
// offsets relativ zur Platine
usb_connector_offset_right = 18;
usb_connector_offset_bottom = 12;
usb_connector_width = 8;
usb_connector_height = 3;

// Stromversorgung Anschluss
// offsets relativ zur Platine
battery_connector_offset_back = 4; // TODO: messen
battery_connector_offset_bottom = 1; // TODO: messen
battery_connector_width = 6; // TODO: messen
battery_connector_height = 2; // TODO: messen

//
// berechnete Parameter
//

circuit_board_length_inset = circuit_board_length + 2*case_wall_inset;
circuit_board_length_top_1_inset = circuit_board_length_top_1 + case_wall_inset;
circuit_board_width_inset = circuit_board_width + 2*case_wall_inset;
circuit_board_length_top_2 = circuit_board_length - circuit_board_length_top_1;
circuit_board_length_top_2_inset = circuit_board_length_top_2 + case_wall_inset;

circuit_board_holder_diameter = circuit_board_hole_diameter - 0.2;
circuit_board_holder_offset = circuit_board_hole_offset + 0.1;
circuit_board_holder_hole_diameter = circuit_board_holder_diameter + 0.2; // fixate the board but do not connect too hard
circuit_board_holder_hole_offset = circuit_board_hole_offset + 0.05;

circuit_board_holder_total_offset = circuit_board_holder_offset + circuit_board_holder_diameter/2;

circuit_board_holder_hole_total_offset = circuit_board_holder_hole_offset + circuit_board_holder_hole_diameter/2;

case_length = circuit_board_length + board_overhang + case_wall_thickness * 2 + case_wall_inset * 2;
case_length_top_1 = circuit_board_length_top_1 + case_wall_thickness + case_wall_inset;
case_length_top_2 = circuit_board_length_top_2 + board_overhang + case_wall_thickness + case_wall_inset;
case_width = circuit_board_width + case_wall_thickness * 2 + case_wall_inset * 2;
circuit_top = case_wall_thickness + case_wall_inset + circuit_height_below_board;
case_height_lower = circuit_top + circuit_board_height;
case_height_upper_1 = circuit_height_above_board_1 + case_wall_inset + case_wall_thickness;
case_height_upper_2 = circuit_height_above_board_2 + case_wall_inset + case_wall_thickness;

hole_stand_size = case_wall_inset + circuit_board_hole_diameter + circuit_board_hole_offset * 2;
hole_pillar_diameter = circuit_board_hole_diameter * 0.9;

button_radius = button_diameter / 2;
led_radius = led_diameter / 2;

// Rendering-Parameter
rotate_top = [render_exploded ? 180 : 0, 0, 0];
translate_top_1 = render_exploded ? [0, case_width*2 + 10, case_height_upper_1] : [0, 0, case_height_lower];
translate_top_2 = render_exploded ? [case_length_top_1 + 3, case_width*2 + 10, case_height_upper_2] : [case_length_top_1, 0, case_height_lower];

// Gehäuse Unterteil
if (render_bottom) {
    difference() {
        // bottom
        linear_extrude(case_height_lower)
            offset(corner_offset) offset(-corner_offset)
                square([case_length, case_width]);
        
        // lower layer cutout
        translate([case_wall_thickness, case_wall_thickness, case_wall_thickness]) {
            linear_extrude(case_height_lower + circuit_board_height + 1) {
                lower_layer_cutout_2d();
            }
        }

        // upper layer cutout
        translate([case_wall_thickness, case_wall_thickness, 
                    case_wall_thickness + case_wall_inset + circuit_height_below_board]) {
            linear_extrude(circuit_board_height + 1) {
                polygon([
                    // bottom left
                    [0, 0],
                    // top left
                    [0, circuit_board_width_inset],
                    // top right
                    [circuit_board_length_inset, circuit_board_width_inset],
                    [circuit_board_length_inset, circuit_board_width_inset - hole_stand_size],
                    [circuit_board_length_inset + board_overhang, circuit_board_width_inset - hole_stand_size],
                    // bottom right
                    [circuit_board_length_inset + board_overhang, hole_stand_size],
                    [circuit_board_length_inset, hole_stand_size],
                    [circuit_board_length_inset, 0]
                ]);
            }
        }
    }
    
    // circuit board halter säulen
    translate([case_wall_thickness + case_wall_inset, case_wall_thickness + case_wall_inset, circuit_top]) {
        translate([circuit_board_holder_total_offset, circuit_board_holder_total_offset, 0])
            cylinder(h = circuit_board_holder_height, d = circuit_board_holder_diameter, center=false);
        translate([circuit_board_length - circuit_board_holder_total_offset, circuit_board_holder_total_offset, 0])
            cylinder(h = circuit_board_holder_height, d = circuit_board_holder_diameter, center=false);
        translate([circuit_board_holder_total_offset, circuit_board_width - circuit_board_holder_total_offset, 0])
            cylinder(h = circuit_board_holder_height, d = circuit_board_holder_diameter, center=false);
        translate([circuit_board_length - circuit_board_holder_total_offset, circuit_board_width - circuit_board_holder_total_offset, 0])
            cylinder(h = circuit_board_holder_height, d = circuit_board_holder_diameter, center=false);
    }
}

// Gehäuse Oberteil 1
if (render_top_1) {
    translate(translate_top_1) rotate(rotate_top) {
        difference() {
            // top
            linear_extrude(case_height_upper_1)
                offset(corner_offset) offset (-corner_offset)
                    square([case_length_top_1, case_width]);
            
            // inner cutout
            translate([case_wall_thickness, case_wall_thickness, -1])
            linear_extrude(circuit_height_above_board_1 + 1)
                lower_layer_cutout_2d();
            
            // parts cutouts
            translate([case_wall_thickness + case_wall_inset, case_wall_thickness + case_wall_inset, 
                        circuit_height_above_board_1 - 1]) {
                linear_extrude(case_wall_thickness + 2) {
                    // button
                    translate([button_offset_front + button_radius, button_offset_right + button_radius])
                        circle(button_radius);
                    
                    // LEDs
                    translate([led_offset_front + led_radius, led_offset_right_1 + led_radius])
                        circle(led_radius);
                    translate([led_offset_front + led_radius, led_offset_right_2 + led_radius])
                        circle(led_radius);
                    
                    // connectors
                    translate([case_length_top_1 - connector_width, connector_offset_left])
                        square([connector_width + 1, circuit_board_width - connector_offset_left - connector_offset_right]);
                    
                }
                
                // connector labels
                translate([0, 0, case_wall_thickness / 2 + 1]) linear_extrude(case_wall_thickness / 2 + 1) {
                    translate([led_offset_front + led_radius, led_offset_right_1 - 1]) rotate(-90)
                      text("HH", size=4, valign="center");
                    translate([led_offset_front + led_radius, led_offset_right_2 - 1]) rotate(-90)
                      text("HZ", size=4, valign="center");
                }
            }
            
            // case opener cutout
            translate([-1, 0, -1]) {
                case_opener_cutout();
            }
            
            // circuit board halter säulen
            translate([case_wall_thickness + case_wall_inset, case_wall_thickness + case_wall_inset, -1]) {
                translate([circuit_board_holder_hole_total_offset, circuit_board_holder_hole_total_offset, 0])
                    cylinder(h = circuit_board_holder_height, d = circuit_board_holder_hole_diameter);
                translate([circuit_board_holder_hole_total_offset, circuit_board_width - circuit_board_holder_hole_total_offset, 0])
                    cylinder(h = circuit_board_holder_height, d = circuit_board_holder_hole_diameter);
            }
        }
        
        translate([case_length_top_1, 0])
            upper_case_connectors();
    }
}

// Gehäuse Oberteil 2
if (render_top_2) {
    translate(translate_top_2) rotate(rotate_top) {
        difference() {
            // top
            linear_extrude(case_height_upper_2)
                offset(corner_offset) offset (-corner_offset)
                    square([case_length_top_2, case_width]);
            
            // main cutout
            translate([- circuit_board_length_top_1, case_wall_thickness, -1])
                //cube([circuit_board_length_top_2_inset + 1, circuit_board_width_inset, circuit_height_above_board_2 + 1]);
            linear_extrude(circuit_height_above_board_2 + 1)
                lower_layer_cutout_2d();

            upper_case_connectors(true);
            
            // back wall cutouts
            translate([case_length_top_2 - case_wall_thickness - 1, 0, -1]) {
                // usb connector cutout
                translate([0, usb_connector_offset_right, usb_connector_offset_bottom])
                    cube([case_wall_thickness + 2, usb_connector_width, usb_connector_height + 1]);

                // case opener cutout
                case_opener_cutout();
            }
            
            // battery connector cutout
            translate([circuit_board_length_top_2 - battery_connector_offset_back - battery_connector_width, 
                        case_width - case_wall_thickness - 1, 
                        battery_connector_offset_bottom]) {
                cube([battery_connector_width, case_wall_thickness + 2, battery_connector_height]);
                            
                // +/- sign
                translate([0, case_wall_thickness+2, -0.5]) {
                  rotate([90, 0, 0])
                  linear_extrude(case_wall_thickness) {
                    translate([-4, 0])
                      text("-", size=4);
                    translate([battery_connector_width + 1, 0])
                      text("+", size=4);
                  }
              }
            }
                       
            // circuit board halter säulen
            translate([0, case_wall_thickness + case_wall_inset, -1]) {
                translate([circuit_board_length_top_2 - circuit_board_holder_hole_total_offset, circuit_board_holder_hole_total_offset, 0])
                    cylinder(h = circuit_board_holder_height, d = circuit_board_holder_hole_diameter);
                translate([circuit_board_length_top_2 - circuit_board_holder_hole_total_offset, 
                            circuit_board_width - circuit_board_holder_hole_total_offset, 0])
                    cylinder(h = circuit_board_holder_height, d = circuit_board_holder_hole_diameter);
            }
        }
        
        // esp opening walls
        opening_wall_height = case_height_upper_2 - case_height_upper_1 + case_wall_thickness;
        translate([0, case_wall_thickness, case_height_upper_1 - case_wall_thickness])
            cube([case_wall_thickness, case_wall_inset + connector_offset_left, opening_wall_height]);
        translate([0, case_width - case_wall_thickness- case_wall_inset - connector_offset_right, 
                   case_height_upper_1 - case_wall_thickness])
            cube([case_wall_thickness, connector_offset_right + case_wall_inset, opening_wall_height]);
    }
        
}

module case_opener_cutout() {
    linear_extrude(case_opener_height + 1) {
        translate([0, case_wall_thickness + case_wall_inset + hole_stand_size + 1])
            square([case_wall_thickness + 2, case_opener_width]);
        translate([0, case_width - case_wall_thickness - case_wall_inset - hole_stand_size - 1 - case_opener_width])
            square([case_wall_thickness + 2, case_opener_width]);
    }
}

module lower_layer_cutout_2d() {
    polygon([
        // bottom left
        [hole_stand_size, 0], 
        [hole_stand_size, hole_stand_size], 
        [0, hole_stand_size],
        // top left
        [0, circuit_board_width_inset - hole_stand_size], 
        [hole_stand_size, circuit_board_width_inset - hole_stand_size],
        [hole_stand_size, circuit_board_width_inset],
        // top right
        [circuit_board_length_inset - hole_stand_size, circuit_board_width_inset], 
        [circuit_board_length_inset - hole_stand_size, circuit_board_width_inset - hole_stand_size], 
        [circuit_board_length_inset + board_overhang, circuit_board_width_inset - hole_stand_size],
        // bottom right
        [circuit_board_length_inset + board_overhang, hole_stand_size], 
        [circuit_board_length_inset - hole_stand_size, hole_stand_size], 
        [circuit_board_length_inset - hole_stand_size, 0]
    ]);
}

module upper_case_connectors(negative = false) {
    upper_case_connector(negative);
    translate([0, case_width - case_wall_thickness])
        upper_case_connector(negative);
}

module upper_case_connector(negative) {
    ci = upper_case_connector_inset;
    cd = case_wall_thickness / 2;
    no = negative ? 1 : 0; // negative_offset
    translate([-no, 0, -no]) linear_extrude(case_height_upper_1 + no)
        polygon([
            [0, ci*2], [cd + no, ci*2], [cd + no, ci], [cd*2 + no, ci],
            [cd*2 + no, case_wall_thickness - ci], [cd + no, case_wall_thickness - ci], [cd + no, case_wall_thickness - ci*2], [0, case_wall_thickness - ci*2]]);
}