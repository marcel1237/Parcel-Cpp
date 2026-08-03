#ifndef PARCEL_THEME_HPP
#define PARCEL_THEME_HPP

#include <QApplication>
#include <QPalette>
#include <QColor>

#include <QFile>
#include <QTextStream>

namespace Parcel::View::Theme {
    static void applyExtraordinarySkin(QApplication& app) {
        QPalette p;
        // Deep Space Cyber Palette
        QColor darkBg(15, 17, 26);
        QColor surfaceBg(22, 25, 37);
        QColor accentBlue(66, 133, 244);
        QColor neonCyan(0, 191, 255);
        QColor textMain(224, 224, 224);
        QColor textDim(136, 136, 136);

        p.setColor(QPalette::Window, darkBg);
        p.setColor(QPalette::WindowText, textMain);
        p.setColor(QPalette::Base, surfaceBg);
        p.setColor(QPalette::AlternateBase, darkBg);
        p.setColor(QPalette::ToolTipBase, darkBg);
        p.setColor(QPalette::ToolTipText, textMain);
        p.setColor(QPalette::Text, textMain);
        p.setColor(QPalette::Button, surfaceBg);
        p.setColor(QPalette::ButtonText, textMain);
        p.setColor(QPalette::BrightText, Qt::white);
        p.setColor(QPalette::Link, neonCyan);
        p.setColor(QPalette::Highlight, accentBlue);
        p.setColor(QPalette::HighlightedText, Qt::white);

        app.setPalette(p);

        // Load external QSS for modularity
        QFile file("resource/qss/cyber-dark.qss");
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream ts(&file);
            app.setStyleSheet(ts.readAll());
        }
    }
}

#endif
