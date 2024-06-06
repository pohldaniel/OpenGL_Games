#include <GL/glew.h>
#include "SimulationObject.h"

void SimulationObject::draw() {
	glBindVertexArray(m_model->vao);
	glDrawElements(GL_TRIANGLES, m_model->GetIndices().size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}