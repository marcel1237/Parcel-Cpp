#include "SSAIModule.hpp"
#include "ScriptValidator.hpp"
#include <QProcessEnvironment>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

namespace SSAI {

SSAIModule::SSAIModule(QObject* parent) : QObject(parent) {
    m_qlmDir = QDir::currentPath() + "/QLM";
    m_qlmEngine = new QLMEngine(m_qlmDir);
    m_localAgent = new LocalAgent(this);
    m_mlService = new MLService(this);
    m_webSearch = new WebSearchService(this);
    m_gemini = new GeminiClient(this);
    m_openai = new OpenAIClient(this);

    // Auto-index project if we are in a known path
    m_localAgent->indexProject(QDir::currentPath());

    // Wire up Gemini
    m_gemini->setApiKey(qgetenv("GEMINI_API_KEY"));
    connect(m_gemini, &GeminiClient::responseReceived, this, [this](const QString& response) {
        m_lastAiResponse = response;
        emit geminiResponseReceived(response);
    });
    connect(m_gemini, &GeminiClient::errorOccurred, this, [this](const QString& err, int code) {
        emit geminiErrorOccurred(QString("Gemini Error (%1): %2").arg(code).arg(err));
    });

    // Wire up OpenAI
    m_openai->setApiKey(qgetenv("OPENAI_API_KEY"));
    connect(m_openai, &OpenAIClient::responseReceived, this, [this](const QString& response) {
        m_lastAiResponse = response;
        emit openaiResponseReceived(response);
    });
    connect(m_openai, &OpenAIClient::errorOccurred, this, [this](const QString& err, int code) {
        emit openaiErrorOccurred(QString("OpenAI Error (%1): %2").arg(code).arg(err));
    });
}

SSAIModule::QLMResponse SSAIModule::processInstruction(const QString& instruction) {
    // 1. Context Update
    QString detectedFile = QLMEngine::extractFilename(instruction);
    if (!detectedFile.isEmpty()) m_currentFile = detectedFile;

    QString lower = instruction.toLower();

    // 2. TRIAGEM DE AGENTE (Multi-Agent Orchestration)

    // A. AGENTE ESTRUTURAL (LocalAgent - Alta Prioridade para comandos diretos)
    LocalAgent::AgentResponse localResp = m_localAgent->process(instruction);
    if (localResp.handled) {
        if (!localResp.action.isEmpty()) return {QLMResponse::ACTION, localResp.message, localResp.action, m_currentFile};
        return {QLMResponse::MESSAGE, localResp.message, "", ""};
    }

    // B. AGENTE WWW (WebSearch - Busca em tempo real na internet)
    if (lower.startsWith("www ") || lower.startsWith("pesquise ") || lower.startsWith("google ")) {
        QString query = instruction.mid(instruction.indexOf(" ") + 1).trimmed();
        return {QLMResponse::MESSAGE, requestWebSearch(query), "", ""};
    }

    if (lower.startsWith("comparar ") || lower.startsWith("compare ")) {
        QString query = instruction.mid(instruction.indexOf(" ") + 1).trimmed();
        QString report = "⚖️ <b>Comparador WWW SSAI: '" + query + "'</b><br>";
        report += "<i>Consultando simultaneamente: Tavily, Serper, Brave e DuckDuckGo...</i><br><br>";

        auto result = m_webSearch->performComparativeSearch(query);
        if (result.success) {
            for (const auto& item : result.items) {
                report += QString("• <b><a href='%1' style='color: #00BFFF;'>%2</a></b><br><i>%3</i><br><br>")
                            .arg(item["href"].toString(), item["title"].toString(), item["body"].toString().left(150) + "...");
            }
        }
        return {QLMResponse::MESSAGE, report, "", ""};
    }

    if (lower.startsWith("code search ") || lower.startsWith("exemplo ")) {
        QString query = instruction.mid(instruction.contains("code") ? 12 : 8).trimmed();
        QString report = "💻 <b>Busca de Código WWW: '" + query + "'</b><br><br>";
        auto result = m_webSearch->performCodeSearch(query);
        if (result.success) {
            for (const auto& item : result.items) {
                report += QString("• <b><a href='%1' style='color: #FFD700;'>%2</a></b><br>").arg(item["href"].toString(), item["title"].toString());
            }
        }
        return {QLMResponse::MESSAGE, report, "", ""};
    }

    if (lower.startsWith("ler ") || lower.startsWith("acesse ")) {
        QString url = instruction.mid(instruction.indexOf(" ") + 1).trimmed();
        return {QLMResponse::MESSAGE, requestUrlContent(url), "", ""};
    }

    if (lower.startsWith("pesquisa completa ") || lower.startsWith("research ")) {
        QString topic = instruction.mid(instruction.contains("pesquisa") ? 18 : 9).trimmed();

        // SSQLM Enhancement: Use the new Deep Technical Analysis
        auto deepResult = m_webSearch->performDeepTechnicalAnalysis(topic);
        if (deepResult.success) {
            QString report = "🔬 <b>SSQLM Deep Research: '" + topic + "'</b><br>";
            report += "📊 " + deepResult.report + "<br><br>";
            report += "🔗 <b>Fontes:</b> " + deepResult.sources.join(", ") + "<br><br>";

            if (!deepResult.synthesizedCode.isEmpty()) {
                report += "💻 <b>Padrões de Código Extraídos:</b><br>";
                report += "<pre style='color: #7AA2F7; background-color: #1a1b26; padding: 10px; border: 1px solid #333;'>" + deepResult.synthesizedCode.toHtmlEscaped() + "</pre>";
                m_localAgent->setLastArtifact(deepResult.synthesizedCode);
                report += "<br>💡 <i>Use 'executar' ou 'salvar' para processar estes padrões.</i>";
            }

            return {QLMResponse::MESSAGE, report, "", ""};
        }

        return {QLMResponse::MESSAGE, requestFullResearch(topic), "", ""};
    }

    if (lower.startsWith("web automate ") || lower.startsWith("automatizar web ")) {
        QString task = instruction.mid(instruction.contains("web") ? 13 : 16).trimmed();
        return {QLMResponse::MESSAGE, requestWebAutomation(task), "", ""};
    }

    // C. MODELOS ESPECIALIZADOS (ML Models)
    if (lower.startsWith("ml estilo")) {
        if (m_currentFile.isEmpty()) return {QLMResponse::MESSAGE, "Selecione um arquivo primeiro para auditoria de estilo ML.", "", ""};
        QFile f(m_currentFile);
        if (f.open(QIODevice::ReadOnly)) {
            QString code = f.readAll();
            f.close();
            MLService::MLResult res = m_mlService->analyzeStyleML(code);
            return {QLMResponse::MESSAGE, "🎨 <b>Especialista em Estilo:</b> " + res.prediction + " (Score: " + QString::number(res.confidence * 100, 'f', 0) + "%)", "", ""};
        }
        return {QLMResponse::MESSAGE, "❌ Erro ao abrir o arquivo para análise de estilo.", "", ""};
    }

    if (lower.startsWith("ml risco")) {
        if (m_currentFile.isEmpty()) return {QLMResponse::MESSAGE, "Selecione um arquivo para prever o risco de bugs.", "", ""};
        QFile f(m_currentFile);
        if (f.open(QIODevice::ReadOnly)) {
            QString code = f.readAll();
            f.close();
            MLService::MLResult res = m_mlService->predictBugRisk(code);
            QString color = res.confidence > 0.6 ? "#FF4500" : "#00FF7F";
            return {QLMResponse::MESSAGE, "🐛 <b>Preditor de Bugs:</b> <font color='" + color + "'>" + res.prediction + "</font> (" + QString::number(res.confidence * 100, 'f', 0) + "%)", "", ""};
        }
        return {QLMResponse::MESSAGE, "❌ Erro ao abrir o arquivo para análise de risco.", "", ""};
    }

    // B. AGENTE SEMÂNTICO (MLService - Para perguntas conceituais sobre o projeto)
    if (lower.contains("onde") || lower.contains("como funciona") || lower.contains("quem cuida")) {
        // Coleta mini-corpus para busca semântica (limitado por performance)
        QMap<QString, QString> corpus;
        QDir dir(QDir::currentPath() + "/src");
        QDirIterator it(dir.path(), QStringList() << "*.hpp" << "*.cpp", QDir::Files, QDirIterator::Subdirectories);
        int count = 0;
        while (it.hasNext() && count < 30) {
            it.next();
            QFile f(it.filePath());
            if (f.open(QIODevice::ReadOnly)) {
                corpus[it.fileName()] = f.read(1000); // Primeiros 1kb
                f.close();
            }
            count++;
        }

        MLService::MLResult ml = m_mlService->semanticSearch(instruction, corpus);
        if (ml.success && ml.confidence > 0.3) {
            return {QLMResponse::MESSAGE,
                "🧠 <b>Orquestrador SSAI:</b> Com base na análise semântica do seu código, acredito que a parte relevante seja: <b>" + ml.prediction + "</b>.<br>"
                "(Confiança ML: " + QString::number(ml.confidence * 100, 'f', 1) + "%)", "", ""};
        }
    }

    // C. AGENTE CRIATIVO/DIFÍCIL (LLM - Fallback para raciocínio profundo)

    // 3. Manual Key Setting Support (Internal override)
    if (instruction.toLower().startsWith("set key ")) {
        QString cmd = instruction.mid(8).trimmed();
        if (cmd.startsWith("openai ")) {
            m_openai->setApiKey(cmd.mid(7).trimmed());
            return {QLMResponse::MESSAGE, "Chave OpenAI atualizada via QLM Engine.", "", ""};
        } else if (cmd.startsWith("tavily ")) {
            QString key = cmd.mid(7).trimmed();
            m_webSearch->setApiKey(WebSearchService::Tavily, key);
            m_webSearch->setProvider(WebSearchService::Tavily);
            return {QLMResponse::MESSAGE, "Chave Tavily atualizada e motor de busca definido para Tavily AI.", "", ""};
        } else if (cmd.startsWith("serper ")) {
            QString key = cmd.mid(7).trimmed();
            m_webSearch->setApiKey(WebSearchService::Serper, key);
            m_webSearch->setProvider(WebSearchService::Serper);
            return {QLMResponse::MESSAGE, "Chave Serper atualizada e motor de busca definido para Google (via Serper).", "", ""};
        } else if (cmd.startsWith("brave ")) {
            QString key = cmd.mid(6).trimmed();
            m_webSearch->setApiKey(WebSearchService::Brave, key);
            m_webSearch->setProvider(WebSearchService::Brave);
            return {QLMResponse::MESSAGE, "Chave Brave atualizada e motor de busca definido para Brave Search.", "", ""};
        } else {
            m_gemini->setApiKey(cmd);
            return {QLMResponse::MESSAGE, "Chave Gemini atualizada via QLM Engine.", "", ""};
        }
    }

    // 4. OpenAI Fallback (With Web Context)
    if (instruction.contains("gpt", Qt::CaseInsensitive) || instruction.contains("openai", Qt::CaseInsensitive)) {
        if (qgetenv("OPENAI_API_KEY").isEmpty()) {
            return {QLMResponse::MESSAGE, "🤖 <b>Aviso:</b> Para usar a OpenAI (GPT), configure a chave primeiro.", "", ""};
        }

        // PESQUISA WEB AUTOMÁTICA PARA CONTEXTO
        auto search = m_webSearch->performSearch(instruction, 3);
        QString webContext = "";
        if (search.success) {
            for (const auto& item : search.items) webContext += item["title"].toString() + ": " + item["body"].toString() + "\n";
        }

        QString finalPrompt = "CONTEÚDO DA WEB EM TEMPO REAL:\n" + webContext + "\n\nPERGUNTA DO USUÁRIO: " + instruction;
        m_openai->sendPrompt(finalPrompt);
        return {QLMResponse::ASYNC_WAIT, "Pesquisando na web e consultando OpenAI...", "", ""};
    }

    // 5. Gemini Dynamic Fallback (Default - With Web Context)
    if (qgetenv("GEMINI_API_KEY").isEmpty()) {
        // ... existing ML/Offline logic ...
        return {QLMResponse::MESSAGE, "🤖 <b>SS AI Agent (Offline):</b> ...", "", ""};
    }

    // PESQUISA WEB AUTOMÁTICA PARA GEMINI
    auto search = m_webSearch->performSearch(instruction, 3);
    QString webContext = "";
    if (search.success) {
        for (const auto& item : search.items) webContext += item["title"].toString() + ": " + item["body"].toString() + "\n";
    }

    QString finalPrompt = "CONTEXTO DA INTERNET (SSQLM WEB-SYNC):\n" + webContext +
                          "\n\nINSTRUÇÃO: Com base nos dados acima e no seu conhecimento, responda: " + instruction;

    m_gemini->sendPrompt(finalPrompt);
    return {QLMResponse::ASYNC_WAIT, "Sincronizando com a Web e consultando Gemini...", "", ""};
}

QString SSAIModule::requestWebSearch(const QString& query) {
    QString report = "🌐 <b>Busca WWW SSQLM: '" + query + "'</b><br><br>";
    auto result = m_webSearch->performSearch(query);

    if (result.success) {
        for (const auto& item : result.items) {
            QString title = item["title"].toString();
            QString href = item["href"].toString();
            QString body = item["body"].toString();
            report += QString("• <b><a href='%1' style='color: #00BFFF;'>%2</a></b><br><i>%3</i><br><br>")
                        .arg(href, title, body.left(200).toHtmlEscaped() + "...");
        }
        if (result.items.isEmpty()) report += "Nenhum resultado encontrado na web.";
    } else {
        report += "<font color='#FF4500'>[ERRO WWW]</font> " + result.errorMessage;
    }

    return report;
}

QString SSAIModule::requestUrlContent(const QString& url) {
    QString report = "📄 <b>Extraindo Conteúdo WWW: " + url + "</b><br><br>";
    QString content = m_webSearch->fetchUrlContent(url);

    if (content.startsWith("Error") || content.startsWith("Extraction Error")) {
        report += "<font color='#FF4500'>" + content + "</font>";
    } else {
        report += "<div style='color: #adbac7; background-color: #161925; padding: 10px; border: 1px solid #333;'>";
        report += content.left(2000).toHtmlEscaped().replace("\n", "<br>") + "...";
        report += "</div><br><font color='#00FF7F'>[SUCCESS]</font> O cérebro da IA agora possui o conteúdo completo desta página.";

        // Se houver uma IA configurada, envia o conteúdo como contexto para uma síntese automática
        if (!qgetenv("GEMINI_API_KEY").isEmpty()) {
             m_gemini->sendPrompt("Analise este conteúdo extraído da web e faça um resumo executivo para o desenvolvedor:\n\n" + content);
        }
    }

    return report;
}

QString SSAIModule::requestFullResearch(const QString& topic) {
    QString report = "🔬 <b>SSQLM Research Cycle: '" + topic + "'</b><br>";
    report += "[STEP 1] Buscando referências na Web...<br>";

    auto search = m_webSearch->performSearch(topic, 3);
    if (!search.success || search.items.isEmpty()) return report + "❌ Falha na etapa de busca.";

    QString fullCorpus = "";
    for (int i = 0; i < search.items.size(); ++i) {
        QString url = search.items[i]["href"].toString();
        report += QString("[STEP 2] Extraindo conteúdo de: %1...<br>").arg(QFileInfo(url).fileName());
        fullCorpus += "\n--- SOURCE: " + url + " ---\n";
        fullCorpus += m_webSearch->fetchUrlContent(url).left(3000); // 3kb per site
    }

    report += "[STEP 3] Sintetizando conhecimento via cérebro IA...<br>";

    if (!qgetenv("GEMINI_API_KEY").isEmpty()) {
        m_gemini->sendPrompt("Você é um Pesquisador de Elite SSQLM. Analise este corpus extraído da web sobre '" + topic + "' e forneça um relatório técnico definitivo:\n\n" + fullCorpus);
    } else {
        report += "<br><font color='#FFD700'>Relatório local resumido:</font><br>" + fullCorpus.left(500).toHtmlEscaped() + "...";
    }

    return report;
}

QString SSAIModule::requestWebAutomation(const QString& task) {
    QString report = "🌐 <b>Extraordinary Web Automation: '" + task + "'</b><br>";
    report += "<i>Buscando solução automatizada na web...</i><br><br>";

    QString script = m_webSearch->searchAndExtractScript(task);

    if (script.isEmpty()) {
        return report + "❌ <b>Falha:</b> Nenhuma solução em Shell Script foi extraída automaticamente para esta tarefa.";
    }

    // Registra como último artefato para permitir o comando 'executar'
    m_localAgent->setLastArtifact(script);

    report += "✅ <b>Script Extraído com Sucesso!</b><br>";
    report += "<pre style='color: #7AA2F7; background-color: #1a1b26; padding: 10px; border: 1px solid #333;'>" + script.toHtmlEscaped() + "</pre>";
    report += "<br><font color='#FFD700'>⚠️ Verifique o código acima cuidadosamente.</font><br>";
    report += "Dica: Digite <b>'executar'</b> para aplicar estas alterações no projeto.";

    return report;
}

QString SSAIModule::saveResearchResult(const QString& topic, const QString& content) {
    QString actualContent = content.isEmpty() ? m_lastAiResponse : content;
    if (actualContent.isEmpty()) return "❌ <b>Erro:</b> Nenhum conteúdo para salvar.";

    QString dirPath = QDir::currentPath() + "/research";
    QDir dir(dirPath);
    if (!dir.exists()) dir.mkpath(".");

    QString safeTopic = topic.toLower().replace(" ", "_");
    if (topic == "AI Knowledge") {
        safeTopic = "ai_knowledge_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    }

    QString filePath = dirPath + "/" + safeTopic + ".md";

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "# Research: " << topic << "\n";
        out << "Date: " << QDateTime::currentDateTime().toString() << "\n\n";
        out << actualContent;
        file.close();

        m_localAgent->indexProject(QDir::currentPath()); // Update index
        return "💾 <b>Sucesso:</b> Conhecimento salvo em <i>research/" + safeTopic + ".md</i>";
    }

    return "❌ <b>Erro:</b> Não foi possível salvar o arquivo de pesquisa.";
}

QString SSAIModule::requestMLPrediction(const QString& code) {
    auto res = m_mlService->predictCodeContext(code);
    if (res.success) {
        return QString("ML Prediction: %1 (Confidence: %2%)").arg(res.prediction).arg(res.confidence * 100);
    }
    return "ML Analysis failed: " + res.details;
}

QString SSAIModule::requestAnalysis(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "❌ Arquivo não encontrado: " + path;

    QString content = file.readAll();
    file.close();

    // Prioritize Extraordinary Local Audit from LocalAgent
    return m_localAgent->auditCode(content);
}

QString SSAIModule::generateExtraordinaryPlan(const QString& task) {
    QString plan = "📜 <b>Extraordinary SSQLM Plan: " + task + "</b><br><br>";

    if (task.contains("view", Qt::CaseInsensitive) || task.contains("tela", Qt::CaseInsensitive)) {
        plan += "1. 🛠️ <b>Criação de Arquivos:</b> Gerar HPP em src/view/<br>";
        plan += "2. 🧬 <b>Registro:</b> Adicionar à SOURCES no CMakeLists.txt<br>";
        plan += "3. 🔗 <b>Navegação:</b> Incluir novo alvo em NavigationTarget.hpp e atualizar NavigationController<br>";
        plan += "4. 🖥️ <b>Integração:</b> Instanciar e adicionar ao contentStack na MainWindow.hpp<br>";
        plan += "5. ✨ <b>Estilo:</b> Aplicar folha de estilo SSQLM Dark Core.";
    } else if (task.contains("build", Qt::CaseInsensitive) || task.contains("compilar", Qt::CaseInsensitive)) {
        plan += "1. 💾 <b>Backup:</b> Executar BackupService::createSave<br>";
        plan += "2. 🧹 <b>Limpeza:</b> Executar 'rm -rf build' para build limpo<br>";
        plan += "3. ⚙️ <b>Configuração:</b> Rodar cmake no diretório /build<br>";
        plan += "4. 🔨 <b>Compilação:</b> Executar make com suporte multi-core<br>";
        plan += "5. 🧪 <b>Teste:</b> Iniciar aplicação com logcat capturado.";
    } else {
        plan += "Solicitação genérica recebida. Recomendo usar a cirurgia de código SSQLM para analisar o impacto desta tarefa nos serviços existentes.";
    }

    return plan;
}

}
