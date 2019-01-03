#include <vector>

#ifndef M3_H
#define M3_H

#define NO_EDGE -1

//Edge: a data structure used to represent street segments
struct Edge {
    unsigned edge_id;

    unsigned to;
    unsigned from;

    double travel_time;

    Edge();

    Edge(const unsigned ss_id) {
        edge_id = ss_id;

        auto info = getStreetSegmentInfo(ss_id);
        to = info.to;
        from = info.from;

        travel_time = find_street_segment_travel_time(ss_id);
    }

    unsigned getToNode(unsigned node_id) {
        return node_id == from ? to : from;
    }
    
};

//Node: a data structure used to represent intersections
struct Node {
    unsigned node_id;

    std::vector<unsigned> outgoing_edges; // Outgoing edge ids 

    int reachingEdge = NO_EDGE; // ID of the edge used to reach this node

    double travel_time = FLT_MAX;
    
    double total_time = 0;

    //constructor
    Node(unsigned intersection_id) {

        node_id = intersection_id;

        for (unsigned i = 0; i < getIntersectionStreetSegmentCount(intersection_id); i++) {
            StreetSegmentIndex ss_id = getIntersectionStreetSegment(intersection_id, i);
            auto info = getStreetSegmentInfo(ss_id);

            if (info.oneWay == false || info.from == intersection_id) // check for outgoing edges
                outgoing_edges.push_back(ss_id);
        }
    }

    //restore node to its original condition
    void restore() {
        reachingEdge = NO_EDGE;
        travel_time = FLT_MAX;
        total_time = 0;
    }
};

//********************  SET UP FUNCTIONS  **************************
void set_up_graph();
void set_up_poi_string_to_ids();


//********************  HELPER FUNCTIONS  **************************
void free_graph();
void restore_node_info();
bool compare_nodes(Node* node1, Node* node2);
std::vector<POIIndex> find_POIs_by_name(std::string name);
bool is_turning(int edge1, int edge2);


//********************  PATH FINDING FUNCTIONS  **************************
bool A_star_find_path(Node* starting_node, unsigned destID, double turning_penalty);
std::vector<unsigned> bfsTraceBack(int destID);


#endif /* M3_H */

