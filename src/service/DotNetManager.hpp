#ifndef DOTNET_MANAGER_HPP
#define DOTNET_MANAGER_HPP

#include <QString>
#include <QObject>

namespace Parcel::Service {

    class DotNetManager : public QObject {
        Q_OBJECT
    public:
        static DotNetManager& getInstance() {
            static DotNetManager instance;
            return instance;
        }

        // Generates complete C# Program.cs template with LINQ and EF Core DbContext
        QString generateCSharpAppCode(const QString& appName = "ParcelDotNetApp") const {
            return QString(
                "using System;\n"
                "using System.Linq;\n"
                "using System.Collections.Generic;\n"
                "using Microsoft.EntityFrameworkCore;\n\n"
                "namespace %1 {\n"
                "    public class Program {\n"
                "        public static void Main(string[] args) {\n"
                "            Console.WriteLine(\"=== %1 .NET 8 / EF Core / LINQ Console ===\");\n\n"
                "            // LINQ Demonstration\n"
                "            var numbers = new List<int> { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };\n"
                "            var evenNumbers = numbers.Where(n => n % 2 == 0).OrderByDescending(n => n).ToList();\n\n"
                "            Console.WriteLine($\"Even numbers (LINQ): {string.Join(\", \", evenNumbers)}\");\n"
                "        }\n"
                "    }\n"
                "}\n"
            ).arg(appName);
        }

        // Generates Entity Framework Core DbContext template
        QString generateEfDbContextTemplate() const {
            return QString(
                "using Microsoft.EntityFrameworkCore;\n\n"
                "public class ParcelDbContext : DbContext {\n"
                "    public DbSet<ProjectRecord> Projects { get; set; }\n\n"
                "    protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)\n"
                "    {\n"
                "        optionsBuilder.UseSqlite(\"Data Source=parcel_enterprise.db\");\n"
                "    }\n"
                "}\n\n"
                "public class ProjectRecord {\n"
                "    public int Id { get; set; }\n"
                "    public string Name { get; set; }\n"
                "    public string Language { get; set; }\n"
                "    public bool IsActive { get; set; }\n"
                "}\n"
            );
        }

        // Generates .csproj template
        QString generateCsprojTemplate() const {
            return QString(
                "<Project Sdk=\"Microsoft.NET.Sdk\">\n\n"
                "  <PropertyGroup>\n"
                "    <OutputType>Exe</OutputType>\n"
                "    <TargetFramework>net8.0</TargetFramework>\n"
                "    <ImplicitUsings>enable</ImplicitUsings>\n"
                "    <Nullable>enable</Nullable>\n"
                "  </PropertyGroup>\n\n"
                "  <ItemGroup>\n"
                "    <PackageReference Include=\"Microsoft.EntityFrameworkCore.Sqlite\" Version=\"8.0.0\" />\n"
                "    <PackageReference Include=\"Microsoft.EntityFrameworkCore.Design\" Version=\"8.0.0\">\n"
                "      <PrivateAssets>all</PrivateAssets>\n"
                "      <IncludeAssets>runtime; build; native; contentfiles; analyzers; build-transitive</IncludeAssets>\n"
                "    </PackageReference>\n"
                "  </ItemGroup>\n\n"
                "</Project>\n"
            );
        }

    private:
        DotNetManager() = default;
        ~DotNetManager() = default;
        DotNetManager(const DotNetManager&) = delete;
        DotNetManager& operator=(const DotNetManager&) = delete;
    };

}

#endif
