#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <X11/keysymdef.h>
#include <chrono>
#include "graphics.h"
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "pathFinding.h"
#include "Map_Data.h"
#include "utilities.h"
#include "Highlight.h"
#include "autocomplete.h"
#include "easygl/easygl_constants.h"
#include "StreetsDatabaseAPI.h"
#include "test_path.h"

extern Map_Data* map_data;

// Used for unknown street names in the database
const std::string UNKNOWN_STREET_NAME = "<unknown>";

const std::string PROGRAM_NAME = "Mapalytics";
std::string name_of_map;

std::unordered_map<std::string, POI_type> POI_type_strings = {
    {"fuel", fuel},
    {"fast_food", fast_food},
    {"bank", bank},
    {"cafe", cafe},
    {"parking", parking},
    {"school", school},
    {"hospital", hospital},
    {"library", library}
};

std::vector<intersection_data> intersections;

std::vector<poi_data> points_of_interest;

std::vector <Highlight*> list_of_highlights;

//streets categorized by speed limits
std::vector<std::pair<unsigned, std::vector<LatLon>>> street_segments_highway;
std::vector<std::pair<unsigned, std::vector<LatLon>>> street_segments_major;
std::vector<std::pair<unsigned, std::vector<LatLon>>> street_segments_minor;

// Each street is mapped to its name and the data of its street segments (sorted)
std::vector< std::pair<std::string, std::set<street_segment_data>> > streets_to_street_segments;

// Features
std::vector<feature_data>feature_curve_points;

//Street segment names (R-tree to draw)
typedef std::pair<Cartesian_point, street_segment_data> StreetSegmentPair;
bgi::rtree< StreetSegmentPair, bgi::quadratic < max_elements_R_tree >> display_segments_tree;

// For POIs (R-tree to draw)
typedef std::pair<Cartesian_point, poi_data> POIPair;

//R-tree to search POIs to draw
bgi::rtree< POIPair, bgi::quadratic < max_elements_R_tree >> display_poi_tree;


const std::string START_PATH_INSTRUCTION = "Starting navigation: use the left/right arrow keys";

// Path to draw on screen - vector of street segments
int path_instruction_index = -1; // -1 for "starting navigation" instruction
IntersectionIndex path_start_int = 0;
IntersectionIndex path_end_int = 0;
std::vector<StreetSegmentIndex> path_to_draw_street_segments;
std::vector<IntersectionIndex> path_to_draw_intersections;
std::vector<path_intersection_pair_data> path_intersection_pairs;
std::vector<std::string> path_directions;
//std::vector<unsigned> path_to_draw;


// For text input
constexpr char ENTER_KEY = static_cast<char> (13);
constexpr char BACKSPACE_KEY = static_cast<char> (8);
constexpr char SPACE_KEY = ' ';
std::string typed_text = "";

// Help menu
int help_instruction_index = 0;

// Different "states" of what the user is doing
enum ScreenState {
    NONE,

    HELP,

    // Following path directions
    PATH,

    // Finding a path between two intersections
    PATH_INT_INT1_STREET1,
    PATH_INT_INT1_STREET2,
    PATH_INT_INT2_STREET1,
    PATH_INT_INT2_STREET2,

    PATH_INT_MARKER_INVALID,

    // Finding a path between an intersection and a POI
    PATH_POI_INT_STREET1,
    PATH_POI_INT_STREET2,
    PATH_POI_POI,


    INVALID_STREET,
    INVALID_INT,
    INVALID_POI,
    INVALID_PATH
};

ScreenState screen_state = NONE;


std::vector<unsigned> path_int_int1_results;
std::vector<unsigned> path_int_int2_results;
std::string path_int_street_name1;
std::string path_int_street_name2;

std::vector<IntersectionIndex> path_poi_int_results;
std::string path_poi_street_name1;
std::string path_poi_street_name2;
std::string path_poi_poi_name;



/*
Note from X11/keysymdef.h:
#define XK_Left                          0xff51   Move left, left arrow 
#define XK_Up                            0xff52   Move up, up arrow 
#define XK_Right                         0xff53   Move right, right arrow 
#define XK_Down                          0xff54   Move down, down arrow 
 */

// For help mode
std::vector<std::string> help_instructions = {
    "Press the left and right arrow keys to switch between instructions.",
    
    "Scroll the mouse wheel to zoom in and out.",
    "Hold the mouse wheel down and move the mouse to pan.",
    "You can also use the navigation controls on the right.",
    "Click on an intersection to highlight it.",
    
    "Use the Clear button to clear map overlays.",
    "Use the Find button to find an intersection.",
    "Use the Int Path button to find a path between typed intersections.",
    "Use the Int Mk Path button to find a path between highlighted intersections.",
    "Use the POI Path button to find a path between a typed intersection and POI.",
};



//*********************** comparison functions ******************************************

/*
 * For one street segment data to be "less" than another,
 * it must be located farther to the left,
 * or father down in the case that the x coordinate is the same.
 */
bool operator<(street_segment_data left, street_segment_data right) {
    if (left.middle_x == right.middle_x) {
        if (left.middle_y <= right.middle_y) {
            return true;
        } else {
            return false;
        }
    } else if (left.middle_x < right.middle_x) {
        return true;
    } else {
        return false;
    }
}

//for sorting features based on layer and area so that they get drawn in the right order

bool compareFeature(feature_data lhs, feature_data rhs) {
    if (lhs.layer != rhs. layer)
        return lhs.layer < rhs. layer;
    else
        return lhs.area > rhs.area;
}


//*********************** Info collection functions ******************************************

//collects all street segment info into a global vector

void collect_street_segments() {
    unsigned numStreetSegments = getNumberOfStreetSegments();

    for (unsigned ss_id = 0; ss_id < numStreetSegments; ss_id++) {
        collect_curve_points_for_street_segment(ss_id);
    }
}

//collects all curve point info for a given street segment into a global vector

void collect_curve_points_for_street_segment(unsigned ss_id) {
    auto info = getStreetSegmentInfo(ss_id);

    //create a new vector to hold curve points
    std::vector<LatLon>* street_segment = new std::vector<LatLon>;
    unsigned numCurvePoints = info.curvePointCount;
    street_segment->resize(numCurvePoints + 2); //to hold curve points and 2 end points

    (*street_segment)[0] = getIntersectionPosition(info.from);
    unsigned cp_id;
    for (cp_id = 1; cp_id <= numCurvePoints; cp_id++) {
        (*street_segment)[cp_id] = getStreetSegmentCurvePoint(ss_id, cp_id - 1);
    }
    (*street_segment)[cp_id] = getIntersectionPosition(info.to);

    std::pair<unsigned, std::vector < LatLon >> pair(ss_id, *street_segment);
    delete street_segment;

    //categorize based on speed limit
    StreetWidth width = getStreetWidth(info);

    switch (width) {
        case Major:
            street_segments_highway.push_back(pair);
            break;
        case Medium:
            street_segments_major.push_back(pair);
            break;
        case Minor:
            street_segments_minor.push_back(pair);
            break;
        default:
            street_segments_minor.push_back(pair);
    }
}


//collect info for all features and store in a global vector

void collect_features() {
    unsigned numFeatures = getNumberOfFeatures();
    feature_curve_points.resize(numFeatures);

    for (unsigned f_id = 0; f_id < numFeatures; f_id++) {
        collect_curve_points_for_features(f_id);
    }

    //sort by layer and area
    std::sort(feature_curve_points.begin(), feature_curve_points.end(), compareFeature);
}

//collect locations of all curve points that make up the feature into a vector

void collect_curve_points_for_features(unsigned f_id) {

    unsigned numCurvePoints = getFeaturePointCount(f_id);
    t_point* point_array = new t_point[numCurvePoints];

    for (unsigned cp_id = 0; cp_id < numCurvePoints; cp_id++) {
        LatLon cp_location = getFeaturePoint(f_id, cp_id);
        point_array[cp_id] = convert_latlon_to_t_point(cp_location);
    }

    feature_curve_points[f_id].id = f_id;
    feature_curve_points[f_id].closed = (same_point(point_array[0], point_array[numCurvePoints - 1]));
    feature_curve_points[f_id].layer = get_map_layer(getFeatureType(f_id));
    feature_curve_points[f_id].area = feature_curve_points[f_id].closed ? calculate_polygon_area(point_array, numCurvePoints) : 0;
    feature_curve_points[f_id].curve_points = point_array;
}

void free_features() {
    for (auto it = feature_curve_points.begin(); it != feature_curve_points.end(); it++)
        delete [] it->curve_points;
}

void collect_POIs() {
    unsigned num_pois = getNumberOfPointsOfInterest();
    points_of_interest.resize(num_pois);

    for (POIIndex i = 0; i < num_pois; ++i) {
        LatLon position = getPointOfInterestPosition(i);

        points_of_interest[i].name = getPointOfInterestName(i);
        points_of_interest[i].type = get_POI_type(getPointOfInterestType(i));
        points_of_interest[i].position_x = convert_latlon_to_x(position);
        points_of_interest[i].position_y = convert_latlon_to_y(position);
    
    
    
    
        poi_data data = points_of_interest[i];

        //put x,y into a indexable Point
        Cartesian_point point(data.position_x, data.position_y, 0);

        //Query R-tree with a vector to store the search results
        std::vector<POIPair> results;
        display_poi_tree.query(bgi::nearest(point, 1), std::back_inserter(results));

        bool should_draw = false;
        if (results.size() > 0) {
            poi_data closest_poi = results[0].second;
            if (distance_between_points(point.get<0>(), point.get<1>(), closest_poi.position_x, closest_poi.position_y) >= min_POI_distance) {
                should_draw = true;
            }
        } else {
            should_draw = true;
        }

        if (should_draw) {
            POIPair poi = std::make_pair(point, data);
            display_poi_tree.insert(poi);
        }
    }
}

void collect_street_names() {

    int num_streets = getNumberOfStreets();
    streets_to_street_segments.resize(num_streets);

    // Loop through streets
    for (int i = 0; i < num_streets; ++i) {
        // Set name
        streets_to_street_segments[i].first = getStreetName(i);

        // Loop through segments
        std::vector<StreetSegmentIndex> segments = find_street_street_segments(i);
        for (auto segment : segments) {
            StreetSegmentInfo info = getStreetSegmentInfo(segment);
            street_segment_data data;

            data.index = segment;

            data.name = getStreetName(i);

            // Calculate the average coordinates of the two endpoint intersections
            LatLon from_position = getIntersectionPosition(info.from);
            LatLon to_position = getIntersectionPosition(info.to);

            double from_x = convert_latlon_to_x(from_position);
            double from_y = convert_latlon_to_y(from_position);
            double to_x = convert_latlon_to_x(to_position);
            double to_y = convert_latlon_to_y(to_position);

            data.middle_x = (from_x + to_x) / 2.0;
            data.middle_y = (from_y + to_y) / 2.0;

            // Calculate the street path angle based on the two endpoint intersections
            data.path_angle = atan2(to_y - from_y, to_x - from_x) / DEG_TO_RAD;

            // Fit the text angle between -90 and 90 degrees
            data.text_angle = data.path_angle;
            while (data.text_angle < MIN_TEXT_ANGLE) {
                data.text_angle += TEXT_FLIP_ANGLE;
            }
            while (data.text_angle > MAX_TEXT_ANGLE) {
                data.text_angle -= TEXT_FLIP_ANGLE;
            }

            // Check one way properties
            if (info.oneWay) {
                data.is_one_way = true;
                if (from_x < to_x) {
                    data.is_one_way_left = false;
                } else {
                    data.is_one_way_left = true;
                }
            } else {
                data.is_one_way = false;
                data.is_one_way_left = false;
            }

            data.width = getStreetWidth(info);

            streets_to_street_segments[i].second.insert(data);
        }
    }
}

void collect_display_segments_tree() {
    // Loop through all streets
    for (size_t i = 0; i < streets_to_street_segments.size(); ++i) {

        // Extract name and segments
        std::string name = streets_to_street_segments[i].first;
        std::set<street_segment_data> segments = streets_to_street_segments[i].second;

        // If the name is known and there are some segments
        if (name != UNKNOWN_STREET_NAME && segments.size() > 0) {
            for (auto segment = segments.begin(); segment != segments.end(); ++segment) {

                //put x,y into a indexable Point
                Cartesian_point point(segment->middle_x, segment->middle_y, 0);

                //Query R-tree with a vector to store the search results
                std::vector<StreetSegmentPair> results;
                display_segments_tree.query(bgi::nearest(point, 1), std::back_inserter(results));

                bool should_draw = false;
                if (results.size() > 0) {
                    street_segment_data closest_segment = results[0].second;

                    double limit;
                    if (segment->name == closest_segment.name) {
                        limit = MIN_TEXT_SAME_STREET_DISTANCE;
                    } else {
                        limit = MIN_TEXT_STREET_DISTANCE;
                    }

                    if (distance_between_points(point.get<0>(), point.get<1>(), closest_segment.middle_x, closest_segment.middle_y) >= limit) {
                        should_draw = true;
                    }
                } else {
                    should_draw = true;
                }

                if (should_draw) {
                    StreetSegmentPair segment_pair = std::make_pair(point, *segment);
                    display_segments_tree.insert(segment_pair);
                }
            }
        }
    }
}

//*********************** Drawing functions ******************************************

// Draws street segment names in the graphical window
// Also draw arrows for one-way streets

void draw_street_segment_names() {

    t_color DARK_GREY(115, 115, 115, 255);
    setcolor(DARK_GREY);

    setfontsize(STREET_SEGMENT_FONT_SIZE);
    settextrotation(0);

    // Loop through and display all POIs in the R-tree
    for (auto segment = display_segments_tree.begin(); segment != display_segments_tree.end(); ++segment) {
        street_segment_data data = segment->second;
        settextrotation(data.text_angle);
        drawtext(
                data.middle_x, data.middle_y,
                get_street_segment_text(data.name, data.is_one_way, data.is_one_way_left),
                get_street_bounding_box(data.width), get_street_bounding_box(data.width));
    }

    settextrotation(0);
}

void draw_poi_names() {
//    auto const start = std::chrono::high_resolution_clock::now();
    
    
    setfontsize(POI_FONT_SIZE);
    settextrotation(0);
    
//    t_color DARK_GREY(115, 115, 115, 255);

    t_bound_box visible = get_visible_world();

    // Loop through and display all POIs in the R-tree
    for (auto it = display_poi_tree.begin(); it != display_poi_tree.end(); ++it) {
        poi_data data = it->second;

        if (getZoomLevel() < zoom_level_threshold_POI_markers &&
                visible.left() <= data.position_x && data.position_x <= visible.right() &&
                visible.bottom() <= data.position_y && data.position_y <= visible.top()) {
        
//        if (getZoomLevel() < zoom_level_threshold_POI_markers) {

            select_POI_text_color(data.type);
//            setcolor(DARK_GREY);
            drawtext(data.position_x, data.position_y - POI_TEXT_OFFSET * getZoomLevel(), data.name, POI_TEXT_BOUNDING_BOX, POI_TEXT_BOUNDING_BOX);
            draw_POI_marker(t_point(data.position_x, data.position_y), data.type);
        }
    }
    
    
//    auto const end = std::chrono::high_resolution_clock::now();
//    auto const delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
//    std::cout << "draw_poi_name(): " << delta_time.count() << "ns" << std::endl;
}

void draw_streets() {
    //draw different streets based on zoom level
    double zoomLevel = getZoomLevel();
       
    if (zoomLevel > zoom_level_threshold_streets_1)
        draw_streets_of_width(Major);
    else if (zoomLevel > zoom_level_threshold_streets_2) {
        draw_streets_of_width(Medium);
        draw_streets_of_width(Major);
    } else {
        draw_streets_of_width(Minor);
        draw_streets_of_width(Medium);
        draw_streets_of_width(Major);
    }
}

void draw_streets_of_width(StreetWidth type) {
    std::vector<std::pair<unsigned, std::vector < LatLon>>>* street_data_ptr;
    int line_width;

    t_color LIGHT_YELLOW(255, 204, 0, 255);

    switch (type) {
        case Major:
            street_data_ptr = &street_segments_highway;
            line_width = std::max(BIG_STREET_WIDTH / getZoomLevel(), MIN_LINE_WIDTH);
            setcolor(LIGHT_YELLOW);
            break;
        case Medium:
            street_data_ptr = &street_segments_major;
            line_width = std::max(MEDIUM_STREET_WIDTH / getZoomLevel(), MIN_LINE_WIDTH);
            setcolor(WHITE);
            break;
        case Minor:
            street_data_ptr = &street_segments_minor;
            line_width = std::max(SMALL_STREET_WIDTH / getZoomLevel(), MIN_LINE_WIDTH);
            setcolor(WHITE);
            break;
        default:
            street_data_ptr = &street_segments_highway;
            line_width = std::max(BIG_STREET_WIDTH / getZoomLevel(), MIN_LINE_WIDTH);
            setcolor(LIGHT_YELLOW);
    }

    for (auto street_seg = (*street_data_ptr).begin(); street_seg != (*street_data_ptr).end(); street_seg++) {

        for (auto curve_pt = ((*street_seg).second).begin(); curve_pt != (((*street_seg).second).end() - 1); curve_pt++) {

            double x1, y1, x2, y2;
            x1 = convert_latlon_to_x(*curve_pt);
            y1 = convert_latlon_to_y(*curve_pt);
            x2 = convert_latlon_to_x(*(curve_pt + 1));
            y2 = convert_latlon_to_y(*(curve_pt + 1));

            setlinewidth(line_width);
            drawline(x1, y1, x2, y2);

        }
    }
}

void draw_features() {
    unsigned numFeatures = getNumberOfFeatures();

    for (unsigned unordered_id = 0; unordered_id < numFeatures; unordered_id++) {
        //don't display if feature is too small
        if (feature_curve_points[unordered_id].area / get_screen_area() < FEATURE_DISPLAY_ZOOM_THRESHOLD)
            continue;

        unsigned id = feature_curve_points[unordered_id].id;
        unsigned numCurvePoints = getFeaturePointCount(id);

        select_feature_color(getFeatureType(id));

        //closed feature, use polygon
        if (feature_curve_points[unordered_id].closed) {
            fillpoly(feature_curve_points[unordered_id].curve_points, numCurvePoints);
        }//open feature, use drawline
        else {
            setlinewidth(OPEN_FEATURE_WIDTH / getZoomLevel());
            for (unsigned cp_id = 0; cp_id < numCurvePoints - 1; cp_id++) {
                drawline((feature_curve_points[unordered_id].curve_points)[cp_id], (feature_curve_points[unordered_id].curve_points)[cp_id + 1]);
            }
        }
    }
}

void draw_screen() {
    //Enable double buffer
    
    set_drawing_buffer(OFF_SCREEN);
    clearscreen();
    draw_features();
    draw_streets();
    draw_street_segment_names();
    draw_poi_names();
    draw_path();
//    draw_text_input_box();
//    draw_text_display_box();

    copy_off_screen_buffer_to_screen();

    //Disable double buffering for highlights
    set_drawing_buffer(ON_SCREEN);
    draw_highlights();
}

//**********************draw marker functions ************************************

void draw_marker(t_point location) {
    Surface marker = load_png_from_file("libstreetmap/resources/marker.png");

    //image size when zoom level is 100%, offset the image so that the tip of the marker points to the point of interest
    t_point shift(-1000, 3000);
    draw_surface(marker, location + (shift * getZoomLevel()));
}

void draw_POI_marker(t_point location, POI_type type) {
    std::string file_path;

    //icon image references:
    //fast food, generic, hospital: https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRkXA9x-ocSWt9Yd9NbppSnoye7ZUe-LiM4_b9-e8tKb84tDwl8gQ
    //library: https://image.freepik.com/free-icon/pins-maps-library_318-27199.jpg
    //fuel: https://cdn3.iconfinder.com/data/icons/oil-gas/100/08-512.png
    //school: https://i.pinimg.com/236x/01/29/38/012938b2c763c53e0e129d9306615432--school-icon-free-icon.jpg
    //cafe: https://cdn4.iconfinder.com/data/icons/maps-and-navigation-solid-icons-vol-1/72/46-512.png
    //bank: https://www.shareicon.net/data/128x128/2016/01/26/709382_bank_512x512.png

    switch (type) {
        case fast_food: file_path = "libstreetmap/resources/food.png";
            break;
        case bank: file_path = "libstreetmap/resources/bank.png";
            break;
        case cafe: file_path = "libstreetmap/resources/food.png";
            break;
        case fuel: file_path = "libstreetmap/resources/fuel.png";
            break;
        case school: file_path = "libstreetmap/resources/school.png";
            break;
        case hospital: file_path = "libstreetmap/resources/hospital.png";
            break;
        case library: file_path = "libstreetmap/resources/library.png";
            break;
        default: file_path = "libstreetmap/resources/generic.png";
    }

    Surface icon = load_png_from_file(file_path.c_str());
    //image size when zoom level is 100%, offset the image so that the tip of the marker points to the point of interest
    t_point shift(-464, 1417);
    draw_surface(icon, location + (shift * getZoomLevel()));
}

//*******************  Highlight functions  **********************************************

void display_intersection_info(t_point highlight_pos, IntersectionIndex intersection_id) {

    std::string intersection_name = getIntersectionName(intersection_id);
    t_point highlight_point = world_to_scrn(highlight_pos);

    set_coordinate_system(GL_SCREEN);

    double text_size = intersection_name.size() * text_size_factor;

    setcolor(BLACK);
    fillrect(
            highlight_point.x - border_width / 2 - text_size / 2,
            highlight_point.y + border_bottom_y,
            highlight_point.x + border_width / 2 + text_size / 2,
            highlight_point.y + border_top_y
            );


    //Create text box
    setcolor(WHITE);
    fillrect(
            highlight_point.x - text_box_width / 2 - text_size / 2,
            highlight_point.y + text_box_bottom_y,
            highlight_point.x + text_box_width / 2 + text_size / 2,
            highlight_point.y + text_box_top_y
            );


    setcolor(BLACK);

    setfontsize(HIGHLIGHT_FONT_SIZE);
    drawtext(
            highlight_point.x,
            highlight_point.y + text_box_mid_height,
            intersection_name,
            FLT_MAX,
            FLT_MAX
            );

    set_coordinate_system(GL_WORLD);
}

void draw_highlights() {

    for (unsigned i = 0; i < list_of_highlights.size(); ++i) {
        t_point point_to_highlight = list_of_highlights[i]->get_highlight_pos();
        IntersectionIndex intersection_id = list_of_highlights[i]->get_intersection_id();

        std::cout<< intersection_id << std::endl; //DEBUGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
        
        draw_marker(point_to_highlight);
        display_intersection_info(point_to_highlight, intersection_id);
    }
}

bool is_mouse_on_highlight(t_point mouse_pos, t_point highlight_pos) {

    //converting mouse and highlight coordinates from world to screen coordinates 
    t_point mouse_point = world_to_scrn(mouse_pos);
    t_point highlight_point = world_to_scrn(highlight_pos);

    set_coordinate_system(GL_SCREEN);

    //Checks if mouse is over icon
    if (mouse_point.x < highlight_point.x + icon_width / 2
            && mouse_point.x > highlight_point.x - icon_width / 2
            && mouse_point.y < highlight_point.y
            && mouse_point.y > highlight_point.y - icon_height) {

        //setting coordinate system back to world
        set_coordinate_system(GL_WORLD);
        return true;
    }
    set_coordinate_system(GL_WORLD);
    return false;
}

bool can_highlight_intersection(t_point point_to_highlight) {

    for (unsigned i = 0; i < list_of_highlights.size(); ++i) {
        t_point highlight_pos = list_of_highlights[i]->get_highlight_pos();

        if (same_point(highlight_pos, point_to_highlight)) {
            return false;
        }
    }
    return true;
}

void insert_highlight(t_point point_to_highlight, IntersectionIndex intersection_id) {
//    std::cout << intersection_id << std::endl;

    Highlight* highlight = new Highlight("highlight", point_to_highlight, intersection_id);
    list_of_highlights.push_back(highlight);

    draw_highlights();
}

void remove_highlight(double mouse_x, double mouse_y) {
    t_point mouse_cartesian(mouse_x, mouse_y);
    t_point highlight_pos;

    for (unsigned i = 0; i < list_of_highlights.size(); ++i) {

        highlight_pos = list_of_highlights[i] -> get_highlight_pos();

        //Free highlight objects 
        if (is_mouse_on_highlight(mouse_cartesian, highlight_pos)) {
            delete list_of_highlights[i];
            list_of_highlights.erase(list_of_highlights.begin() + i);
            draw_screen();
            break;
        }
    }
}

void remove_all_highlights() {

    if (!list_of_highlights.empty()) {
        for (unsigned i = 0; i < list_of_highlights.size(); ++i) {
            delete list_of_highlights[i];
        }
        list_of_highlights.clear();
    }
    draw_screen();
}

void highlight_intersection(IntersectionIndex intersection_id) {

    LatLon intersection_pos = getIntersectionPosition(intersection_id);

    t_point point_to_highlight = convert_latlon_to_t_point(intersection_pos);

    insert_highlight(point_to_highlight, intersection_id);

}

void act_on_button_press(float mouse_x, float mouse_y, t_event_buttonPressed event) {

    LatLon mouse_latlon = convert_cartesian_to_latlon(mouse_x, mouse_y);

    //closest intersection to mouse 
    IntersectionIndex intersection_id_near_mouse = find_closest_intersection(mouse_latlon);
    std::string intersection_name = getIntersectionName(intersection_id_near_mouse);

    LatLon intersection_latlon = getIntersectionPosition(intersection_id_near_mouse);

    double distance_between_intersection_and_mouse = find_distance_between_two_points(mouse_latlon, intersection_latlon);

    //Left click; add highlight
    if (event.button == 1 &&
            !event.ctrl_pressed &&
            !event.shift_pressed &&
            distance_between_intersection_and_mouse < min_distance) {

        t_point point_to_highlight = convert_latlon_to_t_point(intersection_latlon);

        if (can_highlight_intersection(point_to_highlight)) {
            insert_highlight(point_to_highlight, intersection_id_near_mouse);
        }

    }//Right click; remove highlight
    else if (event.button == 3 &&
            !event.ctrl_pressed &&
            !event.shift_pressed &&
            !list_of_highlights.empty()) {

        remove_highlight(mouse_x, mouse_y);
    }
}

void set_max_coordinates() {

    double maximum_latitude = getIntersectionPosition(0).lat();
    double minimum_latitude = maximum_latitude;
    double maximum_longitude = getIntersectionPosition(0).lon();
    double minimum_longitude = maximum_longitude;

    intersections.resize(getNumberOfIntersections());
    for (unsigned id = 0; id < getNumberOfIntersections(); ++id) {
        intersections[id].position = getIntersectionPosition(id);
        intersections[id].name = getIntersectionName(id);

        maximum_latitude = std::max(maximum_latitude, intersections[id].position.lat());
        minimum_latitude = std::min(minimum_latitude, intersections[id].position.lat());

        maximum_longitude = std::max(maximum_longitude, intersections[id].position.lon());
        minimum_longitude = std::min(minimum_longitude, intersections[id].position.lon());
    }

    avg_latitude = (maximum_latitude + minimum_latitude) / 2;

    double max_x = convert_latlon_to_x(avg_latitude, maximum_longitude);
    double max_y = convert_latlon_to_y(maximum_latitude, maximum_longitude);

    double min_x = convert_latlon_to_x(avg_latitude, minimum_longitude);
    double min_y = convert_latlon_to_y(minimum_latitude, minimum_longitude);

    //Use the difference between max and min coordinates as a zoom offset
    double diff_x = max_x - min_x;
    double diff_y = max_y - min_y;

    set_visible_world(
            min_x + diff_x*zoom_factor,
            min_y + diff_y*zoom_factor,
            max_x - diff_x*zoom_factor,
            max_y - diff_y * zoom_factor
            );

    world_width = max_x - min_x;
}

void draw_map() {

    
    set_max_coordinates();



    set_up_poi_string_to_ids();
    
    // Create a window with name and background colour as specified
    t_color light_grey(230, 230, 230, 255);
    init_graphics(PROGRAM_NAME + " - " + name_of_map, light_grey);


    // This message will show up at the bottom of the window.
    update_message("Team 76");
    
    create_button("Window", "Help", act_on_help_button);
    create_button("Help", "Clear", act_on_clear_button);
    create_button("Clear", "Find", act_on_find_button);
//    create_button("Help", "Path", act_on_path_button);
    create_button("Find", "Int Path", act_on_path_int_button);
    create_button("Int Path", "Int Mk Path", act_on_path_int_marker_button);
    create_button("Int Mk Path", "POI Path", act_on_path_poi_button);

    set_keypress_input(true);
    
    //collects information of all objects to be graphed
    collect_street_segments();;
    collect_features();
    collect_POIs();
    collect_street_names();
    collect_display_segments_tree();
//    set_up_graph();
    
//    IntersectionIndex start = 5957;
//    IntersectionIndex end = 16574;
    
//    IntersectionIndex start = 1108;
//    IntersectionIndex end = 8161;
    
//    IntersectionIndex start = 4264;
//    IntersectionIndex end = 72259;
    
//    std::vector<StreetSegmentIndex> path = find_path_between_intersections(start, end, 5);
//    update_path_to_draw(start, end, path);
    
    // Pass control to the window handling routine. It will watch for user input
    event_loop(act_on_button_press, NULL, on_key_press, draw_screen);

    //Clear all highlights
    if (!list_of_highlights.empty()) {
        for (unsigned i = 0; i < list_of_highlights.size(); ++i) {
            delete list_of_highlights[i];
        }
        list_of_highlights.clear();
    }
    
    
    close_graphics();
}

// **************************** Button Functions *********************************

void act_on_find_button(void (*drawscreen) (void)) {

    collect_intersections();
    int intersection_id = autocomplete();

    //if not found, show error message or suggestions
    if (intersection_id == -1)
        std::cerr << "Intersection not found." << std::endl;

        //if found, highlight intersection
    else {
        highlight_intersection(intersection_id);

        //auto zoom to where the first highlight is
        LatLon intersection_latlon = getIntersectionPosition(intersection_id);
        t_point intersection_point = convert_latlon_to_t_point(intersection_latlon);

        set_visible_world(intersection_point.x - AUTO_ZOOM_SIZE, intersection_point.y - AUTO_ZOOM_SIZE, intersection_point.x + AUTO_ZOOM_SIZE, intersection_point.y + AUTO_ZOOM_SIZE);

        std::cout << "Intersection found and highlighted on the map" << std::endl;
    }
    
}

void act_on_clear_button(void (*drawscreen) (void)) {
    screen_state = NONE;

    remove_all_highlights();
    help_instruction_index = 0;
    typed_text = "";
    
    draw_screen();
}


//************************ Other helper functions ***************************************

// Cartesian distance between two points

double distance_between_points(double x1, double y1, double x2, double y2) {
    return sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
}

std::string get_street_segment_text(std::string name, bool is_one_way, bool is_one_way_left) {
    if (is_one_way) {
        if (is_one_way_left) {
            return LEFT_ARROW + name;
        } else {
            return name + RIGHT_ARROW;
        }
    } else {
        return name;
    }
}

double get_street_bounding_box(StreetWidth street_width) {
    switch (street_width) {
        case Minor:
            return minor_street_text_bounding_box;
            break;
        case Medium:
            return medium_street_text_bounding_box;
            break;
        case Major:
        default:
            return major_street_text_bounding_box;
    }
}

POI_type get_POI_type(std::string POI_string) {
    if (POI_type_strings.find(POI_string) != POI_type_strings.end()) {
        return POI_type_strings[POI_string];
    } else {
        return other;
    }
}

void update_path_to_draw(IntersectionIndex start_int, IntersectionIndex end_int, std::vector<StreetSegmentIndex> path) {
    screen_state = NONE;

    path_start_int = start_int;
    path_end_int = end_int;

    path_to_draw_street_segments = path;

    if (path.size() == 0) {
        screen_state = NONE;
        path_instruction_index = -1;
        path_to_draw_intersections.clear();
        path_intersection_pairs.clear();
    } else {
        screen_state = PATH;
        path_instruction_index = -1;
        path_to_draw_intersections.resize(path.size() + 1);

        path_to_draw_intersections[0] = start_int;

        for (size_t i = 0; i < path.size() - 1; ++i) {
            auto info_cur = getStreetSegmentInfo(path[i]);
            auto info_next = getStreetSegmentInfo(path[i + 1]);

            if (info_cur.from == info_next.from || info_cur.from == info_next.to) {
                path_to_draw_intersections[i + 1] = info_cur.from;
            } else if (info_cur.to == info_next.from || info_cur.to == info_next.to) {
                path_to_draw_intersections[i + 1] = info_cur.to;
            }
        }

        path_to_draw_intersections[path_to_draw_intersections.size() - 1] = end_int;

        std::cout << "Bruno's path to intersection: " << std::endl;
        path_intersection_pairs.resize(path_to_draw_intersections.size() - 1);
        for (size_t i = 0; i < path_to_draw_intersections.size() - 1; ++i) {
            path_intersection_pairs[i].street_name = getStreetName(getStreetSegmentInfo(path[i]).streetID);

            IntersectionIndex int1 = path_to_draw_intersections[i];
            IntersectionIndex int2 = path_to_draw_intersections[i + 1];

            LatLon pos1 = getIntersectionPosition(int1);
            LatLon pos2 = getIntersectionPosition(int2);

            
            double x1, y1, x2, y2;
            x1 = convert_latlon_to_x(pos1);
            y1 = convert_latlon_to_y(pos1);
            x2 = convert_latlon_to_x(pos2);
            y2 = convert_latlon_to_y(pos2);

            path_intersection_pairs[i].x1 = x1;
            path_intersection_pairs[i].y1 = y1;
            path_intersection_pairs[i].x2 = x2;
            path_intersection_pairs[i].y2 = y2;
            
            path_intersection_pairs[i].set_up_curve_points(path[i]);

            path_intersection_pairs[i].length = find_street_segment_length(path_to_draw_street_segments[i]);

            path_intersection_pairs[i].middle_x = (x1 + x2) / 2.0;
            path_intersection_pairs[i].middle_y = (y1 + y2) / 2.0;

            // Calculate the street path angle based on the two endpoint intersections
            path_intersection_pairs[i].angle = atan2(y2 - y1, x2 - x1) / DEG_TO_RAD;
        }

        path_directions = get_path_directions();

        std::cout << std::endl << "Path:" << std::endl;
        for (size_t i = 0; i < path_directions.size(); ++i) {
            std::cout << (i + 1) << ". " << path_directions[i] << std::endl;
        }
        std::cout << std::endl;
    }

    draw_screen();
}

void find_int_path(IntersectionIndex start_int, IntersectionIndex end_int) {
    std::cout << "Finding path between " <<
            getIntersectionName(start_int) << " and " <<
            getIntersectionName(end_int) << std::endl;

    std::vector<StreetSegmentIndex> path = find_path_between_intersections(start_int, end_int, DEFAULT_TURN_PENALTY);
    update_path_to_draw(start_int, end_int, path);
}

void find_poi_path(IntersectionIndex start_int, std::string poi_name) {
    std::cout << "Finding path between " <<
            getIntersectionName(start_int) << " and " <<
            poi_name << std::endl;

    std::vector<StreetSegmentIndex> path = find_path_to_point_of_interest(start_int, poi_name, DEFAULT_TURN_PENALTY);
    IntersectionIndex end_int = 0;
    
    if (path.size() == 0) {
        
    } else if (path.size() == 1) {
        auto info = getStreetSegmentInfo(path[0]);
        end_int = (start_int == info.from) ? info.to : info.from;
    } else {
        auto info1 = getStreetSegmentInfo(path[path.size() - 1]);
        auto info2 = getStreetSegmentInfo(path[path.size() - 2]);
        end_int = (info1.from == info2.from || info1.from == info2.to) ? info1.to : info1.from;
    }
    
    update_path_to_draw(start_int, end_int, path);
}

void draw_path() {

    if (screen_state == PATH) {
        // Draw line for each segment (pair of intersections)
        setcolor(102, 179, 255, 255); //light blue
        setlinewidth(12);
        for (auto pair_data : path_intersection_pairs) {

            for (auto i = pair_data.curve_points.begin(); i != pair_data.curve_points.end() - 1; i++) {

                double x1, y1, x2, y2;
                x1 = convert_latlon_to_x(*i);
                y1 = convert_latlon_to_y(*i);
                x2 = convert_latlon_to_x(*(i + 1));
                y2 = convert_latlon_to_y(*(i + 1));

                drawline(x1, y1, x2, y2);
            }
        }

        // Draw arrow for each segment (pair of intersections)
        setcolor(WHITE);
        setfontsize(15);
        for (auto pair_data : path_intersection_pairs) {
            settextrotation(pair_data.angle);
            drawtext(pair_data.middle_x, pair_data.middle_y, RIGHT_ARROW, 100, 100);
        }


        // Draw start and end
        LatLon start = getIntersectionPosition(path_start_int);
        LatLon end = getIntersectionPosition(path_end_int);

        double start_x, start_y, end_x, end_y;
        start_x = convert_latlon_to_x(start);
        start_y = convert_latlon_to_y(start);
        end_x = convert_latlon_to_x(end);
        end_y = convert_latlon_to_y(end);

        setcolor(BLUE);
        double rad = 750 * getZoomLevel();
        fillellipticarc(start_x, start_y, rad, rad, 0, 360);
        fillellipticarc(end_x, end_y, rad, rad, 0, 360);
    }

    settextrotation(0);
    setfontsize(STREET_SEGMENT_FONT_SIZE);
}

std::string get_cardinal_direction(double degrees) {
    while (degrees < 0.0) {
        degrees += 360.0;
    }
    while (degrees >= 360.0) {
        degrees -= 360.0;
    }

    if (45.0 <= degrees && degrees <= 135.0) {
        return "north";
    } else if (135.0 <= degrees && degrees <= 225.0) {
        return "west";
    } else if (225.0 <= degrees && degrees <= 315.0) {
        return "south";
    } else {
        return "east";
    }
}


std::string get_relative_direction(double angle1, double angle2) {
    double diff = angle2 - angle1;
    while (diff < 0.0) {
        diff += 360.0;
    }
    while (diff >= 360.0) {
        diff -= 360.0;
    }

    if (0 <= diff && diff <= 180.0) {
        return "left";
    } else {
        return "right";
    }
}

std::vector<std::string> get_path_directions() {
    std::vector<std::string> directions;

    if (path_intersection_pairs.size() == 0) {
        return directions;
    }
    
    double current_street_length = 0;
    
    for (size_t i = 0; i < path_intersection_pairs.size() - 1; ++i) {
        auto data = path_intersection_pairs[i];
        auto data_next = path_intersection_pairs[i + 1];
        
        current_street_length += data.length;
        
        if (data.street_name == data_next.street_name) {
            continue;
        } else {
            std::stringstream go_stream;
            go_stream << "Go " << round(current_street_length) << "m " << get_cardinal_direction(data.angle) << " along " << data.street_name;
            directions.push_back(go_stream.str());
            
            std::stringstream turn_stream;
            turn_stream << "Turn " << get_relative_direction(data.angle, data_next.angle) << " onto " << data_next.street_name;
            directions.push_back(turn_stream.str());
            
            current_street_length = 0;
        }
    }
    
    auto data_last = path_intersection_pairs[path_intersection_pairs.size() - 1];
    current_street_length += data_last.length;
    
    std::stringstream go_stream;
    go_stream << "Go " << round(current_street_length) << "m " << get_cardinal_direction(data_last.angle) << " along " << data_last.street_name;
    directions.push_back(go_stream.str());

    directions.push_back("You have arrived at your destination");

    return directions;
}

void on_left_arrow_press() {
    switch (screen_state) {
        case HELP:
            if (0 <= help_instruction_index - 1 && help_instruction_index - 1 < static_cast<int> (help_instructions.size())) {
                help_instruction_index--;
            }
            break;

        case PATH:
            if (-1 <= path_instruction_index - 1) {
                path_instruction_index--;
            }
            break;

        default:
            break;
    }

    draw_screen();
}

void on_right_arrow_press() {
    switch (screen_state) {
        case HELP:
            if (0 <= help_instruction_index + 1 && help_instruction_index + 1 < static_cast<int> (help_instructions.size())) {
                help_instruction_index++;
            }
            break;
        case PATH:
            if (path_instruction_index + 1 < static_cast<int> (path_directions.size())) {
                path_instruction_index++;
            }
            break;

        default:
            break;
    }

    draw_screen();
}

void on_enter_key_press() {
    switch (screen_state) {
        case PATH_INT_INT1_STREET1:
            // Clear data for intersection path
            path_int_int1_results.clear();
            path_int_int2_results.clear();
            path_int_street_name1 = "";
            path_int_street_name2 = "";
            
            if (typed_text.empty()) {
                screen_state = INVALID_STREET;
            } else {
                path_int_street_name1 = typed_text;
                typed_text = "";
                screen_state = PATH_INT_INT1_STREET2;
            }

            break;


        case PATH_INT_INT1_STREET2:
            if (typed_text.empty()) {
                screen_state = INVALID_STREET;
            } else {
                path_int_street_name2 = typed_text;
                typed_text = "";

                //search for intersection
                path_int_int1_results =
                        find_intersection_ids_from_street_names(path_int_street_name1, path_int_street_name2);

                //if not found, show error message or suggestions
                if (path_int_int1_results.size() == 0) {
                    screen_state = INVALID_INT;
                } else {
                    screen_state = PATH_INT_INT2_STREET1;
                }
            }

            break;


        case PATH_INT_INT2_STREET1:
            if (typed_text.empty()) {
                screen_state = INVALID_STREET;
            } else {
                path_int_street_name1 = typed_text;
                typed_text = "";
                screen_state = PATH_INT_INT2_STREET2;
            }

            break;


        case PATH_INT_INT2_STREET2:
            if (typed_text.empty()) {
                screen_state = INVALID_STREET;
            } else {
                path_int_street_name2 = typed_text;
                typed_text = "";

                //search for intersection
                path_int_int2_results =
                        find_intersection_ids_from_street_names(path_int_street_name1, path_int_street_name2);

                //if not found, show error message or suggestions
                if (path_int_int2_results.size() == 0) {
                    screen_state = INVALID_INT;
                } else {                    
                    screen_state = NONE;

                    find_int_path(path_int_int1_results[0], path_int_int2_results[0]);
                }
            }

            break;


            
        case PATH_POI_INT_STREET1:
            // Clear POI path data
            path_poi_int_results.clear();
            path_poi_street_name1 = "";
            path_poi_street_name2 = "";
            path_poi_poi_name = "";
            
            if (typed_text.empty()) {
                screen_state = INVALID_STREET;
            } else {
                path_poi_street_name1 = typed_text;
                typed_text = "";
                screen_state = PATH_POI_INT_STREET2;
            }

            break;


        case PATH_POI_INT_STREET2:
            if (typed_text.empty()) {
                screen_state = INVALID_STREET;
            } else {
                path_poi_street_name2 = typed_text;
                typed_text = "";

                //search for intersection
                path_poi_int_results =
                        find_intersection_ids_from_street_names(path_poi_street_name1, path_poi_street_name2);

                //if not found, show error message or suggestions
                if (path_poi_int_results.size() == 0) {
                    screen_state = INVALID_INT;
                } else {
                    screen_state = PATH_POI_POI;
                }
            }

            break;


        case PATH_POI_POI:

            if (typed_text.empty()) {
                screen_state = INVALID_POI;
            } else {
                path_poi_poi_name = typed_text;
                typed_text = "";

                if (find_POIs_by_name(path_poi_poi_name).size() == 0) {
                    screen_state = INVALID_POI;
                } else {
                    screen_state = NONE;
                    find_poi_path(path_poi_int_results[0], path_poi_poi_name);
                }
            }

            break;


        default:
            break;
    }
}

void on_key_press(char key_pressed, int key_sym) {
//    std::cout << "Pressed " << key_pressed << " = " << static_cast<int> (key_pressed) << std::endl;
    
    // Valid input characters
    if ((SPACE_KEY <= key_pressed && key_pressed <= '~')) {
        typed_text += key_pressed;
    }
    
    // Backspace
    else if (key_pressed == BACKSPACE_KEY) {
        if (typed_text.length() > 0) {
            typed_text = typed_text.substr(0, typed_text.length() - 1);
        }
    }
    
    // Left
    else if (key_sym == XK_Left) {
//        std::cout << "Left\n";
        on_left_arrow_press();
    }
    
    // Right
    else if (key_sym == XK_Right) {
//        std::cout << "Right\n";
        on_right_arrow_press();
    }
    
    // Enter
    else if (key_pressed == ENTER_KEY) {
        on_enter_key_press();
    }
    
    // Other
    else {
//        std::cout << "Invalid key" << std::endl;
    }

    draw_screen();
}

void draw_text_input_box() {
    t_bound_box visible_world = get_visible_world();

    t_point bottom_left(visible_world.left(), visible_world.bottom());
    t_point top_right(
            visible_world.right(),
            visible_world.bottom() - ((visible_world.bottom() - visible_world.top()) / 10.0));
    t_point centre = (bottom_left + top_right) * 0.5;

    setcolor(WHITE);
    fillrect(bottom_left, top_right);

    setcolor(BLACK);
    setfontsize(20);
    drawtext(centre, typed_text);

    setfontsize(STREET_SEGMENT_FONT_SIZE);
}

void draw_text_display_box() {
    // At top of screen
    t_bound_box visible_world = get_visible_world();

    t_point bottom_left(
            visible_world.left(),
            visible_world.top() + ((visible_world.bottom() - visible_world.top()) / 10.0));
    t_point top_right(
            visible_world.right(),
            visible_world.top());
    t_point centre = (bottom_left + top_right) * 0.5;

    setcolor(WHITE);
    fillrect(bottom_left, top_right);

    setcolor(BLACK);
    setfontsize(20);

    std::string text = "";

    switch (screen_state) {
        case PATH_INT_INT1_STREET1:
            text = "Enter intersection 1, street 1";
            break;

        case PATH_INT_INT1_STREET2:
            text = "Enter intersection 1, street 2";
            break;

        case PATH_INT_INT2_STREET1:
            text = "Enter intersection 2, street 1";
            break;

        case PATH_INT_INT2_STREET2:
            text = "Enter intersection 2, street 2";
            break;

        case PATH_INT_MARKER_INVALID:
            text = "You must select exactly 2 intersections";
            break;

        case PATH_POI_INT_STREET1:
            text = "Enter intersection, street 1";
            break;

        case PATH_POI_INT_STREET2:
            text = "Enter intersection, street 2";
            break;

        case PATH_POI_POI:
            text = "Enter point of interest";
            break;

        case INVALID_STREET:
            text = "You must enter a street";
            break;

        case INVALID_INT:
            text = "Intersection does not exist";
            break;

        case INVALID_POI:
            text = "Point of interest does not exist";
            break;

        case INVALID_PATH:
            text = "No path found";
            break;

        case HELP:
            if (0 <= help_instruction_index && help_instruction_index < static_cast<int>(help_instructions.size())) {
                text = help_instructions[help_instruction_index];
            }
            break;

        case PATH:
            if (path_instruction_index == -1) {
                text = START_PATH_INSTRUCTION;
            } else {
                if (0 <= path_instruction_index && path_instruction_index < static_cast<int>(path_directions.size())) {
                    std::stringstream stream;
                    stream << (path_instruction_index + 1) << ". " << path_directions[path_instruction_index];
                    text = stream.str();
                } else {
                    text = START_PATH_INSTRUCTION;
                }
            }
            break;

        default:
            break;
    }

    if (text.length() > 0) {
        drawtext(centre, text);
    }
}

void act_on_help_button(void (*drawscreen) (void)) {
    screen_state = HELP;
    help_instruction_index = 0;
    draw_screen();
}

void act_on_path_int_button(void (*drawscreen) (void)) {    
    screen_state = PATH_INT_INT1_STREET1;
    draw_screen();
}

void act_on_path_int_marker_button(void (*drawscreen) (void)) {
    if (list_of_highlights.size() == 2) {
        screen_state = NONE;

        find_int_path(
                list_of_highlights[0]->get_intersection_id(),
                list_of_highlights[1]->get_intersection_id());        
    } else {
        screen_state = PATH_INT_MARKER_INVALID;
    }

    draw_screen();
}

void act_on_path_poi_button(void (*drawscreen) (void)) {
    screen_state = PATH_POI_INT_STREET1;
    draw_screen();
}
