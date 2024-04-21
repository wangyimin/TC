#pragma once
#include "pch.h"

class IStore
{
public:
	virtual void Append(LPVOID data, DWORD length, DWORD total) noexcept = 0;
	virtual void Clear() noexcept = 0;
};

class Store : IStore {

public:
	LPVOID data;
	DWORD length;
	BOOL isCompleted = FALSE;

	Store() : data(0), length(0) {}
	Store(LPVOID data, DWORD length) : data(data), length(length) {}

	void Append(LPVOID data, DWORD length, DWORD total) noexcept;

	void Clear() noexcept;

	~Store();
};
