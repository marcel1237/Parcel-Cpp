#ifndef FILE_PATCHER_HPP
#define FILE_PATCHER_HPP

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>
#include <QSaveFile>

namespace SSAI {

    /**
     * @brief The FilePatcher class implements the "Extraordinary Cat Logic".
     * Based on SSQLM standards for high-fidelity file manipulation.
     */
    class FilePatcher {
    public:
        enum class PatchStatus {
            Success,
            FileNotFound,
            MarkersNotFound,
            DuplicateFound,
            FileAlreadyExists,
            WriteError
        };

        /**
         * @brief MODE 1: FULL_RECONSTRUCT
         * Simulates 'cat > file <<EOF'. Completely overwrites the file.
         */
        static PatchStatus reconstruct(const QString& filePath, const QString& content);

        /**
         * @brief MODE 4: SAFE_CREATE
         * Simulates '[[ -f file ]] || cat > file <<EOF'.
         * Creates a new file only if it doesn't exist, ensuring directory path.
         */
        static PatchStatus safeCreate(const QString& filePath, const QString& content);

        /**
         * @brief MODE 2: SURGICAL_INJECTION
         * Simulates 'sed + cat'. Replaces content between markers.
         */
        static PatchStatus patch(const QString& filePath,
                                 const QString& startMarker,
                                 const QString& endMarker,
                                 const QString& newContent);

        /**
         * @brief MODE 3: SMART_APPEND
         * Simulates 'cat >> file' with a guard check to prevent duplicates.
         */
        static PatchStatus smartAppend(const QString& filePath,
                                       const QString& content,
                                       const QString& duplicateCheckMarker);

    private:
        static bool atomicWrite(const QString& filePath, const QString& content);
    };

}

#endif // FILE_PATCHER_HPP
