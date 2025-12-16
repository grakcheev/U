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

void Map::get_from_JSON(String file_name)
{
    std::ifstream in_file(file_name.c_str());
    json j;
    in_file >> j;
    auto c = j["features"][0]["properties"]["region"];

    SubjectRussia* r2 = new SubjectRussia();
    r2->add_coord({0.5, 0.2});
    r2->add_coord({0.8, 0.2});
    r2->add_coord({0.8, 0.5});
    r2->add_coord({0.5, 0.5});
    r2->visit();
    get_subjects().push(r2);
    std::string s3 = c;
    qInfo() << s3.c_str();
}
