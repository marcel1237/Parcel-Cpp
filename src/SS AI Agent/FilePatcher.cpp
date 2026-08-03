#include "FilePatcher.hpp"
#include <QDir>

namespace SSAI {

FilePatcher::PatchStatus FilePatcher::reconstruct(const QString& filePath, const QString& content) {
    if (atomicWrite(filePath, content)) {
        return PatchStatus::Success;
    }
    return PatchStatus::WriteError;
}

FilePatcher::PatchStatus FilePatcher::safeCreate(const QString& filePath, const QString& content) {
    QFileInfo checkFile(filePath);
    if (checkFile.exists()) {
        return PatchStatus::FileAlreadyExists;
    }

    // Ensure parent directory exists (Simulating mkdir -p)
    QDir dir = checkFile.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return PatchStatus::WriteError;
        }
    }

    if (atomicWrite(filePath, content)) {
        return PatchStatus::Success;
    }
    return PatchStatus::WriteError;
}

FilePatcher::PatchStatus FilePatcher::patch(const QString& filePath,
                                           const QString& startMarker,
                                           const QString& endMarker,
                                           const QString& newContent)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return PatchStatus::FileNotFound;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    int startIndex = content.indexOf(startMarker);
    int endIndex = content.indexOf(endMarker);

    if (startIndex == -1 || endIndex == -1 || endIndex <= startIndex) {
        return PatchStatus::MarkersNotFound;
    }

    // Logic from SSQLM: Keep markers, replace center.
    int endOfStartLine = content.indexOf('\n', startIndex);
    if (endOfStartLine == -1) endOfStartLine = startIndex + startMarker.length();

    QString prefix = content.left(endOfStartLine + 1);
    QString suffix = content.mid(endIndex);

    QString patchedContent = prefix + newContent + (newContent.endsWith('\n') ? "" : "\n") + suffix;

    if (atomicWrite(filePath, patchedContent)) {
        return PatchStatus::Success;
    }
    return PatchStatus::WriteError;
}

FilePatcher::PatchStatus FilePatcher::smartAppend(const QString& filePath,
                                                const QString& content,
                                                const QString& duplicateCheckMarker)
{
    QFile file(filePath);
    QString existingContent;

    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return PatchStatus::FileNotFound;
        }
        existingContent = QTextStream(&file).readAll();
        file.close();

        if (existingContent.contains(duplicateCheckMarker)) {
            return PatchStatus::DuplicateFound;
        }
    }

    QString finalContent = existingContent;
    if (!finalContent.isEmpty() && !finalContent.endsWith('\n')) {
        finalContent += "\n";
    }
    finalContent += content;

    if (atomicWrite(filePath, finalContent)) {
        return PatchStatus::Success;
    }
    return PatchStatus::WriteError;
}

bool FilePatcher::atomicWrite(const QString& filePath, const QString& content) {
    // SSQLM Standard: Atomic Transaction via QSaveFile (simulates mktemp + mv)
    QSaveFile saveFile(filePath);
    if (saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&saveFile);
        out << content;
        return saveFile.commit();
    }
    return false;
}

}
