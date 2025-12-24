#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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
sf::Music dayTheme("Assets\\DayTheme.ogg");
const int worldSize = 200;
const int tileSize = 100;
sf::Color dayTint = sf::Color::Color(255, 200, 100, 255U);
sf::Color nightTint = sf::Color::Color(29, 89, 255, 255U);
sf::Color colorTint = sf::Color::Color(255, 255, 255, 255U);
sf::Color fogColor = sf::Color::Color(128, 128, 128, 255U);
std::string Mode = "Day";
double currentTime = 0.0f;
double deltaTime = 0.0f;
double combinedDeltaTime = 0.0f;
const double pi = 3.14159265358979323846;
double fogStartDay = 450.0;
double fogEndDay = 1300.0;
double fogStartNight = fogStartDay * 0.9;
double fogEndNight = fogEndDay * 0.9;
double dayTime = 20.0;
double nightTime = 120.0;
double fogStart = 0.0;
double fogEnd = 0.0;
double gameTimer = 0.0;
sf::Music Halloween1("Assets\\Halloween1.ogg");
sf::Music Halloween2("Assets\\Halloween2.ogg");
sf::Music Halloween3("Assets\\Halloween3.ogg");
sf::Music HalloweenGhost("Assets\\HalloweenGhost.wav");
std::filesystem::path parentDirectory = std::filesystem::current_path();
std::filesystem::path Assets = parentDirectory / "Assets";

template<typename arrType>
struct dynArray {
    arrType* Core = nullptr;
    int length = 0;
    int occupiedLength = 0;
    dynArray() : Core(nullptr) {
        length = 1;
        if (Core != nullptr) delete[] Core;
        Core = new arrType[length];
        occupiedLength = 0;
    };
    dynArray(arrType* initialSet, int setLength) : Core(nullptr) {
        if (initialSet == nullptr || setLength <= 0) {
            return;
        }
        if (Core != nullptr) delete[] Core;
        length = setLength;
        occupiedLength = setLength;
        Core = initialSet;
    }
    void resize(int newSize) {
        length = newSize;
        arrType* tmp = new arrType[length];
        int iterator;
        if (occupiedLength <= length)
            iterator = occupiedLength;
        else
            iterator = length;
        occupiedLength = 0;
        for (int i = 0; i < iterator; i += 1) {
            tmp[i] = Core[i];
            occupiedLength += 1;
        }
        delete[] Core;
        Core = tmp;
    }
    void insert(arrType data) {
        if (occupiedLength < length) {
            Core[occupiedLength] = data;
            occupiedLength += 1;
            return;
        }
        length *= 2;
        arrType* tmp = new arrType[length];
        for (int i = 0; i < occupiedLength; i += 1) {
            tmp[i] = Core[i];
        }
        tmp[occupiedLength] = data;
        occupiedLength += 1;
        delete[] Core;
        Core = tmp;
    }
    void remove(int Index) {
        if (Index >= occupiedLength) return;
        length -= 1;
        arrType* tmp = new arrType[length];
        for (int i = 0; i < occupiedLength; i += 1) {
            if (i < Index)
                tmp[i] = Core[i];
            else if (i > Index)
                tmp[i - 1] = Core[i];
        }
        occupiedLength -= 1;
        delete[] Core;
        Core = tmp;
    }
    int size() {
        return occupiedLength;
    }
    int find(arrType item) {
        for (int i = 0; i < occupiedLength; i += 1) {
            if (Core[i] == item) {
                return i;
            }
        }
        return -1;
    }
    bool contains(arrType item) {
        for (int i = 0; i < occupiedLength; i += 1) {
            if (Core[i] == item) {
                return true;
            }
        }
        return false;
    }
    arrType& operator[](int index) {
        return Core[index];
    }
    dynArray(const dynArray& other) {
        length = other.length;
        occupiedLength = other.occupiedLength;
        Core = new arrType[length];
        for (int i = 0; i < occupiedLength; i += 1) {
            Core[i] = other.Core[i];
        }
    };
    dynArray& operator=(const dynArray& other) {
        if (this == &other) return *this;
        delete[] Core;
        length = other.length;
        occupiedLength = other.occupiedLength;
        Core = new arrType[length];
        for (int i = 0; i < occupiedLength; i += 1) {
            Core[i] = other.Core[i];
        }
        return *this;
    };
    ~dynArray() {
        delete[] Core;
        Core = nullptr;
        length = 0;
        occupiedLength = 0;
    }
};

dynArray<std::string> animNames;
dynArray<dynArray<sf::Texture>> animData;

void initAnims() {
    dynArray<sf::Texture> playerIdleUp = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Idle\\Up\\Idle1.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Up\\Idle2.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Up\\Idle3.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Up\\Idle4.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Up\\Idle5.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Up\\Idle6.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Up\\Idle7.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Up\\Idle8.png")}, 8 };
    dynArray<sf::Texture> playerIdleDown = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Idle\\Down\\Idle1.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Down\\Idle2.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Down\\Idle3.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Down\\Idle4.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Down\\Idle5.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Down\\Idle6.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Down\\Idle7.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Down\\Idle8.png")}, 8 };
    dynArray<sf::Texture> playerIdleLeft = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Idle\\Left\\Idle1.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Left\\Idle2.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Left\\Idle3.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Left\\Idle4.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Left\\Idle5.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Left\\Idle6.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Left\\Idle7.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Left\\Idle8.png")}, 8 };
    dynArray<sf::Texture> playerIdleRight = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Idle\\Right\\Idle1.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Right\\Idle2.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Right\\Idle3.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Right\\Idle4.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Right\\Idle5.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Right\\Idle6.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Right\\Idle7.png"), sf::Texture("Assets\\Animations\\Player\\Idle\\Right\\Idle8.png")}, 8 };
    dynArray<sf::Texture> playerWalkUp = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Walk\\Up\\Walk1.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Up\\Walk2.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Up\\Walk3.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Up\\Walk4.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Up\\Walk5.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Up\\Walk6.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Up\\Walk7.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Up\\Walk8.png")}, 8 };
    dynArray<sf::Texture> playerWalkDown = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Walk\\Down\\Walk1.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Down\\Walk2.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Down\\Walk3.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Down\\Walk4.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Down\\Walk5.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Down\\Walk6.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Down\\Walk7.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Down\\Walk8.png")}, 8 };
    dynArray<sf::Texture> playerWalkLeft = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Walk\\Left\\Walk1.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Left\\Walk2.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Left\\Walk3.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Left\\Walk4.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Left\\Walk5.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Left\\Walk6.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Left\\Walk7.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Left\\Walk8.png")}, 8 };
    dynArray<sf::Texture> playerWalkRight = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Walk\\Right\\Walk1.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Right\\Walk2.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Right\\Walk3.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Right\\Walk4.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Right\\Walk5.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Right\\Walk6.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Right\\Walk7.png"), sf::Texture("Assets\\Animations\\Player\\Walk\\Right\\Walk8.png")}, 8 };
    dynArray<sf::Texture> playerDeathUp = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Death\\Up\\Death1.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Up\\Death2.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Up\\Death3.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Up\\Death4.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Up\\Death5.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Up\\Death6.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Up\\Death7.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Up\\Death8.png")}, 8 };
    dynArray<sf::Texture> playerDeathDown = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Death\\Down\\Death1.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Down\\Death2.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Down\\Death3.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Down\\Death4.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Down\\Death5.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Down\\Death6.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Down\\Death7.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Down\\Death8.png")}, 8 };
    dynArray<sf::Texture> playerDeathLeft = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Death\\Left\\Death1.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Left\\Death2.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Left\\Death3.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Left\\Death4.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Left\\Death5.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Left\\Death6.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Left\\Death7.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Left\\Death8.png")}, 8 };
    dynArray<sf::Texture> playerDeathRight = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Player\\Death\\Right\\Death1.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Right\\Death2.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Right\\Death3.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Right\\Death4.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Right\\Death5.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Right\\Death6.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Right\\Death7.png"), sf::Texture("Assets\\Animations\\Player\\Death\\Right\\Death8.png")}, 8 };
    animData.insert(playerIdleUp);
    animData.insert(playerIdleDown);
    animData.insert(playerIdleLeft);
    animData.insert(playerIdleRight);
    animData.insert(playerWalkUp);
    animData.insert(playerWalkDown);
    animData.insert(playerWalkLeft);
    animData.insert(playerWalkRight);
    animData.insert(playerDeathUp);
    animData.insert(playerDeathDown);
    animData.insert(playerDeathLeft);
    animData.insert(playerDeathRight);
    animNames.insert("PlayerIdleUp");
    animNames.insert("PlayerIdleDown");
    animNames.insert("PlayerIdleLeft");
    animNames.insert("PlayerIdleRight");
    animNames.insert("PlayerWalkUp");
    animNames.insert("PlayerWalkDown");
    animNames.insert("PlayerWalkLeft");
    animNames.insert("PlayerWalkRight");
    animNames.insert("PlayerDeathUp");
    animNames.insert("PlayerDeathDown");
    animNames.insert("PlayerDeathLeft");
    animNames.insert("PlayerDeathRight");
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
    dynArray<sf::Texture*> KeyFrames;
    Animation() : timePerFrame(0.0), totalTime(0.0) {};
    Animation(double time, std::string combinedName, bool loop = true) : timePerFrame(1.0 / time), Loop(loop) {
        if (!animNames.contains(combinedName)) return;
        int Index = animNames.find(combinedName);
        dynArray<sf::Texture> &data = animData[Index];
        for (int i = 0; i < data.size(); i += 1) {
            KeyFrames.insert(&data[i]);
        }
        totalTime = KeyFrames.size() * (1.0 / time);
    };
};

struct Animator {
    double Alpha = 0;
    double currentTime = 0.0;
    int spriteIndex = 0;
    dynArray<Animation> animationsData;
    dynArray<std::string> animationsName;
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
        for (int i = 0; i < animNames.size(); i += 1) {
            if (animNames[i].starts_with(entityName)) {
                animationsName.insert(animNames[i]);
                animationsData.insert(Animation(FPS, animNames[i], !animNames[i].ends_with("Death")));
            }
        }
    };
    void addAnimation(std::string name, std::string variant, Animation data) {
        animationsName.insert(Entity + name + variant);
        animationsData.insert(data);
    }
    void addAnimation(std::string name, std::string variant, double FPS = 30.0) {
        animationsName.insert(Entity + name + variant);
        animationsData.insert(Animation(FPS, Entity + name + variant));
    }
    void changeAnim(std::string anim, std::string variant) {
        bool found = false;
        for (int i = 0; i < animationsName.size(); i += 1) {
            if (animationsName[i].starts_with(Entity + anim + variant)) {
                found = true;
                break;
            }
        }
        if (!found) return;
        if (Anim == anim && Variant == variant) return;
        if (Anim != anim) {
            Anim = anim;
            currentTime = 0.0;
        }
        Variant = variant;
    }
    void draw(sf::RenderWindow& window, sf::Vector2<double> position, sf::Vector2<double> spriteScale = {1.0, 1.0}) {
        sf::Texture* currentTexture = &defaultTexture;
        bool found = false;
        int Idx = 0;
        for (int i = 0; i < animationsName.size(); i += 1) {
            if (animationsName[i].starts_with(Entity + Anim + Variant)) {
                found = true;
                Idx = i;
                break;
            }
        }
        if (found) {;
            Animation& data = animationsData[Idx];
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
        cameraPos.x = std::clamp<double>(cameraPos.x + offset.x, -(worldSize * tileSize - windowSize.x - 64 * 2), 0);
        cameraPos.y = std::clamp<double>(cameraPos.y + offset.y, -(worldSize * tileSize - windowSize.y - 68), 0);
    }
    void draw(sf::RenderWindow &window) {
        animator.draw(window, position, {2.5, 2.5});
        //Health -= (10.0 * deltaTime);
    }
};

double rad(double self) {
    return self * (pi / 180);
}

int rad(int self) {
    return self * (pi / 180);
}
double nightTimePitches[6] = { 0.3, 0.2, 0.1, 0.1, 0.1, 0.1 };
sf::Music* nightTimeAudios[6] = { &Halloween3, &Halloween2, &Halloween3, &Halloween1, &Halloween2, &HalloweenGhost };
bool nightAudioCool = true;
double lastNightAudioTime = 0.0;
double colorAlpha = 0.0f;
bool cycleTransitionRunning = false;
void setNight() {
    if (cycleTransitionRunning == false) {
        gameTimer = 0.0;
        colorAlpha = 0.0;
        cycleTransitionRunning = true;
    }
    colorAlpha = std::min(colorAlpha + deltaTime, 1.0);
    dayTheme.setVolume((float)Lerp<double>(100.0, 0.0, colorAlpha));
    for (int i = 0; i < 6; i += 1) {
        (*nightTimeAudios[i]).setVolume((float)Lerp<double>(0.0, 100.0, colorAlpha));
    }
    if (FuzzyEq(colorAlpha, 1.0)) {
        for (int i = 0; i < 6; i += 1) {
            (*nightTimeAudios[i]).setVolume(100.0f);
        }
        gameTimer = 0.0;
        colorAlpha = 1.0;
        dayTheme.setVolume(0.0f);
        cycleTransitionRunning = false;
        lastNightAudioTime = 15.0;
        nightAudioCool = false;
        (*nightTimeAudios[4]).setPitch(nightTimePitches[4]);
        (*nightTimeAudios[4]).play();
        Mode = "Night";
    }
}

void setDay() {
    if (cycleTransitionRunning == false) {
        gameTimer = 0.0;
        colorAlpha = 1.0;
        cycleTransitionRunning = true;
        dayTheme.setVolume(0.0);
    }
    colorAlpha = std::max(colorAlpha - deltaTime, 0.0);
    dayTheme.setVolume((float)Lerp<double>(100.0, 0.0, colorAlpha));
    for (int i = 0; i < 6; i += 1) {
        (*nightTimeAudios[i]).setVolume((float)Lerp<double>(0.0, 100.0, colorAlpha));
    }
    if (FuzzyEq(colorAlpha, 0.0)) {
        gameTimer = 0.0;
        colorAlpha = 0.0;
        for (int i = 0; i < 6; i += 1) {
            (*nightTimeAudios[i]).setVolume(0.0f);
        }
        dayTheme.setVolume(100.0f);
        cycleTransitionRunning = false;
        Mode = "Day";
    }
}

void nightTimeAudioPlayLogic() { // Runs every frame
    if (nightAudioCool) {
        lastNightAudioTime = 10.0; // Wait 10 seconds before playing another
        nightAudioCool = false;
        int chance = random(0, 5); // Random integer between 0 and 5
        sf::Music& Audio = *nightTimeAudios[chance]; // Retrieving the pointer value by reference to avoid any copying
        Audio.setPitch(nightTimePitches[chance]); // Set Audio Pitch
        Audio.play(); // Finally, play the damn thing
    }
    else {
        lastNightAudioTime = std::max(lastNightAudioTime - deltaTime, 0.0);
        if (FuzzyEq(lastNightAudioTime, 0.0)) { // FuzzyEq is a custom function, returns true even if floats are not truly equal, e.g, 2.00000000001 == 1.999999999 will return true
            lastNightAudioTime = 0.0;
            nightAudioCool = true;
        }
    }
}

int main()
{
    srand(time(0));
    sf::VideoMode screenMode = sf::VideoMode::getFullscreenModes()[0];
    sf::RenderWindow window(sf::VideoMode(screenMode), "Withered Roots", sf::Style::Titlebar | sf::Style::Close);
    windowSize = sf::Vector2<int>(screenMode.size.x, screenMode.size.y);
    initAnims();
    initTileMap();
    Player player{};
    player.bareMove((sf::Vector2<double>(screenMode.size.x / 2.0f - 25.0f, screenMode.size.y / 2.0f - 25.0f)));
    sf::Clock deltaTimer;
    deltaTimer.start();
    dayTheme.setLooping(true);
    dayTheme.play();
    while (window.isOpen())
    {
        colorTint = dayTint;
        fogStart = (double)Lerp(fogStartDay, fogStartNight, colorAlpha);
        fogEnd = (double)Lerp(fogEndDay, fogEndNight, colorAlpha);
        colorTint = Lerp(dayTint, nightTint, colorAlpha);
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
        if (cycleTransitionRunning == false) {
            gameTimer += deltaTime;
        }
        if (Mode == "Day" && (gameTimer > dayTime || cycleTransitionRunning)) {
            setNight();
        }
        else if (Mode == "Night" && (gameTimer > nightTime || cycleTransitionRunning)) {
            setDay();
        }
        if (Mode == "Night") {
            nightTimeAudioPlayLogic();
        }
        player.move(movementOffset, deltaTime);
        window.clear();
        drawTileMap(window);
        player.draw(window);
        window.display();
    }
}