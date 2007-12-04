#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "System.h"
#include "Plugin.h"
#include "WinampUtil.h"

extern winampGeneralPurposePlugin plugin;

#define MAX_COMMENT_SIZE 16384

typedef struct {
	TCHAR filename[MAX_PATH];
	TCHAR title[MAX_PATH];
	TCHAR album[MAX_PATH];
	TCHAR artist[MAX_PATH];
	TCHAR genre[MAX_PATH];
	TCHAR comment[MAX_COMMENT_SIZE];
	int bitrate;
	int tracknumber;
	int length;
	int year;
}
simpleID3TagStruct;

static simpleID3TagStruct id3tag;
static TCHAR szPlayingFile[MAX_PATH] = {0};

TCHAR *TagFunc(const TCHAR * tag, void * p)
{
	TCHAR retbuf[2048];

	if (!lstrcmpi(tag, L"cpu")) {
		double cpu = GetCPUUsages();
		// CPU usage...
		swprintf(retbuf, L"%2.1f", cpu);
	}
	else if (!lstrcmpi(tag, L"mem")) {
		// Available Physical Memory...
		MEMORYSTATUS stat;
		GlobalMemoryStatus(&stat);
		swprintf(retbuf, L"%d", stat.dwAvailPhys/1024);
	}
	else if (!lstrcmpi(tag, L"percent")) {
		swprintf(retbuf, L"%d", GetCurrentPlayingPercent((HWND)p));
	}
	else if (!lstrcmpi(tag, L"pllen")) {
		swprintf(retbuf, L"%d", GetNumberOfTracksInPlayList((HWND)p));
	}
	else if (!lstrcmpi(tag, L"plpos")) {
		swprintf(retbuf, L"%d", SendMessage((HWND)p, WM_WA_IPC, 0, IPC_GETLISTPOS) + 1); 
	}
	else if (!lstrcmpi(tag, L"lengthf")) {
		int length = (int)::SendMessage((HWND)p, WM_WA_IPC, 1, IPC_GETOUTPUTTIME);
		int s = length % 60, m = (length / 60) % 60, h = (length / 60) / 60;
		if (h > 0) {
			swprintf(retbuf, L"%.2d:%.2d:%.2d", h, m, s);
		}
		else {
			swprintf(retbuf, L"%.2d:%.2d", m, s);
		}
	}
	else if (!lstrcmpi(tag, L"filename")) {
		swprintf(retbuf, L"%s", id3tag.filename);
	}
	else if (!lstrcmpi(tag, L"title")) {
		swprintf(retbuf, L"%s", id3tag.title);
	}
	else if (!lstrcmpi(tag, L"artist")) {
		swprintf(retbuf, L"%s", id3tag.artist);
	}
	else if (!lstrcmpi(tag, L"genre")) {
		swprintf(retbuf, L"%s", id3tag.genre);
	}
	else if (!lstrcmpi(tag, L"album")) {
		swprintf(retbuf, L"%s", id3tag.album);
	}
	else if (!lstrcmpi(tag, L"comment")) {
		swprintf(retbuf, L"%s", id3tag.comment);
	}
	else if (!lstrcmpi(tag, L"bitrate")) {
		swprintf(retbuf, L"%d", id3tag.bitrate);
	}
	else if (!lstrcmpi(tag, L"tracknumber")) {
		swprintf(retbuf, L"%d", id3tag.tracknumber);
	}
	else if (!lstrcmpi(tag, L"length")) {
		swprintf(retbuf, L"%d", id3tag.length);
	}
	else if (!lstrcmpi(tag, L"year")) {
		swprintf(retbuf, L"%d", id3tag.year);
	}
	else if (!lstrcmpi(tag, L"shuffle")) {
		swprintf(retbuf, L"%d", SendMessage((HWND)p, WM_WA_IPC, 0, IPC_GET_SHUFFLE));
	}
	else if (!lstrcmpi(tag, L"channelnum")) {
		swprintf(retbuf, L"%d", SendMessage((HWND)p, WM_WA_IPC, 2, IPC_GETINFO));
	}
	else if (!lstrcmpi(tag, L"channels")) {
		lstrcpyn(retbuf, SendMessage((HWND)p, WM_WA_IPC, 2, IPC_GETINFO) - 1? L"stereo" : L"mono", MAX_PATH);
	}
	else if (!lstrcmpi(tag, L"srate")) {
		swprintf(retbuf, L"%d", SendMessage((HWND)p, WM_WA_IPC, 0, IPC_GETINFO));
	}
	else if (!_wcsnicmp(L"cmp(", tag, 4))
	{
		//%cmp(abc>b,a:b)%
		TCHAR left[MAX_PATH];
		TCHAR right[MAX_PATH];
		for (int tp = 4; tag[tp] != NULL; tp++)
		{
			if (tag[tp] == '=') {
				if (!lstrcmpi(left, right)) {
				}
			}
			else if (tag[tp] == '<') {
				int l = _wtoi(left);
				int r = _wtoi(right);
			}
			else if (tag[tp] == '>') {
				int l = _wtoi(left);
				int r = _wtoi(right);
			}
		}
	}
	if (lstrlen(retbuf) > 0)
	{
		TCHAR* s = (TCHAR*)GlobalAlloc(GPTR, 256*2);
		lstrcpyn(s, retbuf, 256);
		return s;
	}
	return NULL;
}

void TagFree(TCHAR *tag, void *p)
{
	GlobalFree((HGLOBAL) tag);
}

void FormatTitleW(HWND hWnd, LPTSTR lpszTemplate, LPTSTR lpszMessage)
{
	waFormatTitleExtended fmt;
	ZeroMemory(&fmt, sizeof(waFormatTitleExtended));
	fmt.filename = id3tag.filename;
	fmt.useExtendedInfo = 1;
	fmt.spec = lpszTemplate;
	fmt.p = hWnd;
	fmt.out = lpszMessage;
	fmt.out_len = 512;
	fmt.TAGFUNC = TagFunc;
	fmt.TAGFREEFUNC = TagFree;
	SendMessage(hWnd, WM_WA_IPC, (WPARAM)&fmt, IPC_FORMAT_TITLE_EXTENDED);
}


void GetSongMetaData(int nIndex, simpleID3TagStruct& id3tag)
{
	char retbuf[MAX_COMMENT_SIZE > MAX_PATH ? MAX_COMMENT_SIZE : MAX_PATH];
	extendedFileInfoStruct efi;

	::ZeroMemory(retbuf, MAX_COMMENT_SIZE > MAX_PATH ? MAX_COMMENT_SIZE : MAX_PATH);

	efi.filename = GetPlayListFileA((HWND)plugin.hwndParent, nIndex);
	efi.retlen = sizeof(retbuf)-1;

	efi.metadata = "artist";
	efi.ret = retbuf;
	SendMessage((HWND)plugin.hwndParent, WM_WA_IPC, (WPARAM)&efi, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE);
	MultiByteToWideChar(CP_ACP, 0, retbuf, -1, id3tag.artist, MAX_PATH);
			
	efi.metadata = "title";
	efi.ret = retbuf;
	SendMessage((HWND)plugin.hwndParent, WM_WA_IPC, (WPARAM)&efi, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE);
	MultiByteToWideChar(CP_ACP, 0, retbuf, -1, id3tag.title, MAX_PATH);

	efi.metadata = "genre";
	efi.ret = retbuf;
	SendMessage((HWND)plugin.hwndParent, WM_WA_IPC, (WPARAM)&efi, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE);
	MultiByteToWideChar(CP_ACP, 0, retbuf, -1, id3tag.genre, MAX_COMMENT_SIZE);

	efi.metadata = "comment";
	efi.ret = retbuf;
	SendMessage((HWND)plugin.hwndParent, WM_WA_IPC, (WPARAM)&efi, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE);
	MultiByteToWideChar(CP_ACP, 0, retbuf, -1, id3tag.comment, MAX_COMMENT_SIZE);

	efi.metadata = "album";
	efi.ret = retbuf;
	SendMessage((HWND)plugin.hwndParent, WM_WA_IPC, (WPARAM)&efi, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE);
	MultiByteToWideChar(CP_ACP, 0, retbuf, -1, id3tag.album, MAX_COMMENT_SIZE);
	
	efi.metadata = "length";
	efi.ret = retbuf;
	SendMessage((HWND)plugin.hwndParent, WM_WA_IPC, (WPARAM)&efi, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE);
	id3tag.length = atoi(retbuf) / 1000;

	efi.metadata = "bitrate";
	SendMessage((HWND)plugin.hwndParent, WM_WA_IPC, (WPARAM)&efi, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE);
	id3tag.bitrate = atoi(retbuf);

	efi.metadata = "track";
	SendMessage((HWND)plugin.hwndParent, WM_WA_IPC, (WPARAM)&efi, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE);
	id3tag.tracknumber = atoi(retbuf);
	id3tag.tracknumber = (id3tag.tracknumber == -1) ? 0 : id3tag.tracknumber;

	efi.metadata = "year";
	SendMessage((HWND)plugin.hwndParent, WM_WA_IPC, (WPARAM)&efi, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE);
	id3tag.year = atoi(retbuf);

	MultiByteToWideChar(CP_ACP, 0, efi.filename, -1, id3tag.filename, MAX_PATH);
}

HWND GetWinampWindowHandle()
{
	return ::FindWindow(_T("Winamp v1.x"), NULL);
}

int GetPlayingStatus(HWND hWnd)
{
	return (int)::SendMessage(hWnd, WM_WA_IPC, 0, IPC_ISPLAYING);
}

int GetNumberOfTracksInPlayList(HWND hWnd)
{
	return (int)::SendMessage(hWnd, WM_WA_IPC, 0, IPC_GETLISTLENGTH);
}

int GetPlayListPos(HWND hWnd)
{
	return (int)::SendMessage(hWnd, WM_WA_IPC, 0, IPC_GETLISTPOS);
}

LPTSTR GetPlayListTitle(HWND hWnd, int nIndex)
{
	return (LPTSTR)::SendMessage(hWnd, WM_WA_IPC, nIndex, IPC_GETPLAYLISTTITLEW);
}

LPSTR GetPlayListFileA(HWND hWnd, int nIndex)
{
	return (LPSTR)::SendMessage(hWnd, WM_WA_IPC, nIndex, IPC_GETPLAYLISTFILE);
}

LPTSTR GetPlayListFileW(HWND hWnd, int nIndex)
{
	return (LPTSTR)::SendMessage(hWnd, WM_WA_IPC, nIndex, IPC_GETPLAYLISTFILEW);
}

DWORD GetCurrentTrackLength(HWND hWnd)
{
	return (DWORD)::SendMessage(hWnd, WM_WA_IPC, 1, IPC_GETOUTPUTTIME);
}

DWORD GetCurrentTrackPos(HWND hWnd)
{
	return (DWORD)::SendMessage(hWnd, WM_WA_IPC, 0, IPC_GETOUTPUTTIME) / 1000;
}

int GetCurrentPlayingPercent(HWND hWnd) {
	int nTrackPos = (int)::SendMessage(hWnd, WM_WA_IPC, 0, IPC_GETOUTPUTTIME);
	int nTrackLength = (int)::SendMessage(hWnd, WM_WA_IPC, 1, IPC_GETOUTPUTTIME) * 10;
	int nPercent = (nTrackLength != 0)? nTrackPos/nTrackLength : 0;
	if (nPercent > 100)	{
		nPercent = 100;
	}
	return nPercent;
}

BOOL GetFormattedMessage(LPTSTR lpszTemplate, LPTSTR lpszMessage)
{
	HWND hWnd = GetWinampWindowHandle();

	int nStatus = GetPlayingStatus(hWnd);
	if (nStatus != 1 && nStatus != 3) {
		lpszMessage[0] = NULL;
		return FALSE;
	}
	int nIndex = GetPlayListPos(hWnd);
	LPTSTR lpszPlayingFile = GetPlayListFileW(hWnd, nIndex);
	// Get the meta data if the playing file has changed
	if (lstrcmpi(lpszPlayingFile, szPlayingFile))
	{
		GetSongMetaData(nIndex, id3tag);
	}
	FormatTitleW(hWnd, lpszTemplate, lpszMessage);
	return TRUE;
}
