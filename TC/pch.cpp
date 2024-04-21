// pch.cpp: プリコンパイル済みヘッダーに対応するソース ファイル

#include "pch.h"
#include "Processor.h"

LPCSTR CHANNEL_NAME = "TSCS";

PCHANNEL_ENTRY_POINTS entryPoints;
LPVOID initChannel = nullptr;
DWORD openChannel = 0;

extern IProcessor* processor;

BOOL GetUserInformation(CHAR user[])
{
	DWORD size = sizeof(user);
	ZeroMemory(user, size);
	return GetUserNameA(user, &size);
}

VOID VCAPITYPE ChannelOpenEventProc(
	DWORD handle,
	UINT event,
	LPVOID pData,
	UINT32 dataLength,
	UINT32 totalLength,
	UINT32 dataFlags)
{
	LPSTR s = (LPSTR)LocalAlloc(LPTR, CHANNEL_CHUNK_LENGTH + 1);

	switch (event)
	{
	case CHANNEL_EVENT_DATA_RECEIVED:
		processor->Process(pData, dataLength, totalLength);

		//if ((dataFlags & CHANNEL_FLAG_ONLY) || (dataFlags & CHANNEL_FLAG_LAST))
		if (processor->IsCompleted())
		{
			LPSTR reply = processor->Output();

			if (reply)
				if (entryPoints->pVirtualChannelWrite(
						openChannel, reply, (ULONG)strlen(reply), NULL) == CHANNEL_RC_OK)
				{
					OutputDebugStringA((string(reply) + string(" is sent back!")).c_str());
				}
		}

		break;
	case CHANNEL_EVENT_WRITE_COMPLETE:
		// Get pUserData(set by last parameter of pVirtualChannelWrite)
		//OutputDebugStringA(to_string(totalLength).c_str());

		//memcpy(s, pData, strlen(processor->GetNotify()));
		//OutputDebugStringA(s);
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
		LocalFree(entryPoints);
		delete processor;
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
