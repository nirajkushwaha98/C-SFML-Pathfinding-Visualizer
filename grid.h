#pragma once

#include <SFML/Graphics.hpp>

#include <optional>
#include <vector>

struct Cell {
    bool obstacle = false;
    bool visited = false;
    bool inPath = false;
};

class Grid {
public:
    Grid(int rows, int cols, int cellSize);

    void reset();
    void setAllObstacles(bool blocked);
    void clearSearch();
    void generateMaze();

    int getRows() const;
    int getCols() const;
    int getCellSize() const;
    sf::Vector2u getPixelSize() const;

    bool inBounds(const sf::Vector2i& cell) const;
    bool isObstacle(const sf::Vector2i& cell) const;

    void setObstacle(const sf::Vector2i& cell, bool blocked);
    void setStart(const sf::Vector2i& cell);
    void setEnd(const sf::Vector2i& cell);

    bool hasStart() const;
    bool hasEnd() const;
    const sf::Vector2i& getStart() const;
    const sf::Vector2i& getEnd() const;

    void markVisited(const sf::Vector2i& cell);
    void markPath(const sf::Vector2i& cell);

    std::optional<sf::Vector2i> pixelToCell(const sf::Vector2i& pixel) const;

    void draw(sf::RenderWindow& window) const;

private:
    bool isSpecialNode(const sf::Vector2i& cell) const;
    sf::Vector2i findNearestOpenCell(const sf::Vector2i& target) const;

    int rows_;
    int cols_;
    int cellSize_;
    std::vector<std::vector<Cell>> cells_;
    sf::Vector2i start_;
    sf::Vector2i end_;
};
