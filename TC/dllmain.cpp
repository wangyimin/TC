// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include "windows.h"
#include "cchannel.h"
//#include <stdio.h>
#include <stdlib.h>
#include <string> 
using namespace std;

PCHANNEL_ENTRY_POINTS entryPoints;
DWORD OpenChannel = 0;
LPVOID Channel = nullptr;
const char* CHANNEL_NAME = "TSCS";
const char* REPLY_OK = "OK";
const char* NOTIFY = "W"; // don't let NOTIFY have more bytes than REPLY_OK

void __stdcall VirtualChannelOpenEventProc(
	DWORD handle,
	UINT event,
	LPVOID data,
	UINT32 dataLength,
	UINT32 totalLength,
	UINT32 dataFlags)
{
	char* s = (char*)LocalAlloc(LPTR, totalLength + 1);

	switch (event)
	{
	case CHANNEL_EVENT_DATA_RECEIVED:
		/*switch (dataFlags & CHANNEL_FLAG_ONLY)
		{
		case CHANNEL_FLAG_ONLY:
			break;
		case CHANNEL_FLAG_FIRST:
			break;
		case CHANNEL_FLAG_MIDDLE:
			break;
		case CHANNEL_FLAG_LAST:
			break;
		}*/

		memcpy(s, data, totalLength);
		MessageBoxA(0, s, "RECEIVED MESSAGE", MB_ICONINFORMATION);

		if (entryPoints->pVirtualChannelWrite(OpenChannel, (LPVOID)REPLY_OK, (ULONG)strlen(REPLY_OK), (LPVOID)NOTIFY) == CHANNEL_RC_OK)
		{
			OutputDebugStringA("OK is sent back!");
		}

		break;
	case CHANNEL_EVENT_WRITE_COMPLETE:
		// Get pUserData(set by last parameter of pVirtualChannelWrite)
		//OutputDebugStringA(to_string(totalLength).c_str());
		ZeroMemory(s, strlen(NOTIFY)+1);
		memcpy(s, data, strlen(NOTIFY));
		OutputDebugStringA(s);
		break;
	case CHANNEL_EVENT_WRITE_CANCELLED:
		break;
	default:
		break;
	}

	LocalFree(s);
}

void __stdcall VirtualChannelInitEventProc(LPVOID initHandle, UINT event, LPVOID data, UINT dataLength)
{
	switch (event)
	{
		case CHANNEL_EVENT_INITIALIZED:
			break;
		case CHANNEL_EVENT_CONNECTED:
			entryPoints->pVirtualChannelOpen(initHandle, &OpenChannel, (PCHAR)CHANNEL_NAME, VirtualChannelOpenEventProc);
			break;
		case CHANNEL_EVENT_V1_CONNECTED:
			break;
		case CHANNEL_EVENT_DISCONNECTED:
			entryPoints->pVirtualChannelClose(OpenChannel);
			OpenChannel = 0;
			break;
		case CHANNEL_EVENT_TERMINATED:
			LocalFree((HLOCAL)entryPoints);
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

		if (entry->pVirtualChannelInit(&Channel, &cd, 1, VIRTUAL_CHANNEL_VERSION_WIN2000, VirtualChannelInitEventProc) == CHANNEL_RC_OK)
		{
			OutputDebugStringW(L"Channel is initialized.");
			return true;
		}
	}
	return false;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

