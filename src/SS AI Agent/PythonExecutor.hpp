#ifndef PYTHON_EXECUTOR_HPP
#define PYTHON_EXECUTOR_HPP

#include <QString>
#include <QProcess>
#include <QDebug>

namespace SSAI {

    /**
     * @brief The PythonExecutor class provides a high-reliability interface
     * to execute Python scripts from C++, following SSQLM safety standards.
     */
    class PythonExecutor {
    public:
        struct ExecutionResult {
            bool success;
            QString output;
            QString error;
            int exitCode;
        };

        /**
         * @brief Executes a Python script from a string.
         * Creates a temporary file and runs it with the system python3.
         */
        static ExecutionResult executeScript(const QString& scriptContent);

        /**
         * @brief Executes a Python file from path.
         */
        static ExecutionResult executeFile(const QString& filePath, const QStringList& args = {});
    };

}

#endif // PYTHON_EXECUTOR_HPP
