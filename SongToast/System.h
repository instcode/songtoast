#ifndef _THREAD_H_
#define _THREAD_H_

double GetCPUUsages();
BOOL BeginThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter);
BOOL WaitForThreadsToExit();
void ReadProcess(DWORD dwProcessId, LPCVOID lpBaseAddr, LPSTR lpszBuffer, IN OUT DWORD& dwSize);
void SysTrace(const TCHAR *szFormat, ...);

#endif