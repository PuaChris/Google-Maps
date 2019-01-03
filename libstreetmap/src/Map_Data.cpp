#include "Map_Data.h"
#include "m1.h"

//constructor, loads all map data structures
Map_Data::Map_Data() {


    //Populate intersection street segment vector
    unsigned numberOfIntersections = getNumberOfIntersections();
    intersection_street_segments.resize(numberOfIntersections);


    for (unsigned i = 0; i < numberOfIntersections; i++) {
        //function returns number of street segments at intersection id 'i'
        unsigned numberOfStreetSegmentsAtIntersection = getIntersectionStreetSegmentCount(i);

        //inner: iterate thru all street segments at a single intersection
        for (unsigned j = 0; j < numberOfStreetSegmentsAtIntersection; j++) {
            intersection_street_segments[i].push_back(getIntersectionStreetSegment(i, j));
        }
    }

    
    //populate Points of Interests R-tree
    for (unsigned i = 0; i < getNumberOfPointsOfInterest(); ++i) {

        LatLon POI_position = getPointOfInterestPosition(i);

        //convert LatLon to x, y, z
        double x, y, z;
        x = EARTH_RADIUS_IN_METERS * cos(POI_position.lat() * DEG_TO_RAD) * cos(POI_position.lon() * DEG_TO_RAD);
        y = EARTH_RADIUS_IN_METERS * cos(POI_position.lat() * DEG_TO_RAD) * sin(POI_position.lon() * DEG_TO_RAD);
        z = EARTH_RADIUS_IN_METERS * sin(POI_position.lat() * DEG_TO_RAD);

        //put x,y into a indexable Point
        Cartesian_point point(x, y, z);

        LocationIDPair point_of_interest = std::make_pair(point, i);

        // insert POI into R-tree
        POI_tree.insert(point_of_interest);
    }

    
    //populate intersections R-tree
    for (unsigned i = 0; i < getNumberOfIntersections(); ++i) {

        LatLon intersection_position = getIntersectionPosition(i);

        //convert LatLon to x, y, z
        double x, y, z;
        x = EARTH_RADIUS_IN_METERS * cos(intersection_position.lat() * DEG_TO_RAD) * cos(intersection_position.lon() * DEG_TO_RAD);
        y = EARTH_RADIUS_IN_METERS * cos(intersection_position.lat() * DEG_TO_RAD) * sin(intersection_position.lon() * DEG_TO_RAD);
        z = EARTH_RADIUS_IN_METERS * sin(intersection_position.lat() * DEG_TO_RAD);

        //put x,y into a indexable Point
        Cartesian_point point(x, y, z);

        LocationIDPair intersection = std::make_pair(point, i);

        // insert POI into R-tree
        intersection_tree.insert(intersection);
    }


    // Populate street names to IDs global data structure
    // Loop through all streets
    for (unsigned i = 0; i < getNumberOfStreets(); ++i) {
        // Add the street ID to the vector for that street name
        street_names_to_ids[getStreetName(i)].push_back(i);
    }


    // Populate streets to street segments data structure
    streets_to_street_segments.resize(getNumberOfStreets());
    StreetSegmentInfo street_segment_info;

    for (unsigned i = 0; i < getNumberOfStreetSegments(); i++) {

        street_segment_info = getStreetSegmentInfo(i);

        streets_to_street_segments[street_segment_info.streetID].push_back(i);
    }
    //Remove duplicate intersections
    for (unsigned i = 0; i < getNumberOfStreets(); ++i) {
        std::sort(streets_to_street_segments[i].begin(), streets_to_street_segments[i].end());
        streets_to_street_segments[i].erase(
                std::unique(streets_to_street_segments[i].begin(), streets_to_street_segments[i].end()),
                streets_to_street_segments[i].end());
    }


    //Populate street to intersection vector
    unsigned num_streets = getNumberOfStreets();
    streets_to_intersections.resize(num_streets);
    //For each street
    for (unsigned street_id = 0; street_id < num_streets; ++street_id) {

        //Get street segments along the street ID
        std::vector<unsigned> street_segments = find_streets_to_street_segments(street_id);

        //For each street segment
        for (unsigned i = 0; i < street_segments.size(); i++) {

            street_segment_info = getStreetSegmentInfo(street_segments[i]);
            
            //add intersections to that are connected to street segment
            streets_to_intersections[street_id].push_back(street_segment_info.from);
            streets_to_intersections[street_id].push_back(street_segment_info.to);
        }

        //Remove duplicate intersections 
        std::sort(streets_to_intersections[street_id].begin(), streets_to_intersections[street_id].end());
        streets_to_intersections[street_id].erase(
                std::unique(streets_to_intersections[street_id].begin(), streets_to_intersections[street_id].end()),
                streets_to_intersections[street_id].end());
    }


    //Populate street to street intersections vector
    streets_to_streets_intersections.resize(num_streets);
    // For each street
    for (unsigned street_id = 0; street_id < num_streets; ++street_id) {
        // Get intersections along the street
        std::vector <unsigned> intersection_ids_from_street = find_all_street_intersections(street_id);

        // For each intersection
        for (auto intersection : intersection_ids_from_street) {
            // Get street segments connected to the intersection
            std::vector <unsigned> street_segments_for_intersection = find_intersection_street_segments(intersection);

            // For each street segment
            for (auto street_segment : street_segments_for_intersection) {
                StreetSegmentInfo street_segment_info1 = getStreetSegmentInfo(street_segment);
                unsigned other_street = street_segment_info1.streetID;

                if (other_street != street_id) {
                    streets_to_streets_intersections[street_id][other_street].insert(intersection);
                }
            }

        }
    }


    //populate street_segment_lengths_and_travel_time vector
    //Note: Distance time test took ~10 seconds
    for (unsigned i = 0; i < getNumberOfStreetSegments(); i++) {

        double length = find_street_segment_length(i);
        double speed_limit = getStreetSegmentInfo(i).speedLimit / 3.6;
        double travel_time = length / speed_limit;

        street_segment_lengths_and_travel_time.push_back(std::make_pair(length, travel_time));
    }

}

Map_Data::~Map_Data() {
}

std::vector<unsigned> Map_Data::find_street_ids_from_name(std::string street_name) {
    return street_names_to_ids[street_name];
}

std::vector<unsigned> Map_Data::find_intersection_street_segments(unsigned intersection_id) {
    return intersection_street_segments[intersection_id];
}

std::vector<unsigned> Map_Data::find_streets_to_street_segments(unsigned street_id) {
    return streets_to_street_segments[street_id];
}

std::vector<unsigned> Map_Data::find_all_street_intersections(unsigned street_id) {
    return streets_to_intersections[street_id];
}

std::set<unsigned> Map_Data::find_streets_to_streets_intersections(unsigned street_id1, unsigned street_id2) {
    return streets_to_streets_intersections[street_id1][street_id2];
}

std::pair<double, double> Map_Data::find_street_segment_lengths_and_travel_time(unsigned street_segment_id) {
    return street_segment_lengths_and_travel_time[street_segment_id];
}

unsigned Map_Data::find_closest_POI_ID(Cartesian_point current_location) {

    //vector to store the search results
    std::vector<LocationIDPair> results;

    POI_tree.query(bgi::nearest(current_location, 1), std::back_inserter(results));

    unsigned closest_POI_ID = results[0].second;

    return closest_POI_ID;
}

unsigned Map_Data::find_closest_intersection_ID(Cartesian_point current_location) {

    //vector to store the search results
    std::vector<LocationIDPair> results;

    intersection_tree.query(bgi::nearest(current_location, 1), std::back_inserter(results));

    unsigned closest_intersection_ID = results[0].second;

    return closest_intersection_ID;
}
