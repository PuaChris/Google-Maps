#ifndef MAP_DATA_H
#define MAP_DATA_H


#include <vector>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include "StreetsDatabaseAPI.h"

/******    R-TREE libraries and type definitions  *****/
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

// to store queries results

constexpr int max_elements_R_tree = 16;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point <double, 3, bg::cs::cartesian> Cartesian_point;

typedef std::pair<Cartesian_point, unsigned> LocationIDPair;

class Map_Data {
private:
    //GLOBAL DATA STRUCTURES

    // Intersections to their connected street segments
    std::vector<std::vector<unsigned>> intersection_street_segments;

    //R-tree for closest POI search
    bgi::rtree< LocationIDPair, bgi::quadratic < max_elements_R_tree >> POI_tree;

    //R-tree for closest intersection search
    bgi::rtree< LocationIDPair, bgi::quadratic < max_elements_R_tree >> intersection_tree;

    // Map of street names to IDs
    std::unordered_map<std::string, std::vector<unsigned>> street_names_to_ids;

    // Streets paired with their corresponding street segments
    std::vector<std::vector<unsigned>> streets_to_street_segments;

    // Streets to their associated intersections
    std::vector<std::vector<unsigned>> streets_to_intersections;

    // Streets mapped to intersecting streets and the associated intersections
    // street -> { (street, intersections) }
    std::vector<std::map<unsigned, std::set<unsigned>>> streets_to_streets_intersections;

    //lengths of all street segments
    std::vector<std::pair<double, double>> street_segment_lengths_and_travel_time;

public:

    Map_Data();
    ~Map_Data();
    unsigned find_closest_POI_ID(Cartesian_point current_location);
    unsigned find_closest_intersection_ID(Cartesian_point current_location);
    std::vector<unsigned> find_street_ids_from_name(std::string street_name);
    std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id);
    std::vector<unsigned> find_streets_to_street_segments(unsigned street_id);
    std::vector<unsigned> find_all_street_intersections(unsigned street_id);
    std::set<unsigned> find_streets_to_streets_intersections(unsigned street_id1, unsigned street_id2);
    std::pair<double, double> find_street_segment_lengths_and_travel_time(unsigned street_id);
};


#endif /* MAP_DATA_STRUCTURES_H */

