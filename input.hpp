#pragma once

#include <glm.hpp>

#include <map>

class InputHandler {
private:
public:
	int mousex, mousey, mousexv, mouseyv;
	int mouseScroll = 0;
	std::map<long, bool> keys, keysp;
	std::map<int, bool> mbs, mbsp;
	bool caps;

	bool check();
};