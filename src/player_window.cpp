#include "player_window.h"
#include "ui_player_window.h"

#include <QMessageBox>
#include <QCompleter>
#include <QFocusEvent>


PlayerWindow::PlayerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlayerWindow)
    , game(nullptr)
    , map(nullptr)
    , computerTimer(new QTimer(this))
    , isComputerMoving(false)
    , jsonFilePathRegions("data/new_ruusia (1).json")
    , jsonFilePathNeighbours("russia_neighbours.json")
{
    ui->setupUi(this);
    
    setWindowTitle("Турнир Угольникова");
    
    initGame();
    

    connect(ui->makeMoveButton, &QPushButton::clicked, 
            this, &PlayerWindow::on_makeMoveButton_clicked);
    connect(ui->regionInput, &QLineEdit::returnPressed, 
            this, &PlayerWindow::on_regionInput_returnPressed);
    connect(computerTimer, &QTimer::timeout, 
            this, &PlayerWindow::onComputerMove);
    

    QCompleter *completer = new QCompleter(getAllRegionNames(), this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->regionInput->setCompleter(completer);
    
    

    updateUI();
}


PlayerWindow::~PlayerWindow()
{
    delete game;
    delete map;
    delete ui;
}


void PlayerWindow::initGame()
{
    // map creation
    map = new Map();
    map->get_from_JSON(jsonFilePathRegions, jsonFilePathNeighbours);
    

    List<AbstractSubject*>& subjects = map->get_subjects();
    
    game = new Game(subjects.size(), subjects);
}


void PlayerWindow::updateUI()
{
    if (!game) return;
    
    // text field clearing
    ui->gameInfoText->clear();
    

    ui->gameInfoText->append("Турнир Угольникова");
    ui->gameInfoText->append("");
    
    String currentRegion = game->getCurrentRegionName();
    ui->gameInfoText->append("Текущий регион: " + 
                            QString::fromStdString(currentRegion.c_str()));
    
    String finalRegion = game->getFinalRegionName();
    ui->gameInfoText->append("Конечный регион: " + 
                            QString::fromStdString(finalRegion.c_str()));
    ui->gameInfoText->append("");
    

    ui->gameInfoText->append("Ошибок: " + 
                            QString::number(game->getMistakesCount()) + "/3");
    ui->gameInfoText->append("");
    
    
    List<String> visited = game->getVisitedRegionNames();
    if (visited.size() > 0) {
        ui->gameInfoText->append("Путь:");
        QString path;
        for (int i = 0; i < visited.size(); i++) {
            if (i > 0) path += " → ";
            path += QString::fromStdString(visited.Get(i).c_str());
        }
        ui->gameInfoText->append(path);
    }
    
    if (game->isGameFinished()) {
        int winner = game->getWinner();
        ui->gameInfoText->append("");
        if (winner == 0) {
            ui->gameInfoText->append("Игрок победил");
        } else if (winner == 1) {
            ui->gameInfoText->append("Компьютер победил");
        }
    } else {
        ui->gameInfoText->append("");
        if (game->getTurn() == 0) {
            ui->gameInfoText->append("Ваш ход. Введите регион");
        } else {
            ui->gameInfoText->append("Ход компьютера...");
        }
    }
    

    ui->currentRegionLabel->setText("Текущий регион:\n" + 
                                  QString::fromStdString(currentRegion.c_str()));
    
    ui->finalRegionLabel->setText("Конечный регион:\n" + 
                                QString::fromStdString(finalRegion.c_str()));
    

    // check for showing elems                           
    bool isPlayerTurn = (game->getTurn() == 0) && 
                       !game->isGameFinished() && 
                       !isComputerMoving;
    
    ui->regionInput->setEnabled(isPlayerTurn);
    ui->makeMoveButton->setEnabled(isPlayerTurn);
    
    if (isPlayerTurn) {
        ui->regionInput->setFocus();
        if (ui->regionInput->text().isEmpty()) {
            ui->regionInput->setPlaceholderText("Введите название региона");
        }
    } else {
        ui->regionInput->setPlaceholderText("Ожидание...");
    }
    
    // scroll text
    QTextCursor cursor = ui->gameInfoText->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->gameInfoText->setTextCursor(cursor);
}



// make move
void PlayerWindow::on_makeMoveButton_clicked()
{
    on_regionInput_returnPressed();
}

// enter аs make move
void PlayerWindow::on_regionInput_returnPressed()
{
    QString input = ui->regionInput->text().trimmed();
    if (input.isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Введите название региона!");
        return;
    }
    
    processPlayerMove(input);
}



void PlayerWindow::processPlayerMove(const QString& regionName)
{
    if (!game || game->isGameFinished() || isComputerMoving) {
        return;
    }
    
    String destination(regionName.toStdString().c_str());
    int result = game->makePlayerMove(destination);
    
    switch (result) {
    case 0: // good move
        ui->regionInput->clear();
        updateUI();

        if (!game->isGameFinished()) {
            isComputerMoving = true;
            ui->makeMoveButton->setEnabled(false);
            ui->regionInput->setEnabled(false);
            computerTimer->singleShot(2000, this, &PlayerWindow::onComputerMove);
        }
        break;
        
    case 1: // player reached final
        ui->regionInput->clear();
        updateUI();
        handleGameResult(0); // player won
        break;
        
    case -1: // wrong step
        updateUI();
        ui->gameInfoText->append("Неверный ход! Ошибок: " + 
                                QString::number(game->getMistakesCount()) + "/3");
        ui->regionInput->selectAll();
        ui->regionInput->setFocus();
        break;
        
    case -2: // 3 mistakes
        ui->regionInput->clear();
        updateUI();
        handleGameResult(1); // comp won
        break;
    }
}


void PlayerWindow::onComputerMove()
{
    if (!game || game->isGameFinished()) {
        isComputerMoving = false;
        ui->makeMoveButton->setEnabled(true);
        ui->regionInput->setEnabled(true);
        return;
    }
    
    int result = game->makeComputerMove();
    
    switch (result) {
    case 0: // succesful move
        ui->gameInfoText->append("🤖 Компьютер сделал ход");
        updateUI();
        break;
        
    case 1: // comp won
        updateUI();
        handleGameResult(1);
        break;
        
    case -2: // no moves
        updateUI();
        handleGameResult(0);
        break;
    }
    
    isComputerMoving = false;
    ui->makeMoveButton->setEnabled(true);
    ui->regionInput->setEnabled(true);
    ui->regionInput->setFocus();
}



void PlayerWindow::handleGameResult(int result)
{
    QString title, message;
    
    if (result == 0) {
        title = "Поздравляем!";
        message = "Вы достигли конечного региона и победили!";
    } else {
        title = "Игра окончена";
        message = "Компьютер победил. Попробуйте ещё раз!";
    }
    
    QMessageBox::information(this, title, message);
    

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Новая игра", "Хотите начать новую игру?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        // restart
        delete game;
        List<AbstractSubject*>& subjects = map->get_subjects();
        game = new Game(subjects.size(), subjects);
        updateUI();
    }
}


QStringList PlayerWindow::getAllRegionNames() const
{
    QStringList regionNames;
    
    if (!map) return regionNames;
    
    List<AbstractSubject*>& subjects = map->get_subjects();
    for (int i = 0; i < subjects.size(); i++) {
        AbstractSubject* subject = subjects.Get(i);
        List<String>& names = subject->get_names();
        for (int j = 0; j < names.size(); j++) {
            regionNames << QString::fromStdString(names.Get(j).c_str());
        }
    }
    
    return regionNames;
}
