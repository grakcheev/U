#pragma once 

#include "list.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "str.h"
#include <QDebug>

struct Coordinates{
    double x;
    double y;
};



typedef List<Coordinates> Polygon;

class AbstractSubject {
protected:
    List<String> names;
    List<AbstractSubject*> neighbours;
    List<Coordinates> border;
    bool is_visited_;
public:
    AbstractSubject():neighbours(), border(), names(), is_visited_(false){};
    void add_neighbour(AbstractSubject*);
    void add_name(String);
    void add_coord(Coordinates);
    void add_polygon();
    void visit(){ is_visited_ = true; }
    List<String>& get_names(){ return names; }
    List<AbstractSubject*>& get_neighbours(){ return neighbours;}
    List<Coordinates>& get_border(){ return border; };
    bool is_visited() {return is_visited_; };
};

class SubjectRussia: public AbstractSubject{
    int number_of_subject;
};

class Map{
   List <AbstractSubject*> subject_list;
public:
    void get_from_JSON(String);
    bool is_neighbours(String, String);
    List <AbstractSubject*>& get_subjects(){ return subject_list; }
};