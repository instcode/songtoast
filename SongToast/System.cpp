/*
	SongToast Winamp Plugin
	Copyright (C) 2005 Khoa Nguyen <instcode (at) gmail.com>
*/
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include "System.h"

#define MAX_THREAD_COUNT 1

HANDLE hThread[MAX_THREAD_COUNT];
UINT nThreadPointer = 0;
// using undocumented functions and structures

#define SystemBasicInformation		0
#define	SystemPerformanceInformation	2
#define SystemTimeInformation		3

#define Li2Double(x)	((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

typedef struct
{
	DWORD	dwUnknown1;
	ULONG	uKeMaximumIncrement;
	ULONG	uPageSize;
	ULONG	uMmNumberOfPhysicalPages;
	ULONG	uMmLowestPhysicalPage;
	ULONG	UMmHighestPhysicalPage;
	ULONG	uAllocationGranularity;
	PVOID	pLowestUserAddress;
	PVOID	pMmHighestUserAddress;
	ULONG	uKeActiveProcessors;
	BYTE	bKeNumberProcessors;
	BYTE	bUnknown2;
	WORD	bUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
	LARGE_INTEGER	liIdleTime;
	DWORD		dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
	LARGE_INTEGER	liKeBootTime;
	LARGE_INTEGER	liKeSystemTime;
	LARGE_INTEGER	liExpTimeZoneBias;
	ULONG			uCurrentTimeZoneID;
	DWORD			dwReserved;
} SYSTEM_TIME_INFORMATION;

// NtQuerySystemInformation
// The function copies the system information of the specified type into a buffer
// NTSYSAPI 
// NTSTATUS
// NTAPI
// NtQuerySystemInformation(
//		IN UINT SystemInformationClass,		// information type
//		OUT PVOID SystemInformation,		// pointer to buffer
//		IN ULONG SystemInformationLength,	// buffer size in bytes
//		OUT PULONG ReturnLength OPTIONAL	// pointer to a 32 bit variable that
//											// receives the number of bytes written
//											// to the buffer
// );

typedef LONG (WINAPI *PROCNTQSI) (UINT, PVOID, ULONG, PULONG);

// this functions return the CPU usage of one second using
// undocumented Windows NT API's
// this code will not run on Win 9x
double GetCPUUsages()
{
	SYSTEM_BASIC_INFORMATION		SysBaseInfo;
	SYSTEM_TIME_INFORMATION			SysTimeInfo;
	SYSTEM_PERFORMANCE_INFORMATION	SysPerfInfo;
	LONG							status;
	LARGE_INTEGER					liOldIdleTime = {0, 0};
	LARGE_INTEGER					liOldSystemTime = {0, 0};
	double							dbIdleTime;
	double							dbSystemTime;
	PROCNTQSI						NtQuerySystemInformation;

	NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle(_T("ntdll")), 
		"NtQuerySystemInformation");

	if (NtQuerySystemInformation)
	{
		status = NtQuerySystemInformation(SystemBasicInformation, &SysBaseInfo, 
			sizeof(SysBaseInfo), NULL);

		if (status == NO_ERROR)
		{
			// get system time
			status = NtQuerySystemInformation(SystemTimeInformation, &SysTimeInfo, 
				sizeof(SysTimeInfo), NULL);

			if (status == NO_ERROR)
			{
				// get system idle time
				status = NtQuerySystemInformation(SystemPerformanceInformation,
					&SysPerfInfo, sizeof(SysPerfInfo), NULL);

				if (status == NO_ERROR)
				{
					liOldIdleTime = SysPerfInfo.liIdleTime;
					liOldSystemTime = SysTimeInfo.liKeSystemTime;

					// wait one second
					::Sleep(1000);

					// get new System time
					status = NtQuerySystemInformation(SystemTimeInformation, &SysTimeInfo,
						sizeof(SysTimeInfo), NULL);

					if (status == NO_ERROR)
					{
						// get new system idle time

						status = NtQuerySystemInformation(SystemPerformanceInformation,
							&SysPerfInfo, sizeof(SysPerfInfo), NULL);

						if (status == NO_ERROR)
						{
							// current value = new value - old value
							dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
							dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);

							// currentCpuIdle = IdleTime / SystemTime;
							dbIdleTime = dbIdleTime / dbSystemTime;

							// currentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
							dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;
						}
					}
				}
			}
		}
	}

	return dbIdleTime;
}

BOOL BeginThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter)
{
	if (nThreadPointer < MAX_THREAD_COUNT && hThread[nThreadPointer] == NULL)
	{
		DWORD dwThreadID;
		hThread[nThreadPointer] = CreateThread(
			NULL,			// default security attributes
			NULL,			// use default stack size
			lpStartAddress,	// thread function
			lpParameter,	// argument to thread function
			NULL,			// use default creation flags
			&dwThreadID		// returns the thread identifier
		);
		nThreadPointer++;
		return TRUE;
	}
	return FALSE;
}

BOOL WaitForThreadsToExit()
{
	for (UINT i = 0; i < MAX_THREAD_COUNT; i++)
	{
		if (hThread[i] != NULL)
		{
			SysTrace(L"Wait for thread %d to exit...\n", hThread[i]);
			// Wait until the service thread has terminated.
			WaitForSingleObject(hThread[i], INFINITE);
			// Close all thread handles upon completion.
			CloseHandle(hThread[i]);
			SysTrace(L"Thread %d exited", hThread[i]);
		}
	}
	return TRUE;
}

void ReadProcess(DWORD dwProcessId, LPCVOID lpBaseAddr, LPSTR lpszBuffer, DWORD& dwSize)
{
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	ReadProcessMemory(hProcess, lpBaseAddr, lpszBuffer, dwSize, &dwSize);
	CloseHandle(hProcess);
}

void SysTrace(const TCHAR *szFormat, ...)
{
	TCHAR szMessage[1024];
	va_list vl;
	va_start(vl, szFormat);
	vswprintf(szMessage, szFormat, vl);
	va_end(vl);
#ifdef _DEBUG
	OutputDebugString(szMessage);
#else
	wprintf(szMessage);
#endif
}