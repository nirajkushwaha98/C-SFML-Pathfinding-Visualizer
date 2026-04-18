#include "grid.h"

#include <array>
#include <queue>
#include <random>
#include <vector>

namespace {
const sf::Color kEmptyColor = sf::Color::White;
const sf::Color kObstacleColor = sf::Color(70, 74, 82);
const sf::Color kStartColor = sf::Color(60, 120, 255);
const sf::Color kEndColor = sf::Color(240, 70, 70);
const sf::Color kVisitedColor = sf::Color(250, 220, 80);
const sf::Color kPathColor = sf::Color(80, 200, 120);
} // namespace

Grid::Grid(int rows, int cols, int cellSize)
    : rows_(rows),
      cols_(cols),
      cellSize_(cellSize),
      cells_(rows, std::vector<Cell>(cols)),
      start_(-1, -1),
      end_(-1, -1) {}

void Grid::reset() {
    setAllObstacles(false);

    start_ = sf::Vector2i(-1, -1);
    end_ = sf::Vector2i(-1, -1);
}

void Grid::setAllObstacles(bool blocked) {
    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < cols_; ++col) {
            cells_[row][col] = Cell{};
            cells_[row][col].obstacle = blocked;
        }
    }
}

void Grid::clearSearch() {
    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < cols_; ++col) {
            cells_[row][col].visited = false;
            cells_[row][col].inPath = false;
        }
    }
}

void Grid::generateMaze() {
    reset();

    if (rows_ < 3 || cols_ < 3) {
        return;
    }

    setAllObstacles(true);

    std::vector<std::vector<bool>> mazeVisited(rows_, std::vector<bool>(cols_, false));
    std::vector<sf::Vector2i> stack;

    auto canUseAsMazeNode = [&](const sf::Vector2i& cell) {
        return cell.x > 0 && cell.x < cols_ - 1 && cell.y > 0 && cell.y < rows_ - 1;
    };

    sf::Vector2i current(1, 1);
    if (!canUseAsMazeNode(current)) {
        current = sf::Vector2i(0, 0);
    }

    cells_[current.y][current.x].obstacle = false;
    mazeVisited[current.y][current.x] = true;
    stack.push_back(current);

    std::mt19937 rng(static_cast<std::mt19937::result_type>(std::random_device{}()));
    const std::array<sf::Vector2i, 4> directions = {
        sf::Vector2i(2, 0),
        sf::Vector2i(-2, 0),
        sf::Vector2i(0, 2),
        sf::Vector2i(0, -2),
    };

    while (!stack.empty()) {
        current = stack.back();

        std::vector<sf::Vector2i> possibleNeighbors;
        for (const sf::Vector2i& direction : directions) {
            const sf::Vector2i next = current + direction;
            if (!canUseAsMazeNode(next)) {
                continue;
            }
            if (!mazeVisited[next.y][next.x]) {
                possibleNeighbors.push_back(next);
            }
        }

        if (possibleNeighbors.empty()) {
            stack.pop_back();
            continue;
        }

        std::uniform_int_distribution<std::size_t> pick(0, possibleNeighbors.size() - 1);
        const sf::Vector2i next = possibleNeighbors[pick(rng)];
        const sf::Vector2i between((current.x + next.x) / 2, (current.y + next.y) / 2);

        cells_[between.y][between.x].obstacle = false;
        cells_[next.y][next.x].obstacle = false;
        mazeVisited[next.y][next.x] = true;
        stack.push_back(next);
    }

    start_ = findNearestOpenCell(sf::Vector2i(1, 1));
    end_ = findNearestOpenCell(sf::Vector2i(cols_ - 2, rows_ - 2));

    if (start_ == end_) {
        end_ = findNearestOpenCell(sf::Vector2i(cols_ - 1, 0));
    }

    clearSearch();
}

int Grid::getRows() const {
    return rows_;
}

int Grid::getCols() const {
    return cols_;
}

int Grid::getCellSize() const {
    return cellSize_;
}

sf::Vector2u Grid::getPixelSize() const {
    return sf::Vector2u(static_cast<unsigned int>(cols_ * cellSize_),
                        static_cast<unsigned int>(rows_ * cellSize_));
}

bool Grid::inBounds(const sf::Vector2i& cell) const {
    return cell.x >= 0 && cell.x < cols_ && cell.y >= 0 && cell.y < rows_;
}

bool Grid::isObstacle(const sf::Vector2i& cell) const {
    if (!inBounds(cell)) {
        return true;
    }
    return cells_[cell.y][cell.x].obstacle;
}

void Grid::setObstacle(const sf::Vector2i& cell, bool blocked) {
    if (!inBounds(cell) || isSpecialNode(cell)) {
        return;
    }

    Cell& targetCell = cells_[cell.y][cell.x];
    targetCell.obstacle = blocked;

    if (blocked) {
        targetCell.visited = false;
        targetCell.inPath = false;
    }
}

void Grid::setStart(const sf::Vector2i& cell) {
    if (!inBounds(cell)) {
        return;
    }

    if (cell == end_) {
        end_ = sf::Vector2i(-1, -1);
    }

    cells_[cell.y][cell.x].obstacle = false;
    start_ = cell;
}

void Grid::setEnd(const sf::Vector2i& cell) {
    if (!inBounds(cell)) {
        return;
    }

    if (cell == start_) {
        start_ = sf::Vector2i(-1, -1);
    }

    cells_[cell.y][cell.x].obstacle = false;
    end_ = cell;
}

bool Grid::hasStart() const {
    return start_.x >= 0 && start_.y >= 0;
}

bool Grid::hasEnd() const {
    return end_.x >= 0 && end_.y >= 0;
}

const sf::Vector2i& Grid::getStart() const {
    return start_;
}

const sf::Vector2i& Grid::getEnd() const {
    return end_;
}

void Grid::markVisited(const sf::Vector2i& cell) {
    if (!inBounds(cell) || isSpecialNode(cell) || cells_[cell.y][cell.x].obstacle) {
        return;
    }

    cells_[cell.y][cell.x].visited = true;
}

void Grid::markPath(const sf::Vector2i& cell) {
    if (!inBounds(cell) || isSpecialNode(cell) || cells_[cell.y][cell.x].obstacle) {
        return;
    }

    cells_[cell.y][cell.x].inPath = true;
    cells_[cell.y][cell.x].visited = false;
}

std::optional<sf::Vector2i> Grid::pixelToCell(const sf::Vector2i& pixel) const {
    if (pixel.x < 0 || pixel.y < 0 || pixel.x >= cols_ * cellSize_ || pixel.y >= rows_ * cellSize_) {
        return std::nullopt;
    }

    return sf::Vector2i(pixel.x / cellSize_, pixel.y / cellSize_);
}

void Grid::draw(sf::RenderWindow& window) const {
    const float cellDrawSize = static_cast<float>(cellSize_);
    sf::RectangleShape rectangle(sf::Vector2f(cellDrawSize, cellDrawSize));

    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < cols_; ++col) {
            const sf::Vector2i cell(col, row);
            const Cell& data = cells_[row][col];

            sf::Color fill = kEmptyColor;
            if (cell == start_) {
                fill = kStartColor;
            } else if (cell == end_) {
                fill = kEndColor;
            } else if (data.inPath) {
                fill = kPathColor;
            } else if (data.obstacle) {
                fill = kObstacleColor;
            } else if (data.visited) {
                fill = kVisitedColor;
            }

            rectangle.setFillColor(fill);
            rectangle.setPosition(sf::Vector2f(static_cast<float>(col * cellSize_),
                                               static_cast<float>(row * cellSize_)));
            window.draw(rectangle);
        }
    }
}

bool Grid::isSpecialNode(const sf::Vector2i& cell) const {
    return cell == start_ || cell == end_;
}

sf::Vector2i Grid::findNearestOpenCell(const sf::Vector2i& target) const {
    if (!inBounds(target)) {
        return sf::Vector2i(0, 0);
    }

    if (!isObstacle(target)) {
        return target;
    }

    std::vector<std::vector<bool>> seen(rows_, std::vector<bool>(cols_, false));
    std::queue<sf::Vector2i> cellsToVisit;

    cellsToVisit.push(target);
    seen[target.y][target.x] = true;

    const std::array<sf::Vector2i, 4> directions = {
        sf::Vector2i(1, 0),
        sf::Vector2i(-1, 0),
        sf::Vector2i(0, 1),
        sf::Vector2i(0, -1),
    };

    while (!cellsToVisit.empty()) {
        const sf::Vector2i current = cellsToVisit.front();
        cellsToVisit.pop();

        if (!isObstacle(current)) {
            return current;
        }

        for (const sf::Vector2i& direction : directions) {
            const sf::Vector2i next = current + direction;
            if (!inBounds(next) || seen[next.y][next.x]) {
                continue;
            }
            seen[next.y][next.x] = true;
            cellsToVisit.push(next);
        }
    }

    return sf::Vector2i(0, 0);
}
