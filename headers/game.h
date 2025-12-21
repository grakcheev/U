#pragma once

#include "subject.h"
#include "str.h"
#include "list.h"

class Game {
private:
    int NumberOfSubjects;
    List<AbstractSubject*>& Subjects;
    AbstractSubject* StartPosition;
    AbstractSubject* Position;
    AbstractSubject* FinalPosition;
    int Turn;   // 0 - player, 1 - computer
    List<AbstractSubject*> Visited;
    int Mistakes;
    bool GameFinished;
    
public:
    Game(int NumberOfSubjects, List<AbstractSubject*>& Subjects);
    ~Game();
    
    int makePlayerMove(String destination);
    int makeComputerMove();
    
    String getCurrentRegionName() const;
    String getStartRegionName() const;
    String getFinalRegionName() const;
    List<String> getNeighborRegionNames() const;
    List<String> getVisitedRegionNames() const;
    int getMistakesCount() const;
    int getTurn() const;
    bool isGameFinished() const;
    int getWinner() const;
    bool isRegionReachable(String regionName) const;

    List<AbstractSubject*> getAllRegions() const;
    void reset();
    
private:
    int* calculateDistances();
};