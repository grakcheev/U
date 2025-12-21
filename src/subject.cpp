#include "subject.h"
#include "error.h"

using json = nlohmann::json;


AbstractSubject* Map::find_subject_by_name(
        List<AbstractSubject*>& lst,
        const String& name)
{
    List<AbstractSubject*>::Iterator<AbstractSubject*> it = lst.iter();

    while (!it.isEnd()) {
        AbstractSubject* s = it.next();

        List<String>::Iterator<String> nit = s->get_names().iter();
        while (!nit.isEnd()) {
            if (nit.next() == name)
                return s;
        }
    }
    return nullptr;
}


void AbstractSubject::add_neighbour(AbstractSubject* s)
{
    if (!neighbours.in_list(s))
        neighbours.push(s);
}

void AbstractSubject::add_name(String s)
{
    if (!names.in_list(s))
        names.push(s);
}

void AbstractSubject::add_polygon()
{
    Polygon* p = new Polygon();
    border.push(p);
}

void AbstractSubject::add_coord(Coordinates c)
{
    //if (border.size() == 0)
     //   throw Error("add_coord(): no polygon");

    Polygon* p = border.iter().next();
    p->push(c);
}



void Map::get_from_JSON(String subject_borders_file,
                        String subject_neighbours_file)
{
    // borders

    std::ifstream borders_in(subject_borders_file.c_str());
    json borders_json;
    borders_in >> borders_json;

    json::iterator feature_it = borders_json["features"].begin();
    while (feature_it != borders_json["features"].end()) {

        json feature = *feature_it;
        SubjectRussia* subj = new SubjectRussia();

    
        const char* name_cstr =
            feature["properties"]["name"].get_ref<const std::string&>().c_str();
        subj->add_name(String(name_cstr));


        json geom = feature["geometry"];
        const char* type_cstr =
            geom["type"].get_ref<const std::string&>().c_str();

        
        if (String(type_cstr) == String("Polygon")) {

            subj->add_polygon();

            json ring = geom["coordinates"][0];
            json::iterator pt_it = ring.begin();
            while (pt_it != ring.end()) {

                Coordinates c;
                c.x = (*pt_it)[0];
                c.y = (*pt_it)[1];

                subj->add_coord(c);
                ++pt_it;
            }
        }

       
        else if (String(type_cstr) == String("MultiPolygon")) {

            json polys = geom["coordinates"];
            json::iterator poly_it = polys.begin();

            while (poly_it != polys.end()) {

                subj->add_polygon();

                json ring = (*poly_it)[0];
                json::iterator pt_it = ring.begin();
                while (pt_it != ring.end()) {

                    Coordinates c;
                    c.x = (*pt_it)[0];
                    c.y = (*pt_it)[1];

                    subj->add_coord(c);
                    ++pt_it;
                }
                ++poly_it;
            }
        }

        subject_list.push(subj);
        ++feature_it;
    }

    //neighbours 

    std::ifstream neigh_in(subject_neighbours_file.c_str());
    json neigh_json;
    neigh_in >> neigh_json;

    json::iterator it = neigh_json.begin();
    while (it != neigh_json.end()) {

        String subj_name(it.key().c_str());
        AbstractSubject* subj =
            find_subject_by_name(subject_list, subj_name);

        if (!subj) {
            ++it;
            continue;
        }

        json neigh_array = it.value();
        json::iterator neigh_it = neigh_array.begin();

        while (neigh_it != neigh_array.end()) {

            const char* neigh_cstr =
                neigh_it->get_ref<const json::string_t&>().c_str();

            String neigh_name(neigh_cstr);
            AbstractSubject* neigh_subj =
                find_subject_by_name(subject_list, neigh_name);

            if (neigh_subj)
                subj->add_neighbour(neigh_subj);

            ++neigh_it;
        }

        ++it;
    }
}

bool Map::is_neighbours(String subject_name_1, String subject_name_2)
{
    AbstractSubject* subj1 = find_subject_by_name(this->subject_list, subject_name_1);
    return subj1 && find_subject_by_name(subj1->get_neighbours(), subject_name_2);
}

