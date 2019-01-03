/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   highlight.h
 * Author: puachris
 *
 * Created on February 23, 2018, 6:20 PM
 */


/*If we EVER get to highlighting items other than intersections... consider using inheritance
 */
#ifndef SHAPE_H
#define SHAPE_H
#include <string>
#include "graphics_types.h"
#include "StreetsDatabaseAPI.h"

class Highlight {
private:
    std::string name;
    t_point pos;
    IntersectionIndex intersection_id;

public:

    Highlight();
    Highlight(std::string _name, double xcen, double ycen, IntersectionIndex _intersection_id);
    Highlight(std::string _name, t_point _pos, IntersectionIndex _intersection_id);
    ~Highlight();

    void set_highlight_name(std::string _name);
    std::string get_highlight_name();
    void set_highlight_pos(double xcen, double ycen);
    void set_highlight_pos(t_point _pos);
    t_point get_highlight_pos();
    void set_intersection_id(IntersectionIndex _intersection_id);
    IntersectionIndex get_intersection_id();
};

#endif /* SHAPE_H */

