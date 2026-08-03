#ifndef WEB_SEARCH_SERVICE_HPP
#define WEB_SEARCH_SERVICE_HPP

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>

namespace SSAI {

    /**
     * @brief Specialized service for World Wide Web (WWW) searching.
     * Integrates with Python search libraries to provide real-time information.
     */
    class WebSearchService : public QObject {
        Q_OBJECT
    public:
        explicit WebSearchService(QObject* parent = nullptr);

        enum SearchProvider {
            DuckDuckGo,
            Tavily,
            Serper,
            Brave
        };

        struct SearchResult {
            bool success;
            QList<QVariantMap> items; // title, href, body
            QString errorMessage;
        };

        void setProvider(SearchProvider provider) { m_provider = provider; }
        void setApiKey(SearchProvider provider, const QString& key);

        /**
         * @brief Performs a web search using the specified query.
         */
        SearchResult performSearch(const QString& query, int maxResults = 5);

        /**
         * @brief Performs a multi-provider search to get the most comprehensive results.
         */
        SearchResult performComparativeSearch(const QString& query);

        /**
         * @brief Specialized search for code snippets and implementation examples.
         */
        SearchResult performCodeSearch(const QString& query);

        /**
         * @brief Fetches and extracts the main content from a specific URL.
         */
        QString fetchUrlContent(const QString& url);

        /**
         * @brief Searches for a shell script solution and extracts the code.
         */
        QString searchAndExtractScript(const QString& query);

        /**
         * @brief Performs a deep technical crawl to synthesize an answer from multiple sources.
         */
        struct DeepAnalysisResult {
            bool success;
            QString report;
            QString synthesizedCode;
            QStringList sources;
        };
        DeepAnalysisResult performDeepTechnicalAnalysis(const QString& topic);

    private:
        SearchProvider m_provider = DuckDuckGo;
        QMap<SearchProvider, QString> m_apiKeys;
        QString runPythonSearch(const QString& script);
    };

}

#endif // WEB_SEARCH_SERVICE_HPP
