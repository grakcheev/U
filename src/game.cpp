#include "game.h"
#include "queue.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

// method to calculate distances from all vertices to final vertex using BFS
int* Game::calculateDistances() {
  int* distances = new int[NumberOfSubjects];
  bool* visitedInBFS = new bool[NumberOfSubjects];
  
  // Initialize arrays
  for (int i = 0; i < NumberOfSubjects; i++) {
    distances[i] = -1;
    visitedInBFS[i] = false;
  }
  
  // Find index of final position
  int finalIndex = -1;
  bool foundFinal = false;
  for (int i = 0; i < NumberOfSubjects && !foundFinal; i++) {
    if (Subjects.Get(i) == FinalPosition) {
      finalIndex = i;
      foundFinal = true;
    }
  }
  
  if (finalIndex == -1) {
    delete[] distances;
    delete[] visitedInBFS;
    return 0;
  }
  
  // Use BFS from final vertex
  Queue queue;
  distances[finalIndex] = 0;
  visitedInBFS[finalIndex] = true;
  queue.add(FinalPosition);
  
  while (!queue.isEmpty()) {
    AbstractSubject* current = queue.remove();
    int currentIndex = -1;
    
    // Find index of current vertex
    bool foundCurrent = false;
    for (int i = 0; i < NumberOfSubjects && !foundCurrent; i++) {
      if (Subjects.Get(i) == current) {
        currentIndex = i;
        foundCurrent = true;
      }
    }
    
    if (currentIndex != -1) {
      // Check all neighbors
      List<AbstractSubject*>& neighbors = current->get_neighbours();
      int neighborCount = neighbors.size();
      
      for (int i = 0; i < neighborCount; i++) {
        AbstractSubject* neighbor = neighbors.Get(i);
        int neighborIndex = -1;
        
        // Find index of neighbor
        bool foundNeighbor = false;
        for (int j = 0; j < NumberOfSubjects && !foundNeighbor; j++) {
          if (Subjects.Get(j) == neighbor) {
            neighborIndex = j;
            foundNeighbor = true;
          }
        }
        
        if (neighborIndex != -1) {
          // Check if this region was visited in the game
          bool visitedInGame = false;
          int visitedCount = Visited.size();
          
          for (int j = 0; j < visitedCount && !visitedInGame; j++) {
            if (Visited.Get(j) == neighbor) {
              visitedInGame = true;
            }
          }
          
          // Also check if neighbor is marked as visited in AbstractSubject
          if (neighbor->is_visited()) {
            visitedInGame = true;
          }
          
          // If not visited in game and not visited in BFS
          if (!visitedInGame && !visitedInBFS[neighborIndex]) {
            distances[neighborIndex] = distances[currentIndex] + 1;
            visitedInBFS[neighborIndex] = true;
            queue.add(neighbor);
          }
        }
      }
    }
  }
  
  delete[] visitedInBFS;
  return distances;
}





Game::Game(int NumberOfSubjects, List<AbstractSubject*>& Subjects) : 
  NumberOfSubjects(NumberOfSubjects), Subjects(Subjects), StartPosition(0), Position(0), FinalPosition(0), Turn(0), Mistakes(0), GameFinished(false) {
  srand(time(0));
  
  if (NumberOfSubjects <= 0 || Subjects.size() == 0) {
    return;
  }
  
  int startIndex = rand() % NumberOfSubjects;
  StartPosition = Subjects.Get(startIndex);
  Position = StartPosition;
  Visited.Add(Position);
  
  int finalIndex;
  do
    finalIndex = rand() % NumberOfSubjects;
  while (finalIndex == startIndex);
  FinalPosition = Subjects.Get(finalIndex);
  
  StartPosition->visit();
  
  Turn = 0; // Player moves first
}



Game::~Game() {}


// returns
// 0 - successful move
// 1 - player reached final region
// -1 - invalid move (region not found or not accessible)
// -2 - 3 mistakes made, player loses

int Game::makePlayerMove(String destination) {
  if (GameFinished || Turn != 0) return -1;
  
  // Find subject with given name
  AbstractSubject* destSubject = 0;
  for (int i = 0; i < Subjects.size(); i++) {
    AbstractSubject* subject = Subjects.Get(i);
    List<String>& names = subject->get_names();
    
    // Check all names for this region (some regions may have multiple names)
    for (int j = 0; j < names.size(); j++) {
      if (names.Get(j) == destination) {
        destSubject = subject;
        break;
      }
    }
    if (destSubject) break;
  }
  
  if (destSubject == 0) {
    Mistakes++;
    if (Mistakes >= 3) {
      GameFinished = true;
      return -2; // Player loses due to 3 mistakes
    }
    return -1; // Region not found
  }
  
  // Check if region is a neighbor
  bool isNeighbor = false;
  List<AbstractSubject*>& neighbors = Position->get_neighbours();
  int neighborCount = neighbors.size();
  
  for (int i = 0; i < neighborCount; i++) {
    if (neighbors.Get(i) == destSubject) {
      isNeighbor = true;
      break;
    }
  }
  
  bool wasVisited = false;
  int visitedCount = Visited.size();
  for (int i = 0; i < visitedCount; i++) {
    if (Visited.Get(i) == destSubject) {
      wasVisited = true;
      break;
    }
  }
  

  if (destSubject->is_visited()) {
    wasVisited = true;
  }
  
  if (!isNeighbor || wasVisited) {
    Mistakes++;
    if (Mistakes >= 3) {
      GameFinished = true;
      return -2;
    }
    return -1; // Incorrecr move
  }
  
  // Execute move
  Position = destSubject;
  Visited.Add(Position);
  Position->visit(); // Mark as visited
  Turn = 1; // Computer's turn
  
  // Check for victory
  if (Position == FinalPosition) {
    GameFinished = true;
    return 1; // Player reached final region
  }
  
  return 0; // Successful move
}



// Returns:
//   0 - successful move
//   1 - computer reached final region
//  -1 - computer shouldn't move (i hope it wouldnt happen)
//  -2 - computer has no possible moves, loses
int Game::makeComputerMove() {
  if (GameFinished || Turn != 1) return -1;
  
  // Calculate current distances
  int* distances = calculateDistances();
  if (distances == 0) {
    GameFinished = true;
    return -2;
  }
  
  // Find index of current position
  int currentIndex = -1;
  bool foundCurrent = false;
  for (int i = 0; i < NumberOfSubjects && !foundCurrent; i++) {
    if (Subjects.Get(i) == Position) {
      currentIndex = i;
      foundCurrent = true;
    }
  }
  
  // Check if computer can win immediately
  List<AbstractSubject*>& neighbors = Position->get_neighbours();
  int neighborCount = neighbors.size();
  
  bool computerWonImmediately = false;
  for (int i = 0; i < neighborCount && !computerWonImmediately; i++) {
    AbstractSubject* neighbor = neighbors.Get(i);
    if (neighbor == FinalPosition) {
      // Check if final position is already visited
      bool wasVisited = false;
      int visitedCount = Visited.size();
      
      for (int j = 0; j < visitedCount && !wasVisited; j++) {
        if (Visited.Get(j) == FinalPosition) {
          wasVisited = true;
        }
      }
      
      // Also check AbstractSubject visited flag
      if (FinalPosition->is_visited()) {
        wasVisited = true;
      }
      
      if (!wasVisited) {
        Position = FinalPosition;
        Visited.Add(Position);
        Position->visit();
        delete[] distances;
        GameFinished = true;
        computerWonImmediately = true;
        return 1; // Computer wins
      }
    }
  }
  
  // Strategy: find move with odd distance to final position
  AbstractSubject* bestMove = 0;
  int bestDistance = -1;
  bool foundOdd = false;
  
  // First look for moves with odd distance
  for (int i = 0; i < neighborCount; i++) {
    AbstractSubject* neighbor = neighbors.Get(i);
    
    // Check if region was visited
    bool wasVisited = false;
    int visitedCount = Visited.size();
    for (int j = 0; j < visitedCount && !wasVisited; j++) {
      if (Visited.Get(j) == neighbor) {
        wasVisited = true;
      }
    }
    
    // Check AbstractSubject visited flag
    if (neighbor->is_visited()) {
      wasVisited = true;
    }
    
    // Skip visited regions
    if (!wasVisited) {
      // Find index of neighbor
      int neighborIndex = -1;
      bool foundNeighbor = false;
      for (int j = 0; j < NumberOfSubjects && !foundNeighbor; j++) {
        if (Subjects.Get(j) == neighbor) {
          neighborIndex = j;
          foundNeighbor = true;
        }
      }
      
      // Process only if neighbor was found and is reachable
      if (neighborIndex != -1) {
        int distance = distances[neighborIndex];
        
        // Process only reachable neighbors
        if (distance != -1) {
          // If distance is odd
          if (distance % 2 == 1) {
            if (!foundOdd) {
              foundOdd = true;
              bestMove = neighbor;
              bestDistance = distance;
            } else if (distance < bestDistance) {
              // Choose minimal odd distance
              bestMove = neighbor;
              bestDistance = distance;
            }
          } else if (!foundOdd) {
            // If no odd moves, choose minimal distance
            if (bestMove == 0 || distance < bestDistance) {
              bestMove = neighbor;
              bestDistance = distance;
            }
          }
        }
      }
    }
  }
  
  // If no valid moves
  if (bestMove == 0) {
    delete[] distances;
    GameFinished = true;
    return -2; // Computer loses
  }
  
  // Execute best move
  Position = bestMove;
  Visited.Add(Position);
  Position->visit();
  Turn = 0; // Player's turn
  
  // Check if computer won
  if (Position == FinalPosition) {
    delete[] distances;
    GameFinished = true;
    return 1;
  }
  
  delete[] distances;
  return 0;
}




// Getter methods for UI


String Game::getCurrentRegionName() const {
  if (!Position || Position->get_names().size() == 0) {
    return String("");
  }
  return Position->get_names().Get(0);
}


String Game::getStartRegionName() const {
  if (!StartPosition || StartPosition->get_names().size() == 0) {
    return String("");
  }
  return StartPosition->get_names().Get(0);
}


String Game::getFinalRegionName() const {
  if (!FinalPosition || FinalPosition->get_names().size() == 0) {
    return String("");
  }
  return FinalPosition->get_names().Get(0);
}


List<String> Game::getNeighborRegionNames() const {
  List<String> result;
  if (!Position) return result;
  
  List<AbstractSubject*>& neighbors = Position->get_neighbours();
  int count = neighbors.size();
  for (int i = 0; i < count; i++) {
    AbstractSubject* neighbor = neighbors.Get(i);
    if (neighbor->get_names().size() > 0) {
      result.Add(neighbor->get_names().Get(0));
    }
  }
  return result;
}


List<String> Game::getVisitedRegionNames() const {
  List<String> result;
  int count = Visited.size();
  for (int i = 0; i < count; i++) {
    AbstractSubject* visited = Visited.Get(i);
    if (visited->get_names().size() > 0) {
      result.Add(visited->get_names().Get(0));
    }
  }
  return result;
}


int Game::getMistakesCount() const {
  return Mistakes;
}


int Game::getTurn() const {
  return Turn;
}


bool Game::isGameFinished() const {
  return GameFinished;
}

// -1 = game ongoing, 0 = player, 1 = computer
int Game::getWinner() const {
  if (!GameFinished) return -1;
  
  if (Position == FinalPosition) {
    return (Turn == 1) ? 1 : 0;
  }
  
  if (Mistakes >= 3) return 1;
  
  return (Turn == 0) ? 1 : 0;
}



void Game::reset() {
  // Select new start and final positions
  int startIndex = rand() % NumberOfSubjects;
  StartPosition = Subjects.Get(startIndex);
  Position = StartPosition;
  
  int finalIndex;
  do {
    finalIndex = rand() % NumberOfSubjects;
  } while (finalIndex == startIndex);
  FinalPosition = Subjects.Get(finalIndex);
  
  // Clear visited lists
  Visited.clear();
  Visited.Add(Position);
  
  // Reset all subjects' visited flags
  for (int i = 0; i < NumberOfSubjects; i++) {
    // We need unvisit method
  }
  
  // Mark start position as visited
  Position->visit();
  
  // Reset game state
  Turn = 0;
  Mistakes = 0;
  GameFinished = false;
}


// Check if a region is reachable from current position (for UI validation)
bool Game::isRegionReachable(String regionName) const {
  if (!Position || GameFinished || Turn != 0) return false;
  
  // Get current neighbors
  List<AbstractSubject*>& neighbors = Position->get_neighbours();
  int neighborCount = neighbors.size();
  
  for (int i = 0; i < neighborCount; i++) {
    AbstractSubject* neighbor = neighbors.Get(i);
    List<String>& names = neighbor->get_names();
    
    // Check all names for this region
    bool nameFound = false;
    for (int j = 0; j < names.size() && !nameFound; j++) {
      if (names.Get(j) == regionName) {
        nameFound = true;
      }
    }
    
    // If region name was found among this neighbor's names
    if (nameFound) {
      // Check if not visited
      bool wasVisited = false;
      int visitedCount = Visited.size();
      
      for (int k = 0; k < visitedCount && !wasVisited; k++) {
        if (Visited.Get(k) == neighbor) {
          wasVisited = true;
        }
      }
      
      // Also check AbstractSubject visited flag
      if (neighbor->is_visited()) {
        wasVisited = true;
      }
      
      // Return true if region is reachable (not visited)
      return !wasVisited;
    }
  }
  
  return false;
}