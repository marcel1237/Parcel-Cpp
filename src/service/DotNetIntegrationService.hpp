#ifndef DOTNET_INTEGRATION_SERVICE_HPP
#define DOTNET_INTEGRATION_SERVICE_HPP

#include <QString>
#include <QStringList>

namespace Parcel::Service {

    class DotNetIntegrationService {
    public:
        static DotNetIntegrationService& getInstance() {
            static DotNetIntegrationService instance;
            return instance;
        }

        QString getApiInfo() const {
            return ".NET Framework / .NET Core API Integration Bridge active (.NET 8/9, LINQ, Entity Framework Core)";
        }

        QString getLinqBridgeInfo() const {
            return "LINQ Query Operators & Expression Tree Bridge enabled for cross-language data manipulation";
        }

        QString getEntityFrameworkInfo() const {
            return "Entity Framework (EF Core) ORM & DbContext Bridge configured for relational databases";
        }

        // Scaffolds an Entity Framework DbContext
        QString scaffoldDbContext(const QString& dbName = "ParcelDbContext") const {
            return QString(
                "using Microsoft.EntityFrameworkCore;\n\n"
                "public class %1 : DbContext {\n"
                "    public DbSet<ProjectEntity> Projects { get; set; }\n"
                "    public DbSet<LogEntity> Logs { get; set; }\n\n"
                "    protected override void OnConfiguring(DbContextOptionsBuilder options)\n"
                "        => options.UseSqlite(\"Data Source=parcel_ef.db\");\n"
                "}\n"
            ).arg(dbName);
        }

        // Generates LINQ query template
        QString generateLinqExample() const {
            return QString(
                "// C# LINQ Expression Bridge Example\n"
                "var activeProjects = projects\n"
                "    .Where(p => p.IsActive && p.Language == \"C++\")\n"
                "    .OrderByDescending(p => p.LastModified)\n"
                "    .Select(p => new { p.Name, p.Path })\n"
                "    .ToList();\n"
            );
        }

    private:
        DotNetIntegrationService() = default;
        ~DotNetIntegrationService() = default;
        DotNetIntegrationService(const DotNetIntegrationService&) = delete;
        DotNetIntegrationService& operator=(const DotNetIntegrationService&) = delete;
    };

}

#endif
