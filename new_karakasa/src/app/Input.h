#pragma once
#include <Windows.h>

class Input
{
public:
	static void Update();

	static bool GetKey(int vkey);
	static bool GetKeyDown(int vkey);
	static bool GetKeyUp(int vkey);

private:
	static bool m_keys[256];
	static bool m_prevKeys[256];
};





