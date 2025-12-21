#include <QApplication>
#include "demo.h"
#include "subject.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Map map;
    map.get_from_JSON("data/new_russia (1).geojson", "data/russia_neighbours.json");

    MapWidget widget(&map);
    widget.resize(1000, 700);
    widget.show();

    return app.exec();
}
