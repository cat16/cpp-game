#pragma once

#include <glm.hpp>

#include <map>

class InputHandler {
private:
	int *mousex, *mousey, *mousexv, *mouseyv;
	int *mouseScroll = 0;
	std::map<long, bool> *keys, *keysp;
	std::map<int, bool> *mouseButtons, *mouseButtonsPressed;
	bool *caps;
public:
	bool check();
	void setMouse(int *mx, int *my, int *mxv, int *myv);
	void setScroll(int *scroll);
	void setKeys(std::map<long, bool> *k, std::map<long, bool> *kp);
	void setCaps(bool *c);
	void setMouseButtons(std::map<int, bool> *mbs, std::map<int, bool> *mbsp);
};