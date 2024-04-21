#pragma once
#include "pch.h"
#include "Store.h"

class IProcessor
{
public:
	virtual void Process(LPVOID data, DWORD length, DWORD total) noexcept = 0;
	virtual LPSTR Output() noexcept = 0;
	virtual BOOL IsCompleted() noexcept = 0;
};

class Processor : IProcessor {

public:
	// reply message to server
	LPCSTR REPLY_OK = "OK!";
	// notify message for WRITE COMPLETE EVENT
	LPCSTR NOTIFY = "COMPLETED!";

	Store* store = new Store();

	void Process(LPVOID data, DWORD length, DWORD total) noexcept
	{
		store->Append(data, length, total);
	}

	LPSTR Output() noexcept
	{
		MessageBoxA(0, (LPSTR)store->data, "RECEIVED MESSAGE", MB_ICONINFORMATION);
		store->Clear();

		return (LPSTR)REPLY_OK;
	}

	BOOL IsCompleted() noexcept
	{
		return store->isCompleted;
	}

	~Processor()
	{
		delete store;
	}
};
