// KDE Breeze Dark Theme Palette Application Template
#include <QApplication>
#include <QPalette>
#include <QColor>

void applyKdeBreezeDarkTheme(QApplication& app) {
    QPalette breezeDark;
    breezeDark.setColor(QPalette::Window, QColor(26, 26, 46));
    breezeDark.setColor(QPalette::WindowText, QColor(220, 220, 220));
    breezeDark.setColor(QPalette::Base, QColor(20, 20, 30));
    breezeDark.setColor(QPalette::AlternateBase, QColor(30, 30, 45));
    breezeDark.setColor(QPalette::Text, QColor(240, 240, 240));
    breezeDark.setColor(QPalette::Button, QColor(43, 45, 48));
    breezeDark.setColor(QPalette::ButtonText, QColor(240, 240, 240));
    breezeDark.setColor(QPalette::Highlight, QColor(0, 191, 255));
    breezeDark.setColor(QPalette::HighlightedText, QColor(0, 0, 0));

    app.setPalette(breezeDark);
}
