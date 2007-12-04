#ifndef _WINAMPUTIL_H_
#define _WINAMPUTIL_H_

HWND GetWinampWindowHandle();

int GetPlayingStatus(HWND hWnd);

int GetNumberOfTracksInPlayList(HWND hWnd);

int GetPlayListPos(HWND hWnd);

int GetCurrentPlayingPercent(HWND hWnd);

DWORD GetCurrentTrackLength(HWND hWnd);

DWORD GetCurrentTrackPos(HWND hWnd);

BOOL GetFormattedMessage(LPTSTR lpszTemplate, LPTSTR lpszMessage);

LPSTR GetPlayListFileA(HWND hWnd, int nIndex);

LPTSTR GetPlayListTitle(HWND hWnd, int nIndex);

LPTSTR GetPlayListFileW(HWND hWnd, int nIndex);

#endif