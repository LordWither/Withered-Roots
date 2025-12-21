#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

sf::Vector2<float> doubleToFloat(sf::Vector2<double> inVector) {
    return sf::Vector2<float> {(float)inVector.x, (float)inVector.y};
}

sf::Vector2<double> cameraPos = sf::Vector2<double>(0.0f, 0.0f);
sf::Vector2<int> windowSize;
sf::Vector2<double> playerPos{ 0.0, 0.0 };
sf::Texture grassTexture = sf::Texture("Grass.png");
const int worldSize = 200;
const int tileSize = 100;
sf::Color dayTint = sf::Color::Color(255, 200, 100, 255U);
sf::Color nightTint = sf::Color::Color(29, 89, 255, 255U);
sf::Color colorTint = sf::Color::Color(255, 255, 255, 255U);
sf::Color fogColor = sf::Color::Color(128, 128, 128, 255U);
std::string mode = "Day";
double currentTime = 0.0f;
double deltaTime = 0.0f;
double combinedDeltaTime = 0.0f;
const double pi = 3.14159265358979323846;
double fogStartDay = 300.0;
double fogEndDay = 500.0;
double fogStartNight = 300.0 * 0.5;
double fogEndNight = 500.0 * 0.5;
double fogStart = 0.0;
double fogEnd = 0.0;

double Lerp(double Start, double End, double alpha) {
    return Start + (End - Start) * alpha;
}

sf::Color Multiply(sf::Color Start, sf::Color End) {
    return sf::Color::Color(
        (uint8_t)(((int)Start.r * (int)End.r) / 255),
        (uint8_t)(((int)Start.g * (int)End.g) / 255),
        (uint8_t)(((int)Start.b * (int)End.b) / 255),
        255U);
}

double clamp(double self, double min, double max) {
    if (self < min) {
        self = min;
    }
    if (self > max) {
        self = max;
    }
    return self;
}

sf::Color setTransparency(sf::Color self, double transparency) {
    self.a = (uint8_t)(transparency * 255);
    return self;
}

struct Tile {
    sf::RectangleShape fog = sf::RectangleShape({ (double)tileSize, (double)tileSize });
    sf::Vector2<int> originalPosition = { 0, 0 };
    sf::Vector2<double> position = { 0.0f, 0.0f };
    sf::Color color = sf::Color::Color(78, 180, 78, 100U);
    sf::Sprite Sprite = sf::Sprite(grassTexture, sf::IntRect({ 0, 0 }, {tileSize, tileSize}));
    void draw(sf::RenderWindow& window, sf::Vector2<double> cameraPos) {
        sf::Vector2<double> playerCentre = playerPos + sf::Vector2<double>(50.0, 50.0);
        sf::Vector2<double> fogCentre = (position + cameraPos) + (sf::Vector2<double>((double)tileSize, (double)tileSize) / 2.0);
        Sprite.setColor(colorTint);
        Sprite.setPosition(doubleToFloat(position + cameraPos));
        window.draw(Sprite);
        double dist = (fogCentre - playerCentre).length();
        if (dist > fogStart) {
            dist -= fogStart;
            fog.setFillColor(setTransparency(Multiply(fogColor, colorTint), clamp(dist / fogEnd, 0.0, 1.0)));
            fog.setPosition(doubleToFloat(position + cameraPos));
            window.draw(fog);
        }
    }
};

sf::Color getRanColor(){
    return sf::Color::Color(rand() % 255, rand() % 255, rand() % 255, 100U);
}

Tile tileMap[worldSize][worldSize];

sf::Color Lerp(sf::Color Start, sf::Color End, double alpha) {
    return sf::Color::Color(
        (uint8_t)((double)Start.r + ((double)End.r - (double)Start.r) * alpha),
        (uint8_t)((double)Start.g + ((double)End.g - (double)Start.g) * alpha),
        (uint8_t)((double)Start.b + ((double)End.b - (double)Start.b) * alpha),
        (uint8_t)((double)Start.a + ((double)End.a - (double)Start.a) * alpha)
    );
}

void initTileMap() {
    for (int y = 0; y < worldSize; y += 1) {
        for (int x = 0; x < worldSize; x += 1) {
            tileMap[y][x] = { sf::RectangleShape({tileSize, tileSize}), sf::Vector2<int>(tileSize * x, tileSize * y), sf::Vector2<double>((double)tileSize * x, (double)tileSize * y), getRanColor()};
        }
    }
}

int random(int min, int max) {
    int range = max - min + 1;
    return (rand() % range) + min;
}

double random() { //Return random double-precision float between 0 and 1, perfect for a whole lot of things
    return (double)random(0, 100) / 100.0;
}

sf::Vector2<double> snapToGrid(sf::Vector2<double> position) {
    return sf::Vector2<double>(floor(position.x / tileSize) * tileSize, floor(position.y / tileSize) * tileSize);
}

void drawTileMap(sf::RenderWindow &window) {
    int minX = clamp(floor(-cameraPos.x / tileSize) - 2, 0, worldSize - 1);
    int minY = clamp(floor(-cameraPos.y / tileSize) - 2, 0, worldSize - 1);
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

struct Inventory {
    int Cash = 100;
    int Beans = 1;
    int Bandages = 2;
    int chocolateBar = 1;
    void randomize() {
        Cash = random(50, 200);
        Beans = random(0, 1);
        Bandages = random(0, 2);
        chocolateBar = random(0, 1);
    }
};

struct Player {
    sf::RectangleShape sprite = sf::RectangleShape({50.0f, 50.0f});
    sf::Vector2<double> nonCentredPosition = { 0.0f, 0.0f };
    sf::Vector2<double> position = { 0.0f, 0.0f };
    sf::Vector2<double> boundsMax = sf::Vector2<double>(windowSize.x * 0.7, windowSize.y * 0.7);
    sf::Vector2<double> boundsMin = sf::Vector2<double>(windowSize.x * 0.3, windowSize.y * 0.3);;
    sf::Color color = sf::Color::Color(255, 255, 255, 255U);
    double speed = 350;
    void bareMove(sf::Vector2<double> newPosition) {
        position = newPosition;
        playerPos = newPosition;
    }
    void trueMove(sf::Vector2<double> offset) {
        position += offset;
        nonCentredPosition += offset;
        playerPos = position;
    }
    void move(sf::Vector2<double> offset, double deltaTime) {
        offset = offset.normalized() * (speed * deltaTime);
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
        cameraPos.x = clamp(cameraPos.x + offset.x, -(worldSize * tileSize - windowSize.x), 0);
        cameraPos.y = clamp(cameraPos.y + offset.y, -(worldSize * tileSize - windowSize.y), 0);
    }
    void draw(sf::RenderWindow &window) {
        sprite.setFillColor(Multiply(color, colorTint));
        sprite.setPosition(doubleToFloat(position));
        window.draw(sprite);
    }
};

double rad(double self) {
    return self * (pi / 180);
}

int rad(int self) {
    return self * (pi / 180);
}

double colorAlpha = 0.0f;
int main()
{
    srand(time(0));
    sf::VideoMode screenMode = sf::VideoMode::getFullscreenModes()[0];
    sf::RenderWindow window(sf::VideoMode(screenMode), "Withered Roots", sf::Style::Titlebar | sf::Style::Close);
    windowSize = sf::Vector2<int>(screenMode.size.x, screenMode.size.y);
    initTileMap();
    Player player{};
    player.bareMove((sf::Vector2<double>(screenMode.size.x / 2.0f - 25.0f, screenMode.size.y / 2.0f - 25.0f)));
    sf::Clock deltaTimer;
    deltaTimer.start();
    while (window.isOpen())
    {
        colorAlpha += (deltaTime * 50.0f);
        colorTint = dayTint;
        fogStart = (double)Lerp(fogStartDay, fogStartNight, sin(rad(colorAlpha)) * 0.5f + 0.5f);
        fogEnd = (double)Lerp(fogEndDay, fogEndNight, sin(rad(colorAlpha)) * 0.5f + 0.5f);
        colorTint = Lerp(dayTint, nightTint, sin(rad(colorAlpha)) * 0.5f + 0.5f);
        deltaTime = (double)deltaTimer.restart().asSeconds();
        combinedDeltaTime += deltaTime;
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        sf::Vector2<double> movementOffset = sf::Vector2<double>(0, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            window.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            movementOffset += { 1.0f, 0.0f };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            movementOffset += { -1.0f, 0.0f };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            movementOffset += { 0.0f, 1.0f };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            movementOffset += { 0.0f, -1.0f };
        }
        if (!FuzzyEq(movementOffset.length(), 0)) {
            player.move(movementOffset, deltaTime);
        }
        window.clear();
        drawTileMap(window);
        player.draw(window);
        window.display();
    }
}