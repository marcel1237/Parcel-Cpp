# .NET Framework, LINQ & Entity Framework Core Studio Documentation

## 📋 Overview
Parcel C++ features a fully implemented **.NET Studio Module** (`[DotNetManager.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/service/DotNetManager.hpp)` and `[DotNetPane.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/view/DotNetPane.hpp)`), matching the implementation depth of KDE and GTK 4. This module allows developers to instantly generate C# application code with LINQ expressions, Entity Framework Core `DbContext` templates, and `.csproj` files directly within the IDE.

---

## 🛠️ Implemented Components

1. **`DotNetManager` Service (`[DotNetManager.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/service/DotNetManager.hpp)`):**
   - **C# App Generator:** Generates complete C# console application templates incorporating LINQ querying (`.Where()`, `.OrderByDescending()`).
   - **EF Core DbContext Generator:** Generates Entity Framework Core `DbContext` and entity model templates configured for SQLite.
   - **Project File Generator:** Generates `.NET 8` `.csproj` project files pre-configured with NuGet package references for `Microsoft.EntityFrameworkCore.Sqlite` and `Design`.

2. **`DotNetPane` UI Module (`[DotNetPane.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/view/DotNetPane.hpp)`):**
   - An interactive IDE workspace pane featuring instant code generators, .NET SDK diagnostics, and EF Core migration tools.
