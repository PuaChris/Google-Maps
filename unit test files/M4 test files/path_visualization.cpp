#include <iostream>
#include <vector>
#include "m1.h"

std::vector<std::vector < LatLon >> test_street_segments;

void collect_street_segments2(std::vector<unsigned> path) {
    for (unsigned i = 0; i < path.size(); i++) {
        collect_curve_points_for_street_segment2(path[i]);
    }
}

void collect_curve_points_for_street_segment2(unsigned ss_id) {
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

    test_street_segments.push_back(*street_segment);

    delete street_segment;
}

void draw_test_path(std::vector<unsigned>& path) {
    collect_street_segments2(path);

    for (auto it = test_street_segments.begin(); it != test_street_segments.end(); it++) {

        for (auto curve_pt = (*it).begin(); curve_pt != (*it).end() - 1; curve_pt++) {

            double x1, y1, x2, y2;
            x1 = convert_latlon_to_x(*curve_pt);
            y1 = convert_latlon_to_y(*curve_pt);
            x2 = convert_latlon_to_x(*(curve_pt + 1));
            y2 = convert_latlon_to_y(*(curve_pt + 1));
            setcolor(RED);
            setlinewidth(3);
            drawline(x1, y1, x2, y2);

        }
    }
}