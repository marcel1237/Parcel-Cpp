# KDE Frameworks & KDevelop API Integration Documentation

## 📋 Overview
Parcel C++ integrates **KDE Frameworks** (`https://api.kde.org/`, Kirigami index) and **KDevelop** APIs as core architectural services (`[KDEIntegrationService.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/service/KDEIntegrationService.hpp)`). This integration provides robust UI styling, icon resolution, theme palettes, and IDE tooling bridges across the entire application.

---

## 🚀 Key Integration Features

1. **KDE Frameworks / Kirigami Bridge (`api.kde.org`):**
   - Provides Kirigami component wrappers and icon resolvers (`getKirigamiIcon()`) leveraging standard KDE icon themes with reliable fallbacks.
   - Integrates KDE Breeze / Kirigami Dark color schemes (`getKdeDarkPalette()`).

2. **KDevelop Core & Language Support API:**
   - Exposes core plugin management, AST tokenizer helpers, and multi-language support wrappers for C++, QML, Python, and Shell scripting.

3. **Settings & Status Inspection:**
   - Visible directly in the IDE Settings view (`[SettingsView.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/view/SettingsView.hpp)`), displaying active API bridge status and framework information.
