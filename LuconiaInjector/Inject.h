#pragma once

#include <windows.h>
#include <TlHelp32.h>

// https://github.com/fligger/FateInjector/blob/master/src/inject.cpp
DWORD GetProcId(LPCTSTR processName)
{
	PROCESSENTRY32 pt{};
	DWORD processid = NULL;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnap, &pt))
	{
		do
		{
			if (!lstrcmpi(pt.szExeFile, processName))
			{
				CloseHandle(hSnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hSnap, &pt));
	}

	CloseHandle(hSnap);
	return NULL;
}
INT Inject(DWORD processId, LPCWSTR path)
{
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);

	if (hProc && hProc != INVALID_HANDLE_VALUE)
	{
		LPVOID loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		if (loc == NULL) return NULL;
		WriteProcessMemory(hProc, loc, path, wcslen(path) * 2 + 2, 0); // length * 2 for bytes + 2 for end string

		HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, loc, 0, 0);
		if (hThread) CloseHandle(hThread);
	}

	if (hProc)CloseHandle(hProc);
	return NULL;
}