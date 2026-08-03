#ifndef DIALOG_SERVICE_HPP
#define DIALOG_SERVICE_HPP

#include <string>
#include <iostream>

namespace Parcel::Service {
    class DialogService {
    public:
        static bool showConfirmation(const std::string& title, const std::string& message) {
            std::cout << "[" << title << "] " << message << " (y/n): ";
            char response;
            std::cin >> response;
            return response == 'y' || response == 'Y';
        }

        static void showError(const std::string& message) {
            std::cerr << "ERROR: " << message << std::endl;
        }
    };
}
#endif
