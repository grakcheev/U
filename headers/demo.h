#pragma once

#include <QWidget>
#include <QPainter>
#include <QPolygonF>
#include <QMap>

#include "list.h"
#include "subject.h"

struct CachedSubject {
    List<QPolygonF> polygons;
    bool visited;
};

class MapWidget : public QWidget {
    Q_OBJECT

    Map* map;
    QMap<AbstractSubject*, CachedSubject*> cache;

    int widgetWidth;
    int widgetHeight;
    static double normalizeLongitude360(double lon);
public:
    explicit MapWidget(Map* m, QWidget* parent = nullptr);
    ~MapWidget();

    void rebuildCache();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};
