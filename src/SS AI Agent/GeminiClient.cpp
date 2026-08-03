#include "GeminiClient.hpp"
#include <QProcess>
#include <QDebug>

namespace SSAI {

GeminiClient::GeminiClient(QObject* parent) : QObject(parent) {
}

void GeminiClient::sendPrompt(const QString& prompt, const QString& systemInstruction) {
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API Key is missing.", 0);
        return;
    }

    // Usando o SDK Python recomendado (Modelo 2.0 Flash para estabilidade total)
    QString pythonScript = QString(
        "from google import genai\n"
        "import sys\n"
        "try:\n"
        "    client = genai.Client(api_key='%1')\n"
        "    user_input = sys.stdin.read()\n"
        "    # Usamos o 2.0-flash que é o modelo estável e recomendado atualmente\n"
        "    response = client.interactions.create(model='gemini-2.0-flash', input=user_input)\n"
        "\n"
        "    out = getattr(response, 'output_text', '')\n"
        "    if not out and hasattr(response, 'outputs') and response.outputs:\n"
        "        out = response.outputs[-1].text\n"
        "    \n"
        "    if out:\n"
        "        print(out)\n"
        "    else:\n"
        "        print('ERRO_INTERNO: Resposta vazia da API', file=sys.stderr)\n"
        "except Exception as e:\n"
        "    print(f'Erro: {e}', file=sys.stderr)\n"
        "    sys.exit(1)\n"
    ).arg(m_apiKey);

    QProcess* process = new QProcess(this);

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, process](int exitCode) {
        if (exitCode == 0) {
            QString output = QString::fromUtf8(process->readAllStandardOutput()).trimmed();
            if (!output.isEmpty()) {
                emit responseReceived(output);
            } else {
                emit errorOccurred("Sem resposta do Gemini (Python SDK)", exitCode);
            }
        } else {
            QString error = QString::fromUtf8(process->readAllStandardError()).trimmed();
            emit errorOccurred(error.isEmpty() ? "Falha na execução do Python" : error, exitCode);
        }
        process->deleteLater();
    });

    process->start("/home/marcel1237/venvs/gemini/bin/python3", QStringList() << "-c" << pythonScript);

    QString fullInput = prompt;
    if (!systemInstruction.isEmpty()) {
        fullInput = systemInstruction + "\n\n" + prompt;
    }

    process->write(fullInput.toUtf8());
    process->closeWriteChannel();
}

}
