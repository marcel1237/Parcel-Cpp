#include "WebSearchService.hpp"
#include "PythonExecutor.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

namespace SSAI {

WebSearchService::WebSearchService(QObject* parent) : QObject(parent) {
}

void WebSearchService::setApiKey(SearchProvider provider, const QString& key) {
    m_apiKeys[provider] = key;
}

WebSearchService::SearchResult WebSearchService::performSearch(const QString& query, int maxResults) {
    QString escapedQuery = query;
    escapedQuery.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n");

    QString providerStr;
    QString apiKey = m_apiKeys.value(m_provider);

    switch (m_provider) {
        case Tavily: providerStr = "tavily"; break;
        case Serper: providerStr = "serper"; break;
        case Brave: providerStr = "brave"; break;
        default: providerStr = "ddg"; break;
    }

    QString script = QString(
        "import json\n"
        "import requests\n"
        "def search():\n"
        "    provider = \"%1\"\n"
        "    query = \"%2\"\n"
        "    api_key = \"%3\"\n"
        "    max_results = %4\n"
        "    try:\n"
        "        if provider == 'tavily' and api_key:\n"
        "            from tavily import TavilyClient\n"
        "            tavily = TavilyClient(api_key=api_key)\n"
        "            res = tavily.search(query=query, search_depth='basic', max_results=max_results)\n"
        "            return {\"status\": \"success\", \"results\": [{\"title\": r['title'], \"href\": r['url'], \"body\": r['content']} for r in res['results']]}\n"
        "        elif provider == 'serper' and api_key:\n"
        "            headers = {'X-API-KEY': api_key, 'Content-Type': 'application/json'}\n"
        "            payload = json.dumps({\"q\": query, \"num\": max_results})\n"
        "            res = requests.post(\"https://google.serper.dev/search\", headers=headers, data=payload).json()\n"
        "            return {\"status\": \"success\", \"results\": [{\"title\": r.get('title', ''), \"href\": r.get('link', ''), \"body\": r.get('snippet', '')} for r in res.get('organic', [])]}\n"
        "        elif provider == 'brave' and api_key:\n"
        "            headers = {'Accept': 'application/json', 'Accept-Encoding': 'gzip', 'X-Subscription-Token': api_key}\n"
        "            res = requests.get(f\"https://api.search.brave.com/res/v1/web/search?q={query}&count={max_results}\", headers=headers).json()\n"
        "            return {\"status\": \"success\", \"results\": [{\"title\": r.get('title', ''), \"href\": r.get('url', ''), \"body\": r.get('description', '')} for r in res.get('web', {}).get('results', [])]}\n"
        "        else:\n"
        "            from duckduckgo_search import DDGS\n"
        "            with DDGS() as ddgs:\n"
        "                res = [r for r in ddgs.text(query, max_results=max_results)]\n"
        "                return {\"status\": \"success\", \"results\": res}\n"
        "    except Exception as e:\n"
        "        return {\"status\": \"error\", \"message\": str(e)}\n"
        "print(json.dumps(search()))\n"
    ).arg(providerStr, escapedQuery, apiKey).arg(maxResults);

    PythonExecutor executor;
    auto executionResult = executor.executeScript(script);

    SearchResult searchResult;
    searchResult.success = false;

    if (!executionResult.success) {
        searchResult.errorMessage = "Python Execution Failed: " + executionResult.error;
        return searchResult;
    }

    QJsonDocument doc = QJsonDocument::fromJson(executionResult.output.toUtf8());
    QJsonObject obj = doc.object();

    if (obj["status"].toString() == "success") {
        searchResult.success = true;
        QJsonArray results = obj["results"].toArray();
        for (const auto& resVal : results) {
            searchResult.items.append(resVal.toObject().toVariantMap());
        }
    } else {
        searchResult.errorMessage = obj["message"].toString();
    }

    return searchResult;
}

WebSearchService::SearchResult WebSearchService::performComparativeSearch(const QString& query) {
    QString escapedQuery = query;
    escapedQuery.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n");

    QString tavilyKey = m_apiKeys.value(Tavily);
    QString serperKey = m_apiKeys.value(Serper);
    QString braveKey = m_apiKeys.value(Brave);

    QString script = QString(
        "import json\n"
        "import requests\n"
        "from concurrent.futures import ThreadPoolExecutor\n"
        "def run_search(provider, key, query):\n"
        "    try:\n"
        "        p_label = provider.capitalize() if provider != 'serper' else 'Google'\n"
        "        if provider == 'tavily' and key:\n"
        "            from tavily import TavilyClient\n"
        "            res = TavilyClient(api_key=key).search(query=query, max_results=3)\n"
        "            return [{'title': r['title'], 'href': r['url'], 'body': r['content'], 'src': p_label} for r in res['results']]\n"
        "        if provider == 'serper' and key:\n"
        "            res = requests.post('https://google.serper.dev/search', headers={'X-API-KEY': key}, json={'q': query, 'num': 3}).json()\n"
        "            return [{'title': r.get('title',''), 'href': r.get('link',''), 'body': r.get('snippet',''), 'src': 'Google'} for r in res.get('organic', [])]\n"
        "        if provider == 'brave' and key:\n"
        "            res = requests.get(f'https://api.search.brave.com/res/v1/web/search?q={query}&count=3', headers={'X-Subscription-Token': key}).json()\n"
        "            return [{'title': r.get('title',''), 'href': r.get('url',''), 'body': r.get('description',''), 'src': p_label} for r in res.get('web', {}).get('results', [])]\n"
        "        from duckduckgo_search import DDGS\n"
        "        with DDGS() as ddgs: \n"
        "            return [{'title': r['title'], 'href': r['href'], 'body': r['body'], 'src': 'DDG'} for r in ddgs.text(query, max_results=3)]\n"
    ).arg(escapedQuery, tavilyKey, serperKey, braveKey);

    script +=
        "    except: return []\n"
        "query = \"" + escapedQuery + "\"\n"
        "tasks = [('tavily', \"" + tavilyKey + "\"), ('serper', \"" + serperKey + "\"), ('brave', \"" + braveKey + "\"), ('ddg', '')]\n"
        "unique_results = {}\n"
        "with ThreadPoolExecutor(max_workers=4) as executor:\n"
        "    futures = [executor.submit(run_search, t[0], t[1], query) for t in tasks]\n"
        "    for f in futures:\n"
        "        for item in f.result():\n"
        "            url = item['href']\n"
        "            if url in unique_results:\n"
        "                unique_results[url]['src'] += ', ' + item['src']\n"
        "            else:\n"
        "                unique_results[url] = item\n"
        "final_results = []\n"
        "for r in unique_results.values():\n"
        "    r['title'] = f\"{r['title']} ({r['src']})\"\n"
        "    final_results.append(r)\n"
        "print(json.dumps({\"status\": \"success\", \"results\": final_results}))\n";

    PythonExecutor executor;
    auto executionResult = executor.executeScript(script);

    SearchResult searchResult;
    searchResult.success = false;

    if (executionResult.success) {
        QJsonDocument doc = QJsonDocument::fromJson(executionResult.output.toUtf8());
        QJsonObject obj = doc.object();
        if (obj["status"].toString() == "success") {
            searchResult.success = true;
            QJsonArray results = obj["results"].toArray();
            for (const auto& resVal : results) searchResult.items.append(resVal.toObject().toVariantMap());
        }
    }
    return searchResult;
}

WebSearchService::SearchResult WebSearchService::performCodeSearch(const QString& query) {
    QString codeQuery = query + " site:github.com OR site:stackoverflow.com OR site:cppreference.com";
    return performSearch(codeQuery, 5);
}

QString WebSearchService::fetchUrlContent(const QString& url) {
    QString escapedUrl = url;
    escapedUrl.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n");

    QString script = QString(
        "import json\n"
        "import trafilatura\n"
        "try:\n"
        "    downloaded = trafilatura.fetch_url(\"%1\")\n"
        "    if downloaded:\n"
        "        result = trafilatura.extract(downloaded, include_comments=False, include_tables=False)\n"
        "        print(json.dumps({\"status\": \"success\", \"text\": result}))\n"
        "    else:\n"
        "        print(json.dumps({\"status\": \"error\", \"message\": \"Failed to download URL\"}))\n"
        "except Exception as e:\n"
        "    print(json.dumps({\"status\": \"error\", \"message\": str(e)}))\n"
    ).arg(escapedUrl);

    PythonExecutor executor;
    auto executionResult = executor.executeScript(script);
    if (!executionResult.success) return "Error: " + executionResult.error;

    QJsonDocument doc = QJsonDocument::fromJson(executionResult.output.toUtf8());
    QJsonObject obj = doc.object();
    if (obj["status"].toString() == "success") return obj["text"].toString();
    return "Extraction Error: " + obj["message"].toString();
}

QString WebSearchService::searchAndExtractScript(const QString& query) {
    QString escapedQuery = query + " bash shell script solution";
    escapedQuery.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n");

    QString script = QString(
        "import json\n"
        "import trafilatura\n"
        "import re\n"
        "from duckduckgo_search import DDGS\n"
        "try:\n"
        "    with DDGS() as ddgs:\n"
        "        # Search for the most relevant technical result\n"
        "        results = list(ddgs.text(\"%1\", max_results=2))\n"
        "        if not results: print(json.dumps({\"status\": \"error\", \"message\": \"No results\"}))\n"
        "        else:\n"
        "            url = results[0]['href']\n"
        "            downloaded = trafilatura.fetch_url(url)\n"
        "            content = trafilatura.extract(downloaded)\n"
        "            # Regex to find bash code blocks\n"
        "            code_match = re.search(r'```bash\\n(.*?)\\n```', content, re.DOTALL)\n"
        "            if not code_match:\n"
        "                code_match = re.search(r'#!/bin/bash(.*?)(?=\\n\\n|\\n```|$)', content, re.DOTALL)\n"
        "            \n"
        "            if code_match:\n"
        "                extracted = code_match.group(0 if '#!/bin/bash' in code_match.group(0) else 1)\n"
        "                print(json.dumps({\"status\": \"success\", \"script\": extracted, \"url\": url}))\n"
        "            else:\n"
        "                print(json.dumps({\"status\": \"error\", \"message\": \"No script block found in result\", \"url\": url}))\n"
        "except Exception as e:\n"
        "    print(json.dumps({\"status\": \"error\", \"message\": str(e)}))\n"
    ).arg(escapedQuery);

    PythonExecutor executor;
    auto executionResult = executor.executeScript(script);
    if (!executionResult.success) return "";

    QJsonDocument doc = QJsonDocument::fromJson(executionResult.output.toUtf8());
    QJsonObject obj = doc.object();
    if (obj["status"].toString() == "success") return obj["script"].toString();

    return "";
}

WebSearchService::DeepAnalysisResult WebSearchService::performDeepTechnicalAnalysis(const QString& topic) {
    QString escapedTopic = topic;
    escapedTopic.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n");

    QString script = QString(
        "import json\n"
        "import trafilatura\n"
        "import re\n"
        "from duckduckgo_search import DDGS\n"
        "from concurrent.futures import ThreadPoolExecutor\n"
        "def fetch_and_clean(url):\n"
        "    try:\n"
        "        downloaded = trafilatura.fetch_url(url)\n"
        "        return trafilatura.extract(downloaded, include_comments=False)\n"
        "    except: return \"\"\n"
        "try:\n"
        "    with DDGS() as ddgs:\n"
        "        search_results = list(ddgs.text(\"%1 technical implementation\", max_results=4))\n"
        "        urls = [r['href'] for r in search_results]\n"
        "        with ThreadPoolExecutor(max_workers=4) as executor:\n"
        "            contents = list(executor.map(fetch_and_clean, urls))\n"
        "        \n"
        "        full_text = \"\\n\".join(contents)\n"
        "        # SSQLM Synthesis: Extract all bash/cpp blocks\n"
        "        code_blocks = re.findall(r'```(?:bash|cpp)\\n(.*?)\\n```', full_text, re.DOTALL)\n"
        "        \n"
        "        print(json.dumps({\n"
        "            \"status\": \"success\",\n"
        "            \"summary\": f\"Found {len(urls)} sources. Extracted {len(code_blocks)} code patterns.\",\n"
        "            \"code\": \"\\n\\n# --- SYNTHESIZED PATTERN ---\\n\\n\".join(code_blocks[:3]),\n"
        "            \"sources\": urls\n"
        "        }))\n"
        "except Exception as e:\n"
        "    print(json.dumps({\"status\": \"error\", \"message\": str(e)}))\n"
    ).arg(escapedTopic);

    PythonExecutor executor;
    auto executionResult = executor.executeScript(script);

    DeepAnalysisResult result;
    result.success = false;

    if (executionResult.success) {
        QJsonDocument doc = QJsonDocument::fromJson(executionResult.output.toUtf8());
        QJsonObject obj = doc.object();
        if (obj["status"].toString() == "success") {
            result.success = true;
            result.report = obj["summary"].toString();
            result.synthesizedCode = obj["code"].toString();
            for (const auto& s : obj["sources"].toArray()) result.sources.append(s.toString());
        }
    }
    return result;
}

}
