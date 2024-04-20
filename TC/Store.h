#pragma once
#include "pch.h"

class Store {

public:
	LPVOID data = 0;
	DWORD length = 0;

	void Append(LPVOID data, DWORD length, DWORD total);

	void Clear();

	~Store();
};
