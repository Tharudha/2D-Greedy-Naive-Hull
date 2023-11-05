#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

// ... (Point and Edge structs and other code)

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
                        int K = 5;
                        std::vector<Point> clusterCenters;
                        std::vector<std::vector<Point>> pointClusters(K);

                        for (int i = 0; i < K; i++) {
                            clusterCenters.push_back(userVertices[rand() % userVertices.size()]);
                        }

                        for (const Point& point : userVertices) {
                            int closestClusterIndex = 0;
                            double closestDistance = std::numeric_limits<double>::max();

                            for (int i = 0; i < K; i++) {
                                double distance = std::hypot(point.x - clusterCenters[i].x, point.y - clusterCenters[i].y);
                                if (distance < closestDistance) {
                                    closestDistance = distance;
                                    closestClusterIndex = i;
                                }
                            }

                            pointClusters[closestClusterIndex].push_back(point);
                        }

                        std::vector<std::vector<Edge>> clusterConvexHulls(K);
                        std::vector<sf::Color> clusterColors = {
                            sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta
                        };

                        for (int i = 0; i < K; i++) {
                            clusterConvexHulls[i] = computeConvexHull(pointClusters[i]);

                            for (const Edge &edge : clusterConvexHulls[i]) {
                                sf::VertexArray line(sf::Lines, 2);
                                line[0].position = sf::Vector2f(static_cast<float>(edge.start.x), static_cast<float>(edge.start.y));
                                line[1].position = sf::Vector2f(static_cast<float>(edge.end.x), static_cast<float>(edge.end.y));
                                line[0].color = clusterColors[i];
                                line[1].color = clusterColors[i];
                                window.draw(line);
                            }
                            window.display(); // Added for updating the display
                        }
                        std::cout << "Cluster convex hulls drawn." << std::endl; // Debugging output
                    }
                }
                else if (event.key.code == sf::Keyboard::P) {
                    if (inputMode == 'r' && !randomVertices.empty()) {
                        performHullPeel(randomVertices, window);
                    }
                    else if (inputMode == 'm' && !userVertices.empty()) {
                        performHullPeel(userVertices, window);
                    }
                    std::cout << "Hull peel performed." << std::endl; // Debugging output
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

        window.clear(sf::Color::White);

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
            line[0].color = sf::Color::Blue;
            line[1].color = sf::Color::Blue;
            window.draw(line);
        }

        window.display();
    }

    return 0;
}
