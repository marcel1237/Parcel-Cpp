# KDE Frameworks 6 & Kirigami Studio Documentation

## 📋 Overview
Parcel C++ now features a dedicated **KDE Studio & Kirigami Module** (`[KDEManager.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/service/KDEManager.hpp)` and `[KDEPane.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/view/KDEPane.hpp)`), matching the implementation depth of GTK 4 and .NET. This module allows developers to instantly generate Kirigami QML application templates and KF6 CMake build scripts directly inside the IDE.

---

## 🛠️ Implemented Components

1. **`KDEManager` Service (`[KDEManager.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/service/KDEManager.hpp)`):**
   - **Kirigami QML Generator:** Generates convergent application templates using `Kirigami.ApplicationWindow`, `Kirigami.Page`, and `Kirigami.Heading`.
   - **KF6 CMake Generator:** Generates CMake build configurations targeting KDE Frameworks 6 (`KF6::Kirigami`, `KF6::CoreAddons`, `KF6::Config`).

2. **`KDEPane` UI Module (`[KDEPane.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/view/KDEPane.hpp)`):**
   - An interactive IDE workspace pane with instant code generation tools and syntax-highlighted views for KDE development.
