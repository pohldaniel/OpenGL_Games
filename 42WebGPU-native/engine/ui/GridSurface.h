class GridSurface : public Surface {
private:
    int m_cols = 5;
    int m_rows = 5;
    float m_slotSize = 64.0f;
    float m_spacing = 8.0f;

    // Ein 2D-Array oder Vektor, um zu tracken, welche Slots belegt sind
    std::vector<std::vector<Widget*>> m_slots;

public:
    GridSurface(int cols, int rows) : m_cols(cols), m_rows(rows) {
        m_slots.resize(cols, std::vector<Widget*>(rows, nullptr));
    }

    // Wandelt eine Mausposition in einen Grid-Slot um
    bool getSlotFromMouse(int mouseX, int mouseY, int& outX, int& outY) {
        Vector2f localPos = Vector2f(mouseX, mouseY) - getWorldPosition();
        
        // Padding/Offset abziehen falls vorhanden
        int col = static_cast<int>(localPos[0] / (m_slotSize + m_spacing));
        int row = static_cast<int>(localPos[1] / (m_slotSize + m_spacing));

        if (col >= 0 && col < m_cols && row >= 0 && row < m_rows) {
            outX = col;
            outY = row;
            return true;
        }
        return false;
    }

    // Platziert ein Item auf einen Slot und berechnet dessen lokale Position im UI-Baum
    bool tryPlaceItem(Widget* item, int slotX, int slotY) {
        if (m_slots[slotX][slotY] != nullptr) return false; // Slot besetzt!

        m_slots[slotX][slotY] = item;
        
        // Lokale Position für das Snapping berechnen
        float localX = slotX * (m_slotSize + m_spacing);
        float localY = slotY * (m_slotSize + m_spacing);
        item->setPosition(Vector2f(localX, localY));
        
        static_cast<Item*>(item)->setGridPosition(slotX, slotY);
        return true;
    }
    
    void removeItem(Widget* item) {
        for (int x = 0; x < m_cols; ++x) {
            for (int y = 0; y < m_rows; ++y) {
                if (m_slots[x][y] == item) m_slots[x][y] = nullptr;
            }
        }
    }
};