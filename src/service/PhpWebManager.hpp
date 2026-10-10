#ifndef PHP_WEB_MANAGER_HPP
#define PHP_WEB_MANAGER_HPP

#include <QString>
#include <QObject>

namespace Parcel::Service {

    class PhpWebManager : public QObject {
        Q_OBJECT
    public:
        static PhpWebManager& getInstance() {
            static PhpWebManager instance;
            return instance;
        }

        // Modern PHP 8.3 Class snippet generator
        QString generateNativePhpCode() const {
            return QString(
                "<?php\n"
                "// Modern PHP 8.3 Native Class\n"
                "declare(strict_types=1);\n\n"
                "namespace Parcel\\Services;\n\n"
                "readonly class ProjectManager {\n"
                "    public function __construct(\n"
                "        private string $projectName,\n"
                "        private array $modules = []\n"
                "    ) {}\n\n"
                "    public function getSummary(): string {\n"
                "        return sprintf(\"Project: %s | Modules: %d\", $this->projectName, count($this->modules));\n"
                "    }\n"
                "}\n"
            );
        }

        // Laravel 11 Controller & Eloquent Model generator
        QString generateLaravelCode() const {
            return QString(
                "<?php\n"
                "// Laravel 11 Controller & Eloquent Model\n"
                "namespace App\\Http\\Controllers;\n\n"
                "use App\\Models\\Project;\n"
                "use Illuminate\\Http\\JsonResponse;\n"
                "use Illuminate\\Http\\Request;\n\n"
                "class ProjectController extends Controller {\n"
                "    public function index(): JsonResponse {\n"
                "        $projects = Project::where('is_active', true)\n"
                "            ->orderBy('created_at', 'desc')\n"
                "            ->get();\n"
                "        return response()->json(['data' => $projects]);\n"
                "    }\n\n"
                "    public function store(Request $request): JsonResponse {\n"
                "        $validated = $request->validate([\n"
                "            'name' => 'required|string|max:255',\n"
                "            'type' => 'required|string'\n"
                "        ]);\n"
                "        $project = Project::create($validated);\n"
                "        return response()->json($project, 201);\n"
                "    }\n"
                "}\n"
            );
        }

        // WordPress Plugin / Custom Post Type generator
        QString generateWordPressCode() const {
            return QString(
                "<?php\n"
                "/**\n"
                " * Plugin Name: Parcel C++ WordPress Integration\n"
                " * Description: Custom Post Type & REST API hooks for WordPress\n"
                " * Version: 1.0.0\n"
                " */\n\n"
                "if (!defined('ABSPATH')) exit;\n\n"
                "add_action('init', function() {\n"
                "    register_post_type('parcel_project', [\n"
                "        'labels' => [\n"
                "            'name' => __('Parcel Projects'),\n"
                "            'singular_name' => __('Parcel Project')\n"
                "        ],\n"
                "        'public' => true,\n"
                "        'has_archive' => true,\n"
                "        'supports' => ['title', 'editor', 'thumbnail', 'custom-fields'],\n"
                "        'show_in_rest' => true\n"
                "    ]);\n"
                "});\n"
            );
        }

    private:
        PhpWebManager() = default;
        ~PhpWebManager() = default;
        PhpWebManager(const PhpWebManager&) = delete;
        PhpWebManager& operator=(const PhpWebManager&) = delete;
    };

}

#endif
