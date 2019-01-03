#include "StreetsDatabaseAPI.h"

#ifndef TWO_OPT_H
#define TWO_OPT_H

#define DEPOT_ID -1

//class Path{
//public:
//    std::vector<IntersectionIndex> path;
//
//public:
//    Path(){
//        
//    }
//    Path(std::vector<IntersectionIndex> p){
//        path = p;
//    }
//    int pathSize(){
//        return path.size();
//    };
//    void setPathSize(unsigned size){
//        path.resize(size);
//    }
//    double pathDistance(){
//        if (path.empty()) return 0;
//
//        double dist = 0;
//        for(auto it = path.begin(); it != (path.end() -1) ; it++){
//            dist += find_distance_btw_intersections(*it,*(it+1));
//        }
//        return dist;
//    };
//
//    void insertNode(unsigned index, IntersectionIndex id){
//        path[index] = id;
//    };
//
//    unsigned getNode(unsigned id){
//        return path[id];
//    };
//
//    Path& operator=(Path& other){
//        path = other.path;
//        return *this;
//    }
//};
struct DeliveryPoint{
    int deliveryID; //unique identifier for each delivery
    bool pickUp; //true if pickup, false if drop of
    unsigned intersectionID;
    
    DeliveryPoint(){
        
    };
    
    DeliveryPoint(int deliveryID_, bool pickUp_, unsigned intersectionID_){
        deliveryID = deliveryID_;
        pickUp = pickUp_;
        intersectionID = intersectionID_;
    };
    
    bool operator==(const DeliveryPoint& other){
        return deliveryID == other.deliveryID && intersectionID == other.intersectionID;
    };
    
    bool operator!=(const DeliveryPoint& other){
        return deliveryID!=other.deliveryID || pickUp != other.pickUp;
    };
};


double find_distance_btw_intersections(unsigned inter1, unsigned inter2);
double find_distance_btw_delivery_points(DeliveryPoint& dp1, DeliveryPoint& dp2);

class Path{
public:
    std::vector<DeliveryPoint> path;

public:
    Path(){
        
    }
    Path(std::vector<DeliveryPoint> p){
        path = p;
    }
    int pathSize(){
        return path.size();
    };
    void setPathSize(unsigned size){
        path.resize(size);
    }
    double pathDistance(){
        if (path.empty()) return 0;

        double dist = 0;
        for(auto it = path.begin(); it != (path.end() -1) ; it++){
            dist += find_distance_btw_intersections((*it).intersectionID,(*(it+1)).intersectionID);
        }
        return dist;
    };

    void insertNode(unsigned index, DeliveryPoint dp){
        path[index] = dp;
    };

    DeliveryPoint getNode(unsigned id){
        return path[id];
    };
    void swap(int i, int k){
        DeliveryPoint temp = path[i];
        path[i] = path[k];
        path[k] = temp;
    };
    
//    Path& operator=(Path& other){
//        path = other.path;
//        return *this;
//    }
};

Path& TwoOpt(Path& path);
Path TwoOptSwap( const int& i, const int& k, Path& path);
bool isLegal(Path& path_inter);
void test_legality_checking();
Path & local_perturbation(Path& path_inter);
bool isAlreadyPickedUp(unsigned dropOffIndex, unsigned deliveryID_, Path& path_inter);
bool isAdvantageous(unsigned dropOffIndex, std::vector<DeliveryPoint>& path);



#endif /* TWO_OPT_H */
