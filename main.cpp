#include <QApplication>
#include "demo.h"
#include "subject.h"
#include "list.h"
#include "player.h"
#include "str.h"

int main(int argc, char *argv[]) {
    Map map;

    SubjectRussia* r1 = new SubjectRussia();
    r1->add_coord({0.1, 0.1});
    r1->add_coord({0.4, 0.1});
    r1->add_coord({0.4, 0.4});
    r1->add_coord({0.1, 0.4});

    

    map.get_subjects().push(r1);
    
    // map.get_from_JSON(String("data/russia_neighbours.json"), String("data/russia_neighbours.json"));
    
    
    QApplication app(argc, argv);
    // MapWidget mapWidget(&map);
    // for (int i = 0; i < 1000; i++){
    //     SubjectRussia* r2 = new SubjectRussia();
    //     r2->add_coord({(double) rand()/RAND_MAX, (double) rand()/RAND_MAX});
    //     r2->add_coord({(double) rand()/RAND_MAX, (double) rand()/RAND_MAX});
    //     r2->add_coord({(double) rand()/RAND_MAX, (double) rand()/RAND_MAX});
    //     map.get_subjects().push(r2);
    // }

    
    // mapWidget.resize(800, 600);
    // mapWidget.show();

   

    return app.exec();

}