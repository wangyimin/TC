
#include "pch.h"
#include "store.h"

void Store::Append(LPVOID data, DWORD length, DWORD total) noexcept
{
	assert(data);
	assert(total >= length);

	if (!this->data)
	{
		this->data = LocalAlloc(LPTR, total + 1);
		memcpy(this->data, data, length);
	}
	else
	{
		LPSTR tmp = (LPSTR)this->data;
		memcpy_s(tmp + this->length, length, data, length);
	}

	this->length += length;

	isCompleted = this->length == total;
};

void Store::Clear() noexcept
{
	if (this->length > 0)
	{
		LocalFree(this->data);
		this->data = 0;
		this->length = 0;
		this->isCompleted = FALSE;
	}
};

Store::~Store()
{
	Clear();
};
