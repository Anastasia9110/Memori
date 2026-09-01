#include "Database.h"
#include <iostream>

Database::Database(const std::string& path) : dbPath(path) {}
Database::~Database() {}

bool Database::init() {
    std::ifstream file(dbPath);
    if (!file.good()) {
        PlayerProfile newProfile;
        saveProfile(newProfile);
    }
    return true;
}

PlayerProfile Database::loadProfile() {
    PlayerProfile profile;
    std::ifstream file(dbPath, std::ios::binary);
    if (file.good()) {
        file.read(reinterpret_cast<char*>(&profile.totalGames), sizeof(int));
        file.read(reinterpret_cast<char*>(&profile.totalScore), sizeof(int));

        size_t nameSize;
        file.read(reinterpret_cast<char*>(&nameSize), sizeof(size_t));
        profile.name.resize(nameSize);
        file.read(&profile.name[0], nameSize);

        for (int i = 0; i < 4; ++i) {
            file.read(reinterpret_cast<char*>(&profile.levels[i].levelId), sizeof(int));
            file.read(reinterpret_cast<char*>(&profile.levels[i].unlocked), sizeof(bool));
            file.read(reinterpret_cast<char*>(&profile.levels[i].bestTime), sizeof(float));
            file.read(reinterpret_cast<char*>(&profile.levels[i].minMistakes), sizeof(int));
        }
    }
    return profile;
}

void Database::saveProfile(const PlayerProfile& profile) {
    std::ofstream file(dbPath, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&profile.totalGames), sizeof(int));
        file.write(reinterpret_cast<const char*>(&profile.totalScore), sizeof(int));

        size_t nameSize = profile.name.size();
        file.write(reinterpret_cast<const char*>(&nameSize), sizeof(size_t));
        file.write(profile.name.c_str(), nameSize);

        for (int i = 0; i < 4; ++i) {
            file.write(reinterpret_cast<const char*>(&profile.levels[i].levelId), sizeof(int));
            file.write(reinterpret_cast<const char*>(&profile.levels[i].unlocked), sizeof(bool));
            file.write(reinterpret_cast<const char*>(&profile.levels[i].bestTime), sizeof(float));
            file.write(reinterpret_cast<const char*>(&profile.levels[i].minMistakes), sizeof(int));
        }
    }
}

void Database::updateLevelResult(int levelId, double time, int mistakes) {
    if (levelId < 1 || levelId > 4) return;
    PlayerProfile profile = loadProfile();
    int idx = levelId - 1;
    profile.totalGames++;
    profile.totalScore += 10;
    if (profile.levels[idx].bestTime == 0.0f || static_cast<float>(time) < profile.levels[idx].bestTime)
        profile.levels[idx].bestTime = static_cast<float>(time);
    if (mistakes < profile.levels[idx].minMistakes)
        profile.levels[idx].minMistakes = mistakes;
    if (levelId < 4) profile.levels[idx + 1].unlocked = true;
    saveProfile(profile);
}

bool Database::isLevelUnlocked(int levelId) {
    if (levelId < 1 || levelId > 4) return false;
    PlayerProfile profile = loadProfile();
    return profile.levels[levelId - 1].unlocked;
}

void Database::resetProgress() {
    PlayerProfile newProfile;
    newProfile.levels[0].unlocked = true;
    saveProfile(newProfile);
}

void Database::handleError(int rc, const std::string& context) {}