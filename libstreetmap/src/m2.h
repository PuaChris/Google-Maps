/* 
 * File:   m2.h
 * Author: liaoxin7
 *
 * Created on February 23, 2018, 2:38 PM
 */


#ifndef M2_H
#define M2_H

#include "utilities.h"
#include <string>
#include <unordered_map>
#include <vector>
#include "Map_Data.h"
#include "StreetsDatabaseAPI.h"


constexpr double BIG_STREET_WIDTH = 0.6;
constexpr double MEDIUM_STREET_WIDTH = 0.3;
constexpr double SMALL_STREET_WIDTH = 0.22;
constexpr double OPEN_FEATURE_WIDTH = 0.3;
constexpr double MIN_LINE_WIDTH = 2.0;

constexpr double FEATURE_DISPLAY_ZOOM_THRESHOLD = 0.00001;

constexpr double HIGHWAY_SPEED_LIMIT = 90;
constexpr double MAJOR_STREET_SPEED_LIMIT = 50;

constexpr double text_size_factor = 5.5;
constexpr double zoom_factor = 1/2.3;

constexpr int border_bottom_y = 42;
constexpr int border_top_y = 15;

constexpr int text_box_bottom_y = border_bottom_y - 1;
constexpr int text_box_top_y = border_top_y + 1;

constexpr int border_width = 120;
constexpr int text_box_width = 118;
constexpr int text_box_mid_height = (border_bottom_y + border_top_y)/2;

constexpr int icon_width = 44;
constexpr int icon_height = 72;

constexpr int screen_offset = 200;
constexpr int min_distance = 10;

constexpr double zoom_level_threshold_streets_1 = 0.5;
constexpr double zoom_level_threshold_streets_2 = 0.15;
constexpr double zoom_level_threshold_POI_markers = 0.05;

constexpr double min_POI_distance = 125;
//constexpr double min_POI_distance = 0;

constexpr double minor_street_text_bounding_box = 100.0;
constexpr double medium_street_text_bounding_box = 200.0;
constexpr double major_street_text_bounding_box = 500.0;

constexpr double MIN_TEXT_ANGLE = -90.0;
constexpr double MAX_TEXT_ANGLE = 90.0;
// add or subtract this to flip the text
constexpr double TEXT_FLIP_ANGLE = 180.0;

const std::string LEFT_ARROW = "\u2190";
const std::string RIGHT_ARROW = "\u2192";

constexpr double AUTO_ZOOM_SIZE = 200.0;

// Minimum distance (in m) between street segments for the text label to be printed
constexpr double MIN_TEXT_SAME_STREET_DISTANCE = 500;   // when on the same street
constexpr double MIN_TEXT_STREET_DISTANCE = 150;

constexpr int HIGHLIGHT_FONT_SIZE = 11;
constexpr int STREET_SEGMENT_FONT_SIZE = 9;
constexpr int POI_FONT_SIZE = 9;
constexpr double POI_TEXT_BOUNDING_BOX = 250;
constexpr double POI_TEXT_OFFSET = 1000;


constexpr double DEFAULT_TURN_PENALTY = 15;


struct intersection_data {
    LatLon position;
    std::string name;
};

struct poi_data {
    std::string name;
    POI_type type;
    double position_x;
    double position_y;
};

struct street_segment_data {
    StreetSegmentIndex index;
    std::string name;
    double middle_x;
    double middle_y;
    double path_angle;  // in degrees
    double text_angle; // in degrees, should be restricted between -90 and 90
    bool is_one_way; // true if one way
    bool is_one_way_left; // true if going left (only applies if one way)
    StreetWidth width;
};

struct feature_data {
    unsigned id;
    bool closed;
    unsigned layer;
    double area;
    t_point* curve_points;
};

struct path_intersection_pair_data {
    std::string street_name;
    std::vector<LatLon> curve_points;
    double x1, y1, x2, y2;
    double length;
    double middle_x;
    double middle_y;
    double angle;
    
    void set_up_curve_points(unsigned ss_id) {
        auto info = getStreetSegmentInfo(ss_id);

        //create a new vector to hold curve points
        unsigned numCurvePoints = info.curvePointCount;
        curve_points.resize(numCurvePoints + 2); //to hold curve points and 2 end points

        curve_points[0] = getIntersectionPosition(info.from);
        unsigned cp_id;
        for (cp_id = 1; cp_id <= numCurvePoints; cp_id++) {
            curve_points[cp_id] = getStreetSegmentCurvePoint(ss_id, cp_id - 1);
        }
        curve_points[cp_id] = getIntersectionPosition(info.to);

    }
};


extern double avg_latitude;

extern std::string name_of_map;

extern std::unordered_map<std::string, POI_type> POI_type_strings;


bool operator<(street_segment_data left, street_segment_data right);
bool compareFeature(feature_data lhs, feature_data rhs);

//streets
void collect_street_segments() ;
void collect_curve_points_for_street_segment(unsigned ss_id);
void draw_streets();
void draw_streets_of_width(StreetWidth type);
void collect_street_names();

//features
void collect_features();
void collect_curve_points_for_features(unsigned f_id);
void draw_features();
void display_intersection_info(t_point highlight_pos, IntersectionIndex intersection_id);
void free_features();

//mouse
void act_on_button_press(float x, float y, t_event_buttonPressed event) ;

//street names
double distance_between_points(double x1, double y1, double x2, double y2);
std::string get_street_segment_text(std::string name, bool is_one_way, bool is_one_way_left);
double get_street_bounding_box(StreetWidth street_width);
void draw_street_segment_names();
void collect_display_segments_tree();

// Map
void draw_screen();
void set_max_coordinates();
void draw_map();

// Buttons
void act_on_find_button(void (*drawscreen) (void));
void act_on_clear_button(void (*drawscreen) (void));
void act_on_path_int_button(void (*drawscreen) (void));
void act_on_path_int_marker_button(void (*drawscreen) (void));
void act_on_path_poi_button(void (*drawscreen) (void));

// POIs
void draw_POI_marker (t_point location, POI_type type);
POI_type get_POI_type(std::string POI_string);
void collect_POIs();
void draw_poi_names();

// Highlights
void draw_marker(t_point location);
void draw_highlights();
bool is_mouse_on_highlight(t_point mouse_pos, t_point highlight_pos);
bool can_highlight_intersection(t_point point_to_highlight);
void insert_highlight(t_point point_to_highlight, IntersectionIndex intersection_id);
void remove_highlight(double mouse_x, double mouse_y);
void remove_all_highlights();
void highlight_intersection(IntersectionIndex intersection_id);

void draw_path();
void update_path_to_draw(IntersectionIndex start_int, IntersectionIndex end_int, std::vector<StreetSegmentIndex> path);
void find_int_path(IntersectionIndex start_int, IntersectionIndex end_int);
void find_poi_path(IntersectionIndex start_int, std::string poi_name);

std::string get_cardinal_direction(double degrees);
std::string get_relative_direction(double angle1, double angle2);
std::vector<std::string> get_path_directions();

void on_left_arrow_press();
void on_right_arrow_press();
void on_enter_key_press();
void on_key_press(char key_pressed, int key_sym);
void draw_text_input_box();
void draw_text_display_box();
void act_on_help_button(void (*drawscreen) (void));

#endif /* M2_H */

