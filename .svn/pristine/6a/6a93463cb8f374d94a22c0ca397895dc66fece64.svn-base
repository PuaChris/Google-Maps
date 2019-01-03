/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <unittest++/UnitTest++.h>
#include "m1.h"
#include "LatLon.h"

#define PI 3.1415926535

using namespace std;

TEST(distance_test){
    //setup
    LatLon point1(43,-79);
    LatLon point2(42,-80);
    
    LatLon point3(43,-79);
    LatLon point4(42.5,-79.3);
   
    LatLon point5(43,-79);
    LatLon point6(42.92,-79.15);
    
    //processing
    double distance1 = find_distance_between_two_points(point1, point2);
    double distance2 = find_distance_between_two_points(point3, point4);
    double distance3 = find_distance_between_two_points(point5, point6);
    
    //verification
    double answer1 = 138190;
    double answer2 = 60774;
    double answer3 = 15109;
    double tolerance = 5;
    
    CHECK_CLOSE(answer1, distance1, tolerance);
    CHECK_CLOSE(answer2, distance2, tolerance);
    CHECK_CLOSE(answer3, distance3, tolerance);
}