#include "subject.h"
#include <string>
#include <iostream>

using json = nlohmann::json;


void AbstractSubject::add_neighbour(AbstractSubject*)
{

}

void AbstractSubject::add_name(String)
{
}

void AbstractSubject::add_coord(Coordinates c) {
    border.push(c);
}

void AbstractSubject::add_polygon()
{
}


using json = nlohmann::json;

void Map::get_from_JSON(String subject_borders_file,
                        String subject_neighbours_file)
{


    // std::ifstream borders_in(subject_borders_file.c_str());
    // json borders_json;
    // borders_in >> borders_json;
    // qInfo << borders_json["features"].get<char*>();
}


