# Visual Designer: Component Selector ComboBox Documentation

## 📋 Overview
The **Visual Designer** module within **Parcel C++** (`[DesignerPane.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/view/editor/DesignerPane.hpp)`) now features a dynamic **Component Selector ComboBox**. This feature enhances user workflow by allowing developers to easily inspect, select, and navigate between elements placed on the visual design canvas.

---

## ✨ Key Features & Behavior

1. **Conditional Visibility:**
   - The ComboBox and its label (`🎯 Selecionar Componente`) remain completely hidden when the design canvas has no components.
   - They automatically appear as soon as the first component is added to the workspace.

2. **Real-time Synchronization:**
   - Automatically populates with items reflecting all placed components in the format `[Index] ID (Type)` (e.g., `[0] button_0 (Button)`).
   - Stays synchronized with undo/redo operations, deletions, and additions.

3. **Bidirectional Selection:**
   - **From ComboBox to Canvas:** Selecting an item in the dropdown highlights and selects the corresponding element on the canvas and loads its properties into the property inspector panel.
   - **From Canvas to ComboBox:** Clicking an element directly on the canvas updates the ComboBox selection to match.

---

## 🛠️ Implementation Details
- Implemented in `Parcel::View::DesignerPane` (`[DesignerPane.hpp](file:///home/marcel/Parcel-Suite/Parcel%20C++/src/view/editor/DesignerPane.hpp)`).
- Listens to `DesignerModel::modelUpdated` signals to refresh item lists and visibility states.
- Uses `currentIndexChanged` signals connected to `DesignerModel::selectElement()` for seamless cross-navigation.
