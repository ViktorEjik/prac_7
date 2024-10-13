#pragma once
#include <filesystem>

class Logger {
public:
    Logger() {
        log_directory = std::filesystem::current_path() / "game_logs";
        std::filesystem::create_directory(log_directory);
    }
    void logRound(const std::string& log);
    void logFinal(const std::string& final_log);
    void nextRound();
private:
    std::filesystem::path log_directory;
    int round = 1;
};
