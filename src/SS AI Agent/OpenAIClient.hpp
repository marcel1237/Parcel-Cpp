#ifndef OPENAI_CLIENT_HPP
#define OPENAI_CLIENT_HPP

#include <QObject>
#include <QString>

namespace SSAI {

    class OpenAIClient : public QObject {
        Q_OBJECT
    public:
        explicit OpenAIClient(QObject* parent = nullptr);

        void setApiKey(const QString& key) { m_apiKey = key; }
        void setModel(const QString& model) { m_modelName = model; }

        void sendPrompt(const QString& prompt, const QString& systemInstruction = "");

    signals:
        void responseReceived(const QString& text);
        void errorOccurred(const QString& error, int statusCode);

    private:
        QString m_apiKey;
        QString m_modelName = "gpt-4o"; // Default to a stable model
    };

}

#endif // OPENAI_CLIENT_HPP
