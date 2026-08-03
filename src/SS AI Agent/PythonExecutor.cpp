#include "PythonExecutor.hpp"
#include <QTemporaryFile>
#include <QDir>

namespace SSAI {

PythonExecutor::ExecutionResult PythonExecutor::executeScript(const QString& scriptContent) {
    QTemporaryFile tempFile(QDir::tempPath() + "/ssqlm_python_XXXXXX.py");
    if (tempFile.open()) {
        tempFile.write(scriptContent.toUtf8());
        tempFile.close();
        return executeFile(tempFile.fileName());
    }
    return {false, "", "Could not create temporary python file", -1};
}

PythonExecutor::ExecutionResult PythonExecutor::executeFile(const QString& filePath, const QStringList& args) {
    QProcess process;
    QStringList finalArgs;
    finalArgs << filePath << args;

    process.start("/home/marcel1237/venvs/gemini/bin/python3", finalArgs);
    if (!process.waitForStarted()) {
        return {false, "", "/home/marcel1237/venvs/gemini/bin/python3 not found", -1};
    }

    process.waitForFinished();

    ExecutionResult result;
    result.exitCode = process.exitCode();
    result.success = (result.exitCode == 0);
    result.output = QString::fromUtf8(process.readAllStandardOutput());
    result.error = QString::fromUtf8(process.readAllStandardError());

    return result;
}

}
