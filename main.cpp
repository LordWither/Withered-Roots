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
const int pickups = 5;
sf::Texture textureData[textureCount] = {sf::Texture("./Assets/Grass.png"),sf::Texture("./Assets/Tree.png") ,sf::Texture("./Assets/Rocks.png") ,sf::Texture("./Assets/Bush.png") };
sf::Texture pickUpTextures[pickups] = {sf::Texture("./Assets/Bandages.png"), sf::Texture("./Assets/Beans.png"), sf::Texture("./Assets/Chocolate.png"), sf::Texture("./Assets/CampfireMaterials.png"), sf::Texture("./Assets/Cash.png")};
std::string pickupNames[pickups] = {"Bandages", "Beans", "Chocolate", "Campfire Materials", "Cash"};
int pickupPrice[pickups - 1] = { 200, 50, 150, 150};
int pickUpAmounts[pickups] = {1, 1, 1, 1, 20};
char textureKeys[textureCount] = { 'G', 'T', 'R', 'B'};
sf::Vector2<double> textureScales[textureCount] = { {1.0, 1.0},{1.0, 1.0},{0.6, 0.6},{0.7, 0.7} };
bool textureAlwaysDrawsUnderPlayer[textureCount] = { false, false, true, false };

sf::Texture ghostEyesTex{"./Assets/GhostEyes.png"};
sf::Texture ghostSmokeTex{ "./Assets/GhostSmoke.png" };
sf::Texture merchantShopTex("./Assets/Shop.png");
sf::Texture campfire("./Assets/Campfire.png");

const int worldSize = 100;
const int tileSize = 100;
sf::Color dayTint(255, 200, 100, 255U);
sf::Color nightTint(29, 89, 255, 255U);
sf::Color colorTint(255, 255, 255, 255U);
sf::Color fogColor(128, 128, 128, 255U);
std::string Mode = "Day";
const int pickUpCount = 10;
double currentTime = 0.0;
double deltaTime = 0.0;
double combinedDeltaTime = 0.0;
const double pi = 3.14159265358979323846;
double fogStartDay = 450.0;
double fogEndDay = 1300.0;
double fogStartNight = fogStartDay * 0.9;
double fogEndNight = fogEndDay * 0.9;
double dayTime = 150.0;
double nightTime = 300.0;
double fogStart = 0.0;
double fogEnd = 0.0;
double gameTimer = 0.0;

//Ghost logic
double sightRadius = 20 * tileSize;

sf::Music Halloween1("./Assets/Halloween1.ogg");
sf::Music Halloween2("./Assets/Halloween2.ogg");
sf::Music Halloween3("./Assets/Halloween3.ogg");
sf::Music HalloweenGhost("./Assets/HalloweenGhost.wav");
sf::Music purchase("./Assets/purchase.ogg");
sf::Music dayTheme("./Assets/DayTheme.ogg");
sf::Music menuTheme("./Assets/Menu.ogg");
sf::Music pickup("./Assets/pickup.ogg");
sf::Music chase("./Assets/chase.ogg");
sf::Music error("./Assets/error.ogg");
sf::Music death("./Assets/death.ogg");
sf::Music hit("./Assets/hit.ogg");
sf::Music fire("./Assets/fire.ogg");
sf::Music itemUse("./Assets/itemUse.ogg");
sf::Font defFont("./Assets/Font.ttf");

std::string shopDescriptionText = ""; //This is a clever workaround for displaying shop text, works well, comment here just incase anyone reading this code wonders

template <typename arrType>
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
void initMercenaryTextures();
void initAnims() {
    dynArray<sf::Texture> charMIdleUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle1.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle2.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle3.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle4.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle5.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle6.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle7.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle8.png")}, 8 };
    dynArray<sf::Texture> charMIdleDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle1.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle2.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle3.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle4.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle5.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle6.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle7.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle8.png")}, 8 };
    dynArray<sf::Texture> charMIdleLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle1.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle2.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle3.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle4.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle5.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle6.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle7.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle8.png")}, 8 };
    dynArray<sf::Texture> charMIdleRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle1.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle2.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle3.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle4.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle5.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle6.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle7.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle8.png")}, 8 };
    dynArray<sf::Texture> charMWalkUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk1.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk2.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk3.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk4.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk5.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk6.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk7.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk8.png")}, 8 };
    dynArray<sf::Texture> charMWalkDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk1.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk2.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk3.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk4.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk5.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk6.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk7.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk8.png")}, 8 };
    dynArray<sf::Texture> charMWalkLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk1.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk2.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk3.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk4.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk5.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk6.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk7.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk8.png")}, 8 };
    dynArray<sf::Texture> charMWalkRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk1.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk2.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk3.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk4.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk5.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk6.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk7.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk8.png")}, 8 };
    dynArray<sf::Texture> charMDeathUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Death/Up/Death1.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death2.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death3.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death4.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death5.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death6.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death7.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death8.png")}, 8 };
    dynArray<sf::Texture> charMDeathDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Death/Down/Death1.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death2.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death3.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death4.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death5.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death6.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death7.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death8.png")}, 8 };
    dynArray<sf::Texture> charMDeathLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Death/Left/Death1.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death2.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death3.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death4.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death5.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death6.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death7.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death8.png")}, 8 };
    dynArray<sf::Texture> charMDeathRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Death/Right/Death1.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death2.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death3.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death4.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death5.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death6.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death7.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death8.png")}, 8 };
    dynArray<sf::Texture> charFIdleUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle1.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle2.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle3.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle4.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle5.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle6.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle7.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle8.png")}, 8 };
    dynArray<sf::Texture> charFIdleDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle1.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle2.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle3.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle4.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle5.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle6.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle7.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle8.png")}, 8 };
    dynArray<sf::Texture> charFIdleLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle1.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle2.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle3.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle4.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle5.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle6.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle7.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle8.png")}, 8 };
    dynArray<sf::Texture> charFIdleRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle1.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle2.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle3.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle4.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle5.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle6.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle7.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle8.png")}, 8 };
    dynArray<sf::Texture> charFWalkUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk1.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk2.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk3.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk4.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk5.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk6.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk7.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk8.png")}, 8 };
    dynArray<sf::Texture> charFWalkDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk1.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk2.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk3.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk4.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk5.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk6.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk7.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk8.png")}, 8 };
    dynArray<sf::Texture> charFWalkLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk1.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk2.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk3.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk4.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk5.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk6.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk7.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk8.png")}, 8 };
    dynArray<sf::Texture> charFWalkRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk1.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk2.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk3.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk4.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk5.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk6.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk7.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk8.png")}, 8 };
    dynArray<sf::Texture> charFDeathUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Death/Up/Death1.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death2.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death3.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death4.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death5.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death6.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death7.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death8.png")}, 8 };
    dynArray<sf::Texture> charFDeathDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Death/Down/Death1.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death2.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death3.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death4.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death5.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death6.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death7.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death8.png")}, 8 };
    dynArray<sf::Texture> charFDeathLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Death/Left/Death1.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death2.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death3.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death4.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death5.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death6.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death7.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death8.png")}, 8 };
    dynArray<sf::Texture> charFDeathRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Death/Right/Death1.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death2.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death3.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death4.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death5.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death6.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death7.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMIdleUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle1.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle2.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle3.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle4.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle5.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle6.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle7.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Up/Idle8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMIdleDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle1.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle2.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle3.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle4.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle5.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle6.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle7.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Down/Idle8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMIdleLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle1.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle2.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle3.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle4.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle5.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle6.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle7.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Left/Idle8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMIdleRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle1.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle2.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle3.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle4.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle5.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle6.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle7.png"), sf::Texture("./Assets/Animations/Character/M/Idle/Right/Idle8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMWalkUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk1.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk2.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk3.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk4.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk5.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk6.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk7.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Up/Walk8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMWalkDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk1.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk2.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk3.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk4.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk5.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk6.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk7.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Down/Walk8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMWalkLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk1.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk2.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk3.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk4.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk5.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk6.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk7.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Left/Walk8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMWalkRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk1.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk2.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk3.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk4.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk5.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk6.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk7.png"), sf::Texture("./Assets/Animations/Character/M/Walk/Right/Walk8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMDeathUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Death/Up/Death1.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death2.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death3.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death4.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death5.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death6.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death7.png"), sf::Texture("./Assets/Animations/Character/M/Death/Up/Death8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMDeathDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Death/Down/Death1.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death2.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death3.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death4.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death5.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death6.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death7.png"), sf::Texture("./Assets/Animations/Character/M/Death/Down/Death8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMDeathLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Death/Left/Death1.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death2.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death3.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death4.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death5.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death6.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death7.png"), sf::Texture("./Assets/Animations/Character/M/Death/Left/Death8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharMDeathRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/M/Death/Right/Death1.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death2.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death3.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death4.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death5.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death6.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death7.png"), sf::Texture("./Assets/Animations/Character/M/Death/Right/Death8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFIdleUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle1.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle2.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle3.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle4.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle5.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle6.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle7.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Up/Idle8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFIdleDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle1.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle2.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle3.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle4.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle5.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle6.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle7.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Down/Idle8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFIdleLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle1.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle2.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle3.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle4.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle5.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle6.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle7.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Left/Idle8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFIdleRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle1.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle2.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle3.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle4.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle5.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle6.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle7.png"), sf::Texture("./Assets/Animations/Character/F/Idle/Right/Idle8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFWalkUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk1.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk2.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk3.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk4.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk5.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk6.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk7.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Up/Walk8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFWalkDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk1.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk2.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk3.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk4.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk5.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk6.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk7.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Down/Walk8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFWalkLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk1.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk2.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk3.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk4.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk5.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk6.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk7.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Left/Walk8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFWalkRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk1.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk2.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk3.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk4.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk5.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk6.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk7.png"), sf::Texture("./Assets/Animations/Character/F/Walk/Right/Walk8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFDeathUp = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Death/Up/Death1.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death2.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death3.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death4.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death5.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death6.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death7.png"), sf::Texture("./Assets/Animations/Character/F/Death/Up/Death8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFDeathDown = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Death/Down/Death1.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death2.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death3.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death4.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death5.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death6.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death7.png"), sf::Texture("./Assets/Animations/Character/F/Death/Down/Death8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFDeathLeft = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Death/Left/Death1.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death2.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death3.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death4.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death5.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death6.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death7.png"), sf::Texture("./Assets/Animations/Character/F/Death/Left/Death8.png")}, 8 };
    dynArray<sf::Texture> mercenaryCharFDeathRight = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Character/F/Death/Right/Death1.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death2.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death3.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death4.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death5.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death6.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death7.png"), sf::Texture("./Assets/Animations/Character/F/Death/Right/Death8.png")}, 8 };
    dynArray<sf::Texture> merchant = { new sf::Texture[8] {sf::Texture("./Assets/Animations/Merchant/Merchant1.png"), sf::Texture("./Assets/Animations/Merchant/Merchant2.png"), sf::Texture("./Assets/Animations/Merchant/Merchant3.png"), sf::Texture("./Assets/Animations/Merchant/Merchant4.png"), sf::Texture("./Assets/Animations/Merchant/Merchant5.png"), sf::Texture("./Assets/Animations/Merchant/Merchant6.png"), sf::Texture("./Assets/Animations/Merchant/Merchant7.png"), sf::Texture("./Assets/Animations/Merchant/Merchant8.png")}, 8};
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
    animData.insert(mercenaryCharMIdleUp);
    animData.insert(mercenaryCharMIdleDown);
    animData.insert(mercenaryCharMIdleLeft);
    animData.insert(mercenaryCharMIdleRight);
    animData.insert(mercenaryCharMWalkUp);
    animData.insert(mercenaryCharMWalkDown);
    animData.insert(mercenaryCharMWalkLeft);
    animData.insert(mercenaryCharMWalkRight);
    animData.insert(mercenaryCharMDeathUp);
    animData.insert(mercenaryCharMDeathDown);
    animData.insert(mercenaryCharMDeathLeft);
    animData.insert(mercenaryCharMDeathRight);
    animData.insert(mercenaryCharFIdleUp);
    animData.insert(mercenaryCharFIdleDown);
    animData.insert(mercenaryCharFIdleLeft);
    animData.insert(mercenaryCharFIdleRight);
    animData.insert(mercenaryCharFWalkUp);
    animData.insert(mercenaryCharFWalkDown);
    animData.insert(mercenaryCharFWalkLeft);
    animData.insert(mercenaryCharFWalkRight);
    animData.insert(mercenaryCharFDeathUp);
    animData.insert(mercenaryCharFDeathDown);
    animData.insert(mercenaryCharFDeathLeft);
    animData.insert(mercenaryCharFDeathRight);
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
    animNames.insert("MercenaryCharMIdleUp");
    animNames.insert("MercenaryCharMIdleDown");
    animNames.insert("MercenaryCharMIdleLeft");
    animNames.insert("MercenaryCharMIdleRight");
    animNames.insert("MercenaryCharMWalkUp");
    animNames.insert("MercenaryCharMWalkDown");
    animNames.insert("MercenaryCharMWalkLeft");
    animNames.insert("MercenaryCharMWalkRight");
    animNames.insert("MercenaryCharMDeathUp");
    animNames.insert("MercenaryCharMDeathDown");
    animNames.insert("MercenaryCharMDeathLeft");
    animNames.insert("MercenaryCharMDeathRight");
    animNames.insert("MercenaryCharFIdleUp");
    animNames.insert("MercenaryCharFIdleDown");
    animNames.insert("MercenaryCharFIdleLeft");
    animNames.insert("MercenaryCharFIdleRight");
    animNames.insert("MercenaryCharFWalkUp");
    animNames.insert("MercenaryCharFWalkDown");
    animNames.insert("MercenaryCharFWalkLeft");
    animNames.insert("MercenaryCharFWalkRight");
    animNames.insert("MercenaryCharFDeathUp");
    animNames.insert("MercenaryCharFDeathDown");
    animNames.insert("MercenaryCharFDeathLeft");
    animNames.insert("MercenaryCharFDeathRight");
    animNames.insert("Merchant");
    initMercenaryTextures();
}

bool starts_with(std::string self, std::string other) {
    if (self.length() < other.length()) return false;
    bool isSame = true;
    for (int i = 0; i < other.length(); i += 1) {
        if (other[i] != self[i]) {
            isSame = false;
            break;
        }
    }
    return isSame;
}

bool starts_with(std::string self, char other) {
    if (self.length() < 1) return false;
    if (self[0] == other) return true;
    return false;
}

template <typename T>
T Lerp(T Start, T End, T alpha) {
    return Start + (End - Start) * alpha;
}

void initMercenaryTextures() {
    uint8_t mFrom[11][4] = {
        { 60, 84, 143, 255 },
        { 50, 65, 127, 255 },
        { 115, 115, 115, 255 },
        { 172, 50, 50, 255 },
        { 119, 19, 19, 255 },
        { 62, 125, 141, 255 },
        { 102, 51, 75, 255 },
        { 44, 29, 72, 255 },
        { 29, 17, 51, 255 },
        { 60, 41, 94, 255 },
        { 40, 48, 112, 255 }
    };
    uint8_t mTo[11][4] = {
        { 143, 84, 80, 255 },
        { 127, 65, 50, 255 },
        { 115, 115, 115, 255 },
        { 50, 50, 172, 255 },
        { 19, 19, 119, 255 },
        { 191, 40, 40, 255 },
        { 102, 51, 51, 255 },
        { 72, 29, 44, 255 },
        { 51, 17, 29, 255 },
        { 94, 41, 60, 255 },
        { 112, 48, 40, 255 }
    };
    uint8_t fFrom[11][4] = {
        { 42, 105, 87, 255 },
        {59, 128, 77, 255},
        {143, 143, 143, 255},
        {103, 103, 103, 255},
        {115, 115, 115, 255},
        {174, 174, 174, 255},
        {110, 39, 73, 255},
        {87, 23, 72, 255},
        {49, 13, 53, 255},
        {145, 60, 81, 255},
        {103, 103, 103, 255}
    };
    uint8_t fTo[11][4] = {
        { 104, 41, 98, 255 },
        {103, 59, 127, 255},
        {70, 116, 140, 255},
        {51, 85, 102, 255},
        {65, 56, 112, 255},
        {101, 86, 173, 255},
        {25, 9, 17, 255},
        {25, 6, 21, 255},
        {11, 3, 12, 255},
        {25, 10, 14, 255},
        {70, 116, 140, 255}
    };
    for (int i = 0; i < animNames.size(); i += 1) {
        if (starts_with(animNames[i], "MercenaryChar")) {
            uint8_t (*fromArr)[4] = starts_with(animNames[i], "MercenaryCharM") ? mFrom : fFrom;
            uint8_t(*toArr)[4] = starts_with(animNames[i], "MercenaryCharM") ? mTo : fTo;
            dynArray<sf::Texture>& texData = animData[i];
            for (int ii = 0; ii < texData.size(); ii += 1) {
                sf::Texture& original = texData[ii];
                sf::Image workImg = original.copyToImage();
                sf::Vector2u size = workImg.getSize();
                for (unsigned int y = 0; y < size.y; y += 1) {
                    for (unsigned int x = 0; x < size.x; x += 1) {
                        sf::Color pixel = workImg.getPixel({ x, y });
                        for (int iii = 0; iii < 11; iii += 1) {
                            uint8_t *fromColors = fromArr[iii];
                            uint8_t *toColors = toArr[iii];
                            if (pixel.r == fromColors[0] && pixel.g == fromColors[1] && pixel.b == fromColors[2] && pixel.a == fromColors[3]) {
                                workImg.setPixel({ x, y }, sf::Color(toColors[0], toColors[1], toColors[2], toColors[3]));
                            }
                        }
                    }
                }
                (void)(original.loadFromImage(workImg));
            }
        }
    }
}
dynArray<std::string> mercenaryDialogueKey;
dynArray<std::string> mercenaryDialogueValue;
dynArray<std::string> shopDialogueKey;
dynArray<std::string> shopDialogueValue;
void initDialogues() {
    std::ifstream dialogueCont("Dialogues.txt");
    std::string line;
    std::string mode;
    while (std::getline(dialogueCont, line)) {
        if (starts_with(line, "SHOP")) {
            mode = "shop";
            continue;
        }
        else if (starts_with(line, "MERC")) {
            mode = "mercenary";
            continue;
        }
        std::string key, value;
        bool foundSemicolon = false;
        for (int i = 0; i < line.size(); i += 1) {
            if (foundSemicolon == true) {
                if (line[i] != '#') {
                    value += line[i];
                }
                else {
                    value += "\n";
                }
            }
            else {
                if (line[i] == ':') {
                    foundSemicolon = true;
                }
                else {
                    key += line[i];
                }
            }
        }
        if (mode == "shop") {
            shopDialogueKey.insert(key);
            shopDialogueValue.insert(value);
        }else if (mode == "mercenary") {
            mercenaryDialogueKey.insert(key);
            mercenaryDialogueValue.insert(value);
        }
    }
}

sf::Color Lerp(sf::Color Start, sf::Color End, double alpha) {
    return sf::Color(
        (uint8_t)((double)Start.r + ((double)End.r - (double)Start.r) * alpha),
        (uint8_t)((double)Start.g + ((double)End.g - (double)Start.g) * alpha),
        (uint8_t)((double)Start.b + ((double)End.b - (double)Start.b) * alpha),
        (uint8_t)((double)Start.a + ((double)End.a - (double)Start.a) * alpha)
    );
}

sf::Color Multiply(sf::Color Start, sf::Color End, double influence = 1.0) {
    return sf::Color(
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

struct UI {
    std::string text = "";
    unsigned int fontSize = 24;
    sf::Vector2<double> size = { 50, 100 };
    sf::Vector2<double> position = { 0, 0 };
    sf::Text Text{ defFont, text, fontSize };
    sf::Vector2<double> textOffset = { 0, 0 };
    sf::Color backgroundColor{ 255, 255, 255, 255 };
    sf::Color textColor{ 255, 255, 255, 255 };
    sf::Color borderColor{ 255, 255, 255, 255 };
    sf::RectangleShape background{doubleToFloat(size)};
    int borderPixel = 5;
    sf::RectangleShape border{ sf::Vector2f(borderPixel + 1, borderPixel + 1) };
    UI() {};
    void setText(std::string Data) {
        text = Data;
        Text.setString(text);
        sf::FloatRect textRect = Text.getLocalBounds();
        Text.setOrigin(doubleToFloat(sf::Vector2<double>(textRect.position.x + textRect.size.x / 2, textRect.position.y + textRect.size.y / 2)));
    }
    UI(sf::Vector2<double> size, sf::Vector2<double> position, std::string Data = "") : size(size), position(position) {
        if (Data.size() > 0) {
            setText(Data);
        }
    }
    bool mouseOver() {
        sf::Vector2i mouse = sf::Mouse::getPosition();
        return (
            mouse.x - size.x >= position.x &&
            mouse.x <= position.x + size.x &&
            mouse.y - size.y >= position.y &&
            mouse.y <= position.y + size.y
            );
    }
    void draw(sf::RenderWindow& window) {
        background.setFillColor(backgroundColor);
        border.setFillColor(borderColor);
        background.setSize(doubleToFloat(size));
        border.setSize(doubleToFloat(size + sf::Vector2<double>(borderPixel + 1, borderPixel + 1)));
        background.setPosition(doubleToFloat(position));
        border.setPosition(doubleToFloat(position - sf::Vector2<double>(borderPixel / 2, borderPixel / 2)));
        sf::Vector2<double> centrePos = { position.x + size.x / 2.0, position.y + size.y / 2.0 };
        window.draw(border);
        window.draw(background);
        if (text.size() > 0) {
            Text.setCharacterSize(fontSize);
            Text.setFillColor(textColor);
            Text.setPosition(doubleToFloat(centrePos + textOffset));
            window.draw(Text);
        }
    }
};

UI healthBar;
UI staminaBar;
UI timeText;
UI moneyText;
UI shopDialog;
UI shopTitle;
UI buyButtons[pickups - 1];
UI sellButtons[pickups - 1];
UI inventorySlots[pickups - 1];
UI shopInventory[pickups - 1];

bool shopOpen = false;

bool mouseOver(UI& ui) {
    sf::Vector2i mouse = sf::Mouse::getPosition();
    sf::Vector2i mouseFixOffset = {2, 15};
    return (
        mouse.x + mouseFixOffset.x >= ui.position.x + ui.textOffset.x &&
        mouse.x + mouseFixOffset.x <= ui.position.x + ui.size.x + ui.textOffset.x &&
        mouse.y + mouseFixOffset.y >= ui.position.y + ui.textOffset.y &&
        mouse.y + mouseFixOffset.y <= ui.position.y + ui.textOffset.y + ui.size.y
        );
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
            if (starts_with(animNames[i], entityName)) {
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
            if (starts_with(animationsName[i], Entity + anim + variant)) {
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
            if (starts_with(animationsName[i], Entity + Anim + Variant)) {
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
        animSprite.setColor(Multiply(Lerp(sf::Color(255, 255, 255, 255), colorTint, 0.5), customTint));
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
    sf::Vector2<double> size = { tileSize, tileSize };
    sf::Vector2<double> scale = *textureScales;
    bool alwaysDrawUnderPlayer = *textureAlwaysDrawsUnderPlayer;
    char type = *textureKeys;
    sf::Sprite Sprite = sf::Sprite(*textureData, sf::IntRect({ 0, 0 }, { tileSize, tileSize}));
    mapDecor() {}
    mapDecor(sf::Vector2<int> originalPosition, sf::Vector2<double> position, char type) : originalPosition(originalPosition), position(position), type(type) {
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

sf::Color getRanColor() {
    return sf::Color(rand() % 255, rand() % 255, rand() % 255, 100U);
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
                tileMap[y][x] = { sf::Vector2<int>(tileSize * x, tileSize * y), sf::Vector2<double>(tileSize * x, tileSize * y), true };
                grassTile& tile = tileMap[y][x];
                tile.isEmpty = false;
                if (currentLine[x] == 'S' || currentLine[x] == 'T') {
                    tile.isBlocked = true;
                }
                decorMap.insert({ sf::Vector2<int>(tileSize * x, tileSize * y), sf::Vector2<double>(tileSize * x, tileSize * y), currentLine[x] });
            }
        }
        y += 1;
    };
}

int Random(int min, int max) {
    int range = max - min + 1;
    return (rand() % range) + min;
}

double Random() { //Return random double-precision float between 0 and 1, perfect for a whole lot of things
    return (double)Random(0, 100) / 100.0;
}

//sf::Vector2<double> snapToGrid(sf::Vector2<double> position) { Might use it later, might not, commented out for now
//    return sf::Vector2<double>(floor(position.x / tileSize) * tileSize, floor(position.y / tileSize) * tileSize);
//}

bool FuzzyEq(double self, double other, double epsilon = 1e-5) {
    return (std::abs(self - other) <= epsilon);
}

struct Inventory {
    static const int gameItems = 5;
    int itemVals[gameItems] = { 100, 1, 2, 1, 0 };
    std::string itemNames[gameItems] = {"Cash", "Beans", "Bandages", "Chocolate", "Campfire Materials"};
    int itemRandLimits[gameItems][2] = { {50,200},{0,1},{0,2},{0,1}, {0, 0} };
    void randomize() {
        for (int i = 0; i < gameItems; i += 1) {
            itemVals[i] = Random(itemRandLimits[i][0], itemRandLimits[i][1]);
        }
    }
    int getItemCount(std::string item) {
        for (int i = 0; i < gameItems; i += 1) {
            if (itemNames[i] == item) {
                return itemVals[i];
            }
        }
        return 0;
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
};

struct Player {
    sf::Vector2<int> size = { 48, 64 };
    sf::Vector2<int> sizeBias = { - 48 + 16, - 64 + 32 };
    sf::Vector2<int> offsetBias = {-3, -10};
    sf::Vector2<int> offset = { (48 - size.x) / 2 + 2, (64 - size.y) / 2 + 1 };
    sf::Vector2<double> scale = {2.5, 2.5};
    sf::Vector2<double> position = { 0.0, 0.0 };
    sf::Vector2<int> offsetPos = { 48 - 16 / 2, 64 - 32 / 2 };
    double Health = 100.0;
    std::string Gender = Random(1, 2) == 1 ? "CharM" : "CharF";
    std::string Anim = "Idle";
    std::string animVariant = "Down";
    double speed = 250.0;
    double stamina = 100.0;
    double healRate = 1.0;
    double speedToAnimFPSRatio = 5.0 / 200.0;
    double deathTimer = 0.0;
    Inventory playerInv;
    sf::Vector2<double> boundsMax = sf::Vector2<double>(windowSize.x * 0.7, windowSize.y * 0.7);
    sf::Vector2<double> boundsMin = sf::Vector2<double>(windowSize.x * 0.3, windowSize.y * 0.3);;
    sf::Color color = sf::Color(255, 255, 255, 255U);
    Animator animator = Animator(Gender, size, speedToAnimFPSRatio * speed, offset);
    double hitTimer = 0.0;
    double hitCooldown = 0.5;
    bool dead = false;
    Player() {
        playerInv.randomize();
        animator.Anim = Anim;
        animator.Variant = animVariant;
    } //Keeping here in case I wanna init add code, causes no issues if I leave it here, 29th December 2025, 11:08PM, no time to remove this comment now
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
        animator.changeAnim(anim, variant);
    }
    void move(sf::Vector2<double> offset, double deltaTime) {
        if (FuzzyEq(Health, 0.0)) {
            if (Anim != "Death") {
                dead = true;
                changeAnim("Death", animVariant);
            }
            return;
        }
        if (!FuzzyEq(Health, 100.0)) {
            Health = std::min(Health + deltaTime * healRate, 100.0);
        }
        else {
            Health = 100.0;
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
};

struct Merchant {
    sf::Vector2<int> size = {tileSize, tileSize};
    sf::Vector2<double> spriteScale = { 2.0, 2.0 };
    sf::Vector2<double> scale = { 1.0, 1.0 };
    sf::Vector2<double> position = { 0.0, 0.0 };
    sf::Vector2<double> spriteOffset = { 10.0, -15.0 };
    sf::Vector2<double> spritePosition = { spriteOffset.x, spriteOffset.y };
    double animFPS = 5.0 / 200.0 * 250.0;
    sf::Color color = sf::Color(255, 255, 255, 255U);
    sf::Sprite shop{ merchantShopTex, sf::IntRect({0, 0}, size) };
    Animator animator = Animator("Merchant", { 48, 64 }, animFPS, {0, 0});
    std::string fallBackText = "Fallback Text";
    grassTile* associatedTile = nullptr;
    UI shopPrompt;
    double pressTimer = 0.0;
    double hitTimer = 0.0;
    double hitCooldown = 0.5;
    Merchant() {
        shopPrompt.backgroundColor = sf::Color(0, 0, 0, 0);
        shopPrompt.borderColor = sf::Color(0, 0, 0, 0);
        int key = shopDialogueKey.find("Prompt");
        std::string data = fallBackText;
        if (key != -1) {
            data = shopDialogueValue[key];
        }
        shopPrompt.setText(data);
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
            int ranX = Random(5, 95); // By limiting the shop teleport it becomes much easier to find
            int ranY = Random(5, 95);
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
    void Logic(sf::RenderWindow& window, Player& player) {
        pressTimer = std::max(pressTimer - deltaTime, 0.0);
        if (collidingWithPlayer(player)) {
            shopPrompt.position = getWorldPosition() + getSize() / 2.0 + sf::Vector2<double>(0.0, -getSize().y * 1.5);
            shopPrompt.draw(window);
            if (FuzzyEq(pressTimer, 0.0) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                pressTimer = 0.2;
                shopOpen = true;
            }

        }
        else {
            if (shopOpen) {
                shopOpen = false;
            }
        }
    }
    void move(sf::Vector2<double> newPos) {
        position = newPos;
        spritePosition = newPos + spriteOffset;
    }
    void draw(sf::RenderWindow& window, Player& player) {
        if (Mode != "Night") {
            animator.draw(window, spritePosition + cameraPos, spriteScale);
            Logic(window, player);
        }
        else {
            if (shopOpen) {
                shopOpen = false;
            }
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
    sf::Color color = sf::Color(255, 255, 255, 255U);
    sf::Sprite ghostEyes = sf::Sprite(ghostEyesTex);
    static const int smokeAmount = 4;
    int correctedSmokeAmount = smokeAmount;
    double orbitRadius = 12.0;
    double orbitSpeed = 5.0;
    double orbitAlpha = 0.0;
    double sightBoost = 1.0;
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
        if (Mode != "Night") return;
        if (FuzzyEq(targetPos, { 0.0, 0.0 })) {
            state = "Wander";
            wander(targetPos);
            chase.setVolume(0.0);
            orbitSpeed = 5.0;
            return;
        };
        if ((targetPos - getWorldPosition()).length() <= sightRadius * sightBoost && state != "Recover") {
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
            chase.setVolume(100.0);
            moveTo(targetPos);
        }
        else if (state == "Wander") {
            //Randomly walk to a tile, random should be weighted towards the player position, intention is, that even if the ghost is placed on the furthest corner of the map, it can reach the player during the night
            wander(targetPos);
            chase.setVolume(0.0);
            orbitSpeed = 5.0;
        }
        else {
            chase.setVolume(0.0);
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
        return (
            getWorldPosition().x < ghost.getWorldPosition().x + ghost.getSize().x &&
            getWorldPosition().x + getSize().x > ghost.getWorldPosition().x &&
            getWorldPosition().y < ghost.getWorldPosition().y + ghost.getSize().y &&
            getWorldPosition().y + getSize().y > ghost.getWorldPosition().y
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
int getFollowIndex();
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
    UI hirePrompt;
    double pressTimer = 0.0;
    std::string Gender = Random(1, 2) == 1 ? "MercenaryCharM" : "MercenaryCharF";
    std::string Anim = "Idle";
    std::string animVariant = "Down";
    std::string state = "Idle";

    double speed = 250.0;
    double speedToAnimFPSRatio = 5.0 / 200.0;

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
    double healRate = 1.0;
    std::string fallback = "Press SPACEBAR to hire this npc for $200.";
    Mercenary() {
        hirePrompt.backgroundColor = sf::Color(0, 0, 0, 0);
        hirePrompt.borderColor = sf::Color(0, 0, 0, 0);
        int key = mercenaryDialogueKey.find("Prompt");
        hirePrompt.setText(key != -1 ? mercenaryDialogueValue[key] : fallback);
        animator.Anim = Anim;
        animator.Variant = animVariant;
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
        animator.changeAnim(anim, variant);
    }

    void trueMove(sf::Vector2<double> offset) {
        position += offset;
    }
    void bareMove(sf::Vector2<double> offset) {
        position = offset;
    }
    Mercenary(sf::Vector2<double> initialPos, bool hired = false, int followIndex = 0) : hired(hired), followIndex(followIndex) {
        hirePrompt.backgroundColor = sf::Color(0, 0, 0, 0);
        hirePrompt.borderColor = sf::Color(0, 0, 0, 0);
        int key = mercenaryDialogueKey.find("Prompt");
        hirePrompt.setText(key != -1 ? mercenaryDialogueValue[key] : fallback);
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
        sf::Vector2<double> backDir = { 0.0, 1.0 };

        if (animVariant == "Up")    backDir = { 0.0, 1.0 };
        if (animVariant == "Down")  backDir = { 0.0, -1.0 };
        if (animVariant == "Left")  backDir = { 1.0, 0.0 };
        if (animVariant == "Right") backDir = { -1.0, 0.0 };

        sf::Vector2<double> target =
            player.getPosition() +
            backDir * (followSpacing * followIndex);

        sf::Vector2<double> toTarget =
            target - getBiasedWorldPosition();

        double dist = toTarget.length();

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

        sf::Vector2<double> dir = toTarget.normalized();
        sf::Vector2<double> offset = dir * speed * deltaTime;

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
    bool collidingWithPlayer(Player& player) {
        return (
            player.getPosition().x < getBiasedWorldPosition().x + getSize().x &&
            player.getPosition().x + player.getSize().x > getBiasedWorldPosition().x &&
            player.getPosition().y < getBiasedWorldPosition().y + getSize().y &&
            player.getPosition().y + player.getSize().y > getBiasedWorldPosition().y
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

    void Logic(Player& player, Ghost& ghost, sf::RenderWindow& window) {
        if (dead == true) {
            handleDeath();
            return;
        }
        takeDamage(ghost);
        if (FuzzyEq(Health, 0.0)) {
            handleDeath();
            return;
        }
        if (!FuzzyEq(Health, 100.0)) {
            Health = std::min(Health + deltaTime * 2.0 * healRate, 100.0);
        }
        else {
            Health = 100.0;
        }
        if (!hired) {
            pressTimer = std::max(pressTimer - deltaTime, 0.0);
            if (collidingWithPlayer(player)) {
                hirePrompt.position = getWorldPosition() + getSize() / 2.0 + sf::Vector2<double>(0.0, -getSize().y * 1.5);
                hirePrompt.draw(window);
                if (FuzzyEq(pressTimer, 0.0) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                    pressTimer = 0.2;
                    if (player.playerInv.getItemCount("Cash") >= 200) {
                        purchase.play();
                        followIndex = getFollowIndex();
                        hired = true;
                        player.playerInv.removeItem("Cash", 200);
                    }
                    else {
                        error.play();
                    }
                }
            }
            return;
        };
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

int getFollowIndex() {
    int count = 0;
    for (int i = 0; i < mercenaries.size(); i += 1) {
        Mercenary& mercenary = mercenaries[i];
        if (mercenary.hired) {
            count += 1;
        }
    }
    return count;
}

struct Campfire {
    sf::Vector2<double> position = { 0.0, 0.0 };
    sf::Vector2<double> size = { tileSize, tileSize };
    sf::Vector2<double> scale = { 1.0, 1.0 };
    double lifeTime = 120.0;
    bool markForDelete = false;
    sf::Sprite Sprite = sf::Sprite(campfire, sf::IntRect({ 0, 0 }, { tileSize, tileSize }));
    Campfire() {}
    Campfire(sf::Vector2<double> position) : position(position) {
    }
    sf::Vector2<double> getWorldPosition() {
        return position + cameraPos;
    }
    void Logic() {
        lifeTime = std::max(lifeTime - deltaTime, 0.0);
        if (FuzzyEq(lifeTime, 0.0)) {
            markForDelete = true;
        }
    }
    void draw(sf::RenderWindow& window) {
        Logic();
        sf::Vector2<double> playerCentre = playerPos + sf::Vector2<double>(50.0, 50.0);
        sf::Vector2<double> fogCentre = (position + cameraPos) + (sf::Vector2<double>((double)tileSize, (double)tileSize) / 2.0);
        Sprite.setScale(sf::Vector2f(scale.x, scale.y));
        Sprite.setColor(dayTint); //Campfire retains the day color tint even at night
        Sprite.setPosition(doubleToFloat(position + cameraPos));
        window.draw(Sprite);
    }
};
dynArray<Campfire> campfires;
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
            int j = -1;
            for (int i = 0; i < pickups; i += 1) {
                if (pickupNames[i] == type){
                    j = i;
                    break;
                }
            }
            if (j == -1) return;
            player.playerInv.addItem(type, pickUpAmounts[j]);
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
        int ranX = Random(10, 90);
        int ranY = Random(10, 90);
        grassTile& tile = tileMap[ranY][ranX];
        if (tile.isEmpty) {
            mercenaries.insert({ (tile.position) , false});
            i += 1;
        }
    }
}

void makePickups() {
    destroyPickups();
    int i = 0;
    while (i < pickUpCount) {
        int ranX = Random(0, 100);
        int ranY = Random(0, 100);
        int ranType = Random(0, pickups - 1);
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
double colorAlpha = 0.0;
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
            (*nightTimeAudios[i]).setVolume(100.0);
        }
        gameTimer = 0.0;
        colorAlpha = 1.0;
        dayTheme.setVolume(0.0);
        cycleTransitionRunning = false;
        lastNightAudioTime = 15.0;
        nightAudioCool = false;
        (*nightTimeAudios[4]).setPitch(nightTimePitches[4]);
        (*nightTimeAudios[4]).play();
        ghost.randomTeleport();
        Mode = "Night";
    }
}

void setDay(Merchant& merchant, int& nightsSurvived) {
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
            (*nightTimeAudios[i]).setVolume(0.0);
        }
        merchant.teleportShop();
        dayTheme.setVolume(100.0);
        cycleTransitionRunning = false;
        Mode = "Day";
        nightsSurvived += 1;
        chase.setVolume(0.0);
    }
}

void nightTimeAudioPlayLogic() { // Runs every frame
    if (nightAudioCool) {
        lastNightAudioTime = 10.0; // Wait 10 seconds before playing another
        nightAudioCool = false;
        int chance = Random(0, 5); // Random integer between 0 and 5
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
    double mult = 1.0;
    for (int i = campfires.size() - 1; i >= 0; i -= 1) {
        Campfire& campfire = campfires[i];
        if (campfire.markForDelete) {
            campfires.remove(i);
        }
        else {
            mult = 2.0;
            bool visX = (campfire.getWorldPosition().x + tileSize) >= 0 && (campfire.getWorldPosition().x) <= windowSize.x;
            bool visY = (campfire.getWorldPosition().y + tileSize) >= 0 && (campfire.getWorldPosition().y) <= windowSize.y;
            campfire.Logic();
            if (visX && visY) {
                campfire.draw(window);
            }
        }
    }
    player.healRate = mult;
    ghost.sightBoost = mult * 0.5;
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
            mercenary.healRate = mult;
            mercenary.Logic(player, ghost, window);
        }
    }
    merchant.draw(window, player);
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
    std::cout << "Game Started!\n";
    srand(time(0));
    int nightsSurvived = 0;
    sf::VideoMode screenMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(screenMode, "Withered Roots", sf::Style::None);
    window.setView(window.getDefaultView());
    window.setVerticalSyncEnabled(true);
    windowSize = sf::Vector2<int>(screenMode.size.x, screenMode.size.y);
    UI GameOver({ (double)windowSize.x, (double)windowSize.y }, { 0.0, 0.0 });
    UI menuTitle({ 500.0, 100.0 }, { (double)windowSize.x / 2.0 - 250.0, (double)windowSize.y / 2.0 - 200.0 });
    UI menuPlay({ 200.0, 50.0 }, { (double)windowSize.x / 2.0 - 100.0, (double)windowSize.y / 2.0 });
    UI menuQuit({ 200.0, 50.0 }, { (double)windowSize.x / 2.0 - 100.0, (double)windowSize.y / 2.0 + 100.0 });
    initAnims();
    initTileMap();
    initDialogues();
    Player player{};
    Ghost ghost{};
    Merchant merchant{};
    player.bareMove((sf::Vector2<double>(screenMode.size.x / 2.0 - 25.0, screenMode.size.y / 2.0 - 25.0)));
    merchant.teleportShop();
    ghost.randomTeleport();
    sf::Clock deltaTimer;
    deltaTimer.start();
    dayTheme.setLooping(true);
    chase.setLooping(true);
    chase.play();
    chase.setVolume(0.0);
    makePickups();
    spawnMercenaries();
    /*Shop Stuff*/
    shopDialog.position = { 20.0, windowSize.y - 260.0 };
    shopDialog.size = { windowSize.x - 40.0, 240.0 };
    shopDialog.backgroundColor = sf::Color(20, 20, 20, 220);
    shopDialog.borderColor = sf::Color::White;
    shopTitle.position = { shopDialog.position.x + 20.0, shopDialog.position.y + 10.0 };
    shopTitle.size = { 200.0, 30.0 };
    shopTitle.setText("Merchant Shop");
    shopTitle.fontSize = 22;
    shopTitle.backgroundColor = sf::Color(0, 0, 0, 0);
    shopTitle.borderColor = sf::Color(0, 0, 0, 0);
    /*Shop Items*/
    for (int i = 0; i < pickups - 1; i++) {
        double rowY = shopDialog.position.y + 50.0 + i * 40.0;
        shopInventory[i].position = { shopDialog.position.x + 20.0, rowY };
        shopInventory[i].size = { 260.0, 30.0 };
        shopInventory[i].fontSize = 18;
        shopInventory[i].backgroundColor = sf::Color(0, 0, 0, 0);
        shopInventory[i].borderColor = sf::Color(0, 0, 0, 0);
        shopInventory[i].setText(
            pickupNames[i] + " : $" + std::to_string(pickupPrice[i])
        );
        if (pickupNames[i] == "Campfire Materials") {
            shopInventory[i].textOffset = {50.0, 0.0};
        }
        buyButtons[i].position = { shopDialog.position.x + 320.0, rowY };
        buyButtons[i].size = { 80.0, 30.0 };
        buyButtons[i].setText("Buy");
        buyButtons[i].backgroundColor = sf::Color(30, 120, 30);
        buyButtons[i].borderColor = sf::Color::Black;
        sellButtons[i].position = { shopDialog.position.x + 420.0, rowY };
        sellButtons[i].size = { 80.0, 30.0 };
        sellButtons[i].setText("Sell");
        sellButtons[i].backgroundColor = sf::Color(120, 30, 30);
        sellButtons[i].borderColor = sf::Color::Black;
    }
    /*Health, stamina, and money*/
    healthBar.position = sf::Vector2<double>( 30.0, windowSize.y - 90.0 );
    healthBar.size = { 300, 20 };
    healthBar.backgroundColor = sf::Color(120, 20, 20);
    healthBar.borderColor = sf::Color::Black;

    staminaBar.position = sf::Vector2<double>(30.0, windowSize.y - 60.0 );
    staminaBar.size = { 300, 20 };
    staminaBar.backgroundColor = sf::Color(20, 120, 20);
    staminaBar.borderColor = sf::Color::Black;
    timeText.position = sf::Vector2<double>(windowSize.x / 2.0 - 50.0, 20.0 );
    timeText.size = { 100, 30 };
    timeText.fontSize = 20;
    timeText.backgroundColor = sf::Color(0, 0, 0, 0);
    timeText.borderColor = sf::Color(0, 0, 0, 0);
    moneyText.position = sf::Vector2<double>(30.0, 20.0 );
    moneyText.size = { 160, 30 };
    moneyText.fontSize = 20;
    moneyText.backgroundColor = sf::Color(0, 0, 0, 0);
    moneyText.borderColor = sf::Color(0, 0, 0, 0);
    int slotCount = 4;
    float slotSize = 48;
    float startX = windowSize.x / 2.0 - (slotCount * slotSize) / 2.0;
    /*Inventory Stuff*/
    for (int i = 0; i < slotCount; i += 1) {
        UI& slot = inventorySlots[i];
        slot.position = sf::Vector2<double>(startX + i * slotSize + (slot.borderPixel + 1) * i, windowSize.y - (slotSize*2.0));
        slot.size = { slotSize, slotSize };
        slot.backgroundColor = sf::Color(50, 50, 50);
        slot.setText("");
        slot.fontSize = 13;
        slot.textOffset = slot.size / 2.0;
        slot.borderColor = sf::Color::Black;
    }
    int selectedIndex = -1;
    double selectSwitchTimer = 0.0;
    double menuGlowAlpha = 0.0;
    double menuGlowSpeed = 40.0;
    sf::Color glowStart(40, 64, 0, 255);
    sf::Color glowEnd(160, 255, 0, 255);
    bool mainMenuClose = false;
    menuTheme.setLooping(true);
    menuTheme.play();
    while (window.isOpen() && !mainMenuClose) { // Main menu
        deltaTime = deltaTimer.restart().asSeconds();
        menuGlowAlpha = (menuGlowAlpha + deltaTime * menuGlowSpeed);
        while (menuGlowAlpha > 360) {
            menuGlowAlpha -= 360.0;
        }
        double lerpAlpha = std::sin(menuGlowAlpha * pi / 180.0) * 0.5 + 0.5;
        menuTitle.backgroundColor = { 0, 0, 0, 255 };
        menuTitle.borderColor = Lerp(glowStart, glowEnd, lerpAlpha);
        menuTitle.textColor = Lerp(glowStart, glowEnd, lerpAlpha);
        menuTitle.fontSize = 40;
        menuPlay.backgroundColor = { 0, 0, 0, 255 };
        menuPlay.borderColor = Lerp(glowStart, glowEnd, lerpAlpha);
        menuPlay.textColor = Lerp(glowStart, glowEnd, lerpAlpha);
        menuPlay.fontSize = 30;
        menuQuit.backgroundColor = { 0, 0, 0, 255 };
        menuQuit.borderColor = Lerp(glowStart, glowEnd, lerpAlpha);
        menuQuit.textColor = Lerp(glowStart, glowEnd, lerpAlpha);
        menuTitle.borderColor.a = 128;
        menuPlay.borderColor.a = 128;
        menuQuit.borderColor.a = 128;
        menuQuit.fontSize = 30;
        menuTitle.setText("Withered Roots");
        menuPlay.setText("Play");
        menuQuit.setText("Quit");
        while (const std::optional event = window.pollEvent())
        {
            if ((*event).is<sf::Event::Closed>()) {
                window.close();
                return 0;
            }
        }
        if (mouseOver(menuQuit)) {
            menuQuit.borderColor = glowEnd;
            menuQuit.textColor = glowEnd;
        }
        if (mouseOver(menuPlay)) {
            menuPlay.borderColor = glowEnd;
            menuPlay.textColor = glowEnd;
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            if (mouseOver(menuQuit)) {
                pickup.play();
                window.close();
                return 0;
            }
            else if (mouseOver(menuPlay)) {
                pickup.play();
                mainMenuClose = true;
            }
        }
        window.clear();
        menuTitle.draw(window);
        menuPlay.draw(window);
        menuQuit.draw(window);
        window.display();
    }
    menuTheme.stop();
    dayTheme.play(); // Start playing the day music
    /*Main game loop*/
    while (window.isOpen())
    {
        colorTint = dayTint;
        fogStart = (double)Lerp(fogStartDay, fogStartNight, colorAlpha);
        fogEnd = (double)Lerp(fogEndDay, fogEndNight, colorAlpha);
        colorTint = Lerp(dayTint, nightTint, colorAlpha);
        deltaTime = (double)deltaTimer.restart().asSeconds();
        combinedDeltaTime += deltaTime;
        selectSwitchTimer = std::max(selectSwitchTimer - deltaTime, 0.0);
        if (player.dead) {
            player.deathTimer = std::min(player.deathTimer + deltaTime, 10.0);
        }
        if (FuzzyEq(player.deathTimer, 10.0)) { // Death menu logic, triggers once 10 seconds have passed since death
            GameOver.backgroundColor = sf::Color(0, 0, 0, 255);
            GameOver.borderColor = sf::Color(0, 0, 0, 0);
            GameOver.textColor = sf::Color(80, 0, 0, 255);
            dayTheme.stop();
            for (int i = 0; i < 6; i += 1) {
                (*nightTimeAudios[i]).stop();
            }
            menuTheme.play();
            GameOver.setText("Game Over!\nYou survived " + std::to_string(nightsSurvived) + " nights!\nPress Escape to exit");
            while (window.isOpen()) {
                deltaTimer.restart();
                window.clear();
                GameOver.draw(window);
                window.display();
                while (const std::optional event = window.pollEvent())
                {
                    if ((*event).is<sf::Event::Closed>()) {
                        window.close();
                        return 0;
                    }
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
                    window.close();
                    return 0;
                }
            }
        }
        while (const std::optional event = window.pollEvent())
        {
            if ((*event).is<sf::Event::Closed>()) {
                window.close();
                return 0;
            }
        }
        sf::Vector2<double> movementOffset = sf::Vector2<double>(0, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            window.close();
            return 0;
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
        if (FuzzyEq(selectSwitchTimer, 0.0) && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad1))) {
            selectedIndex = (selectedIndex != 0) ? 0 : -1;
            selectSwitchTimer = 0.2;
        }
        if (FuzzyEq(selectSwitchTimer, 0.0) && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad2))) {
            selectedIndex = (selectedIndex != 1) ? 1 : -1;
            selectSwitchTimer = 0.2;
        }
        if (FuzzyEq(selectSwitchTimer, 0.0) && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad3))) {
            selectedIndex = (selectedIndex != 2) ? 2 : -1;
            selectSwitchTimer = 0.2;
        }
        if (FuzzyEq(selectSwitchTimer, 0.0) && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad4))) {
            selectedIndex = (selectedIndex != 3) ? 3 : -1;
            selectSwitchTimer = 0.2;
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            if (selectedIndex >= 0 && selectedIndex <= pickups) {
                std::string name = pickupNames[selectedIndex];
                if (player.playerInv.getItemCount(name) > 0 && !player.dead) {
                    if (name == "Beans") {
                        player.Health = std::min(player.Health + 10.0, 100.0);
                    }else if (name == "Chocolate") {
                        player.Health = std::min(player.Health + 2.5, 100.0);
                        player.stamina = std::min(player.stamina + 40.0, 100.0);
                    }
                    else if (name == "Bandages") {
                        player.Health = std::min(player.Health + 40.0, 100.0);
                    }
                    else if (name == "Campfire Materials") {
                        campfires.insert({ player.getPosition() - cameraPos });
                    }
                    player.playerInv.removeItem(name, 1);
                    itemUse.play();
                }
                else {
                    error.play();
                }
            }
            selectedIndex = -1;
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
        int secondsLeft = 0;
        if (Mode == "Day"){
            secondsLeft = std::max((int)(dayTime - gameTimer), 0);
            if (gameTimer > dayTime || cycleTransitionRunning) {
                setNight(ghost);
            }
        }
        else if (Mode == "Night"){
            secondsLeft = std::max((int)(nightTime - gameTimer), 0);
            if (gameTimer > nightTime || cycleTransitionRunning) {
                setDay(merchant, nightsSurvived);
            }
        }
        if (Mode == "Night") {
            nightTimeAudioPlayLogic();
        }
        player.move(movementOffset, deltaTime);
        window.clear();
        drawMap(window, player, ghost, merchant);
        healthBar.size.x = (player.Health / 100.0) * 300.0;
        staminaBar.size.x = (player.stamina / 100.0) * 300.0;
        timeText.setText("Time Left: " + std::to_string(secondsLeft));
        moneyText.setText("Cash: $" + std::to_string(player.playerInv.itemVals[0]) + ".0");
        for (int i = 0; i < pickups - 1; i += 1) {
            int count = player.playerInv.getItemCount(pickupNames[i]);
            if (count > 0) {
                inventorySlots[i].background.setTexture(&pickUpTextures[i]);
                inventorySlots[i].backgroundColor = sf::Color::White;
                inventorySlots[i].borderColor = sf::Color::Black;
                inventorySlots[i].setText(std::to_string(count));
            }
            else {
                inventorySlots[i].setText("");
                inventorySlots[i].background.setTexture(nullptr);
                inventorySlots[i].backgroundColor = sf::Color(50, 50, 50, 255);
                inventorySlots[i].borderColor = sf::Color::Black;
            }
            inventorySlots[i].borderColor = (i == selectedIndex) ? sf::Color::Yellow : sf::Color::Black;
            inventorySlots[i].draw(window);
        }
        healthBar.draw(window);
        staminaBar.draw(window);
        timeText.draw(window);
        moneyText.draw(window);
        if (shopOpen) {
            shopDialog.textOffset = {100.0, 0.0}; //I'm moving this by 100 pixels so that it doesn't touch the shop buttons
            if (shopDescriptionText != "") { //Remember that std::string that was part of the clever workaround, here is the actual code for it
                shopDialog.setText(shopDescriptionText);
                shopDescriptionText = "";
            }else {
                int key = shopDialogueKey.find("Greet");
                shopDialog.setText(key != -1 ? shopDialogueValue[key] : "");
            }
            shopDialog.draw(window);
            shopTitle.draw(window);

            for (int i = 0; i < pickups - 1; i += 1) {
                if (mouseOver(shopInventory[i])) {
                    int key2 = shopDialogueKey.find(pickupNames[i]);
                    if (key2 != -1) {
                        shopDescriptionText = shopDialogueValue[key2];
                    }
                }
                shopInventory[i].draw(window);
                //Hover logic, the idea is, when you hover over an item name, the shop dialogue changes to the description of the item
                buyButtons[i].borderColor = mouseOver(buyButtons[i]) ? sf::Color::Yellow : sf::Color::Black;
                sellButtons[i].borderColor = mouseOver(sellButtons[i]) ? sf::Color::Yellow : sf::Color::Black;

                buyButtons[i].draw(window);
                sellButtons[i].draw(window);

                //Really simple logic to get a clickable menu, we check if the mouse left button is pressed, and then check if the mouse is over a button while being pressed
                //There's only one scenario where the mouse being over a button and being pressed makes sense, it's when you want to click a button
                if (FuzzyEq(selectSwitchTimer, 0.0) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    // BUY
                    if (mouseOver(buyButtons[i])) {
                        selectSwitchTimer = 0.2;
                        int price = pickupPrice[i];
                        if (player.playerInv.itemVals[0] >= price) {
                            player.playerInv.itemVals[0] -= price;
                            player.playerInv.addItem(pickupNames[i], 1);
                            purchase.play();
                        }
                        else {
                            error.play();
                        }
                    }

                    // SELL
                    if (mouseOver(sellButtons[i])) {
                        selectSwitchTimer = 0.2;
                        if (player.playerInv.getItemCount(pickupNames[i]) > 0) {
                            player.playerInv.removeItem(pickupNames[i], 1);
                            player.playerInv.itemVals[0] += pickupPrice[i];
                            purchase.play();
                        }
                        else {
                            error.play();
                        }
                    }
                }
            }
        }


        window.display();
    }
    return 0;
}