// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include "windows.h"
#include "cchannel.h"
//#include <stdio.h>
#include <stdlib.h>

PCHANNEL_ENTRY_POINTS entryPoints;
unsigned long OpenChannel = 0;
void* Channel = nullptr;
char ok[2] = { 'O', 'K' };

void __stdcall VirtualChannelOpenEventProc(
	unsigned long handle,
	unsigned int event, 
	void* data, 
	unsigned int dataLength, 
	unsigned int totalLength, 
	unsigned int dataFlags)
{
	DWORD size = 1;

	if (dataLength == totalLength)
	{
		char* s = (char*)malloc(dataLength + 1);

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

			memcpy(s, data, dataLength);
			s[dataLength] = 0;
			MessageBoxA(0, s, "Info", MB_ICONINFORMATION);
		
			if (entryPoints->pVirtualChannelWrite(OpenChannel, &ok[0], 2, &size) == CHANNEL_RC_OK)
			{
				OutputDebugStringA("OK is sent back!");
			}

			break;
		case CHANNEL_EVENT_WRITE_COMPLETE:	
			break;
		case CHANNEL_EVENT_WRITE_CANCELLED:
			break;
		default:
			break;
		}

		free(s);
	}
}

void __stdcall VirtualChannelInitEventProc(void* initHandle, unsigned int event, void* data, unsigned int dataLength)
{
	switch (event)
	{
		case CHANNEL_EVENT_INITIALIZED:
			break;
		case CHANNEL_EVENT_CONNECTED:
			entryPoints->pVirtualChannelOpen(initHandle, &OpenChannel, (PCHAR)"TSCS", VirtualChannelOpenEventProc);
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
		strcpy_s(cd.name, strlen("TSCS") + 1, "TSCS");

		if (entry->pVirtualChannelInit(&Channel, &cd, 1, VIRTUAL_CHANNEL_VERSION_WIN2000, VirtualChannelInitEventProc) == CHANNEL_RC_OK)
		{
			OutputDebugStringW(L"TCSC channel is initialized.");
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

