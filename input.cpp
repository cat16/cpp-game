#include "input.hpp"

#include <SDL.h>
#include <iostream>

bool InputHandler::check() {
	SDL_GetMouseState(mousex, mousey);
	SDL_GetRelativeMouseState(mousexv, mouseyv);

	SDL_Event event;

	keysp->clear();
	mouseButtonsPressed->clear();

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return false;
			break;

		case SDL_KEYDOWN:
			(*keys)[event.key.keysym.sym] = true;
			(*keysp)[event.key.keysym.sym] = true;
			break;
		case SDL_KEYUP:
			(*keys)[event.key.keysym.sym] = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			(*mouseButtons)[event.button.button] = true;
			(*mouseButtonsPressed)[event.button.button] = true;
			break;
		case SDL_MOUSEBUTTONUP:
			(*mouseButtons)[event.button.button] = false;
			break;

		case SDL_MOUSEWHEEL:
			(*mouseScroll) += event.wheel.y;
			break;
		}
	}

	int modState = SDL_GetModState();
	modState = modState & KMOD_CAPS;
	if (modState == KMOD_CAPS)
		*caps = true;
	else
		*caps = false;

	return true;
}

void InputHandler::setMouse(int *mx, int *my, int *mxv, int *myv) {
	mousex = mx;
	mousey = my;
	mousexv = mxv;
	mouseyv = myv;
}

void InputHandler::setScroll(int * scroll) {
	mouseScroll = scroll;
}

void InputHandler::setKeys(std::map<long, bool> *k, std::map<long, bool> *kp) {
	keys = k;
	keysp = kp;
}
void InputHandler::setCaps(bool *c) {
	caps = c;
}

void InputHandler::setMouseButtons(std::map<int, bool> *mbs, std::map<int, bool> *mbsp) {
	mouseButtons = mbs;
	mouseButtonsPressed = mbsp;
}