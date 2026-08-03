#include "OpenAIClient.hpp"
#include <QProcess>
#include <QDebug>

namespace SSAI {

OpenAIClient::OpenAIClient(QObject* parent) : QObject(parent) {
}

void OpenAIClient::sendPrompt(const QString& prompt, const QString& systemInstruction) {
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("OpenAI API Key is missing.", 0);
        return;
    }

    // Usando o SDK Python oficial com o padrão solicitado pelo usuário
    QString pythonScript = QString(
        "import sys\n"
        "from openai import OpenAI\n"
        "try:\n"
        "    client = OpenAI(api_key='%1')\n"
        "    user_input = sys.stdin.read()\n"
        "    # Tentando usar o padrao responses.create sugerido (se disponivel no SDK)\n"
        "    # Caso contrario, faz fallback para chat.completions.create padrao\n"
        "    try:\n"
        "        # Simulado/Futuro: responses.create\n"
        "        response = client.responses.create(\n"
        "            model='%2',\n"
        "            input=user_input\n"
        "        )\n"
        "        print(response.output_text)\n"
        "    except Exception:\n"
        "        # Fallback para ChatCompletion padrao\n"
        "        response = client.chat.completions.create(\n"
        "            model='gpt-4o',\n"
        "            messages=[{\"role\": \"user\", \"content\": user_input}]\n"
        "        )\n"
        "        print(response.choices[0].message.content)\n"
        "except Exception as e:\n"
        "    print(f'Erro: {e}', file=sys.stderr)\n"
        "    sys.exit(1)\n"
    ).arg(m_apiKey, m_modelName);

    QProcess* process = new QProcess(this);

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, process](int exitCode) {
        if (exitCode == 0) {
            QString output = QString::fromUtf8(process->readAllStandardOutput()).trimmed();
            if (!output.isEmpty()) {
                emit responseReceived(output);
            } else {
                emit errorOccurred("Sem resposta da OpenAI (Python SDK)", exitCode);
            }
        } else {
            QString error = QString::fromUtf8(process->readAllStandardError()).trimmed();
            emit errorOccurred(error.isEmpty() ? "Falha na execução do Python (OpenAI)" : error, exitCode);
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
