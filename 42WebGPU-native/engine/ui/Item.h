class Item : public Widget {
private:
    int m_gridX = 0, m_gridY = 0; // Aktueller Slot im Grid
    bool m_isDragged = false;
    Vector2f m_dragOffset;        // Wo innerhalb des Items wurde geklickt?

public:
    void setGridPosition(int x, int y) { m_gridX = x; m_gridY = y; }
    
    bool isMouseOver(int mouseX, int mouseY) const override {
        Vector2f pos = getWorldPosition();
        return (mouseX >= pos[0] && mouseX <= pos[0] + m_width &&
                mouseY >= pos[1] && mouseY <= pos[1] + m_height);
    }

    bool inputDefault(int mouseX, int mouseY, bool buttonLeft) override {
        if (buttonLeft) {
            if (!m_isDragged) {
                m_isDragged = true;
                Widget::ActiveWidget = this; // Maus exklusiv krallen!
                m_dragOffset = Vector2f(mouseX, mouseY) - getWorldPosition();
                pushToFront(); // Item visuell ganz nach oben holen
            }
            
            // Während des Drags: Direkt der Maus folgen (absolute Position)
            // parentScale-Korrektur falls nötig analog zu deiner Surface
            setPosition(Vector2f(mouseX, mouseY) - m_dragOffset); 
        } 
        else if (m_isDragged) {
			m_isDragged = false;
			Widget::ActiveWidget = nullptr; // Fokus freigeben

			GridSurface* targetGrid = nullptr;
			int targetSlotX = -1, targetSlotY = -1;

			// Wir durchsuchen alle registrierten Widgets (oder deine Szene) nach einer GridSurface unter der Maus
			// Hier nutzen wir eine Abfrage über deine Root-Szene oder den globalen Baum:
			// Pseudocode: targetGrid = findGridAtPosition(mouseX, mouseY);

			if (targetGrid && targetGrid->getSlotFromMouse(mouseX, mouseY, targetSlotX, targetSlotY)) {
				// Altes Grid informieren, dass das Item weg ist
				if (m_parent) {
					static_cast<GridSurface*>(m_parent)->removeItem(this);
					// Aus dem alten Parent-Widget-Baum entfernen und in das neue Grid einhängen!
					// (Hier nutzt du deine addChild / removeChild Logik des UI Systems)
					
					// Reparenting:
					// targetGrid->addChildExisting(this); 
				}
				
				// Im neuen Grid einrasten lassen
				if (!targetGrid->tryPlaceItem(this, targetSlotX, targetSlotY)) {
					// Slot besetzt -> Zurück zum alten Slot/Grid (Fallback)
				}
        } else {
        // Irgendwo ins Nichts gedroppt -> Zurück auf alten Slot snappen
    }
        return true;
    }
};