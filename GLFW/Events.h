#pragma once
#ifndef WINDOW_EVENTS_H_
#define WINDOW_EVENTS_H_

#include "tools/camera.h"
#include "src/vk.h"

typedef unsigned int uint;

class Events {
public:

	static bool* _keys;
	static uint* _frames;
	static uint _current;
	static double deltaX;
	static double deltaY;
	static double x;
	static double y;
	static bool _cursor_locked;
	static bool _cursor_started;

	static int eventInitialize(class vk* p_vk);
	static void pollEvents();

	static bool pressed(int keycode);
	static bool jpressed(int keycode);

	static bool clicked(int button);
	static bool jclicked(int button);

	static void toggleCursor();
};

#endif /* WINDOW_EVENTS_H_ */