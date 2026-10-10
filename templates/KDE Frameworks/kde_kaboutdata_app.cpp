// KDE KAboutData Application Metadata Template
#include <KAboutData>
#include <KLocalizedString>

void setupKdeAboutData() {
    KAboutData aboutData(
        QStringLiteral("parcel_cpp"),
        i18n("Parcel C++"),
        QStringLiteral("1.0.0"),
        i18n("AI-Driven Industrial Development Toolkit"),
        KAboutLicense::GPL,
        i18n("Copyright (C) 2026 Marcel Aparecido de Andrade"),
        QString(),
        QStringLiteral("https://github.com/your-repo/parcel-cpp")
    );

    aboutData.addAuthor(
        i18n("Marcel Aparecido de Andrade"),
        i18n("Lead Architect & Specialist"),
        QStringLiteral("marcel@example.com")
    );

    KAboutData::setApplicationData(aboutData);
}
