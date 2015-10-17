#pragma once

extern bool gQuit;

struct Gui {
	void run();

	static int threadFunc(void* data);
};