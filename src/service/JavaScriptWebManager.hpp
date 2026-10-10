#ifndef JAVASCRIPT_WEB_MANAGER_HPP
#define JAVASCRIPT_WEB_MANAGER_HPP

#include <QString>
#include <QObject>

namespace Parcel::Service {

    class JavaScriptWebManager : public QObject {
        Q_OBJECT
    public:
        static JavaScriptWebManager& getInstance() {
            static JavaScriptWebManager instance;
            return instance;
        }

        // jQuery snippet generator
        QString generateJQueryCode() const {
            return QString(
                "<!-- jQuery 3.7 Snippet -->\n"
                "<script src=\"https://code.jquery.com/jquery-3.7.1.min.js\"></script>\n"
                "<script>\n"
                "$(document).ready(function() {\n"
                "    console.log(\"jQuery Initialized in Parcel C++\");\n"
                "    $(\"#btnSubmit\").on(\"click\", function() {\n"
                "        $.ajax({\n"
                "            url: \"/api/data\",\n"
                "            method: \"GET\",\n"
                "            success: function(response) {\n"
                "                $(\"#content\").html(response.data);\n"
                "            }\n"
                "        });\n"
                "    });\n"
                "});\n"
                "</script>\n"
            );
        }

        // Vue.js 3 Composition API snippet generator
        QString generateVueCode() const {
            return QString(
                "<!-- Vue.js 3 Composition API Component -->\n"
                "<script src=\"https://unpkg.com/vue@3/dist/vue.global.js\"></script>\n"
                "<div id=\"app\">\n"
                "    <h1>{{ title }}</h1>\n"
                "    <button @click=\"increment\">Count: {{ count }}</button>\n"
                "</div>\n"
                "<script>\n"
                "const { createApp, ref } = Vue;\n"
                "createApp({\n"
                "    setup() {\n"
                "        const title = ref(\"Parcel C++ - Vue 3 Studio\");\n"
                "        const count = ref(0);\n"
                "        const increment = () => count.value++;\n"
                "        return { title, count, increment };\n"
                "    }\n"
                "}).mount('#app');\n"
                "</script>\n"
            );
        }

        // Angular Standalone Component generator
        QString generateAngularCode() const {
            return QString(
                "// Angular 17+ Standalone Component\n"
                "import { Component, signal } from '@angular/core';\n"
                "import { CommonModule } from '@angular/common';\n\n"
                "@Component({\n"
                "  selector: 'app-parcel-root',\n"
                "  standalone: true,\n"
                "  imports: [CommonModule],\n"
                "  template: `\n"
                "    <div class=\"angular-container\">\n"
                "      <h2>{{ title() }}</h2>\n"
                "      <button (click)=\"counter.set(counter() + 1)\">Clicks: {{ counter() }}</button>\n"
                "    </div>\n"
                "  `\n"
                "})\n"
                "export class AppComponent {\n"
                "  title = signal('Parcel C++ - Angular Studio');\n"
                "  counter = signal(0);\n"
                "}\n"
            );
        }

        // React 18 Functional Component generator
        QString generateReactCode() const {
            return QString(
                "// React 18 Functional Component with Hooks\n"
                "import React, { useState, useEffect } from 'react';\n\n"
                "export const ParcelReactComponent = () => {\n"
                "    const [count, setCount] = useState(0);\n"
                "    const [data, setData] = useState(null);\n\n"
                "    useEffect(() => {\n"
                "        console.log(\"React Component Mounted in Parcel C++\");\n"
                "    }, []);\n\n"
                "    return (\n"
                "        <div className=\"react-card\">\n"
                "            <h3>Parcel React 18 Studio</h3>\n"
                "            <p>Count: {count}</p>\n"
                "            <button onClick={() => setCount(count + 1)}>Increment</button>\n"
                "        </div>\n"
                "    );\n"
                "};\n"
            );
        }

    private:
        JavaScriptWebManager() = default;
        ~JavaScriptWebManager() = default;
        JavaScriptWebManager(const JavaScriptWebManager&) = delete;
        JavaScriptWebManager& operator=(const JavaScriptWebManager&) = delete;
    };

}

#endif
