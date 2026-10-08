#ifndef KDE_MANAGER_HPP
#define KDE_MANAGER_HPP

#include <QString>
#include <QObject>

namespace Parcel::Service {

    class KDEManager : public QObject {
        Q_OBJECT
    public:
        static KDEManager& getInstance() {
            static KDEManager instance;
            return instance;
        }

        // Generates Kirigami QML application template
        QString generateKirigamiAppCode(const QString& appName = "ParcelKirigamiApp") const {
            return QString(
                "import QtQuick\n"
                "import QtQuick.Controls\n"
                "import org.kde.kirigami as Kirigami\n\n"
                "Kirigami.ApplicationWindow {\n"
                "    id: root\n"
                "    title: \"%1\"\n"
                "    width: 800; height: 600\n\n"
                "    pageStack.initialPage: Kirigami.Page {\n"
                "        title: \"KDE Kirigami Welcome\"\n\n"
                "        columnLayout {\n"
                "            anchors.centerIn: parent\n"
                "            spacing: 16\n\n"
                "            Kirigami.Heading {\n"
                "                text: \"Welcome to KDE Kirigami 6\"\n"
                "            }\n\n"
                "            Button {\n"
                "                text: \"Click Kirigami Action\"\n"
                "                onClicked: console.log(\"Kirigami action triggered!\")\n"
                "            }\n"
                "        }\n"
                "    }\n"
                "}\n"
            ).arg(appName);
        }

        // Generates CMakeLists template for KDE KF6
        QString generateKdeCMakeTemplate() const {
            return QString(
                "cmake_minimum_required(VERSION 3.16)\n"
                "project(KdeKirigamiApp LANGUAGES CXX)\n\n"
                "set(CMAKE_CXX_STANDARD 17)\n"
                "set(CMAKE_AUTOMOC ON)\n\n"
                "find_package(Qt6 REQUIRED COMPONENTS Core Quick Widgets)\n"
                "find_package(KF6 REQUIRED COMPONENTS Kirigami CoreAddons Config)\n\n"
                "add_executable(KdeKirigamiApp main.cpp)\n"
                "target_link_libraries(KdeKirigamiApp PRIVATE Qt6::Core Qt6::Quick Qt6::Widgets KF6::Kirigami KF6::CoreAddons)\n"
            );
        }

    private:
        KDEManager() = default;
        ~KDEManager() = default;
        KDEManager(const KDEManager&) = delete;
        KDEManager& operator=(const KDEManager&) = delete;
    };

}

#endif
