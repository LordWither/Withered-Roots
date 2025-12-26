#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <cmath>

sf::Vector2<float> doubleToFloat(sf::Vector2<double> inVector) {
    return sf::Vector2<float> {(float)inVector.x, (float)inVector.y};
}

sf::Vector2<double> cameraPos = sf::Vector2<double>(0.0, 0.0);
sf::Vector2<int> windowSize;
sf::Vector2<double> playerPos{ 0.0, 0.0 };
const int textureCount = 4;
sf::Texture textureData[textureCount] = {sf::Texture("Assets\\Grass.png"),sf::Texture("Assets\\Tree.png") ,sf::Texture("Assets\\Rocks.png") ,sf::Texture("Assets\\Bush.png") };
char textureKeys[textureCount] = { 'G', 'T', 'R', 'B'};
sf::Vector2<double> textureScales[textureCount] = { {1.0, 1.0},{1.0, 1.0},{0.6, 0.6},{0.7, 0.7} };
bool textureAlwaysDrawsUnderPlayer[textureCount] = { false, false, true, false };
sf::Music dayTheme("Assets\\DayTheme.ogg");
const int worldSize = 100;
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

template<typename arrType>
struct dynArray {
    arrType* Core = nullptr;
    int length = 0;
    int occupiedLength = 0;
    bool accurateSizing = false;
    dynArray(bool accurateSizing = false) : Core(nullptr), accurateSizing(accurateSizing) {
        length = 1;
        if (Core != nullptr) delete[] Core;
        Core = new arrType[length];
        occupiedLength = 0;
    };
    dynArray(arrType* initialSet, int setLength, bool accurateSizing = false) : Core(nullptr), accurateSizing(accurateSizing) {
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
        if (accurateSizing)
            length += 1;
        else
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
    dynArray<sf::Texture> charMIdleUp = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Up\\Idle1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Up\\Idle2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Up\\Idle3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Up\\Idle4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Up\\Idle5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Up\\Idle6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Up\\Idle7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Up\\Idle8.png")}, 8 };
    dynArray<sf::Texture> charMIdleDown = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Down\\Idle1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Down\\Idle2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Down\\Idle3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Down\\Idle4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Down\\Idle5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Down\\Idle6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Down\\Idle7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Down\\Idle8.png")}, 8 };
    dynArray<sf::Texture> charMIdleLeft = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Left\\Idle1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Left\\Idle2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Left\\Idle3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Left\\Idle4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Left\\Idle5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Left\\Idle6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Left\\Idle7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Left\\Idle8.png")}, 8 };
    dynArray<sf::Texture> charMIdleRight = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Right\\Idle1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Right\\Idle2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Right\\Idle3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Right\\Idle4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Right\\Idle5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Right\\Idle6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Right\\Idle7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Idle\\Right\\Idle8.png")}, 8 };
    dynArray<sf::Texture> charMWalkUp = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Up\\Walk1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Up\\Walk2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Up\\Walk3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Up\\Walk4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Up\\Walk5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Up\\Walk6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Up\\Walk7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Up\\Walk8.png")}, 8 };
    dynArray<sf::Texture> charMWalkDown = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Down\\Walk1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Down\\Walk2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Down\\Walk3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Down\\Walk4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Down\\Walk5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Down\\Walk6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Down\\Walk7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Down\\Walk8.png")}, 8 };
    dynArray<sf::Texture> charMWalkLeft = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Left\\Walk1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Left\\Walk2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Left\\Walk3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Left\\Walk4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Left\\Walk5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Left\\Walk6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Left\\Walk7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Left\\Walk8.png")}, 8 };
    dynArray<sf::Texture> charMWalkRight = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Right\\Walk1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Right\\Walk2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Right\\Walk3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Right\\Walk4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Right\\Walk5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Right\\Walk6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Right\\Walk7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Walk\\Right\\Walk8.png")}, 8 };
    dynArray<sf::Texture> charMDeathUp = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Death\\Up\\Death1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Up\\Death2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Up\\Death3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Up\\Death4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Up\\Death5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Up\\Death6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Up\\Death7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Up\\Death8.png")}, 8 };
    dynArray<sf::Texture> charMDeathDown = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Death\\Down\\Death1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Down\\Death2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Down\\Death3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Down\\Death4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Down\\Death5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Down\\Death6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Down\\Death7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Down\\Death8.png")}, 8 };
    dynArray<sf::Texture> charMDeathLeft = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Death\\Left\\Death1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Left\\Death2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Left\\Death3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Left\\Death4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Left\\Death5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Left\\Death6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Left\\Death7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Left\\Death8.png")}, 8 };
    dynArray<sf::Texture> charMDeathRight = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\M\\Death\\Right\\Death1.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Right\\Death2.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Right\\Death3.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Right\\Death4.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Right\\Death5.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Right\\Death6.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Right\\Death7.png"), sf::Texture("Assets\\Animations\\Character\\M\\Death\\Right\\Death8.png")}, 8 };
    dynArray<sf::Texture> charFIdleUp = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Up\\Idle1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Up\\Idle2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Up\\Idle3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Up\\Idle4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Up\\Idle5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Up\\Idle6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Up\\Idle7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Up\\Idle8.png")}, 8 };
    dynArray<sf::Texture> charFIdleDown = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Down\\Idle1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Down\\Idle2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Down\\Idle3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Down\\Idle4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Down\\Idle5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Down\\Idle6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Down\\Idle7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Down\\Idle8.png")}, 8 };
    dynArray<sf::Texture> charFIdleLeft = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Left\\Idle1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Left\\Idle2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Left\\Idle3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Left\\Idle4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Left\\Idle5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Left\\Idle6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Left\\Idle7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Left\\Idle8.png")}, 8 };
    dynArray<sf::Texture> charFIdleRight = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Right\\Idle1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Right\\Idle2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Right\\Idle3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Right\\Idle4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Right\\Idle5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Right\\Idle6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Right\\Idle7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Idle\\Right\\Idle8.png")}, 8 };
    dynArray<sf::Texture> charFWalkUp = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Up\\Walk1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Up\\Walk2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Up\\Walk3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Up\\Walk4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Up\\Walk5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Up\\Walk6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Up\\Walk7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Up\\Walk8.png")}, 8 };
    dynArray<sf::Texture> charFWalkDown = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Down\\Walk1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Down\\Walk2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Down\\Walk3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Down\\Walk4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Down\\Walk5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Down\\Walk6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Down\\Walk7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Down\\Walk8.png")}, 8 };
    dynArray<sf::Texture> charFWalkLeft = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Left\\Walk1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Left\\Walk2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Left\\Walk3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Left\\Walk4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Left\\Walk5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Left\\Walk6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Left\\Walk7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Left\\Walk8.png")}, 8 };
    dynArray<sf::Texture> charFWalkRight = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Right\\Walk1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Right\\Walk2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Right\\Walk3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Right\\Walk4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Right\\Walk5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Right\\Walk6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Right\\Walk7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Walk\\Right\\Walk8.png")}, 8 };
    dynArray<sf::Texture> charFDeathUp = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Death\\Up\\Death1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Up\\Death2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Up\\Death3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Up\\Death4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Up\\Death5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Up\\Death6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Up\\Death7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Up\\Death8.png")}, 8 };
    dynArray<sf::Texture> charFDeathDown = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Death\\Down\\Death1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Down\\Death2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Down\\Death3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Down\\Death4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Down\\Death5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Down\\Death6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Down\\Death7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Down\\Death8.png")}, 8 };
    dynArray<sf::Texture> charFDeathLeft = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Death\\Left\\Death1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Left\\Death2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Left\\Death3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Left\\Death4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Left\\Death5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Left\\Death6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Left\\Death7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Left\\Death8.png")}, 8 };
    dynArray<sf::Texture> charFDeathRight = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Character\\F\\Death\\Right\\Death1.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Right\\Death2.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Right\\Death3.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Right\\Death4.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Right\\Death5.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Right\\Death6.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Right\\Death7.png"), sf::Texture("Assets\\Animations\\Character\\F\\Death\\Right\\Death8.png")}, 8 };
    animData.insert(charMIdleUp);
    animData.insert(charMIdleDown);
    animData.insert(charMIdleLeft);
    animData.insert(charMIdleRight);
    animData.insert(charMWalkUp);
    animData.insert(charMWalkDown);
    animData.insert(charMWalkLeft);
    animData.insert(charMWalkRight);
    animData.insert(charMDeathUp);
    animData.insert(charMDeathDown);
    animData.insert(charMDeathLeft);
    animData.insert(charMDeathRight);
    animData.insert(charFIdleUp);
    animData.insert(charFIdleDown);
    animData.insert(charFIdleLeft);
    animData.insert(charFIdleRight);
    animData.insert(charFWalkUp);
    animData.insert(charFWalkDown);
    animData.insert(charFWalkLeft);
    animData.insert(charFWalkRight);
    animData.insert(charFDeathUp);
    animData.insert(charFDeathDown);
    animData.insert(charFDeathLeft);
    animData.insert(charFDeathRight);
    animNames.insert("CharMIdleUp");
    animNames.insert("CharMIdleDown");
    animNames.insert("CharMIdleLeft");
    animNames.insert("CharMIdleRight");
    animNames.insert("CharMWalkUp");
    animNames.insert("CharMWalkDown");
    animNames.insert("CharMWalkLeft");
    animNames.insert("CharMWalkRight");
    animNames.insert("CharMDeathUp");
    animNames.insert("CharMDeathDown");
    animNames.insert("CharMDeathLeft");
    animNames.insert("CharMDeathRight");
    animNames.insert("CharFIdleUp");
    animNames.insert("CharFIdleDown");
    animNames.insert("CharFIdleLeft");
    animNames.insert("CharFIdleRight");
    animNames.insert("CharFWalkUp");
    animNames.insert("CharFWalkDown");
    animNames.insert("CharFWalkLeft");
    animNames.insert("CharFWalkRight");
    animNames.insert("CharFDeathUp");
    animNames.insert("CharFDeathDown");
    animNames.insert("CharFDeathLeft");
    animNames.insert("CharFDeathRight");
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

bool findInString(std::string input, std::string toFind) {
    if (toFind.size() > input.size()) return false;
    for (int i = 0; i < input.size() - toFind.size(); i += 1) {
        for (int ii = 0; ii <= toFind.size(); ii += 1) {
            if (toFind[ii] == input[i + ii] && ii == toFind.size() - 1) {
                return true;
            }
            else if (toFind[ii] != input[i + ii]) {
                break;
            }
        }
    }
    return false;
}

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
                animationsData.insert(Animation(FPS, animNames[i], !findInString(animNames[i], "Death")));
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

struct grassTile {
    sf::RectangleShape fog = sf::RectangleShape({ (double)tileSize, (double)tileSize });
    sf::Vector2<int> originalPosition = { 0, 0 };
    sf::Vector2<double> position = { 0.0, 0.0 };
    sf::Sprite Sprite = sf::Sprite(*textureData, sf::IntRect({ 0, 0 }, {tileSize, tileSize}));
    bool fogIsLinked = false;
    grassTile() {}
    grassTile(sf::Vector2<int> iPosition, sf::Vector2<double> dPosition, bool fogIsLinked = false) : originalPosition(iPosition), position(dPosition), fogIsLinked(fogIsLinked) {}
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
            if (!fogIsLinked)
                window.draw(fog);
        }
    }
};

struct mapDecor {
    sf::Vector2<int> originalPosition = { 0, 0 };
    sf::Vector2<double> position = { 0.0, 0.0 };
    sf::Color color = sf::Color::Color(78, 180, 78, 100U);
    sf::Vector2<double> size = { tileSize, tileSize };
    sf::Vector2<double> scale = *textureScales;
    bool alwaysDrawUnderPlayer = *textureAlwaysDrawsUnderPlayer;
    char type = *textureKeys;
    sf::Sprite Sprite = sf::Sprite(*textureData, sf::IntRect({ 0, 0 }, { tileSize, tileSize}));
    sf::RectangleShape* fog = nullptr;
    mapDecor() : fog(nullptr) {}
    mapDecor(sf::Vector2<int> originalPosition, sf::Vector2<double> position, sf::RectangleShape* fog, char type) : originalPosition(originalPosition), position(position), fog(fog), type(type) {
        int foundIdx = -1;
        for (int i = 0; i < textureCount; i += 1) {
            if (textureKeys[i] == type) {
                foundIdx = i;
                break;
            }
        }
        if (foundIdx == -1) return;
        Sprite = sf::Sprite(textureData[foundIdx], sf::IntRect({ 0, 0 }, {tileSize, tileSize}));
        scale = textureScales[foundIdx];
        alwaysDrawUnderPlayer = textureAlwaysDrawsUnderPlayer[foundIdx];
    }
    void draw(sf::RenderWindow& window, sf::Vector2<double> cameraPos) {
        sf::Vector2<double> playerCentre = playerPos + sf::Vector2<double>(50.0, 50.0);
        sf::Vector2<double> fogCentre = (position + cameraPos) + (sf::Vector2<double>((double)tileSize, (double)tileSize) / 2.0);
        Sprite.setScale(sf::Vector2f(scale.x, scale.y));
        Sprite.setColor(colorTint);
        Sprite.setPosition(doubleToFloat(position + cameraPos));
        window.draw(Sprite);
        if (fog) {
            double dist = (fogCentre - playerCentre).length();
            if (dist > fogStart) {
                dist -= fogStart;
                (*fog).setFillColor(setTransparency(Multiply(fogColor, colorTint, 0.3), std::clamp(dist / fogEnd, 0.0, 1.0)));
                (*fog).setPosition(doubleToFloat(position + cameraPos));
                window.draw(*fog);
            }
        }
    }
};

sf::Color getRanColor(){
    return sf::Color::Color(rand() % 255, rand() % 255, rand() % 255, 100U);
}

grassTile tileMap[worldSize][worldSize];
dynArray<mapDecor>decorMap = {true};
void initTileMap() {
    std::ifstream map("Map.txt");
    std::string currentLine;
    int y = 0;
    while (std::getline(map, currentLine)) {
        for (int x = 0; x < currentLine.size(); x += 1) {
            if (currentLine[x] == 'G' || currentLine[x] == 'S') {
                tileMap[y][x] = { sf::Vector2<int>(tileSize * x, tileSize * y), sf::Vector2<double>(tileSize * x, tileSize * y) };
            }
            else {
                tileMap[y][x] = { sf::Vector2<int>(tileSize * x, tileSize * y), sf::Vector2<double>(tileSize * x, tileSize * y), true};
                decorMap.insert({sf::Vector2<int>(tileSize * x, tileSize * y), sf::Vector2<double>(tileSize * x, tileSize * y), &tileMap[y][x].fog, currentLine[x]});
            }
        }
        y += 1;
    };
    //for (int y = 0; y < worldSize; y += 1) {
    //    for (int x = 0; x < worldSize; x += 1) {
    //        tileMap[y][x] = { sf::Vector2<int>(tileSize * x, tileSize * y), sf::Vector2<double>((double)tileSize * x, (double)tileSize * y)};
    //    }
    //}
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

bool FuzzyEq(double self, double other, double epsilon = 1e-5) {
    return (abs(self - other) <= epsilon);
}

struct Inventory {
    static const int gameItems = 4;
    int itemVals[gameItems] = { 100, 1, 2, 1 };
    std::string itemNames[gameItems] = {"Cash", "Beans", "Bandages", "Chocolate"};
    int itemRandLimits[gameItems][2] = { {50,200},{0,1},{0,2},{0,1} };
    bool holdingGun = false;
    void randomize() {
        for (int i = 0; i < gameItems; i += 1) {
            itemVals[i] = random(itemRandLimits[i][0], itemRandLimits[i][1]);
        }
    }
    void addItem(std::string item, int count) {
        for (int i = 0; i < gameItems; i += 1) {
            if (itemNames[i] == item)
                itemVals[i] = std::max(itemVals[i] + count, 0);
        }
    }
    void removeItem(std::string item, int count) {
        for (int i = 0; i < gameItems; i += 1) {
            if (itemNames[i] == item)
                itemVals[i] = std::max(itemVals[i] - count, 0);
        }
    }
    void equipGun() {
        holdingGun = true;
    }
    void unEquipGun() {
        holdingGun = false;
    }
};

struct Player {
    sf::Vector2<int> size = { 48, 64 };
    sf::Vector2<int> trueSize = { 16, 32 };
    sf::Vector2<double> scale = {2.5, 2.5};
    sf::Vector2<double> position = { 0.0f, 0.0f };
    double Health = 100.0;
    std::string Gender = random(1, 2) == 1 ? "CharM" : "CharF";
    std::string Anim = "Idle";
    std::string animVariant = "Down";
    double speed = 250.0;
    double speedToAnimFPSRatio = 5.0 / 200.0;
    Inventory playerInv;
    sf::Vector2<double> boundsMax = sf::Vector2<double>(windowSize.x * 0.7, windowSize.y * 0.7);
    sf::Vector2<double> boundsMin = sf::Vector2<double>(windowSize.x * 0.3, windowSize.y * 0.3);;
    sf::Color color = sf::Color::Color(255, 255, 255, 255U);
    Animator animator = Animator(Gender, size, speedToAnimFPSRatio * speed);
    Player() {
        playerInv.randomize();
        animator.Anim = Anim;
        animator.Variant = animVariant + (playerInv.holdingGun ? "Gun" : "");
    } //Keeping here in case I wanna init add code, causes no issues if I leave it here
    void bareMove(sf::Vector2<double> newPosition) {
        position = newPosition;
        playerPos = newPosition;
    }
    void trueMove(sf::Vector2<double> offset) {
        position += offset;
        playerPos = position;
    }
    void changeAnim(std::string anim, std::string variant) {
        Anim = anim;
        animVariant = variant;
        animator.changeAnim(anim, variant + ((playerInv.holdingGun && anim != "Death") ? "Gun" : ""));
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
        animator.draw(window, position, scale);
        //Health -= (10.0 * deltaTime);
    }
    void equipGun(bool equip) {
        equip ? playerInv.equipGun() : playerInv.unEquipGun();
        changeAnim(Anim, animVariant);
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

void drawMap(sf::RenderWindow& window, Player& player) {
    int minX = std::clamp<int>(floor(-cameraPos.x / tileSize) - 2, 0, worldSize - 1);
    int minY = std::clamp<int>(floor(-cameraPos.y / tileSize) - 2, 0, worldSize - 1);
    int maxX = std::clamp<int>(ceil((-cameraPos.x + windowSize.x) / tileSize), 0, worldSize - 1);
    int maxY = std::clamp<int>(ceil((-cameraPos.y + windowSize.y) / tileSize), 0, worldSize - 1);
    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            tileMap[y][x].draw(window, cameraPos);
        }
    }
    for (int i = 0; i < decorMap.size(); i += 1) {
        mapDecor& decor = decorMap[i];
        bool visX = (decor.position.x + cameraPos.x + tileSize) >= 0 && (decor.position.x + cameraPos.x) <= windowSize.x;
        bool visY = (decor.position.y + cameraPos.y + tileSize) >= 0 && (decor.position.y + cameraPos.y) <= windowSize.y;
        if (visX && visY) {
            double playerDepth = player.position.y + ((double)player.trueSize.y * player.scale.y * 1.3);
            double selfDepth = decor.position.y + cameraPos.y + (decor.size.y * decor.scale.y);
            if (decor.alwaysDrawUnderPlayer == true || playerDepth > selfDepth)
                decor.draw(window, cameraPos);
        }
    }
    player.draw(window);
    for (int i = 0; i < decorMap.size(); i += 1) {
        mapDecor& decor = decorMap[i];
        bool visX = (decor.position.x + cameraPos.x + tileSize) >= 0 && (decor.position.x + cameraPos.x) <= windowSize.x;
        bool visY = (decor.position.y + cameraPos.y + tileSize) >= 0 && (decor.position.y + cameraPos.y) <= windowSize.y;
        if (visX && visY) {
            double playerDepth = player.position.y + ((double)player.trueSize.y * player.scale.y * 1.3);
            double selfDepth = decor.position.y + cameraPos.y + (decor.size.y * decor.scale.y);
            if (decor.alwaysDrawUnderPlayer == false && playerDepth < selfDepth)
                decor.draw(window, cameraPos);
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::H)) {
            player.equipGun(!player.playerInv.holdingGun);
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
        drawMap(window, player);
        window.display();
    }
}