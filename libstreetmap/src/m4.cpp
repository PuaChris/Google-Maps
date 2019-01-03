/* Performance results (ID: 408) http://ug251.eecg.utoronto.ca/ece297s/contest_2018/
 * M4 Quality of Result (QoR)        Percent Difference to Best         Percent Difference to Previous      ta_bad?     ta_good?                    Algorithm used                
 *55       98793.636                           25.03 %                                0.68 %                    No           No      Greedy algo (go to all pickups then all drop offs) 
 *53       96152.368                           21.69 %                                0.41 %                    No           No      Greedy algo (Option to either go to pickup or dropoff)
 *31       92538.772                           17.12 %                                0.0 %                     Yes          No      Greedy algo with multi-start on depots
 *         92374.526                           23.96 %                                0.25 %                    Yes          No      Greedy algo with multi-start and 2 opt
 */

//ideas:
//include hashtable
//when encountering a pickup, push onto hash table
//when encountering a drop-off, check hash table, if pickup is present, legal and pop it off, if not present, illegal



/* exiting before finding a path to the last node to visit. Undo changes and test this out 
 */

/*          for testing purposes
 *         deliveries = {DeliveryInfo(9140, 19741), DeliveryInfo(67133, 63045)};
        depots = {24341, 84950};
 */
#include "m3.h"
#include "m4.h"
#include "math.h"
#include <vector>
#include <unordered_set>
#include "utilities.h"
#include "StreetsDatabaseAPI.h"
#include "two_opt.h"
#include "test_path.h"












typedef unsigned pickUpID;
typedef unsigned dropOffID;
typedef unsigned depotID;

std::vector<unsigned> traveling_courier_greedy_old_version(const std::vector<DeliveryInfo>& deliveries,
        const std::vector<unsigned>& depots,
        const float turn_penalty) {

    std::vector<StreetSegmentIndex> best_path;
    std::vector<IntersectionIndex> best_intersections;
    double best_time = FLT_MAX;

    // Selecting initial depot
    for (depotID depot_id : depots) {

        std::vector<IntersectionIndex> path_intersections;
        std::vector<StreetSegmentIndex> path_street_segments;
        std::vector<IntersectionIndex> nodes_to_visit;
        std::vector<pickUpID> pick_up_remaining;

        for (DeliveryInfo delivery_info : deliveries) {
            pick_up_remaining.push_back(delivery_info.pickUp);
            nodes_to_visit.push_back(delivery_info.pickUp);
        }
        path_intersections.push_back(depot_id);


        IntersectionIndex current_intersection = depot_id;
        //Going through all pickups with dropoff options

        while (!nodes_to_visit.empty()) {
            std::priority_queue<std::pair<double, IntersectionIndex>, std::vector <std::pair<double, IntersectionIndex>>, std::greater<std::pair<double, IntersectionIndex>>> explore_front;

            for (IntersectionIndex next_intersection : nodes_to_visit) {
                double distance = find_distance_between_two_points(getIntersectionPosition(current_intersection), getIntersectionPosition(next_intersection));
                explore_front.push(std::make_pair(distance, next_intersection));
            }


            if (current_intersection != depot_id && !pick_up_remaining.empty()) {

                for (DeliveryInfo delivery_info : deliveries) {
                    if (current_intersection == delivery_info.pickUp) {

                        //Find the distance between the pickUp and dropOff and push it onto the queue
                        double distance = find_distance_between_two_points(getIntersectionPosition(current_intersection), getIntersectionPosition(delivery_info.dropOff));
                        explore_front.push(std::make_pair(distance, delivery_info.dropOff));

                        nodes_to_visit.push_back(delivery_info.dropOff);

                        //Once all the pickUps have been visited, all the nodes remaining are dropOffs and this if statement is no longer needed
                        pick_up_remaining.erase(std::remove(pick_up_remaining.begin(), pick_up_remaining.end(), current_intersection), pick_up_remaining.end());

                    }
                }
            }

            std::vector<StreetSegmentIndex> path;
            IntersectionIndex next_intersection;

            while (path.empty() && !explore_front.empty()) {
                next_intersection = explore_front.top().second;
                explore_front.pop();

                path = find_path_between_intersections(current_intersection, next_intersection, turn_penalty);
            }

            if (path.empty()) {
                //                return path;
                break;
            } else {
                path_intersections.push_back(next_intersection);

                path_street_segments.insert(path_street_segments.end(), path.begin(), path.end());

                nodes_to_visit.erase(std::remove(nodes_to_visit.begin(), nodes_to_visit.end(), next_intersection), nodes_to_visit.end());


                if (current_intersection = depot_id) {
                    for (DeliveryInfo delivery_info : deliveries) {
                        if (next_intersection == delivery_info.pickUp) {

                            //Find the distance between the pickUp and dropOff and push it onto the queue

                            nodes_to_visit.push_back(delivery_info.dropOff);

                            //Once all the pickUps have been visited, all the nodes remaining are dropOffs and this if statement is no longer needed
                            pick_up_remaining.erase(std::remove(pick_up_remaining.begin(), pick_up_remaining.end(), current_intersection), pick_up_remaining.end());

                        }
                    }
                }
                current_intersection = next_intersection;
            }
        }

        // Finding end depot
        std::priority_queue<std::pair<double, depotID>, std::vector <std::pair<double, depotID>>, std::greater<std::pair<double, depotID>>> depot_front;

        for (depotID next_intersection : depots) {
            double distance = find_distance_between_two_points(getIntersectionPosition(current_intersection), getIntersectionPosition(next_intersection));
            depot_front.push(std::make_pair(distance, next_intersection));
        }


        std::vector<StreetSegmentIndex> path;
        depotID next_intersection;

        while (path.empty() && !depot_front.empty()) {
            next_intersection = depot_front.top().second;
            depot_front.pop();

            path = find_path_between_intersections(current_intersection, next_intersection, turn_penalty);
        }

        if (path.empty()) {
            //            return path;
            continue;
        } else {
            path_intersections.push_back(next_intersection);

            path_street_segments.insert(path_street_segments.end(), path.begin(), path.end());
        }


        double time = compute_path_travel_time(path_street_segments, turn_penalty);
        if (!path_street_segments.empty() && time < best_time) {
            best_time = time;
            best_path = path_street_segments;
            best_intersections = path_intersections;
        }

    }


    return best_path;
}

bool compare_points(std::pair <double, DeliveryPoint> point1, std::pair <double, DeliveryPoint> point2) {
    return point1.first > point2.first;
}

struct DeliveryPointHash {
public:

    size_t operator()(const DeliveryPoint & delivery) const {
        return (delivery.deliveryID * 2) + static_cast<int> (delivery.pickUp);
    }
};

struct DeliveryPointEqual {
public:

    bool operator()(const DeliveryPoint& first, const DeliveryPoint& second) const {
        return first.deliveryID == second.deliveryID && first.intersectionID == second.intersectionID;
    }
};

std::vector<unsigned> traveling_courier_greedy(const std::vector<DeliveryInfo>& deliveries,
        const std::vector<unsigned>& depots,
        const float turn_penalty) {

    // Best of all depot starts
    std::vector<StreetSegmentIndex> best_path_ss;
    std::vector<DeliveryPoint> best_path_inters;
    double best_path_time = FLT_MAX;


    //Populate delivery_points (all pickups and drop-offs, no depots) 
    std::vector<std::pair<DeliveryPoint, DeliveryPoint >> delivery_points(deliveries.size());
    for (unsigned i = 0; i < deliveries.size(); ++i) {
        DeliveryPoint pickup_point(i, true, deliveries[i].pickUp);
        DeliveryPoint dropoff_point(i, false, deliveries[i].dropOff);
        delivery_points[i] = std::make_pair(pickup_point, dropoff_point);
    }

    //construct vector of depots
    std::vector<DeliveryPoint> depot_points(depots.size());
    for (unsigned i = 0; i < depots.size(); ++i) {
        DeliveryPoint depot_insert(DEPOT_ID, false, depots[i]);
        depot_points[i] = depot_insert;
    }




    // multi start - for each depot
#pragma omp parallel for

    for (int i = 0; i < depot_points.size(); ++i) {
    
//    for (int i = 0; i < 1; ++i) {

        //        // Populate delivery_points (all pickups and drop-offs, no depots) 
        //        std::vector<std::pair<DeliveryPoint, DeliveryPoint >> delivery_points(deliveries.size());
        //        for (unsigned i = 0; i < deliveries.size(); ++i) {
        //            DeliveryPoint pickup_point(i, true, deliveries[i].pickUp);
        //            DeliveryPoint dropoff_point(i, false, deliveries[i].dropOff);
        //            delivery_points[i] = std::make_pair(pickup_point, dropoff_point);
        //        }
        //
        //        //construct vector of depots
        //        std::vector<DeliveryPoint> depot_points(depots.size());
        //        for (unsigned i = 0; i < depots.size(); ++i) {
        //            DeliveryPoint depot_insert(DEPOT_ID, false, depots[i]);
        //            depot_points[i] = depot_insert;
        //        }



        DeliveryPoint depot_point = depot_points[i];
        std::vector<DeliveryPoint> path_inters;
        std::vector<StreetSegmentIndex> path_street_segments;
        std::unordered_set<DeliveryPoint, DeliveryPointHash, DeliveryPointEqual> nodes_to_visit;

        path_inters.push_back(depot_point);
        DeliveryPoint current_intersection = depot_point;

        for (std::pair<DeliveryPoint, DeliveryPoint> delivery_point : delivery_points) {
            nodes_to_visit.insert(delivery_point.first);
        }





        // Keep looping while there are still nodes to visit (pickups/dropoffs)
        while (!nodes_to_visit.empty()) {

            // Priority queue to get the closest next intersection
            std::priority_queue < std::pair<double, DeliveryPoint>,
                    std::vector <std::pair<double, DeliveryPoint>>,
                    std::function<bool (std::pair<double, DeliveryPoint>, std::pair<double, DeliveryPoint>) >> next_intersection_front(compare_points);

            // Populate next_intersection_front with distances to nodes to visit
            for (DeliveryPoint next_intersection : nodes_to_visit) {
                double distance = find_distance_between_two_points(
                        getIntersectionPosition(current_intersection.intersectionID),
                        getIntersectionPosition(next_intersection.intersectionID));
                next_intersection_front.push(std::make_pair(distance, next_intersection));
                
//                #pragma omp critical
//                {
//                    std::vector<StreetSegmentIndex> path = find_path_between_intersections(
//                            current_intersection.intersectionID, next_intersection.intersectionID, 15);
//                    double time = compute_path_travel_time(path, 15);
//                    next_intersection_front.push(std::make_pair(time, next_intersection));
//                }
            }




            // Find the closest reachable next intersection
            DeliveryPoint next_intersection;

            if (next_intersection_front.top().second.intersectionID == current_intersection.intersectionID) {
                next_intersection = next_intersection_front.top().second;
                next_intersection_front.pop();
            } else {
                std::vector<StreetSegmentIndex> next_inter_path;

                // While we have not found a reachable next intersection
                while (next_inter_path.empty() && !next_intersection_front.empty()) {
                    next_intersection = next_intersection_front.top().second;
                    next_intersection_front.pop();
#pragma omp critical
                    next_inter_path = find_path_between_intersections(current_intersection.intersectionID, next_intersection.intersectionID, turn_penalty);
                }

                // If none of the next intersection candidates can be reached, stop checking the path for this start depot
                if (next_inter_path.empty()) {
                    path_inters.clear();
                    break;
                }

                path_street_segments.insert(path_street_segments.end(), next_inter_path.begin(), next_inter_path.end());
            }








            // If the current intersection is a pickup and there are pickups remaining
            // Loop through all delivery pairs with this pickup location, deleting the pickups and adding the dropoffs to nodes to visit
            if (next_intersection.pickUp) {
                for (std::pair<DeliveryPoint, DeliveryPoint> delivery_pair : delivery_points) {
                    //                    if (next_intersection.intersectionID == delivery_pair.first.intersectionID) {
                    if (next_intersection == delivery_pair.first) {
                        nodes_to_visit.insert(delivery_pair.second);

                        // Remove all pickups at the same location
                        //                        nodes_to_visit.erase(std::remove(nodes_to_visit.begin(), nodes_to_visit.end(), delivery_pair.first), nodes_to_visit.end());

                        break;
                    }
                }
            }

            nodes_to_visit.erase(next_intersection);

            path_inters.push_back(next_intersection);
            current_intersection = next_intersection;
        }

        if (path_inters.empty()) {
            continue;
        }




        // Finding end depot
        // Priority queue to find the closest end depot
        std::priority_queue < std::pair<double, DeliveryPoint>,
                std::vector <std::pair<double, DeliveryPoint>>,
                std::function<bool (std::pair<double, DeliveryPoint>, std::pair<double, DeliveryPoint>) >> end_depot_front(compare_points);

        // Populate end depots
        for (DeliveryPoint end_depot : depot_points) {
            double distance = find_distance_between_two_points(
                    getIntersectionPosition(current_intersection.intersectionID),
                    getIntersectionPosition(end_depot.intersectionID));

            end_depot_front.push(std::make_pair(distance, end_depot));
        }


        // Find the closest reachable end depot
        std::vector<StreetSegmentIndex> end_depot_path;
        DeliveryPoint end_depot;

        // While we have not found a reachable end depot
        while (end_depot_path.empty() && !end_depot_front.empty()) {
            end_depot = end_depot_front.top().second;
            end_depot_front.pop();
#pragma omp critical
            end_depot_path = find_path_between_intersections(current_intersection.intersectionID, end_depot.intersectionID, turn_penalty);
        }

        // If there is no valid path to any end depot
        if (end_depot_path.empty()) {
            continue;
        }

        path_street_segments.insert(path_street_segments.end(), end_depot_path.begin(), end_depot_path.end());

        path_inters.push_back(end_depot);



        // Construct path of street segments
        //        std::vector<StreetSegmentIndex> path_ss;
        //        for (int i = 0; i < path_inters.size() - 1; ++i) {
        //            std::vector<StreetSegmentIndex> subpath = find_path_between_intersections(path_inters[i].intersectionID, path_inters[i + 1].intersectionID, turn_penalty);
        //            path_ss.insert(path_ss.end(), subpath.begin(), subpath.end());
        //        }
        //
        //        double path_time = compute_path_travel_time(path_ss, turn_penalty);
        //        if (!path_ss.empty() && path_time < best_path_time) {
        //            best_path_time = path_time;
        //            best_path_ss = path_ss;
        //            best_path_inters = path_inters;
        //        }


        Path path(path_inters);
        Path new_path = TwoOpt(path);
        std::vector<IntersectionIndex> new_path_int = convert_delivery_points_to_inter(new_path.path);
        
//        std::vector<IntersectionIndex> new_path_int = convert_delivery_points_to_inter(path_inters);

#pragma omp critical
        {
            std::vector<unsigned> new_path_ss = convert_inter_to_street_seg(new_path_int);
            double path_time = compute_path_travel_time(new_path_ss, turn_penalty);

            //        double path_time = compute_path_travel_time(path_street_segments, turn_penalty);


            if (!new_path_ss.empty() && path_time < best_path_time) {
                best_path_time = path_time;
                best_path_ss = new_path_ss;
                best_path_inters = path_inters;
            }
        }
    }




    //    Path path(best_path_inters);
    //    Path new_path = TwoOpt(path);
    //    std::vector<IntersectionIndex> new_path_int = convert_delivery_points_to_inter(new_path.path);
    //    std::vector<unsigned> new_path_ss = convert_inter_to_street_seg(new_path_int);
    //    return new_path_ss;

    return best_path_ss;

}

std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries,
        const std::vector<unsigned>& depots,
        const float turn_penalty) {
    return traveling_courier_greedy(deliveries, depots, turn_penalty);
    //    return traveling_courier_greedy_old_version(deliveries, depots, turn_penalty);
    //        test();
    //        return std::vector<unsigned>();
}

