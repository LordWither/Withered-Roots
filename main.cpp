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
const int pickups = 4;
sf::Texture textureData[textureCount] = {sf::Texture("Assets\\Grass.png"),sf::Texture("Assets\\Tree.png") ,sf::Texture("Assets\\Rocks.png") ,sf::Texture("Assets\\Bush.png") };
sf::Texture pickUpTextures[pickups] = {sf::Texture("Assets\\Bandages.png"), sf::Texture("Assets\\Beans.png"), sf::Texture("Assets\\Cash.png"), sf::Texture("Assets\\Chocolate.png")};
std::string pickupNames[pickups] = {"bandages", "beans", "cash", "chocolate"};
char textureKeys[textureCount] = { 'G', 'T', 'R', 'B'};
sf::Vector2<double> textureScales[textureCount] = { {1.0, 1.0},{1.0, 1.0},{0.6, 0.6},{0.7, 0.7} };
bool textureAlwaysDrawsUnderPlayer[textureCount] = { false, false, true, false };

sf::Texture ghostEyesTex{"Assets\\GhostEyes.png"};
sf::Texture ghostSmokeTex{ "Assets\\GhostSmoke.png" };
sf::Texture merchantShopTex("Assets\\Shop.png");

const int worldSize = 100;
const int tileSize = 100;
sf::Color dayTint = sf::Color::Color(255, 200, 100, 255U);
sf::Color nightTint = sf::Color::Color(29, 89, 255, 255U);
sf::Color colorTint = sf::Color::Color(255, 255, 255, 255U);
sf::Color fogColor = sf::Color::Color(128, 128, 128, 255U);
std::string Mode = "Day";
const int pickUpCount = 200;
double currentTime = 0.0f;
double deltaTime = 0.0f;
double combinedDeltaTime = 0.0f;
const double pi = 3.14159265358979323846;
double fogStartDay = 450.0;
double fogEndDay = 1300.0;
double fogStartNight = fogStartDay * 0.9;
double fogEndNight = fogEndDay * 0.9;
double dayTime = 15.0;
double nightTime = 100.0;
double fogStart = 0.0;
double fogEnd = 0.0;
double gameTimer = 0.0;

//Ghost logic
double sightRadius = 20 * tileSize;

sf::Music Halloween1("Assets\\Halloween1.ogg");
sf::Music Halloween2("Assets\\Halloween2.ogg");
sf::Music Halloween3("Assets\\Halloween3.ogg");
sf::Music HalloweenGhost("Assets\\HalloweenGhost.wav");
sf::Music purchase("Assets\\purchase.ogg");
sf::Music dayTheme("Assets\\DayTheme.ogg");
sf::Music pickup("Assets\\pickup.ogg");
sf::Music chase("Assets\\chase.ogg");
sf::Music error("Assets\\error.ogg");
sf::Music death("Assets\\death.ogg");
sf::Music hit("Assets\\hit.ogg");
sf::Music fire("Assets\\fire.ogg");

template<typename arrType>
struct dynArray {
    arrType* Core = nullptr;
    int length = 1;
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
        setLength = std::max(setLength, 1);
        Core = new arrType[setLength];
        for (int i = 0; i < setLength; i += 1) {
            Core[i] = initialSet[i];
        }
        delete[] initialSet;
        length = setLength;
        occupiedLength = setLength;
    }
    void resize(int newSize) {
        length = std::max(newSize, 1);
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
        length = std::max(length - 1, 1);
        arrType* tmp = new arrType[length];
        for (int i = 0; i < occupiedLength; i += 1) {
            if (i < Index)
                tmp[i] = Core[i];
            else if (i > Index)
                tmp[i - 1] = Core[i];
        }
        occupiedLength = std::max(occupiedLength - 1, 0);
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
        length = std::max(other.length, 1);
        occupiedLength = other.occupiedLength;
        Core = new arrType[length];
        for (int i = 0; i < occupiedLength; i += 1) {
            Core[i] = other.Core[i];
        }
    };
    dynArray& operator=(const dynArray& other) {
        if (this == &other) return *this;
        delete[] Core;
        length = std::max(other.length, 1);
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
    dynArray<sf::Texture> merchant = { new sf::Texture[8] {sf::Texture("Assets\\Animations\\Merchant\\Merchant1.png"), sf::Texture("Assets\\Animations\\Merchant\\Merchant2.png"), sf::Texture("Assets\\Animations\\Merchant\\Merchant3.png"), sf::Texture("Assets\\Animations\\Merchant\\Merchant4.png"), sf::Texture("Assets\\Animations\\Merchant\\Merchant5.png"), sf::Texture("Assets\\Animations\\Merchant\\Merchant6.png"), sf::Texture("Assets\\Animations\\Merchant\\Merchant7.png"), sf::Texture("Assets\\Animations\\Merchant\\Merchant8.png")}, 8};
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
    animData.insert(merchant);
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
    animNames.insert("Merchant");
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
    return (std::abs(self.x - other.x) <= epsilon && std::abs(self.y - other.y) <= epsilon);
}

struct Animation {
    double timePerFrame;
    double totalTime;
    bool Loop = true;
    std::string combinedName;
    dynArray<sf::Texture*> KeyFrames;
    Animation() : timePerFrame(0.0), totalTime(0.0) {};
    Animation(double time, std::string combinedName, bool loop = true) : timePerFrame(1.0 / time), Loop(loop), combinedName(combinedName) {
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
                animationsData.insert({FPS, animNames[i], !findInString(animNames[i], "Death")});
            }
        }
    };
    void addAnimation(std::string name, std::string variant, Animation data) {
        animationsName.insert(Entity + name + variant);
        animationsData.insert({1.0 / data.timePerFrame, data.combinedName, data.Loop});
    }
    void addAnimation(std::string name, std::string variant, double FPS = 30.0) {
        animationsName.insert(Entity + name + variant);
        animationsData.insert({ FPS, Entity + name + variant });
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
    void draw(sf::RenderWindow& window, sf::Vector2<double> position, sf::Vector2<double> spriteScale = { 1.0, 1.0 }, sf::Color customTint = sf::Color(255, 255, 255, 255)) {
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
        if (!FuzzyEq(scale, spriteScale)) {
            scale = spriteScale;
            animSprite.setScale(doubleToFloat(scale));
        }
        animSprite.setColor(Multiply(Lerp(sf::Color::Color(255, 255, 255, 255), colorTint, 0.5), customTint));
        animSprite.setPosition(doubleToFloat(position));
        window.draw(animSprite);
    }
};

struct grassTile {
    sf::RectangleShape fog = sf::RectangleShape({ (double)tileSize, (double)tileSize });
    sf::Vector2<int> originalPosition = { 0, 0 };
    sf::Vector2<double> position = { 0.0, 0.0 };
    sf::Sprite Sprite = sf::Sprite(*textureData, sf::IntRect({ 0, 0 }, {tileSize, tileSize}));
    bool isEmpty = true;
    bool isBlocked = false;
    double F = 0.0;
    double G = 0.0;
    double H = 0.0;
    grassTile() {}
    grassTile(sf::Vector2<int> iPosition, sf::Vector2<double> dPosition, bool fogIsLinked = false) : originalPosition(iPosition), position(dPosition) {}
    void draw(sf::RenderWindow& window) {
        Sprite.setColor(colorTint);
        Sprite.setPosition(doubleToFloat(position + cameraPos));
        window.draw(Sprite);
    }
    void drawFog(sf::RenderWindow& window) {
        sf::Vector2<double> playerCentre = playerPos + sf::Vector2<double>(50.0, 50.0);
        sf::Vector2<double> fogCentre = (position + cameraPos) + (sf::Vector2<double>((double)tileSize, (double)tileSize) / 2.0);
        double dist = (fogCentre - playerCentre).length();
        if (dist > fogStart) {
            dist -= fogStart;
            fog.setFillColor(setTransparency(Multiply(fogColor, colorTint, 0.3), std::clamp(dist / fogEnd, 0.0, 1.0)));
            fog.setPosition(doubleToFloat(position + cameraPos));
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
    mapDecor() {}
    mapDecor(sf::Vector2<int> originalPosition, sf::Vector2<double> position, sf::RectangleShape* fog, char type) : originalPosition(originalPosition), position(position), type(type) {
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
    void draw(sf::RenderWindow& window) {
        sf::Vector2<double> playerCentre = playerPos + sf::Vector2<double>(50.0, 50.0);
        sf::Vector2<double> fogCentre = (position + cameraPos) + (sf::Vector2<double>((double)tileSize, (double)tileSize) / 2.0);
        Sprite.setScale(sf::Vector2f(scale.x, scale.y));
        Sprite.setColor(colorTint);
        Sprite.setPosition(doubleToFloat(position + cameraPos));
        window.draw(Sprite);
    }
};

sf::Color getRanColor(){
    return sf::Color::Color(rand() % 255, rand() % 255, rand() % 255, 100U);
}

grassTile tileMap[worldSize][worldSize];
dynArray<mapDecor>decorMap = {};
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
                grassTile& tile = tileMap[y][x];
                tile.isEmpty = false;
                if (currentLine[x] == 'S' || currentLine[x] == 'T') {
                    tile.isBlocked = true;
                }
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
    return (std::abs(self - other) <= epsilon);
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
    sf::Vector2<int> sizeBias = { - 48 + 16, - 64 + 32 };
    sf::Vector2<int> offsetBias = {-3, -10};
    sf::Vector2<int> offset = { (48 - size.x) / 2 + 2, (64 - size.y) / 2 + 1 };
    sf::Vector2<double> scale = {2.5, 2.5};
    sf::Vector2<double> position = { 0.0f, 0.0f };
    sf::Vector2<int> offsetPos = { 48 - 16 / 2, 64 - 32 / 2 };
    double Health = 100.0;
    std::string Gender = random(1, 2) == 1 ? "CharM" : "CharF";
    std::string Anim = "Idle";
    std::string animVariant = "Down";
    double speed = 250.0;
    double stamina = 100.0;
    double speedToAnimFPSRatio = 5.0 / 200.0;
    Inventory playerInv;
    sf::Vector2<double> boundsMax = sf::Vector2<double>(windowSize.x * 0.7, windowSize.y * 0.7);
    sf::Vector2<double> boundsMin = sf::Vector2<double>(windowSize.x * 0.3, windowSize.y * 0.3);;
    sf::Color color = sf::Color::Color(255, 255, 255, 255U);
    Animator animator = Animator(Gender, size, speedToAnimFPSRatio * speed, offset);
    double hitTimer = 0.0;
    double hitCooldown = 0.5;
    bool dead = false;
    Player() {
        playerInv.randomize();
        animator.Anim = Anim;
        animator.Variant = animVariant + (playerInv.holdingGun ? "Gun" : "");
    } //Keeping here in case I wanna init add code, causes no issues if I leave it here
    sf::Vector2<double> getSize() {
        return sf::Vector2<double>((double)(size.x + sizeBias.x + offsetBias.x) * scale.x, (double)(size.y + sizeBias.y + offsetBias.y) * scale.y);
    }
    sf::Vector2<double> getPosition() {
        return sf::Vector2<double>(position.x + offsetPos.x, position.y + offsetPos.y);
    }
    void bareMove(sf::Vector2<double> newPosition) {
        position = newPosition;
        playerPos = newPosition;
    }
    void trueMove(sf::Vector2<double> offset) {
        if (position.x + offset.x <= 0.0 || position.x + offset.x >= windowSize.x - getSize().x * 4.0) {
            position.x = std::clamp<double>(position.x + offset.x, 0.0, windowSize.x - getSize().x * 4.0);
        } else {
            position.x += offset.x;
        }
        if (position.y + offset.y <= 0.0 || position.y + offset.y >= windowSize.y - getSize().y * 3.0) {
            position.y = std::clamp<double>(position.y + offset.y, 0.0, windowSize.y - getSize().y * 3.0);
        }
        else {
            position.y += offset.y;
        }
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
                dead = true;
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

        bool shouldScrollX =
            (realOffset.x < 0 && position.x <= boundsMin.x) ||
            (realOffset.x > 0 && position.x >= boundsMax.x);

        bool shouldScrollY =
            (realOffset.y < 0 && position.y <= boundsMin.y) ||
            (realOffset.y > 0 && position.y >= boundsMax.y);
        bool canScrollX = shouldScrollX && cameraPos.x + offset.x <= 0 &&
            cameraPos.x + offset.x >= -(worldSize * tileSize - windowSize.x);
        bool canScrollY = shouldScrollY && cameraPos.y + offset.y <= 0 &&
            cameraPos.y + offset.y >= -(worldSize * tileSize - windowSize.y + tileSize / 4.0);
        if (!canScrollX) {
            if (shouldScrollX) {
                cameraPos.x = std::clamp<double>(cameraPos.x + offset.x, -(worldSize * tileSize - windowSize.x), 0);
            }
            trueMove({ realOffset.x, 0.0 });
        } else {
            cameraPos.x = std::clamp<double>(cameraPos.x + offset.x, -(worldSize * tileSize - windowSize.x), 0);
        }
        if (!canScrollY) {
            if (shouldScrollY) {
                cameraPos.y = std::clamp<double>(cameraPos.y + offset.y, -(worldSize * tileSize - windowSize.y + tileSize / 4.0), 0);
            }
            trueMove({ 0.0, realOffset.y });
        } else {
            cameraPos.y = std::clamp<double>(cameraPos.y + offset.y, -(worldSize * tileSize - windowSize.y + tileSize / 4.0), 0);
        }
    }
    void draw(sf::RenderWindow &window) {
        animator.draw(window, position, scale);
    }
    void equipGun(bool equip) {
        equip ? playerInv.equipGun() : playerInv.unEquipGun();
        changeAnim(Anim, animVariant);
    }
};

struct Merchant {
    sf::Vector2<int> size = {tileSize, tileSize};
    sf::Vector2<double> spriteScale = { 2.0, 2.0 };
    sf::Vector2<double> scale = { 1.0, 1.0 };
    sf::Vector2<double> position = { 0.0, 0.0 };
    sf::Vector2<double> spriteOffset = { 10.0, -15.0 };
    sf::Vector2<double> spritePosition = { spriteOffset.x, spriteOffset.y };
    double animFPS = 5.0 / 200.0 * 250.0;
    sf::Color color = sf::Color::Color(255, 255, 255, 255U);
    sf::Sprite shop{ merchantShopTex, sf::IntRect({0, 0}, size) };
    Animator animator = Animator("Merchant", { 48, 64 }, animFPS, {0, 0});
    grassTile* associatedTile = nullptr;
    double hitTimer = 0.0;
    double hitCooldown = 0.5;
    Merchant() {
        animator.Anim = "";
        animator.Variant = "";
    } //Keeping here in case I wanna init add code, causes no issues if I leave it here
    sf::Vector2<double> getSize() {
        return sf::Vector2<double>(size.x, size.y);
    }
    sf::Vector2<double> getWorldPosition() {
        return sf::Vector2<double>(position.x + cameraPos.x, position.y + cameraPos.y);
    }
    void teleportShop() {
        if (associatedTile != nullptr) {
            (*associatedTile).isEmpty = true;
        }
        bool placed = false;
        while (!placed) {
            int ranX = random(1, 100);
            int ranY = random(1, 100);
            grassTile& tile = tileMap[ranY][ranX];
            if (tile.isEmpty) {
                associatedTile = &tile;
                tile.isEmpty = false;
                placed = true;
                position = tile.position;
                spritePosition = position + spriteOffset;
            }
        }
    }
    bool collidingWithPlayer(Player& player) {
        return (
            getWorldPosition().x < player.getPosition().x + player.getSize().x &&
            getWorldPosition().x + getSize().x > player.getPosition().x &&
            getWorldPosition().y < player.getPosition().y + player.getSize().y &&
            getWorldPosition().y + getSize().y > player.getPosition().y
            );
    }
    void move(sf::Vector2<double> newPos) {
        position = newPos;
        spritePosition = newPos + spriteOffset;
    }
    void draw(sf::RenderWindow& window) {
        if (Mode != "Night") {
            animator.draw(window, spritePosition + cameraPos, spriteScale);
        }
        shop.setPosition(doubleToFloat(getWorldPosition()));
        shop.setScale(doubleToFloat(scale));
        shop.setColor(colorTint);
        window.draw(shop);
    }
};
sf::Vector2<double> getFurthestPosition();
struct Ghost {
    sf::Vector2<int> size = { 40, 40 };
    sf::Vector2<double> scale = { 1, 1 };
    sf::Vector2<double> position = { 0.0, 0.0 };
    std::string state = "Wander";
    double Health = 100.0;
    double speed = 300.0;
    sf::Color color = sf::Color::Color(255, 255, 255, 255U);
    sf::Sprite ghostEyes = sf::Sprite(ghostEyesTex);
    static const int smokeAmount = 4;
    int correctedSmokeAmount = smokeAmount;
    double orbitRadius = 12.0;
    double orbitSpeed = 5.0;
    double orbitAlpha = 0.0;
    sf::Sprite smoke = sf::Sprite(ghostSmokeTex);
    sf::Vector2<double> wanderTarget = { 0.0, 0.0 };
    double wanderTimer = 0.0;
    double wanderInterval = 2.5;
    double damage = 25.0;
    sf::Vector2<double> getSize() {
        return sf::Vector2<double>((double)(size.x) * scale.x, (double)(size.y) * scale.y);
    }
    Ghost() {
        correctedSmokeAmount = smokeAmount;
        if (correctedSmokeAmount % 2 != 0) {
            correctedSmokeAmount -= 1;
        }
    }
    void move(sf::Vector2<double> newPosition) {
        position = newPosition;
    }
    void randomTeleport() {
        move(getFurthestPosition() + getSize() / 2.0);
    }
    sf::Vector2<double> getWorldPosition() {
        return position + cameraPos;
    }
    void animate(sf::RenderWindow& window) {
        orbitAlpha = orbitAlpha + orbitSpeed * deltaTime;
        while (orbitAlpha > 360.0) {
            orbitAlpha -= 360.0;
        }
        int increment = 360 / correctedSmokeAmount;
        for (int i = 0; i < correctedSmokeAmount; i += 1) {
            double angleRad = (increment * i) * pi / 180.0;
            smoke.setPosition(
                doubleToFloat(sf::Vector2<double>(
                    (sin(angleRad + orbitAlpha) * orbitRadius) + getWorldPosition().x,
                    (cos(angleRad + orbitAlpha) * orbitRadius) + getWorldPosition().y
                )
                ));
            window.draw(smoke);
        }
    }
    void moveTo(sf::Vector2<double> targetPos) {
        if (Mode != "Night") return;
        sf::Vector2<double> directionVector = targetPos - (getWorldPosition());
        if (directionVector.x == 0 && directionVector.y == 0) return;
        if (directionVector.length() > getSize().length() / 2.0) {
            position += directionVector.normalized() * deltaTime * speed;
        }
    }
    void pickWanderTarget(sf::Vector2<double> targetPos) {
        sf::Vector2<double> ghostPosRef = getWorldPosition();

        sf::Vector2<double> toPlayer = targetPos - ghostPosRef;
        if (toPlayer.length() == 0.0) return;

        sf::Vector2<double> dir = toPlayer.normalized();

        double rand01 = (double)(rand() % 2001 - 1000) / 1000.0;
        double angle = rand01 * (pi / 4.0);

        double cosBias = cos(angle);
        double sinBias = sin(angle);

        sf::Vector2<double> biasedDir = {
            dir.x * cosBias - dir.y * sinBias,
            dir.x * sinBias + dir.y * cosBias
        };

        // Distance toward player
        double dist = (double)(3 + rand() % 4) * tileSize;

        wanderTarget = getWorldPosition() + biasedDir * dist;
    }
    void wander(sf::Vector2<double> targetPos) {
        wanderTimer += deltaTime;

        if (wanderTimer >= wanderInterval) {
            wanderTimer = 0.0;
            pickWanderTarget(targetPos);
        }

        sf::Vector2<double> dir = wanderTarget - getWorldPosition();
        if (dir.length() > 1.0) {
            position += dir.normalized() * speed * 0.5 * deltaTime;
        }
    }
    void Logic(sf::Vector2<double> targetPos) {
        if (Mode != "Night") return; // avoid operations during the day
        if (FuzzyEq(targetPos, { 0.0, 0.0 })) {
            state = "Wander";
            wander(targetPos);
            chase.setVolume(0.0f);
            orbitSpeed = 5.0;
            return;
        };
        if ((targetPos - getWorldPosition()).length() <= sightRadius && state != "Recover") {
            if (state != "Chase") {
                state = "Chase";
                orbitSpeed = 25.0;
                chase.setPlayingOffset(sf::Time());
            }
        }
        else if (state != "Recover") {
            state = "Wander";
        }
        if (FuzzyEq(Health, 0.0)) {
            if (state != "Recover") {
                state = "Recover";
                randomTeleport();
                Health = 10.0;
            }
        }
        if (FuzzyEq(Health, 100.0) && state == "Recover") {
            Health = 100.0;
            state = "Wander";
        }
        if (state == "Chase") {
            chase.setVolume(100.0f);
            moveTo(targetPos);
        }
        else if (state == "Wander") {
            //Randomly walk to a tile, random should be weighted towards the player position, intention is, that even if the ghost is placed on the furthest corner of the map, it can reach the player during the night
            wander(targetPos);
            chase.setVolume(0.0f);
            orbitSpeed = 5.0;
        }
        else {
            chase.setVolume(0.0f);
            orbitSpeed = 5.0;
            Health = std::min(Health + 10.0 * deltaTime, 100.0);
        }
        
    }
    void draw(sf::RenderWindow& window, Player& player) {
        if (Mode != "Night") return;
        if (state == "recover") return;
        ghostEyes.setPosition(doubleToFloat(position + cameraPos));
        animate(window);
        window.draw(ghostEyes);
    }
};

bool playerCollidingWithGhost(Player& player, Ghost& ghost) {
    return (
        ghost.getWorldPosition().x < player.getPosition().x + player.getSize().x &&
        ghost.getWorldPosition().x + ghost.getSize().x > player.getPosition().x &&
        ghost.getWorldPosition().y < player.getPosition().y + player.getSize().y &&
        ghost.getWorldPosition().y + ghost.getSize().y > player.getPosition().y
        );
}
void takeDamage(Player& player, Ghost& ghost) {
    if (Mode == "Day") return;
    if (playerCollidingWithGhost(player, ghost) && player.dead != true) { // Always running even if idle
        player.hitTimer = std::max(player.hitTimer - deltaTime, 0.0);
        if (player.hitTimer <= 0.0) {
            hit.play();
            player.Health = std::max(player.Health - ghost.damage, 0.0);
            if (player.Health <= 0.0) {
                death.play();
            }
            player.hitTimer = player.hitCooldown;
        }
    }
}

struct Bullet {
    bool active = false;
    sf::Vector2<double> position = { 0.0, 0.0 };
    sf::Vector2<double> velocity = { 0.0, 0.0 };
    
    double speed = 1200.0;
    double lifetime = 1.5;
    double age = 0.0;

    double radius = 2.0;
    sf::CircleShape c{(float)radius};
    double damage = 5.0;
    Bullet() {}
    Bullet(sf::Vector2<double> startPos, sf::Vector2<double> dir) {
        active = true;
        position = startPos;
        velocity = dir.normalized() * speed;
        age = 0.0;
    }

    sf::Vector2<double> getWorldPosition() {
        return position;
    }

    sf::Vector2<double> getSize() {
        double d = radius * 2.0;
        return { d, d };
    }

    bool collidesWithGhost(Ghost& ghost) {
        sf::Vector2<double> aPos = getWorldPosition();
        sf::Vector2<double> aSize = getSize();
        sf::Vector2<double> bPos = ghost.getWorldPosition();
        sf::Vector2<double> bSize = ghost.getSize();

        return (
            aPos.x < bPos.x + bSize.x &&
            aPos.x + aSize.x > bPos.x &&
            aPos.y < bPos.y + bSize.y &&
            aPos.y + aSize.y > bPos.y
            );
    }

    void update(Ghost& ghost) {
        if (!active) return;

        age += deltaTime;
        if (collidesWithGhost(ghost)) {
            active = false;
            ghost.Health = std::max(ghost.Health - damage, 0.0);
            return;
        }
        if (age >= lifetime) {
            active = false;
            return;
        }

        position += velocity * deltaTime;
    }

    void draw(sf::RenderWindow& window) {
        if (!active) return;
        c.setFillColor(sf::Color::Yellow);
        c.setPosition(doubleToFloat(position));
        window.draw(c);
    }
};
dynArray<Bullet> Bullets = {};

void spawnBullet(sf::Vector2<double> startPos, sf::Vector2<double> dir) {
    fire.play();
    Bullets.insert({startPos, dir});
}

struct Mercenary {
    sf::Vector2<int> size = { 48, 64 };
    sf::Vector2<int> sizeBias = { -48 + 16, -64 + 32 };
    sf::Vector2<int> offsetBias = { -3, -10 };
    sf::Vector2<int> offset = { (48 - size.x) / 2 + 2, (64 - size.y) / 2 + 1 };
    sf::Vector2<double> scale = { 2.5, 2.5 };
    sf::Vector2<double> position = { 0.0, 0.0 };
    sf::Vector2<int> offsetPos = { 48 - 16 / 2, 64 - 32 / 2 };

    double Health = 100.0;
    bool hired = false;

    std::string Gender = random(1, 2) == 1 ? "CharM" : "CharF";
    std::string Anim = "Idle";
    std::string animVariant = "Down";
    std::string state = "Idle";

    double speed = 250.0;
    double speedToAnimFPSRatio = 5.0 / 200.0;

    Inventory mercInv;
    Animator animator = Animator(Gender, size, speedToAnimFPSRatio * speed, offset);

    int followIndex = 0;
    double followSpacing = 50.0;
    double moveDeadzoneEnd = 80.0;
    double moveDeadzoneStart = 40.0;
    bool hitDeadzone = false;
    double fireTimer = 0.0;
    bool dead = false;
    double deathTimer = 0.0;
    double corpseLifetime = 15.0;
    bool markforDelete = false;
    double hitTimer = 0.0;
    double hitCooldown = 0.5;
    Mercenary() {
        mercInv.randomize();
        animator.Anim = Anim;
        animator.Variant = animVariant + (mercInv.holdingGun ? "Gun" : "");
    }
    sf::Vector2<double> getSize() {
        return sf::Vector2<double>(
            (size.x + sizeBias.x + offsetBias.x) * scale.x,
            (size.y + sizeBias.y + offsetBias.y) * scale.y
        );
    }
    sf::Vector2<double> getBiasedWorldPosition() {
        return sf::Vector2<double>(position.x + offsetPos.x + cameraPos.x, position.y + offsetPos.y + cameraPos.y);
    }
    sf::Vector2<double> getWorldPosition() {
        return position + cameraPos;
    }

    void changeAnim(std::string anim, std::string variant) {
        Anim = anim;
        animVariant = variant;
        animator.changeAnim(anim, variant + ((mercInv.holdingGun && anim != "Death") ? "Gun" : ""));
    }

    void trueMove(sf::Vector2<double> offset) {
        position += offset;
    }
    void bareMove(sf::Vector2<double> offset) {
        position = offset;
    }
    Mercenary(sf::Vector2<double> initialPos, bool hired = false, int followIndex = 0) : hired(hired), followIndex(followIndex) {
        bareMove(initialPos + cameraPos + getSize() / 2.0);
    }
    void move(sf::Vector2<double> dir, Player& player) {
        if (Health <= 0.0) return;

        if (FuzzyEq(dir, { 0.0, 0.0 })) {
            if (Anim != "Idle") {
                changeAnim("Idle", animVariant);
            }
            return;
        }

        dir = dir.normalized();
        sf::Vector2<double> offset = dir * speed * deltaTime;
        changeAnim(player.Anim, player.animVariant);
        trueMove(offset);
    }

    void followPlayer(Player& player) {
        // Direction player is facing
        sf::Vector2<double> backDir = { 0.0, 1.0 }; // default Down

        if (animVariant == "Up")    backDir = { 0.0, 1.0 };
        if (animVariant == "Down")  backDir = { 0.0, -1.0 };
        if (animVariant == "Left")  backDir = { 1.0, 0.0 };
        if (animVariant == "Right") backDir = { -1.0, 0.0 };

        // Target position behind player
        sf::Vector2<double> target =
            player.getPosition() +
            backDir * (followSpacing * followIndex);

        sf::Vector2<double> toTarget =
            target - getBiasedWorldPosition();

        double dist = toTarget.length();

        // --- DEADZONE ---
        if ((dist <= moveDeadzoneEnd && hitDeadzone == false) || (dist <= moveDeadzoneStart && hitDeadzone == true)) {
            hitDeadzone = false;
            if (Anim != "Idle") {
                changeAnim("Idle", animVariant);
            }
            return;
        }
        if (dist > moveDeadzoneEnd) {
            hitDeadzone = true;
        }

        // Movement
        sf::Vector2<double> dir = toTarget.normalized();
        sf::Vector2<double> offset = dir * speed * deltaTime;

        // Animation direction
        changeAnim(player.Anim, player.animVariant);

        trueMove(offset);
    }


    void acquireTarget(Ghost& ghost) {
        if (ghost.Health <= 0.0) return;
        if (Mode == "Day") return;

        double dist =
            (ghost.getWorldPosition() - getBiasedWorldPosition()).length();

        if (dist < sightRadius) {
            state = "Combat";
        }
    }

    void combat(Player& player, Ghost& ghost) {
        if (ghost.Health <= 0.0 || (ghost.getWorldPosition() - getBiasedWorldPosition()).length() > sightRadius || Mode == "Day") {
            state = "Follow";
            return;
        }

        sf::Vector2<double> toGhost = ghost.getWorldPosition() - getBiasedWorldPosition();

        if (std::abs(toGhost.x) > std::abs(toGhost.y))
            changeAnim("Idle", toGhost.x < 0 ? "Left" : "Right");
        else
            changeAnim("Idle", toGhost.y < 0 ? "Up" : "Down");

        fireTimer = std::max(fireTimer - deltaTime, 0.0);
        if (FuzzyEq(fireTimer, 0.0)) {
            fireTimer = 1.0;
            spawnBullet(
                getBiasedWorldPosition() + getSize() / 2.0,
                toGhost.normalized()
            );
        }

        followPlayer(player);
    }

    void handleDeath() {
        if (state != "Dead") {
            state = "Dead";
            dead = true;
            changeAnim("Death", animVariant);
            deathTimer = 0.0;
        }
        deathTimer += deltaTime;
        if (deathTimer >= corpseLifetime) {
            markforDelete = true;
        }
    }
    bool collidingWithGhost(Ghost& ghost) {
        return (
            ghost.getWorldPosition().x < getBiasedWorldPosition().x + getSize().x &&
            ghost.getWorldPosition().x + ghost.getSize().x > getBiasedWorldPosition().x &&
            ghost.getWorldPosition().y < getBiasedWorldPosition().y + getSize().y &&
            ghost.getWorldPosition().y + ghost.getSize().y > getBiasedWorldPosition().y
            );
    }
    void takeDamage(Ghost& ghost) {
        if (Mode == "Day") return;
        if (collidingWithGhost(ghost) && dead != true) { // Always running even if idle
            hitTimer = std::max(hitTimer - deltaTime, 0.0);
            if (hitTimer <= 0.0) {
                hit.play();
                Health = std::max(Health - ghost.damage, 0.0);
                if (Health <= 0.0) {
                    death.play();
                }
                hitTimer = hitCooldown;
            }
        }
    }

    void Logic(Player& player, Ghost& ghost) {
        if (dead == true) {
            handleDeath();
            return;
        }
        takeDamage(ghost);
        if (FuzzyEq(Health, 0.0)) {
            handleDeath();
            return;
        }

        if (!hired) return;
        speed = player.speed;
        acquireTarget(ghost);
        if (state == "Combat")
            combat(player, ghost);
        else {
            state = "Follow";
            followPlayer(player);
        }
    }

    void draw(sf::RenderWindow& window) {
        animator.draw(window, getWorldPosition(), scale);
    }
};
dynArray<Mercenary> mercenaries = {};

sf::Vector2<double> getClosestTarget(Ghost& ghost, Player& player) {
    int size = mercenaries.size();
    sf::Vector2<double>* positions = new sf::Vector2<double>[size + 1];
    int occupied = 0;
    for (int i = 0; i < size; i += 1) {
        Mercenary& mercenary = mercenaries[i];
        if (!mercenary.dead) {
            positions[occupied] = mercenary.getBiasedWorldPosition();
            occupied += 1;
        }
    }
    if (!player.dead) {
        positions[occupied] = player.getPosition();
        occupied += 1;
    }
    double closestDist;
    sf::Vector2<double> closestPos = {0.0, 0.0};
    bool firstInit = true;
    for (int i = 0; i < occupied; i += 1) {
        sf::Vector2<double> pos = positions[i] - ghost.getWorldPosition();
        double dist = 0.0;
        if (FuzzyEq(pos, { 0.0, 0.0 })) {
            dist = 0.0;
        }
        else {
            dist = pos.length();
        }
        if (firstInit || dist < closestDist) {
            firstInit = false;
            closestDist = dist;
            closestPos = positions[i];
        }
    }
    delete[] positions;
    return closestPos;
}

sf::Vector2<double> getFurthestPosition() {
    double furthestDist;
    sf::Vector2<double> furthestPos;
    bool firstInit = true;
    int furthestX, furthestY;
    for (int y = 0; y < worldSize; y += 1) {
        for (int x = 0; x < worldSize; x += 1) {
            grassTile& tile = tileMap[y][x];
            sf::Vector2<double> pos = playerPos - (tile.position);
            double dist = 0.0;
            if (FuzzyEq(pos, { 0.0, 0.0 })) {
                dist = 0.0;
            }
            else {
                dist = pos.length();
            }
            if (firstInit || dist > furthestDist) {
                firstInit = false;
                furthestDist = dist;
                furthestPos = (tile.position);
            }
        }
    }
    return furthestPos;
}

sf::Vector2<double> Lerp(sf::Vector2<double> Start, sf::Vector2<double> End, double alpha) {
    return {
        Start.x + (End.x - Start.x) * alpha,
        Start.y + (End.y - Start.y) * alpha,
    };
}

struct Pickup {
    std::string type;
    sf::Vector2<double> position = {0.0, 0.0};
    sf::Vector2<int> size = {tileSize, tileSize};
    sf::Vector2<double> scale = {0.4, 0.4};
    sf::Sprite Sprite = sf::Sprite(*pickUpTextures, sf::IntRect({0, 0}, size));
    double floatAlpha = 0.0;
    double floatAnimSpeed = 50.0;
    sf::Vector2<double> Top = { 0.0, 10.0 };
    sf::Vector2<double> Bottom = { 0.0, -5.0 };
    grassTile* associatedTile = nullptr;
    bool markforDelete = false;
    int getIdx(std::string type) {
        int returnIdx = -1;
        for (int i = 0; i < pickups; i += 1) {
            if (pickupNames[i] == type) {
                returnIdx = i;
                break;
            }
        }
        return returnIdx;
    }
    Pickup() : associatedTile(nullptr) {};
    void assign(sf::Vector2<double> Position, std::string Type, grassTile *AssociatedTile) {
        associatedTile = AssociatedTile;
        type = Type;
        position = { Position.x + (size.x * scale.x) / 2, Position.y + (size.y * scale.y) / 2 };
        int Index = getIdx(type);
        Sprite.setTexture(pickUpTextures[Index]);

    }
    void draw(sf::RenderWindow& window) {
        if (markforDelete) return;
        Sprite.setScale(doubleToFloat(scale));
        sf::Vector2<double> Final = position + cameraPos;
        Sprite.setPosition(doubleToFloat(Lerp(Final, Final + Top, sin(floatAlpha * pi / 180.0))));
        Sprite.setColor(colorTint);
        window.draw(Sprite);
    }
    bool isCollidingWithPlayer(Player& player) {
        sf::Vector2<double> playerSize = player.getSize();
        return (
            position.x + cameraPos.x < player.getPosition().x + playerSize.x &&
            position.x + cameraPos.x + (size.x * scale.x) > player.getPosition().x &&
            position.y + cameraPos.y < player.getPosition().y + playerSize.y &&
            position.y + cameraPos.y + (size.y * scale.y) > player.getPosition().y
            );
    }
    void Logic(Player &player) {
        if (markforDelete) return;
        floatAlpha = (floatAlpha + floatAnimSpeed * deltaTime);
        while (floatAlpha > 360.0) {
            floatAlpha -= 360.0;
        }
        if (isCollidingWithPlayer(player)) {
            markforDelete = true;
            player.playerInv.addItem(type, 1);
            pickup.play();
        }
    }
};
Pickup pickUps[pickUpCount];

double rad(double self) {
    return self * (pi / 180);
}

int rad(int self) {
    return self * (pi / 180);
}

void destroyPickups() {
    for (int i = pickUpCount - 1; i >= 0; i -= 1) {
        pickUps[i].markforDelete = true;
    }
}

void killUnhiredMercenaries() {
    for (int i = mercenaries.size() - 1; i >= 0; i -= 1) {
        if (!mercenaries[i].hired) {
            mercenaries[i].Health = 0.0;
        }
    }
}

void spawnMercenaries() {
    killUnhiredMercenaries();
    const int currSize = mercenaries.size();
    for (int i = 0; i < 3 - currSize;) {
        int ranX = random(0, 100);
        int ranY = random(0, 100);
        grassTile& tile = tileMap[ranY][ranX];
        if (tile.isEmpty) {
            mercenaries.insert({ (tile.position) , true, i});
            i += 1;
        }
    }
}

void makePickups() {
    destroyPickups();
    int i = 0;
    while (i < pickUpCount) {
        int ranX = random(0, 100);
        int ranY = random(0, 100);
        int ranType = random(0, pickups - 1);
        grassTile& tile = tileMap[ranY][ranX];
        if (tile.isEmpty) {
            pickUps[i].assign(sf::Vector2<double>((double)ranX * (double)tileSize, (double)ranY * (double)tileSize), pickupNames[ranType], &tile);
            pickUps[i].markforDelete = false;
            tile.isEmpty = false;
            i += 1;
        }
    }
}

double nightTimePitches[6] = { 0.3, 0.2, 0.1, 0.1, 0.1, 0.1 };
sf::Music* nightTimeAudios[6] = { &Halloween3, &Halloween2, &Halloween3, &Halloween1, &Halloween2, &HalloweenGhost };
bool nightAudioCool = true;
double lastNightAudioTime = 0.0;
double colorAlpha = 0.0f;
bool cycleTransitionRunning = false;
void setNight(Ghost& ghost) {
    if (cycleTransitionRunning == false) {
        destroyPickups();
        killUnhiredMercenaries();
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
        ghost.randomTeleport();
        Mode = "Night";
    }
}

void setDay(Merchant& merchant) {
    if (cycleTransitionRunning == false) {
        makePickups();
        spawnMercenaries();
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
        merchant.teleportShop();
        dayTheme.setVolume(100.0f);
        cycleTransitionRunning = false;
        Mode = "Day";
        chase.setVolume(0.0f);
        //for (int i = 0; i < mercenaries.size(); i += 1) {
            //mercenaries[i].Health = 0.0;
        //}
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

void drawMap(sf::RenderWindow& window, Player& player, Ghost& ghost, Merchant& merchant) {
    int minX = std::clamp<int>(floor(-cameraPos.x / tileSize) - 2, 0, worldSize - 1);
    int minY = std::clamp<int>(floor(-cameraPos.y / tileSize) - 2, 0, worldSize - 1);
    int maxX = std::clamp<int>(ceil((-cameraPos.x + windowSize.x) / tileSize) + 2, 0, worldSize - 1);
    int maxY = std::clamp<int>(ceil((-cameraPos.y + windowSize.y) / tileSize) + 2, 0, worldSize - 1);
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            tileMap[y][x].draw(window);
        }
    }
    for (int i = pickUpCount - 1; i >= 0; i -= 1) {
        Pickup& pickup = pickUps[i];
        if (pickup.markforDelete) {
            grassTile* associatedTile = pickup.associatedTile;
            if (associatedTile != nullptr) {
                (*associatedTile).isEmpty = true;
                associatedTile = nullptr;
            }
        }
        else {
            bool visX = (pickup.position.x + cameraPos.x + tileSize) >= 0 && (pickup.position.x + cameraPos.x) <= windowSize.x;
            bool visY = (pickup.position.y + cameraPos.y + tileSize) >= 0 && (pickup.position.y + cameraPos.y) <= windowSize.y;
            if (visX && visY) {
                pickup.Logic(player);
                pickUps[i].draw(window);
            }
        }
    }
    for (int i = Bullets.size() - 1; i >= 0; i -= 1) {
        Bullet& bullet = Bullets[i];
        if (!bullet.active) {
            Bullets.remove(i);
        }
        else {
            bool visX = (bullet.getWorldPosition().x + tileSize) >= 0 && (bullet.getWorldPosition().x) <= windowSize.x;
            bool visY = (bullet.getWorldPosition().y + tileSize) >= 0 && (bullet.getWorldPosition().y) <= windowSize.y;
            if (visX && visY) {
                bullet.update(ghost);
                bullet.draw(window);
            }
        }
    }
    for (int i = 0; i < decorMap.size(); i += 1) {
        mapDecor& decor = decorMap[i];
        bool visX = (decor.position.x + cameraPos.x + tileSize) >= 0 && (decor.position.x + cameraPos.x) <= windowSize.x;
        bool visY = (decor.position.y + cameraPos.y + tileSize) >= 0 && (decor.position.y + cameraPos.y) <= windowSize.y;
        if (visX && visY) {
            double playerDepth = player.getPosition().y + player.getSize().y;
            double selfDepth = decor.position.y + cameraPos.y + (decor.size.y * decor.scale.y);
            if (decor.alwaysDrawUnderPlayer == true || playerDepth > selfDepth)
                decor.draw(window);
        }
    }
    for (int i = mercenaries.size() - 1; i >= 0; i -= 1) {
        Mercenary& mercenary = mercenaries[i];
        if (mercenary.markforDelete) {
            mercenaries.remove(i);
        }
        else {
            bool visX = (mercenary.getWorldPosition().x + tileSize) >= 0 && (mercenary.getWorldPosition().x) <= windowSize.x;
            bool visY = (mercenary.getWorldPosition().y + tileSize) >= 0 && (mercenary.getWorldPosition().y) <= windowSize.y;
            if (visX && visY) {
                mercenary.draw(window);
            }
            mercenary.Logic(player, ghost);
        }
    }
    merchant.draw(window);
    player.draw(window);
    ghost.Logic(getClosestTarget(ghost, player));
    if (playerCollidingWithGhost(player, ghost)) {
        takeDamage(player, ghost);
    }
    ghost.draw(window, player);
    for (int i = 0; i < decorMap.size(); i += 1) {
        mapDecor& decor = decorMap[i];
        bool visX = (decor.position.x + cameraPos.x + tileSize) >= 0 && (decor.position.x + cameraPos.x) <= windowSize.x;
        bool visY = (decor.position.y + cameraPos.y + tileSize) >= 0 && (decor.position.y + cameraPos.y) <= windowSize.y;
        if (visX && visY) {
            double playerDepth = player.getPosition().y + player.getSize().y;
            double selfDepth = decor.position.y + cameraPos.y + (decor.size.y * decor.scale.y);
            if (decor.alwaysDrawUnderPlayer == false && playerDepth < selfDepth)
                decor.draw(window);
        }
    }
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            tileMap[y][x].drawFog(window);
        }
    }
}

int main()
{
    srand(time(0));
    sf::VideoMode screenMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(screenMode), "Withered Roots", sf::Style::Titlebar | sf::Style::Close);
    windowSize = sf::Vector2<int>(screenMode.size.x, screenMode.size.y);
    initAnims();
    initTileMap();
    Player player{};
    Ghost ghost{};
    Merchant merchant{};
    player.bareMove((sf::Vector2<double>(screenMode.size.x / 2.0f - 25.0f, screenMode.size.y / 2.0f - 25.0f)));
    merchant.teleportShop();
    ghost.randomTeleport();
    sf::Clock deltaTimer;
    deltaTimer.start();
    dayTheme.setLooping(true);
    dayTheme.play();
    chase.setLooping(true);
    chase.play();
    chase.setVolume(0.0f);
    makePickups();
    spawnMercenaries();
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
            if ((*event).is<sf::Event::Closed>())
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
            if (!FuzzyEq(player.stamina, 0)) {
                player.speed = 400.0;
            }
            else {
                player.speed = 250.0;
            }
            player.stamina = std::max(0.0, player.stamina - 10.0 * deltaTime);
        }
        else {
            player.speed = 250.0;
            player.stamina = std::min(100.0, player.stamina + 4.0 * deltaTime);
        }
        if (cycleTransitionRunning == false) {
            gameTimer += deltaTime;
        }
        if (Mode == "Day" && (gameTimer > dayTime || cycleTransitionRunning)) {
            setNight(ghost);
        }
        else if (Mode == "Night" && (gameTimer > nightTime || cycleTransitionRunning)) {
            setDay(merchant);
        }
        if (Mode == "Night") {
            nightTimeAudioPlayLogic();
        }
        player.move(movementOffset, deltaTime);
        window.clear();
        drawMap(window, player, ghost, merchant);
        window.display();
    }
    return 0;
}