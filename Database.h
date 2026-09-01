#pragma once
#include <string>
#include <vector>
#include <fstream>

struct PlayerProfile {
    struct LevelStats {
        int levelId = 0;
        bool unlocked = false;
        float bestTime = 0.0f;
        int minMistakes = 999;
    };
    std::vector<LevelStats> levels;
    std::string name;
    int totalGames = 0;
    int totalScore = 0;

    PlayerProfile() : levels(4), name("Player") {
        levels[0].levelId = 1; levels[0].unlocked = true;
        levels[1].levelId = 2; levels[2].levelId = 3; levels[3].levelId = 4;
    }
};

class Database {
private:
    std::string dbPath;
public:
    Database(const std::string& path);
    ~Database();
    bool init();
    PlayerProfile loadProfile();
    void saveProfile(const PlayerProfile& profile);
    void updateLevelResult(int levelId, double time, int mistakes);
    bool isLevelUnlocked(int levelId);
    void resetProgress();
    void handleError(int rc, const std::string& context);
};