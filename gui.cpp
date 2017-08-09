#include "gui.hpp"

#include <glew.h>

void GUI::draw(World w) {
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glBegin(GL_LINES);
	glColor3f(1.0, 1.0, 1.0);
	glVertex2f(-0.02, 0.0);
	glVertex2f(+0.02, 0.0);
	glVertex2f(0.0, -0.02);
	glVertex2f(0.0, +0.02);
	glEnd();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (w.paused) {
		glBegin(GL_QUADS);
		glColor4f(0.0, 0.0, 0.0, 0.5);
		glVertex2f(-1, -1);
		glVertex2f(+1, -1);
		glVertex2f(+1, +1);
		glVertex2f(-1, +1);
		glEnd();
	}
}