#pragma once

#include <QMainWindow>
#include <QTimer>
#include "game.h"
#include "subject.h"

namespace Ui {
    class PlayerWindow;
}

class PlayerWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit PlayerWindow(QWidget *parent = nullptr);
    ~PlayerWindow();

private Q_SLOTS:
    void on_makeMoveButton_clicked();
    void on_regionInput_returnPressed();
    void onComputerMove();

private:
    void initGame();
    void updateUI();
    void handleGameResult(int result);
    QStringList getAllRegionNames() const;
    void processPlayerMove(const QString& regionName);

    Ui::PlayerWindow *ui;
    Game *game;
    Map *map;
    QTimer *computerTimer;
    bool isComputerMoving;
    String jsonFilePathRegions;
    String jsonFilePathNeighbours;
};
