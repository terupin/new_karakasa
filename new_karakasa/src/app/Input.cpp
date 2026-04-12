#include "Input.h"

bool Input::m_keys[256] = {};
bool Input::m_prevKeys[256] = {};

void Input::Update()
{
	for (int i = 0; i < 256; ++i)
	{
		m_prevKeys[i] = m_keys[i];
		m_keys[i] = (GetAsyncKeyState(i)) != 0;
	}
}

bool Input::GetKey(int vkey)
{
	return m_keys[vkey];
}

bool Input::GetKeyDown(int vkey)
{
	return m_keys[vkey] && !m_prevKeys[vkey];
}

bool Input::GetKeyUp(int vkey)
{
	return !m_keys[vkey] && m_prevKeys[vkey];
}