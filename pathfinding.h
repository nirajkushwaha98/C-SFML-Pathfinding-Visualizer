#pragma once

#include "grid.h"

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include <map>
#include <queue>
#include <stack>
#include <vector>

enum class Algorithm {
    BFS,
    DFS
};

class Pathfinder {
public:
    explicit Pathfinder(Grid& grid);

    bool start();
    void update();
    void stop();

    bool isRunning() const;
    bool hasFinished() const;
    bool pathFound() const;

    void setStepDelay(sf::Time delay);
    void setAlgorithm(Algorithm algorithm);
    Algorithm getAlgorithm() const;

private:
    enum class Phase {
        Idle,
        Exploring,
        DrawingPath,
        Finished
    };

    int toKey(const sf::Vector2i& cell) const;
    bool canVisit(const sf::Vector2i& cell) const;

    void stepExplore();
    void stepBfs();
    void stepDfs();
    void preparePathAnimation();
    void finishWithoutPath();

    Grid& grid_;
    Algorithm algorithm_;
    Phase phase_;
    bool foundPath_;

    sf::Clock stepClock_;
    sf::Time stepDelay_;

    std::vector<std::vector<bool>> discovered_;
    std::map<int, sf::Vector2i> parent_;

    std::queue<sf::Vector2i> bfsQueue_;
    std::stack<sf::Vector2i> dfsStack_;

    std::vector<sf::Vector2i> pathCells_;
    std::size_t nextPathIndex_;
};
