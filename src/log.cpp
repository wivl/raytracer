#include "log.hpp"


void print_log(const char *type, const char *from, const char *content) {
    std::cout << "[" << type << "]" << from << ": " << content << std::endl;
}

// void print_progress(int completed, int total, int width) {
    // std::cout << "Progress: [";
    // int pos = static_cast<int>(completed * width / total);
    // for (int i = 0; i < width; i++) {
        // if (i < pos) std::cout << "=";
        // else if (i == pos) std::cout << ">";
        // else std::cout << " ";
    // }
    // std::cout << "] " << std::setw(3) << static_cast<int>(completed * 100.0 / total) << " %" << " " << completed << "/" << total << "\r";
    // std::cout.flush();
// }

void print_progress(int completed, int total, int width) {
    std::cout << "Progress: [";
    int pos = static_cast<int>(completed * width / total);
    for (int i = 0; i < width; i++) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    
    // Calculate ETA
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    static std::chrono::steady_clock::time_point start = now;
    std::chrono::steady_clock::duration elapsed = now - start;
    double elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    double eta_seconds = (elapsed_seconds / completed) * (total - completed);
    
    // Convert ETA to hours, minutes, and seconds
    int eta_h = static_cast<int>(eta_seconds / 3600);
    int eta_m = static_cast<int>((eta_seconds - eta_h * 3600) / 60);
    int eta_s = static_cast<int>(eta_seconds - eta_h * 3600 - eta_m * 60);
    
    std::cout << "] " << std::setfill(' ') << std::setw(3) << static_cast<int>(completed * 100.0 / total) << " %" << " " << completed << "/" << total;
    std::cout << " ETA: " << std::setfill('0') << std::setw(2) << eta_h << "h "
              << std::setw(2) << eta_m << "m " << std::setw(2) << eta_s << "s\r";
    std::cout.flush();
}
