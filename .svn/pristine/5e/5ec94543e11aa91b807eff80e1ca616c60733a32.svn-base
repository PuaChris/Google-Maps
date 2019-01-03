#include <iostream>
#include <vector>
#include "m1.h"
#include "test_path.h"
#include "m3.h"
#include "m2.h"
#include "easygl/easygl_constants.h"
#include "utilities.h"
#include "graphics.h"


std::vector<unsigned> an_easy_path_inter = {1450,33029,2290,3394};
std::vector<unsigned> an_medium_path_inter = {1567,8252,1094,82551,5183,354};
std::vector<unsigned> a_strategic_path = {12566,1530,21319,5783,295,285};
std::vector<unsigned> a_good_path = {60219,1218,16939,71418,69698,19335};



std::vector<unsigned> convert_delivery_points_to_inter(std::vector<DeliveryPoint>& path_dp){
    
    std::vector<unsigned> path_inter;
    path_inter.resize(path_dp.size());
    
    for (unsigned i = 0; i < path_dp.size(); i++){
        path_inter[i] = path_dp[i].intersectionID;
    }
    
    return path_inter;
}

std::vector<unsigned> convert_inter_to_street_seg(std::vector<unsigned> intersections){
    std::vector<unsigned> street_segments;
    
//    for (auto it = intersections.begin(); it != (intersections.end()-2); it++){
//        
//        std::vector<unsigned> sub_path = find_path_between_intersections(*it,*(it+1),0);
    for(int i = 0; i < static_cast<int>(intersections.size()) - 1 ; i++){
        std::vector<unsigned> sub_path = find_path_between_intersections(intersections[i],intersections[i+1],0); 
        street_segments.insert( street_segments.end(), sub_path.begin(), sub_path.end() );
    }
    
    return street_segments;
}

std::vector<std::vector < LatLon >> test_street_segments;

void collect_street_segments2(std::vector<unsigned> path) {
    test_street_segments.clear();
    
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

void test(){
//    std::vector<unsigned> path_street_seg = convert_inter_to_street_seg(a_good_path);
//    draw_test_path(path_street_seg); //draw original path
//    
    //12566,1530,21319,5783,295,285
    //69698,60219,16939,18183,69698,19335
    
    std::vector<DeliveryPoint> deliveries;
    
//    for(auto it = a_strategic_path.begin(); it != a_strategic_path.end(); it++){
//        highlight_intersection(69698);
//    }
    
    deliveries.resize(6);
    
    DeliveryPoint depot1(-1,true,60219);
    DeliveryPoint p1(1,true,1218);
    DeliveryPoint d1(1,false,16939); 
    DeliveryPoint p2(2,true,71418); //18183
    DeliveryPoint d2(2,false,69698); 
    DeliveryPoint depot2(-1,true,19335);
    
    deliveries = {depot1,p1,d1,p2,d2,depot2};
    
//    std::cout<< "The old path is: " << std::endl;
//    
//    for(auto it = a_good_path.begin(); it != a_good_path.end(); it++){
//        std::cout<< * it << " -> ";
//    }
    
    Path path(deliveries);
    Path path_dp = local_perturbation(path);
    std::vector<unsigned> path_inter = convert_delivery_points_to_inter(path_dp.path);
    
//    std::cout<< "The new path is: " << std::endl;
//    
//    for(auto it = path_inter.begin(); it != path_inter.end(); it++){
//        std::cout<< * it << " -> ";
//    }
//    std::vector<unsigned> path_ss = convert_inter_to_street_seg(path_inter);
    
    
//    draw_test_path(path_ss); //draw modified path
    
}