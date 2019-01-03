#include <iostream>
#include <vector>
#include "m1.h"

#include "two_opt.h"

#ifndef TEST_PATH_H
#define TEST_PATH_H


std::vector<unsigned> convert_inter_to_street_seg(std::vector<unsigned> intersections);



void collect_street_segments2(std::vector<unsigned> path) ;


void collect_curve_points_for_street_segment2(unsigned ss_id) ;


void draw_test_path(std::vector<unsigned>& path) ;


void test();

std::vector<unsigned> convert_delivery_points_to_inter(std::vector<DeliveryPoint>& path_dp);

#endif /* TEST_PATH_H */

