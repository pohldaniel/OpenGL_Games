#include <engine/input/Mouse.h>
#include <engine/DebugRenderer.h>
#include <engine/Shader.h>

#include "NavPolygonHelper.h"
#include "Globals.h"
#include "Application.h"

NavPolygonHelper::NavPolygonHelper(const MousePicker& mousePicker, const Camera& camera) : mousePicker(mousePicker), camera(camera), m_globalUserIndex(-1), m_currentPolygon(nullptr){
	loadPolygonCache();
}

void NavPolygonHelper::draw(Shader* shader) {
	if (m_drawPolygon) {
		for (const EditPolygon& editPolygon : m_editPolygons) {

			for (int i = editPolygon.userPointerOffset, j = 0; i < editPolygon.userPointerOffset + editPolygon.size; i++, j++) {
				shader->loadMatrix("u_model", Matrix4f::Translate(m_edgePoints[i]) * Matrix4f::Scale(m_markerSize, m_markerSize, m_markerSize));
				shader->loadVector("u_color", i == m_globalUserIndex ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : Vector4f::ONE);
				Globals::shapeManager.get("sphere").drawRaw();
				if (editPolygon.size > 1 && i < editPolygon.userPointerOffset + editPolygon.size - 1) {
					DebugRenderer::Get().AddLine(m_edgePoints[i] + Vector3f(0.0f, 1.0f, 0.0f), m_edgePoints[i + 1] + Vector3f(0.0f, 1.0f, 0.0f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (i == editPolygon.userPointerOffset + editPolygon.size - 1) {
					DebugRenderer::Get().AddLine(m_edgePoints[i] + Vector3f(0.0f, 1.0f, 0.0f), m_edgePoints[editPolygon.userPointerOffset] + Vector3f(0.0f, 1.0f, 0.0f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
				}
			}
		}
	}
}

void NavPolygonHelper::addPolygon(int x, int y, const std::vector<btCollisionObject*>& colliosionFilter) {
	Mouse::instance().attach(Application::GetWindow(), false, false, false);
	if (mousePicker.clickOrthographicAll(x, y, camera, nullptr, colliosionFilter, -1)) {
		const MousePickCallbackAll& callbackAll = mousePicker.getCallbackAll();
		if (callbackAll.m_userIndex1 >= 0) {
			m_globalUserIndex = m_globalUserIndex >= 0 ? -1 : callbackAll.m_userIndex1;
		}else {
			Vector3f pos = callbackAll.m_hitPointWorld[callbackAll.index];
			m_edgePoints.push_back(pos);
			btCollisionObject* collisionObject = Physics::AddKinematicObject(Physics::BtTransform(pos), new btSphereShape(m_markerSize * 0.5f), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
			collisionObject->setUserIndex(m_edgePoints.size() - 1);
			m_collisionObjects.push_back(collisionObject);
			if (m_editPolygons.empty()) {
				m_editPolygons.push_back(EditPolygon());
				m_currentPolygon = &m_editPolygons.back();
			}
			m_currentPolygon->size = m_edgePoints.size() - m_currentPolygon->userPointerOffset;
		}
	}
}

void NavPolygonHelper::movePolygon(int x, int y, const btCollisionObject* colliosionFilter) {
	if (m_drawPolygon && mousePicker.updatePositionOrthographicAll(x, y, camera, colliosionFilter)) {
		const MousePickCallbackAll& callbackAll = mousePicker.getCallbackAll();
		if (m_globalUserIndex >= 0) {
			m_edgePoints[m_globalUserIndex] = Physics::VectorFrom(callbackAll.m_hitPointWorld[callbackAll.index]);
			btTransform& transform = m_collisionObjects[m_globalUserIndex]->getWorldTransform();
			transform.getOrigin() = callbackAll.m_hitPointWorld[callbackAll.index];
			m_collisionObjects[m_globalUserIndex]->setWorldTransform(transform);
		}
	}
}

void NavPolygonHelper::remove() {
	if (Keyboard::instance().keyDown(Keyboard::KEY_LCTRL)) {
		if (m_edgePoints.empty())
			return;

		if (m_currentPolygon->size == 0 && m_editPolygons.size() > 1) {
			m_editPolygons.pop_back();
			m_currentPolygon = &m_editPolygons.back();
		}

		m_edgePoints.pop_back();
		Physics::DeleteCollisionObject(m_collisionObjects.back());
		m_collisionObjects.pop_back();
		if (m_currentPolygon->size > 0) {
			m_currentPolygon->size--;
			m_currentPolygon->edgePoints.pop_back();
		}

	}else {
		if (m_editPolygons.empty())
			return;

		if (m_currentPolygon->size == 0 && m_editPolygons.size() > 1) {
			m_editPolygons.pop_back();
			m_currentPolygon = &m_editPolygons.back();
		}

		m_edgePoints.resize(m_edgePoints.size() - m_currentPolygon->size);

		m_edgePoints.shrink_to_fit();
		for (int i = m_currentPolygon->userPointerOffset; i < m_currentPolygon->userPointerOffset + m_currentPolygon->size; i++) {
			Physics::DeleteCollisionObject(m_collisionObjects[i]);
		}
		m_collisionObjects.resize(m_collisionObjects.size() - m_currentPolygon->size);
		m_collisionObjects.shrink_to_fit();

		m_editPolygons.pop_back();

		if (!m_editPolygons.empty()) {
			m_currentPolygon = &m_editPolygons.back();
		}
	}
}

void NavPolygonHelper::clearPolygonCache() {
	m_edgePoints.clear();
	m_edgePoints.shrink_to_fit();
	for (btCollisionObject* obj : m_collisionObjects) {
		Physics::DeleteCollisionObject(obj);
	}
	m_collisionObjects.clear();
	m_collisionObjects.shrink_to_fit();

	m_editPolygons.clear();
	m_editPolygons.shrink_to_fit();
}

void NavPolygonHelper::savePolygonCache() {
	std::ofstream fileOut;
	fileOut.open("res/polygon_cache.txt");
	fileOut << std::setprecision(6) << std::fixed;
	fileOut << "# Polygon Cache\n";
	for (EditPolygon& editPolygon : m_editPolygons) {
		std::vector<Vector3f> edgePoints = { m_edgePoints.begin() + editPolygon.userPointerOffset, m_edgePoints.begin() + editPolygon.userPointerOffset + editPolygon.size };
		for (int i = 0; i < edgePoints.size(); i++) {
			fileOut << "v " << edgePoints[i][0] << " " << edgePoints[i][1] << " " << edgePoints[i][2] << std::endl;
		}
		if (&editPolygon != &m_editPolygons.back())
			fileOut << "#" << std::endl;
		else
			fileOut << "#";
	}
	fileOut.close();
}

bool NavPolygonHelper::loadPolygonCache(){
	FILE * pFile = fopen("res/polygon_cache.txt", "r");
	if (pFile == NULL) {
		return false;
	}
	char buffer[250];
	int size = 0;
	while (fscanf(pFile, "%s", buffer) != EOF) {
		switch (buffer[0]) {

		case 'v': {

			switch (buffer[1]) {
			case '\0': {
				float x, y, z;
				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%f %f %f", &x, &y, &z);
				m_edgePoints.push_back({ x, y, z });
				btCollisionObject* collisionObject = Physics::AddKinematicObject(Physics::BtTransform(m_edgePoints.back()), new btSphereShape(m_markerSize * 0.5f), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
				collisionObject->setUserIndex(m_edgePoints.size() - 1);
				m_collisionObjects.push_back(collisionObject);
				size++;
			}default: {
				break;
			}
			}
			break;
		}case '#': {
			if (!m_edgePoints.size())
				break;

			m_editPolygons.push_back(EditPolygon());
			m_currentPolygon = &m_editPolygons.back();
			m_currentPolygon->size = size;
			m_currentPolygon->userPointerOffset = m_edgePoints.size() - size;

			size = 0;
		}default: {
			break;
		}

		}//end switch
	}// end while


	m_editPolygons.push_back(EditPolygon());
	m_currentPolygon = &m_editPolygons.back();
	m_currentPolygon->userPointerOffset = m_edgePoints.size();
	m_currentPolygon->size = 0;

	return true;
}