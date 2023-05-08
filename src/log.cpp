#include "log.hpp"


void print_progress(int completed, int total, int width) {
    std::cout << "Progress: [";
    int pos = static_cast<int>(completed * width / total);
    for (int i = 0; i < width; i++) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::setw(3) << static_cast<int>(completed * 100.0 / total) << " %" << " " << completed << "/" << total << "\r";
    std::cout.flush();
}
