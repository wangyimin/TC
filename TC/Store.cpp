
#include "pch.h"
#include "store.h"

void Store::Append(LPVOID data, DWORD length, DWORD total)
{
	if (!this->data)
	{
		this->data = LocalAlloc(LPTR, total + 1);
		memcpy(this->data, data, length);
	}
	else
	{
		char* tmp = (char*)this->data;
		memcpy_s(tmp + this->length, length, data, length);
	}
	this->length += length;
};

void Store::Clear()
{
	if (this->length > 0)
	{
		LocalFree(this->data);
		this->data = 0;
		this->length = 0;
	}
};

Store::~Store()
{
	Clear();
};
