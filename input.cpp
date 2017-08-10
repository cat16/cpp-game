#include "input.hpp"

#include <SDL.h>
#include <iostream>

bool InputHandler::check() {
	SDL_GetMouseState(&mousex, &mousey);
	SDL_GetRelativeMouseState(&mousexv, &mouseyv);

	SDL_Event event;

	keysp.clear();
	mbsp.clear();

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return false;
			break;

		case SDL_KEYDOWN:
			keys[event.key.keysym.sym] = true;
			keysp[event.key.keysym.sym] = true;
			break;
		case SDL_KEYUP:
			keys[event.key.keysym.sym] = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			mbs[event.button.button] = true;
			mbsp[event.button.button] = true;
			break;
		case SDL_MOUSEBUTTONUP:
			mbs[event.button.button] = false;
			break;

		case SDL_MOUSEWHEEL:
			mouseScroll += event.wheel.y;
			break;
		}
	}

	int modState = SDL_GetModState();
	modState = modState & KMOD_CAPS;
	if (modState == KMOD_CAPS)
		caps = true;
	else
		caps = false;

	return true;
}