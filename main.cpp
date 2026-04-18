#include "grid.h"
#include "pathfinding.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <optional>
#include <string>

enum class PlacementMode {
    Obstacles,
    PlaceStart,
    PlaceEnd
};

namespace {
int parsePositiveInt(const char* text, int fallback) {
    try {
        const int value = std::stoi(text);
        return value > 0 ? value : fallback;
    } catch (...) {
        return fallback;
    }
}

std::string algorithmToString(Algorithm algorithm) {
    switch (algorithm) {
        case Algorithm::BFS:
            return "BFS";
        case Algorithm::DFS:
            return "DFS";
        default:
            return "Unknown";
    }
}
} // namespace

int main(int argc, char** argv) {
    int rows = 41;
    int cols = 51;
    int cellSize = 13;

    // Optional command line usage: ./app [rows cols cellSize]
    if (argc >= 3) {
        rows = parsePositiveInt(argv[1], rows);
        cols = parsePositiveInt(argv[2], cols);
    }
    if (argc >= 4) {
        cellSize = parsePositiveInt(argv[3], cellSize);
    }

    Grid grid(rows, cols, cellSize);
    grid.generateMaze();

    sf::RenderWindow window(
        sf::VideoMode(grid.getPixelSize()),
        "Maze Graphics",
        sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    Pathfinder pathfinder(grid);
    pathfinder.setStepDelay(sf::milliseconds(35));

    Algorithm selectedAlgorithm = Algorithm::BFS;
    pathfinder.setAlgorithm(selectedAlgorithm);

    PlacementMode placementMode = PlacementMode::Obstacles;

    auto updateWindowTitle = [&]() {
        std::string title =
            "Maze Graphics | Algo: " + algorithmToString(selectedAlgorithm) +
            " (1=BFS, 2=DFS) | S=start E=end Space=run M=maze R=reset";

        if (placementMode == PlacementMode::PlaceStart) {
            title += " | Click to place START";
        } else if (placementMode == PlacementMode::PlaceEnd) {
            title += " | Click to place END";
        }

        window.setTitle(title);
    };

    auto applyMouseAction = [&](const sf::Vector2i& pixelPosition, sf::Mouse::Button button) {
        const auto cellOpt = grid.pixelToCell(pixelPosition);
        if (!cellOpt.has_value() || pathfinder.isRunning()) {
            return;
        }

        const sf::Vector2i cell = *cellOpt;

        if (button == sf::Mouse::Button::Left) {
            if (placementMode == PlacementMode::PlaceStart) {
                grid.setStart(cell);
                grid.clearSearch();
                placementMode = PlacementMode::Obstacles;
                updateWindowTitle();
                return;
            }

            if (placementMode == PlacementMode::PlaceEnd) {
                grid.setEnd(cell);
                grid.clearSearch();
                placementMode = PlacementMode::Obstacles;
                updateWindowTitle();
                return;
            }

            grid.setObstacle(cell, true);
            grid.clearSearch();
        } else if (button == sf::Mouse::Button::Right) {
            placementMode = PlacementMode::Obstacles;
            grid.setObstacle(cell, false);
            grid.clearSearch();
            updateWindowTitle();
        }
    };

    updateWindowTitle();

    std::cout << "Controls:\n"
              << "  Left click  : Add obstacles\n"
              << "  Right click : Remove obstacles\n"
              << "  S           : Place start node\n"
              << "  E           : Place end node\n"
              << "  Space       : Run selected algorithm\n"
              << "  M           : Generate a new random maze\n"
              << "  R           : Reset entire grid\n"
              << "  1           : BFS mode\n"
              << "  2           : DFS mode\n"
              << "  Esc         : Exit\n";

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                switch (keyPressed->code) {
                    case sf::Keyboard::Key::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::Key::S:
                        if (!pathfinder.isRunning()) {
                            placementMode = PlacementMode::PlaceStart;
                            updateWindowTitle();
                        }
                        break;
                    case sf::Keyboard::Key::E:
                        if (!pathfinder.isRunning()) {
                            placementMode = PlacementMode::PlaceEnd;
                            updateWindowTitle();
                        }
                        break;
                    case sf::Keyboard::Key::R:
                        pathfinder.stop();
                        grid.reset();
                        placementMode = PlacementMode::Obstacles;
                        updateWindowTitle();
                        break;
                    case sf::Keyboard::Key::M:
                        if (!pathfinder.isRunning()) {
                            grid.generateMaze();
                            placementMode = PlacementMode::Obstacles;
                            updateWindowTitle();
                        }
                        break;
                    case sf::Keyboard::Key::Space:
                        if (!pathfinder.start()) {
                            std::cout << "Set both start and end nodes before running.\n";
                        }
                        placementMode = PlacementMode::Obstacles;
                        updateWindowTitle();
                        break;
                    case sf::Keyboard::Key::Num1:
                    case sf::Keyboard::Key::Numpad1:
                        if (!pathfinder.isRunning()) {
                            selectedAlgorithm = Algorithm::BFS;
                            pathfinder.setAlgorithm(selectedAlgorithm);
                            updateWindowTitle();
                        }
                        break;
                    case sf::Keyboard::Key::Num2:
                    case sf::Keyboard::Key::Numpad2:
                        if (!pathfinder.isRunning()) {
                            selectedAlgorithm = Algorithm::DFS;
                            pathfinder.setAlgorithm(selectedAlgorithm);
                            updateWindowTitle();
                        }
                        break;
                    default:
                        break;
                }
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                applyMouseAction(mousePressed->position, mousePressed->button);
            }
        }

        if (!pathfinder.isRunning() && placementMode == PlacementMode::Obstacles) {
            const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                applyMouseAction(mousePos, sf::Mouse::Button::Left);
            } else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                applyMouseAction(mousePos, sf::Mouse::Button::Right);
            }
        }

        pathfinder.update();

        window.clear(sf::Color(210, 210, 210));
        grid.draw(window);
        window.display();
    }

    return 0;
}
