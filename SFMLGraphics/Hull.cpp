#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cmath>

struct Point {
    int x, y;

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct Edge {
    Point start, end;
};

int orientation(Point p, Point q, Point r) {
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;
    return (val > 0) ? 1 : 2;
}

std::vector<Edge> computeConvexHull(const std::vector<Point>& points) {
    std::vector<Edge> hullEdges;
    size_t n = points.size();

    if (n < 3) {
        std::cerr << "Convex hull not possible with less than 3 points." << std::endl;
        return hullEdges;
    }

    std::vector<Point> sortedPoints = points;
    std::sort(sortedPoints.begin(), sortedPoints.end(), [](const Point& a, const Point& b) {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
        });

    std::vector<Point> convexHull;
    convexHull.push_back(sortedPoints[0]);
    convexHull.push_back(sortedPoints[1]);

    for (size_t i = 2; i < n; i++) {
        convexHull.push_back(sortedPoints[i]);

        while (convexHull.size() >= 3 && orientation(convexHull[convexHull.size() - 3], convexHull[convexHull.size() - 2], convexHull[convexHull.size() - 1]) != 2) {
            convexHull.erase(convexHull.end() - 2);
        }
    }

    for (size_t i = 0; i < convexHull.size() - 1; i++) {
        hullEdges.push_back({ convexHull[i], convexHull[i + 1] });
    }
    hullEdges.push_back({ convexHull.back(), convexHull.front() });

    return hullEdges;
}

void performHullPeel(std::vector<Point>& points, sf::RenderWindow& window, sf::Color peelColor) {
    int peelCount = 0;

    while (points.size() >= 3) {
        std::vector<Edge> peelEdges = computeConvexHull(points);

        std::cout << "Peel #" << peelCount << ": Peel Edges Count: " << peelEdges.size() << std::endl;

        for (const Edge& edge : peelEdges) {
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = sf::Vector2f(static_cast<float>(edge.start.x), static_cast<float>(edge.start.y));
            line[1].position = sf::Vector2f(static_cast<float>(edge.end.x), static_cast<float>(edge.end.y));
            line[0].color = peelColor;
            line[1].color = peelColor;
            window.draw(line);
        }

        std::vector<Point> remainingPoints;
        for (const Point& point : points) {
            auto it = std::find_if(peelEdges.begin(), peelEdges.end(), [&point](const Edge& edge) {
                return edge.start == point || edge.end == point;
                });
            if (it == peelEdges.end()) {
                remainingPoints.push_back(point);
            }
        }

        std::cout << "Peel #" << peelCount << ": Remaining Points Count: " << remainingPoints.size() << std::endl;

        points = remainingPoints;
        peelCount++;

        window.display();  // Display the peels on the window
    }
}

void performPeelsOnClusters(const std::vector<Point>& points, int K, sf::RenderWindow& window) {
    std::vector<sf::Color> peelColors = {
        sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta
    };

    std::vector<Point> remainingPoints = points;

    for (int i = 0; i < K; i++) {
        if (remainingPoints.empty()) {
            break;
        }

        std::vector<Point> cluster;
        Point randomPoint = remainingPoints[rand() % remainingPoints.size()];
        cluster.push_back(randomPoint);

        // Remove the selected point from remainingPoints
        remainingPoints.erase(std::remove(remainingPoints.begin(), remainingPoints.end(), randomPoint), remainingPoints.end());

        // Expand the cluster by finding the closest points
        while (!remainingPoints.empty()) {
            int closestPointIndex = -1;
            int closestDistance = std::numeric_limits<int>::max();

            for (size_t j = 0; j < remainingPoints.size(); j++) {
                int distance = std::abs(cluster[i].x - remainingPoints[j].x) + std::abs(cluster[i].y - remainingPoints[j].y);

                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestPointIndex = j;
                }
            }

            if (closestPointIndex >= 0) {
                cluster.push_back(remainingPoints[closestPointIndex]);
                remainingPoints.erase(remainingPoints.begin() + closestPointIndex);
            }
        }

        // Perform hull peel on the current cluster with the assigned color
        performHullPeel(cluster, window, peelColors[i]);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Convex Hull Computation");
    std::vector<Point> randomVertices;
    std::vector<Point> userVertices;
    std::vector<Edge> convexHullEdges;
    char inputMode = 'r';

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    inputMode = 'r';
                    randomVertices.clear();
                    for (int i = 0; i < 100; i++) {
                        randomVertices.push_back({ rand() % 800, rand() % 600 });
                    }
                    convexHullEdges = computeConvexHull(randomVertices);
                }
                else if (event.key.code == sf::Keyboard::M) {
                    inputMode = 'm';
                    userVertices.clear();
                    convexHullEdges.clear();
                }
                else if (event.key.code == sf::Keyboard::C) {
                    if (inputMode == 'm' && !userVertices.empty()) {
                        convexHullEdges = computeConvexHull(userVertices);
                    }
                }
                else if (event.key.code == sf::Keyboard::H) {
                    if (inputMode == 'm' && !userVertices.empty()) {
                        int K;
                        std::cout << "Enter the value of K: ";
                        std::cin >> K;
                        performPeelsOnClusters(userVertices, K, window);
                    }
                }
                else if (event.key.code == sf::Keyboard::P) {
                    if (inputMode == 'r' and !randomVertices.empty()) {
                        performHullPeel(randomVertices, window, sf::Color::Cyan);
                    }
                    else if (inputMode == 'm' and !userVertices.empty()) {
                        performHullPeel(userVertices, window, sf::Color::Cyan);
                    }
                }
            }
            else if (inputMode == 'm' && event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    Point newPoint = { event.mouseButton.x, event.mouseButton.y };

                    if (std::find(userVertices.begin(), userVertices.end(), newPoint) == userVertices.end()) {
                        userVertices.push_back(newPoint);
                    }
                }
            }
        }

        window.clear();

        if (inputMode == 'r') {
            for (const Point& vertex : randomVertices) {
                sf::CircleShape circle(3);
                circle.setFillColor(sf::Color::Blue);
                circle.setPosition(static_cast<float>(vertex.x), static_cast<float>(vertex.y));
                window.draw(circle);
            }
        }
        else if (inputMode == 'm') {
            for (const Point& vertex : userVertices) {
                sf::CircleShape circle(3);
                circle.setFillColor(sf::Color::Red);
                circle.setPosition(static_cast<float>(vertex.x), static_cast<float>(vertex.y));
                window.draw(circle);
            }
        }

        for (const Edge& edge : convexHullEdges) {
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = sf::Vector2f(static_cast<float>(edge.start.x), static_cast<float>(edge.start.y));
            line[1].position = sf::Vector2f(static_cast<float>(edge.end.x), static_cast<float>(edge.end.y));
            line[0].color = sf::Color::Green;
            line[1].color = sf::Color::Green;
            window.draw(line);
        }

        window.display();
    }

    return 0;
}
