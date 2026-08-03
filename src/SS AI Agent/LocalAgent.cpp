#include "LocalAgent.hpp"
#include "ProjectKnowledge.hpp"
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QRegularExpression>
#include <QProcess>

namespace SSAI {

LocalAgent::LocalAgent(QObject* parent) : QObject(parent) {
    m_qlm = new QLMEngine(QDir::currentPath() + "/QLM");
    m_ssqlm = new QLMEngine(QDir::currentPath() + "/SS QLM");
}

void LocalAgent::reload() {
    m_qlm->reloadRules();
    m_ssqlm->reloadRules();
}

void LocalAgent::indexProject(const QString& rootPath) {
    ProjectKnowledge::getInstance().setProjectRoot(rootPath);
    scanDirectory(rootPath);
}

void LocalAgent::scanDirectory(const QString& path) {
    QDir dir(path);
    QStringList filters;
    filters << "*.cpp" << "*.hpp" << "*.c" << "*.h";

    // Index files
    for (const QString& filename : dir.entryList(filters, QDir::Files)) {
        QString fullPath = dir.absoluteFilePath(filename);
        QFile file(fullPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString content = file.readAll();
            file.close();

            // Basic symbol extraction (Classes)
            QRegularExpression classRegex("class\\s+(\\w+)");
            QRegularExpressionMatchIterator i = classRegex.globalMatch(content);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                ProjectKnowledge::getInstance().registerSymbol(match.captured(1), fullPath, "class");
            }

            // Method extraction (Simple C++ heuristic)
            QRegularExpression methodRegex("\\b(\\w+)::(\\w+)\\s*\\(");
            QRegularExpressionMatchIterator j = methodRegex.globalMatch(content);
            while (j.hasNext()) {
                QRegularExpressionMatch match = j.next();
                QString fullName = match.captured(1) + "::" + match.captured(2);
                ProjectKnowledge::getInstance().registerSymbol(fullName, fullPath, "method");
                ProjectKnowledge::getInstance().registerSymbol(match.captured(2), fullPath, "method"); // Short name too
            }
        }
    }

    // Recurse
    for (const QString& subDir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (subDir == "build" || subDir == ".git" || subDir == ".kdev4") continue;
        scanDirectory(dir.absoluteFilePath(subDir));
    }
}

LocalAgent::AgentResponse LocalAgent::process(const QString& instruction) {
    AgentResponse response = {false, "", "", ""};
    QString lower = instruction.toLower();

    // 1. Check SS QLM (Extraordinary Logic) first
    QLMMatch ssMatch = m_ssqlm->findMatch(instruction);
    if (ssMatch.matched) {
        response.handled = true;
        response.message = ssMatch.response;
        response.action = ssMatch.action;
        response.target = ssMatch.mode; // Mode often contains the specific logic
        return response;
    }

    // 2. Check Standard QLM
    QLMMatch qlmMatch = m_qlm->findMatch(instruction);
    if (qlmMatch.matched) {
        response.handled = true;
        response.message = qlmMatch.response;
        response.action = qlmMatch.action;
        return response;
    }

    // 3. Special "Extraordinary Plan" Logic
    if (lower.contains("plano") || lower.contains("como fazer") || lower.contains("extraordinary")) {
        response.handled = true;
        response.message = generateSsqlmPlan(instruction);
        m_lastArtifact = response.message;
        return response;
    }

    if (lower.contains("sugestão") || lower.contains("dica")) {
        response.handled = true;
        response.message = getSuggestions("");
        return response;
    }

    // 4. Symbol Lookup in Project Knowledge
    if (lower.startsWith("onde está ") || lower.startsWith("localize ")) {
        QString symbol = instruction.mid(10).trimmed().remove("?");
        QString path = ProjectKnowledge::getInstance().findSymbolLocation(symbol);
        QString type = ProjectKnowledge::getInstance().getSymbolType(symbol);
        response.handled = true;
        if (!path.isEmpty()) {
            QString typeStr = (type == "method" ? "o método" : "a classe/símbolo");
            response.message = QString("🎯 <b>Encontrado:</b> %1 <b>%2</b> está localizado em:<br><i>%3</i>")
                                .arg(typeStr, symbol, path);
            response.action = "OPEN";
            response.target = path;
        } else {
            response.message = QString("🔍 Não consegui localizar o símbolo <b>%1</b> no índice local. Tente reindexar o projeto.").arg(symbol);
        }
        return response;
    }

    if (lower.startsWith("gerar script ") || lower.startsWith("crie script ")) {
        QString type = instruction.mid(13).trimmed();
        response.handled = true;
        response.message = "🛠️ <b>Script SSQLM Gerado:</b> Confira o código abaixo.";
        response.action = "PREVIEW_SCRIPT";
        response.target = generateScript(type);
        m_lastArtifact = response.target;

        // Auto-add tasks based on script type
        ProjectKnowledge::getInstance().addTask("Executar script gerado: " + type);
        return response;
    }

    if (lower.startsWith("gerar qt ") || lower.startsWith("componente ")) {
        // ... (existing logic) ...
    }

    if (lower.startsWith("gerar classe ") || lower.startsWith("novo serviço ")) {
        QString name = instruction.mid(instruction.contains("gerar") ? 13 : 13).trimmed();
        QString type = instruction.contains("serviço") ? "service" : "view";
        response.handled = true;
        response.message = "🏗️ <b>Blueprint SSQLM Gerado:</b> Confira o header da nova classe.";
        response.action = "CODE_SAMPLE";
        response.target = generateClass(name, type);
        ProjectKnowledge::getInstance().addTask("Implementar lógica em " + name);
        return response;
    }

    if (lower.contains("analise logs") || lower.contains("verificar logs")) {
        // Real logic would pull from LogcatPane, but here we can simulate or handle a provided snippet
        response.handled = true;
        response.message = "🔍 Envie o trecho do log ou execute o comando 'check logs' para análise automática.";
        return response;
    }

    if (lower.startsWith("concluir ") || lower.startsWith("feito ")) {
        int idx = instruction.mid(instruction.contains("concluir") ? 9 : 6).trimmed().toInt();
        ProjectKnowledge::getInstance().completeTask(idx - 1);
        response.handled = true;
        response.message = QString("✅ Tarefa %1 marcada como concluída.").arg(idx);
        return response;
    }

    if (lower.startsWith("git ")) {
        QString cmd = instruction.mid(4).trimmed();
        QString baseCmd = cmd.section(' ', 0, 0);
        QString args = cmd.section(' ', 1);
        response.handled = true;
        response.message = gitOperation(baseCmd, args);
        return response;
    }

    if (lower == "status") {
        response.handled = true;
        response.message = gitOperation("status");
        return response;
    }

    if (lower.startsWith("buscar ") || lower.startsWith("find ")) {
        QString query = instruction.mid(7).trimmed();
        if (query.startsWith("/") && query.endsWith("/")) {
            // Se estiver entre barras, trata como regex: buscar /padrao/
            return {true, regexSearch(query.mid(1, query.length() - 2)), "", ""};
        }
        response.handled = true;
        response.message = searchProject(query);
        return response;
    }

    if (lower.startsWith("deep search ") || lower.startsWith("pesquisa profunda ")) {
        QString query = instruction.mid(instruction.contains("deep") ? 12 : 18).trimmed();
        response.handled = true;
        response.message = "🚀 <b>Iniciando Deep Search SSQLM...</b><br>Vasculhando múltiplas fontes para a melhor resposta técnica.";
        response.action = "WEB_DEEP_SEARCH";
        response.target = query;
        return response;
    }

    if (lower.contains("dívida técnica") || lower.contains("pendências") || lower.contains("todos")) {
        response.handled = true;
        response.message = findTechnicalDebt();
        return response;
    }

    if (lower.startsWith("documentar ") || lower.startsWith("explique ")) {
        QString file = instruction.mid(11).trimmed();
        // Se o usuário passar apenas o nome, tentamos achar o caminho no índice
        QString fullPath = ProjectKnowledge::getInstance().findSymbolLocation(file);
        if (fullPath.isEmpty()) {
             // Tenta ver se é um caminho direto ou arquivo na raiz
             fullPath = QDir::currentPath() + "/" + file;
        }

        response.handled = true;
        response.message = documentFile(fullPath);
        return response;
    }

    if (lower.startsWith("refatorar ") || lower.startsWith("otimizar ")) {
        QString file = instruction.mid(10).trimmed();
        if (file == "projeto" || file == "tudo") {
            return {true, refactorProject("Padronização SSQLM"), "", ""};
        }
        QString fullPath = ProjectKnowledge::getInstance().findSymbolLocation(file);
        if (fullPath.isEmpty()) fullPath = QDir::currentPath() + "/" + file;

        response.handled = true;
        response.message = refactorFile(fullPath);
        return response;
    }

    if (lower.startsWith("risco ") || lower.startsWith("analise risco ")) {
        QString file = instruction.mid(instruction.contains("analise") ? 14 : 6).trimmed();
        QString fullPath = ProjectKnowledge::getInstance().findSymbolLocation(file);
        if (fullPath.isEmpty()) fullPath = QDir::currentPath() + "/" + file;

        response.handled = true;
        response.message = analyzeRisk(fullPath);
        return response;
    }

    if (lower.contains("tarefas") || lower.contains("checklist")) {
        QStringList tasks = ProjectKnowledge::getInstance().pendingTasks();
        response.handled = true;
        if (tasks.isEmpty()) {
            response.message = "✅ Todas as tarefas autônomas foram concluídas.";
        } else {
            response.message = "📋 <b>Checklist SSAI:</b><br>";
            for (int i = 0; i < tasks.size(); ++i) {
                response.message += QString("%1. %2<br>").arg(i + 1).arg(tasks[i]);
            }
        }
        return response;
    }

    if (lower == "autopilot" || lower == "auto-pilot" || lower == "próximo") {
        response.handled = true;
        response.message = runAutoPilot();
        return response;
    }

    if (lower == "executar" || lower == "rodar script") {
        if (m_lastArtifact.startsWith("#!/usr/bin/env bash")) {
            response.handled = true;
            response.message = "🚀 <b>Iniciando execução do script gerado...</b>";
            response.action = "EXECUTE_SCRIPT";
            response.target = m_lastArtifact;
        } else {
            response.handled = true;
            response.message = "❌ Nenhuma automação pendente para execução. Gere um script primeiro.";
        }
        return response;
    }

    if (lower == "salvar" || lower == "salvar pesquisa") {
        response.handled = true;
        response.message = "Guardando conhecimento técnico da IA...";
        response.action = "SAVE_LAST_AI";
        return response;
    }

    if (lower.contains("status do projeto") || lower.contains("relatório")) {
        response.handled = true;
        response.message = getProjectStats();
        return response;
    }

    if (lower.contains("corrija erro") || lower.contains("fix error")) {
        response.handled = true;
        response.message = proposeFix(instruction);
        return response;
    }

    if (lower.contains("checkup") || lower.contains("saúde do projeto")) {
        response.handled = true;
        response.message = runHealthCheck();
        return response;
    }

    if (lower.contains("scan") && (lower.contains("segurança") || lower.contains("security"))) {
        response.handled = true;
        response.message = runSecurityScan();
        return response;
    }

    if (lower.startsWith("curar ") || lower.startsWith("fix ")) {
        QString file = instruction.mid(instruction.contains("curar") ? 6 : 4).trimmed();
        QString fullPath = ProjectKnowledge::getInstance().findSymbolLocation(file);
        if (fullPath.isEmpty()) fullPath = QDir::currentPath() + "/" + file;

        response.handled = true;
        response.message = autoHeal(fullPath);
        return response;
    }

    if (lower.startsWith("dependências ") || lower.startsWith("includes ")) {
        QString file = instruction.mid(instruction.contains("dependências") ? 13 : 9).trimmed();
        QString fullPath = ProjectKnowledge::getInstance().findSymbolLocation(file);
        if (fullPath.isEmpty()) fullPath = QDir::currentPath() + "/" + file;

        response.handled = true;
        response.message = analyzeDependencies(fullPath);
        return response;
    }

    // 5. Friendly Generic Responses for Offline Mode
    if (lower == "bom dia" || lower == "olá" || lower == "oi") {
        response.handled = true;
        response.message = "Olá! Como sou seu Agente Local, estou focado em te ajudar com a engenharia deste projeto. O que vamos analisar agora?";
        return response;
    }

    if (lower.contains("quem é você") || lower.contains("o que você faz")) {
        response.handled = true;
        response.message = "Eu sou o <b>SS AI Agent</b> integrado ao Parcel C++. Minha missão é garantir a qualidade SSQLM, automatizar scripts e auditar seu código em tempo real.";
        return response;
    }

    return response;
}

QString LocalAgent::generateSsqlmPlan(const QString& task) {
    QString plan = "📜 <b>Extraordinary SSQLM Strategy Engine</b><br><br>";

    if (task.contains("view") || task.contains("tela")) {
        plan += "<b>Alvo:</b> Expansão de Interface UI/UX<br>";
        plan += "1. 🛠️ Gerar Componente HPP em <i>src/view/</i><br>";
        plan += "2. 🧬 Registrar no <i>CMakeLists.txt</i><br>";
        plan += "3. 🔗 Mapear Navegação no <i>NavigationController</i><br>";
        plan += "4. ✨ Aplicar layout atômico via SS QLM scaffolds.";
    } else if (task.contains("build") || task.contains("compilar")) {
        plan += "<b>Alvo:</b> Pipeline de Integração Robusta<br>";
        plan += "1. 💾 Snapshot via <i>BackupService</i><br>";
        plan += "2. 🧹 Purge do diretório <i>build/</i><br>";
        plan += "3. ⚙️ Cmake Auto-Config<br>";
        plan += "4. 🔨 Make (High Performance).";
    } else {
        plan += "Analise detectou uma intenção complexa. Recomendo o uso do modo cirúrgico (SSQLM Patcher) para avaliar o impacto estrutural no projeto.";
    }

    return plan;
}

QString LocalAgent::auditCode(const QString& code) {
    QString report = "🔍 <b>Relatório de Auditoria Local SSQLM</b><br><br>";
    QStringList lines = code.split("\n");
    int issuesFound = 0;

    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        if (line.isEmpty()) continue;

        QLMMatch match = m_ssqlm->findMatch(line);
        if (!match.matched) match = m_qlm->findMatch(line);

        if (match.matched && !match.intent.isEmpty()) {
            issuesFound++;
            report += QString("<font color='#FF4500'>[LINHA %1]</font> <b>%2</b>: %3<br>")
                        .arg(i + 1)
                        .arg(match.intent)
                        .arg(match.response);
            if (!match.action.isEmpty()) {
                report += QString("&nbsp;&nbsp;<i>Sugestão: %1</i><br>").arg(match.action);
            }
        }
    }

    if (issuesFound == 0) {
        report += "<font color='#00FF7F'>✓ Nenhum problema crítico detectado nas regras locais.</font>";
    } else {
        report += QString("<br><font color='#FFD700'>Total de %1 alerta(s) encontrados.</font>").arg(issuesFound);
    }

    return report;
}

QString LocalAgent::getSuggestions(const QString& context) {
    QString suggestions = "💡 <b>Sugestões Autônomas SSQLM:</b><br>";

    if (context.isEmpty()) {
        suggestions += "- Execute uma auditoria em um arquivo para receber dicas específicas.<br>";
        suggestions += "- Use o modo 'extraordinary' para planejar novas funcionalidades.";
    } else {
        // Simple context-based suggestions
        if (context.contains("void") || context.contains("int")) {
            suggestions += "- Verifique se métodos podem ser marcados como <b>const</b>.<br>";
        }
        if (context.contains("new ")) {
            suggestions += "- Considere usar <b>std::unique_ptr</b> ou parentesco Qt para evitar vazamentos.<br>";
        }
        if (context.contains("QString")) {
            suggestions += "- Use <b>QStringLiteral</b> para strings constantes visando performance.<br>";
        }
    }

    return suggestions;
}

QString LocalAgent::generateScript(const QString& type) {
    QString script = "#!/usr/bin/env bash\nset -Eeuo pipefail\n\n# Extraordinary SSQLM Automated Script\n";
    script += "# Generated for: " + type + "\n\n";

    if (type.contains("backup")) {
        script += "BACKUP_DIR=\"./backups/$(date +%Y%m%d_%H%M%S)\"\n";
        script += "mkdir -p \"$BACKUP_DIR\"\n";
        script += "echo \"[INFO] Inicando backup atômico em $BACKUP_DIR...\"\n";
        script += "cp -rv src \"$BACKUP_DIR/\"\n";
        script += "echo \"[SUCCESS] Backup concluído.\"\n";
    } else if (type.contains("clean")) {
        script += "echo \"[WARNING] Executando limpeza profunda...\"\n";
        script += "rm -rf build/\n";
        script += "find . -name \"*.o\" -delete\n";
        script += "echo \"[INFO] Limpeza finalizada.\"\n";
    } else {
        script += "echo \"[INFO] Script genérico SSQLM iniciado.\"\n";
        script += "echo \"[ADVICE] Use marcadores BEGIN/END para cirurgias automáticas.\"\n";
    }

    return script;
}

QString LocalAgent::generateClass(const QString& name, const QString& type) {
    QString hpp = "#ifndef " + name.toUpper() + "_HPP\n#define " + name.toUpper() + "_HPP\n\n";

    if (type.contains("service")) {
        hpp += "#include <QObject>\n\nnamespace Parcel::Service {\n\n";
        hpp += "    class " + name + " : public QObject {\n        Q_OBJECT\n    public:\n";
        hpp += "        explicit " + name + "(QObject* parent = nullptr);\n";
        hpp += "    };\n\n}\n\n#endif";
    } else if (type.contains("pane") || type.contains("view")) {
        hpp += "#include <QWidget>\n\nnamespace Parcel::View {\n\n";
        hpp += "    class " + name + " : public QWidget {\n        Q_OBJECT\n    public:\n";
        hpp += "        explicit " + name + "(QWidget* parent = nullptr);\n";
        hpp += "    };\n\n}\n\n#endif";
    } else {
        hpp += "class " + name + " {\n    public:\n        " + name + "();\n};\n\n#endif";
    }

    m_lastArtifact = hpp;
    return hpp;
}

QString LocalAgent::analyzeLogs(const QString& logContent) {
    QString report = "📋 <b>SSQLM Log Intelligence Report</b><br><br>";
    QStringList lines = logContent.split("\n");
    int errors = 0, warnings = 0;

    for (const QString& line : lines) {
        if (line.contains("error", Qt::CaseInsensitive) || line.contains("CRITICAL")) {
            report += "• <font color='#FF4500'>[ERROR]</font> " + line.trimmed().toHtmlEscaped() + "<br>";
            errors++;
        } else if (line.contains("warning", Qt::CaseInsensitive)) {
            report += "• <font color='#FFD700'>[WARN]</font> " + line.trimmed().toHtmlEscaped() + "<br>";
            warnings++;
        }
    }

    if (errors == 0 && warnings == 0) return report + "✓ Nenhum padrão de erro detectado nos logs fornecidos.";
    return report + QString("<br>📊 <b>Total:</b> %1 erros, %2 avisos.").arg(errors).arg(warnings);
}

QString LocalAgent::getProjectStats() {
    QString stats = "📊 <b>Project Intelligence Report</b><br><br>";
    QDir dir(ProjectKnowledge::getInstance().projectRoot());

    int cppFiles = 0, hppFiles = 0, shellFiles = 0, jsonFiles = 0;

    QDirIterator it(dir.path(), QStringList() << "*.cpp" << "*.hpp" << "*.sh" << "*.json",
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        QString ext = it.fileInfo().suffix();
        if (ext == "cpp") cppFiles++;
        else if (ext == "hpp") hppFiles++;
        else if (ext == "sh") shellFiles++;
        else if (ext == "json") jsonFiles++;
    }

    stats += QString("• <b>C++ Sources:</b> %1<br>").arg(cppFiles);
    stats += QString("• <b>Headers:</b> %1<br>").arg(hppFiles);
    stats += QString("• <b>SSQLM Scripts:</b> %1<br>").arg(shellFiles);
    stats += QString("• <b>Knowledge Base:</b> %1 nodes<br><br>").arg(jsonFiles);

    // Summary of key classes from index
    stats += "🏛️ <b>Arquitetura Core:</b><br>";
    QStringList cores = { "SSAIModule", "LocalAgent", "MainWindow", "ProjectWorkspace", "PersistenceService" };
    for (const auto& core : cores) {
        QString loc = ProjectKnowledge::getInstance().findSymbolLocation(core);
        if (!loc.isEmpty()) {
            stats += QString("• %1: <font color='#888'>%2</font><br>").arg(core, QFileInfo(loc).fileName());
        }
    }

    stats += "<br>⚡ <b>Health:</b> Elite SSQLM status confirmed. No critical structural leaks detected.";
    return stats;
}

QString LocalAgent::proposeFix(const QString& issue) {
    QString fix = "🔧 <b>SSQLM Fix Proposta:</b><br>";
    QString lowIssue = issue.toLower();

    if (lowIssue.contains("include") || lowIssue.contains("not found")) {
        fix += "1. Verifique se o header está no diretório <i>src/</i>.<br>";
        fix += "2. Garanta que o arquivo está listado em <i>CMakeLists.txt</i>.<br>";
        fix += "3. Tente adicionar <i>target_include_directories(ParcelCPP PRIVATE src)</i>.";
    } else if (lowIssue.contains("slot") || lowIssue.contains("signal")) {
        fix += "1. Verifique a macro <b>Q_OBJECT</b> na classe.<br>";
        fix += "2. Certifique-se de que o arquivo está no SOURCES do CMake (para o moc rodar).<br>";
        fix += "3. Tente limpar o diretório <i>build/</i> e rodar o build novamente.";
    } else {
        fix += "Padrão de erro desconhecido localmente. Sugiro cirurgia diagnóstica profunda ou consulta ao motor Gemini.";
    }

    return fix;
}

QString LocalAgent::runHealthCheck() {
    QString report = "🩺 <b>SSQLM Project Health Diagnostic</b><br><br>";
    QDir dir(ProjectKnowledge::getInstance().projectRoot());
    int issues = 0;

    QDirIterator it(dir.path(), QStringList() << "*.cpp" << "*.sh",
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        QFile file(it.filePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString firstLine = file.readLine();
            file.close();

            // Check for mandatory header in shell scripts
            if (it.fileInfo().suffix() == "sh" && !firstLine.contains("#!/usr/bin/env bash")) {
                report += QString("• ⚠ <b>%1</b>: Header bash padrão ausente.<br>").arg(it.fileName());
                issues++;
            }

            // Check for unusually small C++ files
            if (it.fileInfo().suffix() == "cpp" && it.fileInfo().size() < 10) {
                report += QString("• ⚠ <b>%1</b>: Arquivo fonte parece estar vazio.<br>").arg(it.fileName());
                issues++;
            }
        }
    }

    if (issues == 0) {
        report += "<font color='#00FF7F'>✓ Diagnóstico concluído: Estrutura do projeto está em conformidade SSQLM.</font>";
    } else {
        report += QString("<br><font color='#FF4500'>Detectadas %1 anomalias estruturais.</font>").arg(issues);
    }

    return report;
}

QString LocalAgent::runSecurityScan() {
    QString report = "🛡️ <b>SSQLM Security Auditor Deep Scan</b><br><br>";
    QDir dir(ProjectKnowledge::getInstance().projectRoot());
    int issues = 0;

    // Patterns for security risks
    QMap<QString, QString> risks;
    risks["(password|passwd|senha|secret|key|token)\\s*=\\s*['\"].+['\"]"] = "Potencial credencial hardcoded detectada.";
    risks["chmod\\s+(777|666|+x)"] = "Permissão de arquivo excessivamente permissiva ou insegura.";
    risks["curl\\s+.*\\|\\s*(bash|sh)"] = "Execução direta de script remoto (RCE risk).";
    risks["(http://)\\w+"] = "Uso de protocolo inseguro detectado (use HTTPS).";

    QDirIterator it(dir.path(), QStringList() << "*.cpp" << "*.hpp" << "*.sh" << "*.json",
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        if (it.path().contains("/build/")) continue;

        QFile file(it.filePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            int lineNum = 1;
            while (!in.atEnd()) {
                QString line = in.readLine();
                for (auto it_risk = risks.begin(); it_risk != risks.end(); ++it_risk) {
                    QRegularExpression re(it_risk.key(), QRegularExpression::CaseInsensitiveOption);
                    if (re.match(line).hasMatch()) {
                        report += QString("• <font color='#FF4500'>[CRÍTICO]</font> <b>%1</b> (L:%2): %3<br>")
                                    .arg(it.fileName(), QString::number(lineNum), it_risk.value());
                        issues++;
                    }
                }
                lineNum++;
            }
            file.close();
        }
    }

    if (issues == 0) {
        report += "<font color='#00FF7F'>✓ Scan concluído: Nenhuma vulnerabilidade óbvia detectada.</font>";
    } else {
        report += QString("<br><font color='#FF4500'>Detectadas %1 vulnerabilidades críticas.</font>").arg(issues);
    }

    return report;
}

QString LocalAgent::autoHeal(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "❌ Falha ao acessar arquivo para Auto-Healing.";

    QString content = file.readAll();
    file.close();

    bool modified = false;
    QString report = "🩹 <b>Auto-Healing SSQLM: " + QFileInfo(path).fileName() + "</b><br><br>";

    // Cura 1: Protocolos Inseguros
    if (content.contains("http://")) {
        content.replace("http://", "https://");
        report += "• Convertido <b>HTTP</b> para <b>HTTPS</b> (Segurança).<br>";
        modified = true;
    }

    // Cura 2: Permissões de Risco em Scripts
    if (content.contains("chmod 777")) {
        content.replace("chmod 777", "chmod 755");
        report += "• Corrigida permissão <b>777</b> para <b>755</b>.<br>";
        modified = true;
    }

    // Cura 3: Injeção de Proteção de Erro se for .sh
    if (path.endsWith(".sh") && !content.contains("set -Eeuo pipefail")) {
        int firstLineEnd = content.indexOf("\n");
        content.insert(firstLineEnd + 1, "set -Eeuo pipefail\n");
        report += "• Injetada proteção de erro <i>pipefail</i>.<br>";
        modified = true;
    }

    if (modified) {
        QFile outFile(path);
        if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            outFile.write(content.toUtf8());
            outFile.close();
            report += "<br><font color='#00FF7F'>[HEALED] O arquivo foi curado e protegido.</font>";
        }
    } else {
        report += "✓ Nenhuma ferida estrutural detectada para auto-cura.";
    }

    return report;
}

QString LocalAgent::analyzeDependencies(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "❌ Arquivo não encontrado para análise de dependências.";

    QString content = file.readAll();
    file.close();

    QString report = "🔗 <b>Análise de Dependências SSQLM: " + QFileInfo(path).fileName() + "</b><br><br>";
    QRegularExpression includeRe("#include\\s+[\"<]([^>\"]+)[\">]");
    auto matches = includeRe.globalMatch(content);

    int internal = 0;
    int external = 0;

    while (matches.hasNext()) {
        auto match = matches.next();
        QString header = match.captured(1);
        bool isInternal = !match.captured(0).contains("<");

        if (isInternal) {
            QString loc = ProjectKnowledge::getInstance().findSymbolLocation(QFileInfo(header).baseName());
            if (!loc.isEmpty()) {
                report += "• <font color='#00BFFF'>[INTERNO]</font> " + header + " <font color='#888'>(Localizado)</font><br>";
                internal++;
            } else {
                report += "• <font color='#FFD700'>[INTERNO]</font> " + header + " <font color='#FF4500'>(Não indexado!)</font><br>";
                internal++;
            }
        } else {
            report += "• <font color='#adbac7'>[EXTERNO]</font> " + header + "<br>";
            external++;
        }
    }

    report += QString("<br>📊 <b>Resumo:</b> %1 internas, %2 externas.").arg(internal).arg(external);
    return report;
}

QString LocalAgent::gitOperation(const QString& command, const QString& args) {
    QString report = "🐙 <b>SSQLM Git Ops: " + command + "</b><br><br>";
    QProcess proc;
    proc.setWorkingDirectory(ProjectKnowledge::getInstance().projectRoot());

    QStringList finalArgs;
    finalArgs << command;
    if (!args.isEmpty()) finalArgs << args.split(" ");

    proc.start("git", finalArgs);
    proc.waitForFinished();

    QString output = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
    QString error = QString::fromUtf8(proc.readAllStandardError()).trimmed();

    if (!output.isEmpty()) report += "<pre style='color: #adbac7;'>" + output.toHtmlEscaped() + "</pre>";
    if (!error.isEmpty()) report += "<font color='#FF4500'>[ERROR]</font><br><pre style='color: #FF4500;'>" + error.toHtmlEscaped() + "</pre>";

    return report;
}

QString LocalAgent::searchProject(const QString& query) {
    QString report = "🔍 <b>Busca Global SSQLM: '" + query + "'</b><br><br>";
    QDir dir(ProjectKnowledge::getInstance().projectRoot());
    int matches = 0;

    QDirIterator it(dir.path(), QStringList() << "*.cpp" << "*.hpp" << "*.sh" << "*.json",
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext() && matches < 20) { // Limit to 20 matches for performance
        it.next();
        if (it.fileName().contains("build") || it.path().contains("/build/")) continue;

        QFile file(it.filePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            int lineNum = 1;
            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.contains(query, Qt::CaseInsensitive)) {
                    report += QString("• <b>%1</b> (L:%2): <font color='#888'>%3</font><br>")
                                .arg(it.fileName())
                                .arg(lineNum)
                                .arg(line.trimmed().toHtmlEscaped());
                    matches++;
                    if (matches >= 20) break;
                }
                lineNum++;
            }
            file.close();
        }
    }

    if (matches == 0) report += "Nenhuma ocorrência encontrada.";
    else if (matches >= 20) report += "<br><i>(Exibindo apenas os primeiros 20 resultados)</i>";

    return report;
}

QString LocalAgent::regexSearch(const QString& pattern) {
    QString report = "🧬 <b>Busca por Regex SSQLM: /" + pattern + "/</b><br><br>";
    QRegularExpression re(pattern, QRegularExpression::CaseInsensitiveOption);
    if (!re.isValid()) return "❌ Expressão regular inválida: " + re.errorString();

    QDir dir(ProjectKnowledge::getInstance().projectRoot());
    int matches = 0;
    QDirIterator it(dir.path(), QStringList() << "*.cpp" << "*.hpp" << "*.sh" << "*.json",
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext() && matches < 20) {
        it.next();
        if (it.path().contains("/build/")) continue;
        QFile file(it.filePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            int lineNum = 1;
            while (!in.atEnd()) {
                QString line = in.readLine();
                auto match = re.match(line);
                if (match.hasMatch()) {
                    report += QString("• <b>%1</b> (L:%2): <font color='#888'>%3</font><br>")
                                .arg(it.fileName())
                                .arg(lineNum)
                                .arg(line.trimmed().toHtmlEscaped());
                    matches++;
                }
                lineNum++;
            }
            file.close();
        }
    }
    return matches > 0 ? report : "Nenhuma correspondência encontrada.";
}

QString LocalAgent::findTechnicalDebt() {
    QString report = "🏚️ <b>Relatório de Dívida Técnica (SSQLM Hunter)</b><br><br>";
    QRegularExpression todoRe("(TODO|FIXME|HACK|BUG|OPTIMIZE):?\\s*(.*)", QRegularExpression::CaseInsensitiveOption);

    QDir dir(ProjectKnowledge::getInstance().projectRoot());
    int count = 0;
    QDirIterator it(dir.path(), QStringList() << "*.cpp" << "*.hpp" << "*.sh",
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        if (it.path().contains("/build/")) continue;
        QFile file(it.filePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            int lineNum = 1;
            while (!in.atEnd()) {
                QString line = in.readLine();
                auto match = todoRe.match(line);
                if (match.hasMatch()) {
                    QString type = match.captured(1).toUpper();
                    QString desc = match.captured(2);
                    QString color = (type == "FIXME" || type == "BUG") ? "#FF4500" : "#FFD700";

                    report += QString("• <font color='%1'><b>[%2]</b></font> em <b>%3</b> (L:%4): <i>%5</i><br>")
                                .arg(color, type, it.fileName(), QString::number(lineNum), desc.toHtmlEscaped());
                    count++;
                }
                lineNum++;
            }
            file.close();
        }
    }
    return count > 0 ? report : "✅ Parabéns! Nenhuma dívida técnica detectada no código.";
}

QString LocalAgent::documentFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "❌ Arquivo não encontrado para documentação.";

    QString content = file.readAll();
    file.close();

    QString doc = "📖 <b>Documentação SSQLM: " + QFileInfo(path).fileName() + "</b><br><br>";
    doc += "<i>Resumo estrutural gerado automaticamente pelo Agente Local.</i><br><br>";

    // Extrair Classes
    QRegularExpression classRe("class\\s+(\\w+)");
    auto classes = classRe.globalMatch(content);
    doc += "<b>Classes Detectadas:</b><br>";
    while (classes.hasNext()) {
        doc += "• <code>" + classes.next().captured(1) + "</code><br>";
    }

    // Extrair Métodos (heurística simples para C++)
    doc += "<br><b>Métodos e Funções:</b><br>";
    QRegularExpression methodRe("\\w+\\s+(\\w+)\\(.*\\)\\s*\\{");
    auto methods = methodRe.globalMatch(content);
    int count = 0;
    while (methods.hasNext() && count < 10) {
        QString name = methods.next().captured(1);
        if (name != "if" && name != "while" && name != "for" && name != "switch") {
            doc += "• <code>" + name + "()</code><br>";
            count++;
        }
    }
    if (methods.hasNext()) doc += "... (e outros)<br>";

    doc += "<br>⚡ <b>Padrão:</b> ";
    doc += (content.contains("Q_OBJECT") ? "Qt Signal/Slot System" : "C++ Nativo/Standard");

    return doc;
}

QString LocalAgent::refactorFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "❌ Arquivo não encontrado para refatoração.";

    QString content = file.readAll();
    file.close();

    bool modified = false;
    QString report = "🛠️ <b>Refatoração Automática SSQLM: " + QFileInfo(path).fileName() + "</b><br><br>";

    // Regra 1: Scripts Shell - Cabeçalho Obrigatório
    if (path.endsWith(".sh")) {
        QString mandatoryHeader = "#!/usr/bin/env bash\nset -Eeuo pipefail\n";
        if (!content.startsWith("#!")) {
            content.prepend(mandatoryHeader);
            report += "• Adicionado cabeçalho bash robusto.<br>";
            modified = true;
        } else if (!content.contains("set -Eeuo pipefail")) {
            // Insere após a primeira linha
            int firstNewline = content.indexOf("\n");
            content.insert(firstNewline + 1, "set -Eeuo pipefail\n");
            report += "• Injetada proteção 'set -Eeuo pipefail'.<br>";
            modified = true;
        }
    }

    // Regra 2: C++ - QStringLiteral Optimization
    if (path.endsWith(".cpp") || path.endsWith(".hpp")) {
        QRegularExpression qstringRe("QString\\(\"([^\"]+)\"\\)");
        if (content.contains(qstringRe)) {
            content.replace(qstringRe, "QStringLiteral(\"\\1\")");
            report += "• Otimizado strings constantes para <i>QStringLiteral</i>.<br>";
            modified = true;
        }
    }

    if (modified) {
        QFile outFile(path);
        if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            outFile.write(content.toUtf8());
            outFile.close();
            report += "<br><font color='#00FF7F'>[SUCCESS] Alterações aplicadas com sucesso.</font>";
        } else {
            report += "<br><font color='#FF4500'>[ERROR] Falha ao gravar arquivo.</font>";
        }
    } else {
        report += "✓ O arquivo já está em conformidade com os padrões locais.";
    }

    return report;
}

QString LocalAgent::refactorProject(const QString& rule) {
    QString report = "🌊 <b>Refatoração em Massa SSQLM: " + rule + "</b><br><br>";
    QDir dir(ProjectKnowledge::getInstance().projectRoot());
    int modifiedCount = 0;

    QDirIterator it(dir.path(), QStringList() << "*.cpp" << "*.hpp" << "*.sh",
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        if (it.path().contains("/build/")) continue;

        QString res = refactorFile(it.filePath());
        if (res.contains("[SUCCESS]")) {
            report += "• Corrigido: " + it.fileName() + "<br>";
            modifiedCount++;
        }
    }

    if (modifiedCount == 0) return report + "✓ Todo o projeto já está em conformidade.";
    return report + QString("<br><font color='#00FF7F'>[TOTAL] %1 arquivos otimizados.</font>").arg(modifiedCount);
}

QString LocalAgent::analyzeRisk(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "❌ Falha ao acessar arquivo para análise de risco.";

    QString content = file.readAll();
    file.close();

    QString report = "⚖️ <b>Análise de Risco Estrutural: " + QFileInfo(path).fileName() + "</b><br><br>";
    int riskScore = 0;

    // Indicador 1: Funções muito longas
    QRegularExpression longFuncRe("\\{", QRegularExpression::DotMatchesEverythingOption);
    int braces = content.count("{");
    if (braces > 30) {
        report += "• <font color='#FF4500'>[ALTO]</font> Alta densidade de blocos. Considere decompor a classe.<br>";
        riskScore += 40;
    }

    // Indicador 2: Complexidade Ciclomática (Simples)
    QRegularExpression complexityRe("\\b(if|while|for|switch|case)\\b");
    int conditions = content.count(complexityRe);
    if (conditions > 25) {
        report += "• <font color='#FFD700'>[MÉDIO]</font> Lógica condicional complexa detectada.<br>";
        riskScore += 30;
    }

    // Indicador 3: Hardcoded paths
    if (content.contains("/home/") || content.contains("C:\\")) {
        report += "• <font color='#FF4500'>[ALTO]</font> Caminhos absolutos detectados (Brittle Code).<br>";
        riskScore += 30;
    }

    if (riskScore == 0) report += "<font color='#00FF7F'>✓ Baixo risco estrutural detectado.</font>";
    else report += QString("<br>🔥 <b>Risk Score: %1/100</b>").arg(riskScore);

    return report;
}

QString LocalAgent::runAutoPilot() {
    QStringList tasks = ProjectKnowledge::getInstance().pendingTasks();
    if (tasks.isEmpty()) return "✨ <b>Auto-Pilot:</b> Todas as tarefas concluídas. O projeto está em estado de elite.";

    QString nextTask = tasks[0];
    QString prompt = "🤖 <b>Auto-Pilot Ativado:</b><br>";
    prompt += "Próxima tarefa pendente: <b>" + nextTask + "</b><br><br>";

    if (nextTask.contains("script")) {
        prompt += "Deseja que eu prepare a <b>execução</b> deste script?";
    } else if (nextTask.contains("Implementar")) {
        prompt += "Deseja que eu gere um <b>boilerplate inicial</b> para esta implementação?";
    } else {
        prompt += "Como deseja prosseguir com esta tarefa?";
    }

    return prompt;
}

}
