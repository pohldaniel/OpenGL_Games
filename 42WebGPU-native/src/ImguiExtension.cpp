#include <imgui_internal.h>
#include "ImguiExtension.h"

namespace ImGui {
	bool DragDirection(const char* label, Vector4f& direction) {
		Vector2f angles = Vector3f::Polar(Vector3f(direction)) * _180_ON_PI;
		bool changed = ImGui::DragFloat2(label, &angles[0]);
		direction = Vector4f(Vector3f::Euclidean(angles * PI_ON_180), direction[3]);
		return changed;
	}
}