#ifndef NAVIGATION_CONTROLLER_HPP
#define NAVIGATION_CONTROLLER_HPP

#include <map>
#include <memory>
#include <functional>
#include <string>
#include "NavigationTarget.hpp"

namespace Parcel::Core::Navigation {
    class NavigationController {
    public:
        using NavCallback = std::function<void(NavigationTarget, const std::string&)>;

        static NavigationController& getInstance() {
            static NavigationController instance;
            return instance;
        }

        void navigateTo(NavigationTarget target, const std::string& path = "") {
            if (current != target || (!path.empty() && path != currentPath)) {
                previous = current;
                current = target;
                currentPath = path;
                if (onNavigate) onNavigate(target, path);
            }
        }

        void goBack() { navigateTo(previous); }
        void setOnNavigate(NavCallback cb) { onNavigate = cb; }

    private:
        NavigationController() : current(NavigationTarget::HOME), previous(NavigationTarget::HOME) {}
        NavigationTarget current;
        NavigationTarget previous;
        std::string currentPath;
        NavCallback onNavigate;
    };
}
#endif
