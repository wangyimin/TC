// pch.cpp: プリコンパイル済みヘッダーに対応するソース ファイル

#include "pch.h"
#include "Store.h"

const char* CHANNEL_NAME = "TSCS";
// reply message to server
const char* REPLY_OK = "OK";
// notify message for WRITE COMPLETE EVENT
const char* NOTIFY = "COMPLETED";

PCHANNEL_ENTRY_POINTS entryPoints;
LPVOID initChannel = nullptr;
DWORD openChannel = 0;

Store* store = new Store();

void GetUserInformation(char user[])
{
	DWORD size = sizeof(user);
	ZeroMemory(user, size);
	GetUserNameA(user, &size);
}

VOID VCAPITYPE ChannelOpenEventProc(
	DWORD handle,
	UINT event,
	LPVOID pData,
	UINT32 dataLength,
	UINT32 totalLength,
	UINT32 dataFlags)
{
	char* s = (char*)LocalAlloc(LPTR, CHANNEL_CHUNK_LENGTH + 1);

	switch (event)
	{
	case CHANNEL_EVENT_DATA_RECEIVED:
		store->Append(pData, dataLength, totalLength);

		//if ((dataFlags & CHANNEL_FLAG_ONLY) || (dataFlags & CHANNEL_FLAG_LAST))
		if (store->length == totalLength)
		{
			//output
			MessageBoxA(0, (char*)store->data, "RECEIVED MESSAGE", MB_ICONINFORMATION);
			store->Clear();

			if (entryPoints->pVirtualChannelWrite(
				openChannel, (LPVOID)REPLY_OK, (ULONG)strlen(REPLY_OK), (LPVOID)NOTIFY) == CHANNEL_RC_OK)
			{
				OutputDebugStringA("OK is sent back!");
			}
		}

		break;
	case CHANNEL_EVENT_WRITE_COMPLETE:
		// Get pUserData(set by last parameter of pVirtualChannelWrite)
		//OutputDebugStringA(to_string(totalLength).c_str());
		//ZeroMemory(s, strlen(NOTIFY)+1);
		memcpy(s, pData, strlen(NOTIFY));
		OutputDebugStringA(s);
		break;
	case CHANNEL_EVENT_WRITE_CANCELLED:
		break;
	default:
		break;
	}

	LocalFree(s);
}

VOID VCAPITYPE ChannelInitEventProc(
	LPVOID initHandle,
	UINT event,
	LPVOID pData,
	UINT dataLength)
{
	switch (event)
	{
	case CHANNEL_EVENT_INITIALIZED:
		break;
	case CHANNEL_EVENT_CONNECTED:
		entryPoints->pVirtualChannelOpen(
			initHandle, &openChannel, (PCHAR)CHANNEL_NAME, ChannelOpenEventProc);
		break;
	case CHANNEL_EVENT_V1_CONNECTED:
		break;
	case CHANNEL_EVENT_DISCONNECTED:
		entryPoints->pVirtualChannelClose(openChannel);
		openChannel = 0;
		break;
	case CHANNEL_EVENT_TERMINATED:
		LocalFree((HLOCAL)entryPoints);
		delete store;
		break;
	default:
		break;
	}
}

//コンピューター\HKEY_CURRENT_USER\SOFTWARE\Microsoft\Terminal Server Client\Default\AddIns\TC
//Name/REG_SZ/C:\...\github\TC\x64\Debug\TC.dll
EXTERN_C _declspec(dllexport) BOOL VCAPITYPE VirtualChannelEntry(PCHANNEL_ENTRY_POINTS entry)
{
	if (entry)
	{
		entryPoints = (PCHANNEL_ENTRY_POINTS)LocalAlloc(LPTR, entry->cbSize);
		memcpy(entryPoints, entry, entry->cbSize);

		CHANNEL_DEF cd;
		ZeroMemory(&cd, sizeof(CHANNEL_DEF));
		strcpy_s(cd.name, strlen(CHANNEL_NAME) + 1, CHANNEL_NAME);

		if (entry->pVirtualChannelInit(
			&initChannel, &cd, 1, VIRTUAL_CHANNEL_VERSION_WIN2000, ChannelInitEventProc) == CHANNEL_RC_OK)
		{
			OutputDebugStringW(L"Channel is initialized.");
			return true;
		}
	}
	return false;
}

// プリコンパイル済みヘッダーを使用している場合、コンパイルを成功させるにはこのソース ファイルが必要です。
