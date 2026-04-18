#include "pathfinding.h"

Pathfinder::Pathfinder(Grid& grid)
    : grid_(grid),
      algorithm_(Algorithm::BFS),
      phase_(Phase::Idle),
      foundPath_(false),
      stepDelay_(sf::milliseconds(35)),
      nextPathIndex_(0) {}

bool Pathfinder::start() {
    if (!grid_.hasStart() || !grid_.hasEnd()) {
        return false;
    }

    grid_.clearSearch();

    const int rows = grid_.getRows();
    const int cols = grid_.getCols();

    discovered_.assign(rows, std::vector<bool>(cols, false));
    parent_.clear();

    while (!bfsQueue_.empty()) {
        bfsQueue_.pop();
    }

    while (!dfsStack_.empty()) {
        dfsStack_.pop();
    }

    pathCells_.clear();
    nextPathIndex_ = 0;

    const sf::Vector2i startCell = grid_.getStart();
    discovered_[startCell.y][startCell.x] = true;

    if (algorithm_ == Algorithm::BFS) {
        bfsQueue_.push(startCell);
    } else {
        dfsStack_.push(startCell);
    }

    foundPath_ = false;
    phase_ = Phase::Exploring;
    stepClock_.restart();

    return true;
}

void Pathfinder::update() {
    if (phase_ == Phase::Idle || phase_ == Phase::Finished) {
        return;
    }

    if (stepClock_.getElapsedTime() < stepDelay_) {
        return;
    }

    stepClock_.restart();

    if (phase_ == Phase::Exploring) {
        stepExplore();
    } else if (phase_ == Phase::DrawingPath) {
        if (nextPathIndex_ < pathCells_.size()) {
            grid_.markPath(pathCells_[nextPathIndex_]);
            ++nextPathIndex_;
        } else {
            phase_ = Phase::Finished;
        }
    }
}

void Pathfinder::stop() {
    phase_ = Phase::Idle;
    foundPath_ = false;

    while (!bfsQueue_.empty()) {
        bfsQueue_.pop();
    }

    while (!dfsStack_.empty()) {
        dfsStack_.pop();
    }

    pathCells_.clear();
    nextPathIndex_ = 0;
}

bool Pathfinder::isRunning() const {
    return phase_ == Phase::Exploring || phase_ == Phase::DrawingPath;
}

bool Pathfinder::hasFinished() const {
    return phase_ == Phase::Finished;
}

bool Pathfinder::pathFound() const {
    return foundPath_;
}

void Pathfinder::setStepDelay(sf::Time delay) {
    stepDelay_ = delay;
}

void Pathfinder::setAlgorithm(Algorithm algorithm) {
    algorithm_ = algorithm;
}

Algorithm Pathfinder::getAlgorithm() const {
    return algorithm_;
}

int Pathfinder::toKey(const sf::Vector2i& cell) const {
    return cell.y * grid_.getCols() + cell.x;
}

bool Pathfinder::canVisit(const sf::Vector2i& cell) const {
    if (!grid_.inBounds(cell) || grid_.isObstacle(cell)) {
        return false;
    }

    return !discovered_[cell.y][cell.x];
}

void Pathfinder::stepExplore() {
    if (algorithm_ == Algorithm::BFS) {
        stepBfs();
    } else {
        stepDfs();
    }
}

void Pathfinder::stepBfs() {
    if (bfsQueue_.empty()) {
        finishWithoutPath();
        return;
    }

    const sf::Vector2i current = bfsQueue_.front();
    bfsQueue_.pop();

    if (current == grid_.getEnd()) {
        foundPath_ = true;
        preparePathAnimation();
        return;
    }

    static const sf::Vector2i directions[] = {
        sf::Vector2i(0, -1),
        sf::Vector2i(1, 0),
        sf::Vector2i(0, 1),
        sf::Vector2i(-1, 0),
    };

    for (const sf::Vector2i& direction : directions) {
        const sf::Vector2i next = current + direction;
        if (!canVisit(next)) {
            continue;
        }

        discovered_[next.y][next.x] = true;
        parent_[toKey(next)] = current;
        bfsQueue_.push(next);

        if (next != grid_.getStart() && next != grid_.getEnd()) {
            grid_.markVisited(next);
        }
    }
}

void Pathfinder::stepDfs() {
    if (dfsStack_.empty()) {
        finishWithoutPath();
        return;
    }

    const sf::Vector2i current = dfsStack_.top();
    dfsStack_.pop();

    if (current != grid_.getStart() && current != grid_.getEnd()) {
        grid_.markVisited(current);
    }

    if (current == grid_.getEnd()) {
        foundPath_ = true;
        preparePathAnimation();
        return;
    }

    // Reverse push order so DFS explores Up, Right, Down, Left.
    static const sf::Vector2i pushOrder[] = {
        sf::Vector2i(-1, 0),
        sf::Vector2i(0, 1),
        sf::Vector2i(1, 0),
        sf::Vector2i(0, -1),
    };

    for (const sf::Vector2i& direction : pushOrder) {
        const sf::Vector2i next = current + direction;
        if (!canVisit(next)) {
            continue;
        }

        discovered_[next.y][next.x] = true;
        parent_[toKey(next)] = current;
        dfsStack_.push(next);
    }
}

void Pathfinder::preparePathAnimation() {
    const sf::Vector2i start = grid_.getStart();
    const sf::Vector2i end = grid_.getEnd();

    if (start == end) {
        pathCells_.clear();
        phase_ = Phase::Finished;
        return;
    }

    std::vector<sf::Vector2i> reversedPath;
    sf::Vector2i current = end;

    while (current != start) {
        const auto parentIt = parent_.find(toKey(current));
        if (parentIt == parent_.end()) {
            finishWithoutPath();
            return;
        }

        if (current != end) {
            reversedPath.push_back(current);
        }
        current = parentIt->second;
    }

    pathCells_.assign(reversedPath.rbegin(), reversedPath.rend());
    nextPathIndex_ = 0;

    if (pathCells_.empty()) {
        phase_ = Phase::Finished;
    } else {
        phase_ = Phase::DrawingPath;
    }
}

void Pathfinder::finishWithoutPath() {
    foundPath_ = false;
    pathCells_.clear();
    nextPathIndex_ = 0;
    phase_ = Phase::Finished;
}
