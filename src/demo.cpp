#include "demo.h"
#include <limits>

MapWidget::MapWidget(Map* m, QWidget* parent): QWidget(parent), map(m), widgetWidth(0), widgetHeight(0)
{
    setMinimumSize(400, 300);
}




MapWidget::~MapWidget()
{
    QMap<AbstractSubject*, CachedSubject*>::iterator it = cache.begin();
    while (it != cache.end()) {
        delete it.value();
        ++it;
    }
    cache.clear();
}

double MapWidget::normalizeLongitude360(double lon) {
    while (lon < 0) lon += 360;
    while (lon >= 360) lon -= 360;
    return lon;
}

void MapWidget::rebuildCache()
{
    // Очистка кэша
    QMap<AbstractSubject*, CachedSubject*>::iterator itc = cache.begin();
    while (itc != cache.end()) {
        delete itc.value();
        ++itc;
    }
    cache.clear();

    List<AbstractSubject*>& subjects = map->get_subjects();
    List<AbstractSubject*>::Iterator<AbstractSubject*> sit = subjects.iter();


    QVector<double> longitudes;
    double minY =  1e100;
    double maxY = -1e100;

    while (!sit.isEnd()) {
        AbstractSubject* subj = sit.next();

        List<Polygon*>& borders = subj->get_border();
        List<Polygon*>::Iterator<Polygon*> pit = borders.iter();

        while (!pit.isEnd()) {
            Polygon* poly = pit.next();

            List<Coordinates>::Iterator<Coordinates> cit = poly->iter();
            while (!cit.isEnd()) {
                Coordinates c = cit.next();

                double normLon = normalizeLongitude360(c.x);
                longitudes.append(normLon);

                if (c.y < minY) minY = c.y;
                if (c.y > maxY) maxY = c.y;
            }
        }
    }

    if (longitudes.isEmpty() || minY >= maxY)
        return;

    // Шаг 2: Найдём min и max для долготы
    double minX = *std::min_element(longitudes.begin(), longitudes.end());
    double maxX = *std::max_element(longitudes.begin(), longitudes.end());

    // Шаг 3: Проверяем, есть ли wrap-around (разрыв больше 180 градусов)
    if (maxX - minX > 180) {
        // Тогда сдвигаем все долготы > 180 на -360
        for (int i = 0; i < longitudes.size(); ++i) {
            if (longitudes[i] > 180)
                longitudes[i] -= 360;
        }
        minX = *std::min_element(longitudes.begin(), longitudes.end());
        maxX = *std::max_element(longitudes.begin(), longitudes.end());
    }

    double mapWidth  = maxX - minX;
    double mapHeight = maxY - minY;

    if (mapWidth <= 0 || mapHeight <= 0)
        return;

    const int topMargin = 40;

    double scaleX = widgetWidth / mapWidth;
    double scaleY = (widgetHeight - topMargin) / mapHeight;

    double scale = (scaleX < scaleY) ? scaleX : scaleY;

    double offsetX = (widgetWidth  - mapWidth  * scale) / 2.0;
    double offsetY = (widgetHeight - topMargin - mapHeight * scale) / 2.0;

    // Шаг 4: Строим кеш с учетом wrap-around
    sit = subjects.iter();

    while (!sit.isEnd()) {
        AbstractSubject* subj = sit.next();

        CachedSubject* cached = new CachedSubject();
        cached->visited = subj->is_visited();

        List<Polygon*>& borders = subj->get_border();
        List<Polygon*>::Iterator<Polygon*> pit = borders.iter();

        while (!pit.isEnd()) {
            Polygon* poly = pit.next();
            QPolygonF qpoly;

            List<Coordinates>::Iterator<Coordinates> cit = poly->iter();
            while (!cit.isEnd()) {
                Coordinates c = cit.next();

                double normLon = normalizeLongitude360(c.x);
                if (maxX - minX > 180 && normLon > 180)
                    normLon -= 360;  // повторяем сдвиг для текущей точки

                double nx = (normLon - minX) * scale + offsetX;
                double ny = (maxY - c.y) * scale + offsetY;

                qpoly << QPointF(nx, ny);
            }

            cached->polygons.push(qpoly);
        }

        cache.insert(subj, cached);
    }
}



void MapWidget::resizeEvent(QResizeEvent* event)
{
    widgetWidth = width();
    widgetHeight = height();
    rebuildCache();
    QWidget::resizeEvent(event);
}


void MapWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    const int topMargin = 40;

    QFont font = painter.font();
    font.setPointSize(16);
    painter.setFont(font);
    painter.setPen(Qt::black);
    painter.drawText(0, 0, width(), topMargin,
                     Qt::AlignCenter, "Карта России");

    painter.translate(0, topMargin);

    QPen pen(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);

    QMap<AbstractSubject*, CachedSubject*>::iterator it = cache.begin();
    while (it != cache.end()) {

        CachedSubject* cached = it.value();

        if (cached->visited)
            painter.setBrush(QColor(0, 180, 0, 160));
        else
            painter.setBrush(Qt::NoBrush);

        List<QPolygonF>::Iterator<QPolygonF> pit =
            cached->polygons.iter();

        while (!pit.isEnd())
            painter.drawPolygon(pit.next());

        ++it;
    }
}
