#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <filesystem>
#include <initializer_list>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

sf::Vector2<float> doubleToFloat(sf::Vector2<double> inVector) {
    return sf::Vector2<float> {(float)inVector.x, (float)inVector.y};
}

sf::Vector2<double> cameraPos = sf::Vector2<double>(0.0f, 0.0f);
sf::Vector2<int> windowSize;
sf::Vector2<double> playerPos{ 0.0, 0.0 };
sf::Texture grassTexture = sf::Texture("Assets\\Grass.png");
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
double fogStartDay = 450.0;
double fogEndDay = 700.0;
double fogStartNight = fogStartDay * 0.9;
double fogEndNight = fogEndDay * 0.9;
double fogStart = 0.0;
double fogEnd = 0.0;
std::filesystem::path parentDirectory = std::filesystem::current_path();
std::filesystem::path Assets = parentDirectory / "Assets";

std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::vector<sf::Texture>>>> animTextureCache = {
    {"Player", {
            {"Idle", {
                    {"Down", {}},
                    {"Left", {}},
                    {"Right", {}},
                    {"Up", {}}
                }
            },
            {"Walk", {
                    {"Down", {}},
                    {"Left", {}},
                    {"Right", {}},
                    {"Up", {}}
                }
            },
            {"Death", {
                    {"Down", {}},
                    {"Left", {}},
                    {"Right", {}},
                    {"Up", {}}
                }
            }
        }
    }
};

void initAnimDict() {
    if (!std::filesystem::exists(Assets)) {
        std::cerr << "Critical Failiure: Asset folder not found at " << Assets.string() << "\n";
        exit(0);
    }
    for (std::pair<const std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::vector<sf::Texture>>>>& i : animTextureCache) {
        std::filesystem::path currentDiri = Assets / i.first;
        if (!std::filesystem::exists(currentDiri)) {
            std::cout << "Missing Directory: " << currentDiri.string() << "\n";
            continue;
        };
        for (std::pair<const std::string, std::unordered_map<std::string, std::vector<sf::Texture>>>& ii : i.second) {
            std::filesystem::path currentDirii = currentDiri / ii.first;
            if (!std::filesystem::exists(currentDirii)) {
                std::cout << "Missing Directory: " << currentDirii.string() << "\n";
                continue;
            };
            for (std::pair<const std::string, std::vector<sf::Texture>>& iii : ii.second) {
                std::filesystem::path currentDiriii = currentDirii / iii.first;
                if (!std::filesystem::exists(currentDiriii)) {
                    std::cout << "Missing Directory: " << currentDiriii.string() << "\n";
                    continue;
                };
                for (const std::filesystem::directory_entry& iiii : std::filesystem::directory_iterator(currentDiriii)) {
                    if (iiii.is_regular_file()) {
                        std::string localPath = "Assets\\" + i.first + "\\" + ii.first + "\\" + iii.first + "\\" + iiii.path().filename().string();
                        iii.second.push_back(sf::Texture(localPath));
                    }
                }
            }
        }
    }
}

template <typename T>
T Lerp(T Start, T End, T alpha) {
    return Start + (End - Start) * alpha;
}

sf::Color Lerp(sf::Color Start, sf::Color End, double alpha) {
    return sf::Color::Color(
        (uint8_t)((double)Start.r + ((double)End.r - (double)Start.r) * alpha),
        (uint8_t)((double)Start.g + ((double)End.g - (double)Start.g) * alpha),
        (uint8_t)((double)Start.b + ((double)End.b - (double)Start.b) * alpha),
        (uint8_t)((double)Start.a + ((double)End.a - (double)Start.a) * alpha)
    );
}

sf::Color Multiply(sf::Color Start, sf::Color End, double influence = 1.0) {
    return sf::Color::Color(
        (uint8_t)(((int)Start.r * (int)(Lerp<double>(255, End.r, influence))) / 255),
        (uint8_t)(((int)Start.g * (int)(Lerp<double>(255, End.g, influence))) / 255),
        (uint8_t)(((int)Start.b * (int)(Lerp<double>(255, End.b, influence))) / 255),
        255U);
}

sf::Color setTransparency(sf::Color self, double transparency) {
    self.a = (uint8_t)(transparency * 255);
    return self;
}

bool FuzzyEq(sf::Vector2<double> self, sf::Vector2<double> other, double epsilon = 1e-5) {
    return (abs(self.x - other.x) <= epsilon && abs(self.y - other.y) <= epsilon);
}

struct Animation {
    double timePerFrame;
    double totalTime;
    bool Loop = true;
    std::vector<sf::Texture*> KeyFrames;
    Animation() : timePerFrame(0.0), totalTime(0.0) {};
    Animation(double time, std::string entityName, std::string animName, std::string variantName, bool loop = true) : timePerFrame(1.0 / time), Loop(loop) {
        if (!animTextureCache.contains(entityName) || !animTextureCache[entityName].contains(animName) || !animTextureCache[entityName][animName].contains(variantName)) return;
        std::vector<sf::Texture>& Data = animTextureCache[entityName][animName][variantName];
        for (int i = 0; i < Data.size(); i += 1) {
            KeyFrames.push_back(&Data[i]);
        }
        totalTime = KeyFrames.size() * (1.0 / time);
    };
    Animation(double time, std::initializer_list<sf::Texture*> args, bool loop = true) : timePerFrame(1.0 / time), KeyFrames(args), totalTime(args.size()* (1.0 / time)), Loop(loop) {};
};

struct Animator {
    double Alpha = 0;
    double currentTime = 0.0;
    int spriteIndex = 0;
    std::unordered_map<std::string, std::unordered_map<std::string, Animation>> Animations;
    std::string Anim = "";
    std::string Variant = "";
    std::string Entity = "";
    sf::Texture defaultTexture;
    sf::Vector2<double> scale = { 1.0, 1.0 };
    sf::Sprite animSprite = sf::Sprite(defaultTexture, sf::IntRect({ 0, 0 }, { 1, 1 }));
    Animator() {};
    Animator(std::string entityName, sf::Vector2i size = { 1, 1 }, double FPS = 30.0, sf::Vector2i offset = { 0, 0 }) {
        animSprite = sf::Sprite(defaultTexture, sf::IntRect(offset, size));
        Entity = entityName;
        if (animTextureCache.contains(entityName)) {
            for ( std::pair<const std::string, std::unordered_map<std::string, std::vector<sf::Texture>>>& i : animTextureCache[entityName]) {
                for (std::pair<const std::string, std::vector<sf::Texture>>& ii : i.second) {
                    Animations[i.first][ii.first] = Animation(FPS, entityName, i.first, ii.first, i.first != "Death");
                }
            }
        }
    };
    void addAnimation(std::string name, std::string variant, Animation data) {
        Animations[name][variant] = data;
    }
    void addAnimation(std::string name, std::string variant, double FPS = 30.0) {
        Animations[name][variant] = Animation(FPS, Entity, name, variant);
    }
    void changeAnim(std::string anim, std::string variant) {
        if (!Animations.contains(anim) || !Animations[anim].contains(variant)) return;
        if (Anim == anim && Variant == variant) return;
        if (Anim != anim) {
            Anim = anim;
            currentTime = 0.0;
        }
        Variant = variant;
    }
    void draw(sf::RenderWindow& window, sf::Vector2<double> position, sf::Vector2<double> spriteScale = {1.0, 1.0}) {
        sf::Texture* currentTexture = &defaultTexture;
        if (Animations.contains(Anim) && Animations[Anim].contains(Variant)) {
            Animation& data = Animations[Anim][Variant];
            currentTime = currentTime + deltaTime;
            if (currentTime >= data.totalTime && data.Loop) {
                currentTime = 0.0;
            }
            int index = (int)(currentTime / data.timePerFrame);
            if (index < 0)
                index = 0;
            if (index > data.KeyFrames.size() - 1)
                index = data.KeyFrames.size() - 1;
            currentTexture = data.KeyFrames[index];
        }
        animSprite.setTexture(*currentTexture);
        animSprite.setColor(Lerp(sf::Color::Color(255, 255, 255, 255), colorTint, 0.5));
        if (!FuzzyEq(scale, spriteScale)) {
            scale = spriteScale;
            animSprite.setScale(doubleToFloat(scale));
        }
        animSprite.setPosition(doubleToFloat(position));
        window.draw(animSprite);
    }
};

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
            fog.setFillColor(setTransparency(Multiply(fogColor, colorTint, 0.3), std::clamp(dist / fogEnd, 0.0, 1.0)));
            fog.setPosition(doubleToFloat(position + cameraPos));
            window.draw(fog);
        }
    }
};

sf::Color getRanColor(){
    return sf::Color::Color(rand() % 255, rand() % 255, rand() % 255, 100U);
}

Tile tileMap[worldSize][worldSize];

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
    int minX = std::clamp<int>(floor(-cameraPos.x / tileSize) - 2, 0, worldSize - 1);
    int minY = std::clamp<int>(floor(-cameraPos.y / tileSize) - 2, 0, worldSize - 1);
    int maxX = std::clamp<int>(ceil((-cameraPos.x + windowSize.x) / tileSize), 0, worldSize - 1);
    int maxY = std::clamp<int>(ceil((-cameraPos.y + windowSize.y) / tileSize), 0, worldSize - 1);
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
    sf::Vector2<double> nonCentredPosition = { 0.0f, 0.0f };
    sf::Vector2<double> position = { 0.0f, 0.0f };
    double Health = 100.0;
    std::string Anim = "Idle";
    std::string animVariant = "Down";
    double speed = 250.0;
    double speedToAnimFPSRatio = 5.0 / 200.0;
    sf::Vector2<double> boundsMax = sf::Vector2<double>(windowSize.x * 0.7, windowSize.y * 0.7);
    sf::Vector2<double> boundsMin = sf::Vector2<double>(windowSize.x * 0.3, windowSize.y * 0.3);;
    sf::Color color = sf::Color::Color(255, 255, 255, 255U);
    Animator animator = Animator("Player", { 48, 64 }, speedToAnimFPSRatio * speed);
    Player() {
        animator.Anim = Anim;
        animator.Variant = animVariant;
    } //Keeping here in case I wanna init add code, causes no issues if I leave it here
    void bareMove(sf::Vector2<double> newPosition) {
        position = newPosition;
        playerPos = newPosition;
    }
    void trueMove(sf::Vector2<double> offset) {
        position += offset;
        nonCentredPosition += offset;
        playerPos = position;
    }
    void changeAnim(std::string anim, std::string variant) {
        Anim = anim;
        animVariant = variant;
        animator.changeAnim(anim, variant);
    }
    void move(sf::Vector2<double> offset, double deltaTime) {
        if (Health <= 0.0) {
            if (Anim != "Death") {
                changeAnim("Death", animVariant);
            }
            return;
        }
        if (FuzzyEq(offset.x, 0) && FuzzyEq(offset.y, 0)) {
            if (Anim != "Idle") {
                changeAnim("Idle", animVariant);
            }
            return;
        }
        offset = offset.normalized() * (speed * deltaTime);
        sf::Vector2<double> realOffset = sf::Vector2<double>(-offset.x, -offset.y);
        if (realOffset.y < 0) {
            changeAnim("Walk", "Up");
        }
        else if (!FuzzyEq(realOffset.y, 0)) {
            changeAnim("Walk", "Down");
        }//X variants take priority over Y, this is so they play when moving diagonally
        if (realOffset.x < 0) {
            changeAnim("Walk", "Left");
        }
        else if (!FuzzyEq(realOffset.x, 0)) {
            changeAnim("Walk", "Right");
        }

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
        cameraPos.x = std::clamp<double>(cameraPos.x + offset.x, -(worldSize * tileSize - windowSize.x), 0);
        cameraPos.y = std::clamp<double>(cameraPos.y + offset.y, -(worldSize * tileSize - windowSize.y), 0);
    }
    void draw(sf::RenderWindow &window) {
        animator.draw(window, position, {2.5, 2.5});
        Health -= (10.0 * deltaTime);
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
    initAnimDict();
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
            movementOffset += { 1.0, 0.0 };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            movementOffset += { -1.0, 0.0 };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            movementOffset += { 0.0, 1.0 };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            movementOffset += { 0.0, -1.0 };
        }
        player.move(movementOffset, deltaTime);
        window.clear();
        drawTileMap(window);
        player.draw(window);
        window.display();
    }
}