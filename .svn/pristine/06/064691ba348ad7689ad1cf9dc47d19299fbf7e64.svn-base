/* 
 * File:   utilities.h
 * Author: puachris
 *
 * Created on February 17, 2018, 5:28 PM
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <math.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>

#include "m1.h"
#include "graphics_types.h"
#include "StreetsDatabaseAPI.h"


namespace bg = boost::geometry;

typedef bg::model::point <double, 3, bg::cs::cartesian> Cartesian_point;

extern double world_width;


enum StreetWidth{
    Minor = 0,
    Medium,
    Major
};

typedef enum {
    fuel = 0,
    fast_food,
    bank,
    cafe,
    parking,
    school,
    hospital,
    library,
    other
} POI_type;


// Coordinate conversion
Cartesian_point convert_to_cartesian_point(LatLon position);
double convert_latlon_to_x(double avg_lat, double lon);
double convert_latlon_to_y(double lat, double lon);
double convert_latlon_to_x(LatLon position) ;
double convert_latlon_to_y(LatLon position) ;
LatLon convert_cartesian_to_latlon(double x, double y);
t_point convert_latlon_to_t_point(LatLon position);

double distance_between_two_intersections(IntersectionIndex id1, IntersectionIndex id2);

// color
void select_feature_color(FeatureType type);
void select_POI_text_color(POI_type type);

//POI helper functions
unsigned get_map_layer(FeatureType type);
double calculate_polygon_area(t_point* points, int numPoints);
double get_screen_area();

//other
bool same_point(t_point p1, t_point p2);
bool same_point(LatLon p1, LatLon p2);
StreetWidth getStreetWidth(StreetSegmentInfo info) ;
double getZoomLevel();


#endif /* UTILITIES_H */

