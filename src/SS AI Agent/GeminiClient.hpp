#ifndef GEMINI_CLIENT_HPP
#define GEMINI_CLIENT_HPP

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace SSAI {

    /**
     * @brief Specialized client for Google Gemini AI REST API.
     * Follows SSQLM principles of asynchronous reliability.
     */
    class GeminiClient : public QObject {
        Q_OBJECT
    public:
        explicit GeminiClient(QObject* parent = nullptr);

        void setApiKey(const QString& key) { m_apiKey = key; }
        void setModel(const QString& model) { m_modelName = model; }

        /**
         * @brief Sends a prompt to Gemini using the official Python SDK.
         * Result comes via signals.
         */
        void sendPrompt(const QString& prompt, const QString& systemInstruction = "");

    signals:
        void responseReceived(const QString& text);
        void errorOccurred(const QString& error, int statusCode);

    private:
        QString m_apiKey;
        QString m_modelName = "gemini-3.5-flash";
    };

}

#endif // GEMINI_CLIENT_HPP
