#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

sf::Vector2<float> doubleToFloat(sf::Vector2<double> inVector) {
    return sf::Vector2<float> {(float)inVector.x, (float)inVector.y};
}

sf::Vector2<double> cameraPos = sf::Vector2<double>(0.0f, 0.0f);
sf::Vector2<int> windowSize;
sf::Texture grassTexture = sf::Texture("Grass.png");
const int worldSize = 50;

struct Tile {
    sf::RectangleShape sprite = sf::RectangleShape({ 100.0f, 100.0f });
    sf::Vector2<int> originalPosition = { 0, 0 };
    sf::Vector2<double> position = { 0.0f, 0.0f };
    sf::Color color = sf::Color::Color(78, 180, 78, 100U);
    sf::Sprite Sprite = sf::Sprite(grassTexture, sf::IntRect({ 0, 0 }, {100, 100}));
    void draw(sf::RenderWindow& window, sf::Vector2<double> cameraPos) {
        //sprite.setFillColor(color);
        Sprite.setPosition(doubleToFloat(position + cameraPos));
        window.draw(Sprite);
    }
};

sf::Color getRanColor(){
    return sf::Color::Color(rand() % 255, rand() % 255, rand() % 255, 100U);
}

Tile tileMap[worldSize][worldSize];

void initTileMap() {
    int tileSizeX = 100;
    int tileSizeY = 100;
    for (int y = 0; y < worldSize; y += 1) {
        for (int x = 0; x < worldSize; x += 1) {
            tileMap[y][x] = { sf::RectangleShape({150.0f, 150.0f}), sf::Vector2<int>((int)tileSizeX * x, (int)tileSizeY * y), sf::Vector2<double>(tileSizeX * x, tileSizeY * y), getRanColor()};
        }
    }
}

int clamp(int val, int min, int max) {
    if (val < min) {
        val = min;
    }
    if (val > max) {
        val = max;
    }
    return val;
}

sf::Vector2<double> snapToGrid(sf::Vector2<double> position) {
    return sf::Vector2<double>(floor(position.x / 100.0f) * 100.0f, floor(position.y / 100.0f) * 100.0f);
}

void drawTileMap(sf::RenderWindow &window) {
    double tileSize = 100.0f;
    int minX = clamp(floor(-cameraPos.x / (tileSize * 2)), 0, worldSize - 1);
    int minY = clamp(floor(-cameraPos.y / (tileSize * 2)), 0, worldSize - 1);
    int maxX = clamp(ceil((-cameraPos.x + windowSize.x) / tileSize), 0, worldSize - 1);
    int maxY = clamp(ceil((-cameraPos.y + windowSize.y) / tileSize), 0, worldSize - 1);
    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            tileMap[y][x].draw(window, cameraPos);
        }
    }
}

bool FuzzyEq(double self, double other, double epsilon = 1e-5) {
    return (abs(self - other) <= epsilon);
}

struct Player {
    sf::RectangleShape sprite = sf::RectangleShape({50.0f, 50.0f});
    sf::Vector2<double> nonCentredPosition = { 0.0f, 0.0f };
    sf::Vector2<double> position = { 0.0f, 0.0f };
    sf::Vector2<double> boundsMax = sf::Vector2<double>(windowSize.x * 0.9, windowSize.y * 0.9);
    sf::Vector2<double> boundsMin = sf::Vector2<double>(windowSize.x * 0.1, windowSize.y * 0.1);;
    sf::Color color = sf::Color::Color(255, 255, 255, 255U);
    double speed = 250;
    void trueMove(sf::Vector2<double> offset) {
        position += offset;
        nonCentredPosition += offset;
    }
    void move(sf::Vector2<double> offset) {
        sf::Vector2<double> realOffset = sf::Vector2<double>(-offset.x, -offset.y);
        bool scrollX =
            (realOffset.x < 0 && position.x <= boundsMin.x) ||
            (realOffset.x > 0 && position.x >= boundsMax.x);

        bool scrollY =
            (realOffset.y < 0 && position.y <= boundsMin.y) ||
            (realOffset.y > 0 && position.y >= boundsMax.y);
        if (!scrollX && !scrollY) {
            trueMove(realOffset);
            return;
        }
        cameraPos += offset;
    }
    void draw(sf::RenderWindow &window) {
        sprite.setFillColor(color);
        sprite.setPosition(doubleToFloat(position));
        window.draw(sprite);
    }
};


int main()
{
    srand(time(0));
    sf::VideoMode screenMode = sf::VideoMode::getFullscreenModes()[0];
    sf::RenderWindow window(sf::VideoMode(screenMode), "SFML works!", sf::Style::None);
    windowSize = sf::Vector2<int>(screenMode.size.x, screenMode.size.y);
    initTileMap();
    Player player{};
    player.position = (sf::Vector2<double>(screenMode.size.x / 2.0f - 25.0f, screenMode.size.y / 2.0f - 25.0f));
    sf::Clock deltaTimer;
    deltaTimer.start();
    while (window.isOpen())
    {
        double deltaTime = (double)deltaTimer.restart().asSeconds();
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            window.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            player.move({ player.speed * deltaTime, 0.0f });
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            player.move({ -player.speed * deltaTime, 0.0f });
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            player.move({ 0.0f, player.speed * deltaTime });
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            player.move({ 0.0f, -player.speed * deltaTime });
        }

        window.clear();
        drawTileMap(window);
        player.draw(window);
        window.display();
    }
}