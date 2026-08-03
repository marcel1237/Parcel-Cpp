#include "MLService.hpp"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

namespace SSAI {

MLService::MLService(QObject* parent) : QObject(parent) {
}

MLService::MLResult MLService::predictCodeContext(const QString& codeSnippet) {
    // Escapa o código para o script Python
    QString escapedCode = codeSnippet;
    escapedCode.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n");

    QString script = QString(
        "import sys\n"
        "import json\n"
        "try:\n"
        "    from sklearn.feature_extraction.text import TfidfVectorizer\n"
        "    from sklearn.naive_bayes import MultinomialNB\n"
        "    \n"
        "    # Simulated Small Training Set for context recognition\n"
        "    train_data = [\n"
        "        ('void setup() { ... }', 'Setup/Init'),\n"
        "        ('int calculate() { return a + b; }', 'Logic'),\n"
        "        ('#include <QString>', 'Header/Include'),\n"
        "        ('emit signalName();', 'Qt Communication'),\n"
        "        ('connect(a, &B::c, d, &E::f);', 'Qt Connection')\n"
        "    ]\n"
        "    X_train, y_train = zip(*train_data)\n"
        "    \n"
        "    vectorizer = TfidfVectorizer()\n"
        "    X = vectorizer.fit_transform(X_train)\n"
        "    clf = MultinomialNB()\n"
        "    clf.fit(X, y_train)\n"
        "    \n"
        "    # Predict input\n"
        "    code_input = \"%1\"\n"
        "    X_test = vectorizer.transform([code_input])\n"
        "    pred = clf.predict(X_test)[0]\n"
        "    prob = float(clf.predict_proba(X_test).max())\n"
        "    \n"
        "    print(json.dumps({\"prediction\": pred, \"confidence\": prob, \"status\": \"success\"}))\n"
        "except Exception as e:\n"
        "    print(json.dumps({\"status\": \"error\", \"message\": str(e)}))\n"
    ).arg(escapedCode);

    QString output = runPythonML(script);
    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    QJsonObject obj = doc.object();

    MLResult result;
    result.success = (obj["status"].toString() == "success");
    if (result.success) {
        result.prediction = obj["prediction"].toString();
        result.confidence = obj["confidence"].toDouble();
    } else {
        result.details = obj["message"].toString();
    }
    return result;
}

MLService::MLResult MLService::analyzeStyleML(const QString& code) {
    QString escaped = code;
    escaped.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n");

    QString script = QString(
        "import json\n"
        "try:\n"
        "    # Model specialized in SSQLM Style deviations\n"
        "    code = \"%1\"\n"
        "    score = 1.0\n"
        "    reasons = []\n"
        "    if 'QString(' in code and 'QStringLiteral' not in code: \n"
        "        score -= 0.3; reasons.append('Uso de QString em vez de QStringLiteral')\n"
        "    if 'std::cout' in code: \n"
        "        score -= 0.2; reasons.append('Uso de std::cout em vez de qDebug()')\n"
        "    \n"
        "    print(json.dumps({\"prediction\": \", \".join(reasons) if reasons else \"Estilo Perfeito\", \"confidence\": score, \"status\": \"success\"}))\n"
        "except Exception as e:\n"
        "    print(json.dumps({\"status\": \"error\", \"message\": str(e)}))\n"
    ).arg(escaped);

    QString output = runPythonML(script);
    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    QJsonObject obj = doc.object();

    MLResult result;
    result.success = (obj["status"].toString() == "success");
    result.prediction = obj["prediction"].toString();
    result.confidence = obj["confidence"].toDouble();
    return result;
}

MLService::MLResult MLService::predictBugRisk(const QString& code) {
    QString escaped = code;
    escaped.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n");

    QString script = QString(
        "import json\n"
        "import numpy as np\n"
        "try:\n"
        "    # Logistic Regression Simulation for Bug Prediction\n"
        "    # Features: Line length, Nesting level, Number of conditions\n"
        "    code = \"%1\"\n"
        "    nesting = code.count('{')\n"
        "    ifs = code.count('if')\n"
        "    loops = code.count('for') + code.count('while')\n"
        "    \n"
        "    # Weights for a higher bug risk\n"
        "    risk = (nesting * 0.1) + (ifs * 0.15) + (loops * 0.2)\n"
        "    risk = min(risk, 1.0)\n"
        "    \n"
        "    print(json.dumps({\"prediction\": \"Probabilidade de Bug\", \"confidence\": risk, \"status\": \"success\"}))\n"
        "except Exception as e:\n"
        "    print(json.dumps({\"status\": \"error\", \"message\": str(e)}))\n"
    ).arg(escaped);

    QString output = runPythonML(script);
    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    QJsonObject obj = doc.object();

    MLResult result;
    result.success = (obj["status"].toString() == "success");
    result.confidence = obj["confidence"].toDouble();
    result.prediction = result.confidence > 0.6 ? "Risco de Bug Elevado" : "Código Estável";
    return result;
}

MLService::MLResult MLService::semanticSearch(const QString& query, const QMap<QString, QString>& corpus) {
    if (corpus.isEmpty()) return {false, "", 0, "Corpus vazio"};

    // Prepara os dados para o Python (Otimização SSQLM: TF-IDF com N-grams e LSA)
    QJsonObject corpusJson;
    for (auto it = corpus.begin(); it != corpus.end(); ++it) corpusJson[it.key()] = it.value();

    QString corpusStr = QJsonDocument(corpusJson).toJson(QJsonDocument::Compact);
    corpusStr.replace("\"", "\\\"");

    QString script = QString(
        "import json\n"
        "import numpy as np\n"
        "from sklearn.feature_extraction.text import TfidfVectorizer\n"
        "from sklearn.decomposition import TruncatedSVD\n"
        "from sklearn.metrics.pairwise import cosine_similarity\n"
        "try:\n"
        "    corpus_data = json.loads(\"%1\")\n"
        "    query = \"%2\"\n"
        "    \n"
        "    filenames = list(corpus_data.keys())\n"
        "    documents = list(corpus_data.values())\n"
        "    \n"
        "    # SSQLM Otimização: Uso de n-grams (1,2) para capturar contexto sintático C++\n"
        "    vectorizer = TfidfVectorizer(stop_words='english', ngram_range=(1,2))\n"
        "    tfidf_matrix = vectorizer.fit_transform(documents)\n"
        "    \n"
        "    # Redução de dimensionalidade (LSA) para capturar semântica latente\n"
        "    n_components = min(tfidf_matrix.shape[1] - 1, 100)\n"
        "    lsa = TruncatedSVD(n_components=n_components)\n"
        "    lsa_matrix = lsa.fit_transform(tfidf_matrix)\n"
        "    \n"
        "    query_vec = vectorizer.transform([query])\n"
        "    query_lsa = lsa.transform(query_vec)\n"
        "    \n"
        "    similarities = cosine_similarity(query_lsa, lsa_matrix).flatten()\n"
        "    best_idx = np.argmax(similarities)\n"
        "    \n"
        "    result = {\n"
        "        \"prediction\": filenames[best_idx],\n"
        "        \"confidence\": float(similarities[best_idx]),\n"
        "        \"status\": \"success\"\n"
        "    }\n"
        "    print(json.dumps(result))\n"
        "except Exception as e:\n"
        "    print(json.dumps({\"status\": \"error\", \"message\": str(e)}))\n"
    ).arg(corpusStr, query.toHtmlEscaped());

    QString output = runPythonML(script);
    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    QJsonObject obj = doc.object();

    MLResult result;
    result.success = (obj["status"].toString() == "success");
    if (result.success) {
        result.prediction = obj["prediction"].toString();
        result.confidence = obj["confidence"].toDouble();
    }
    return result;
}

MLService::MLResult MLService::runAnomalyDetection() {
    QString script =
        "import json\n"
        "import numpy as np\n"
        "from sklearn.ensemble import IsolationForest\n"
        "try:\n"
        "    # Simple numerical data simulation (e.g. file size vs line count)\n"
        "    data = np.array([[100, 10], [150, 12], [3000, 250], [120, 11], [50000, 50]]) # The last one is an outlier\n"
        "    clf = IsolationForest(contamination=0.2, random_state=42)\n"
        "    preds = clf.fit_predict(data)\n"
        "    outliers = int(np.sum(preds == -1))\n"
        "    print(json.dumps({\"status\": \"success\", \"outliers\": outliers}))\n"
        "except Exception as e:\n"
        "    print(json.dumps({\"status\": \"error\", \"message\": str(e)}))\n";

    QString output = runPythonML(script);
    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    QJsonObject obj = doc.object();

    MLResult result;
    result.success = (obj["status"].toString() == "success");
    if (result.success) {
        result.prediction = QString::number(obj["outliers"].toInt()) + " anomalias detectadas";
    }
    return result;
}

QString MLService::runPythonML(const QString& script) {
    PythonExecutor executor;
    auto result = executor.executeScript(script);
    if (result.success) return result.output;
    return "{\"status\": \"error\", \"message\": \"Python execution failed\"}";
}

}
