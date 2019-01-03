#include "Highlight.h"

Highlight::Highlight() {

}

Highlight::~Highlight(){
    
}

Highlight::Highlight(std::string _name, double xcen, double ycen, IntersectionIndex _intersection_id) {
    name = _name;
    pos.x = xcen;
    pos.y = ycen;
    intersection_id = _intersection_id;
}

Highlight::Highlight(std::string _name, t_point _pos, IntersectionIndex _intersection_id) {
    name = _name;
    pos = _pos;
    intersection_id = _intersection_id;
}

void Highlight::set_highlight_name(std::string _name) {
    name = _name;
}

std::string Highlight::get_highlight_name() {
    return name;
}

void Highlight::set_highlight_pos(double xcen, double ycen) {
    pos.x = xcen;
    pos.y = ycen;
}

void Highlight::set_highlight_pos(t_point _pos) {
    pos = _pos;
}

t_point Highlight::get_highlight_pos() {
    return pos;
}

void Highlight::set_intersection_id(IntersectionIndex _intersection_id){
    intersection_id = _intersection_id;
}

IntersectionIndex Highlight::get_intersection_id(){
    return intersection_id;
}