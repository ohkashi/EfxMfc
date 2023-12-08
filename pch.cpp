// pch.cpp: source file corresponding to the pre-compiled header

#include "pch.h"

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.

int WideCharToUtf8(LPSTR lpszUtf8, int nBuffLen, LPCWSTR lpszWideChar)
{
	int wlen = wcslen(lpszWideChar);
	int nLen = WideCharToMultiByte(CP_UTF8, 0, lpszWideChar, wlen, NULL, 0, NULL, NULL);
	if (nBuffLen <= nLen)
		nLen = nBuffLen - 1;
	lpszUtf8[nLen] = '\0';
	return WideCharToMultiByte(CP_UTF8, 0, lpszWideChar, wlen, lpszUtf8, nLen, NULL, NULL);
}

int Utf8ToWideChar(LPWSTR lpszWideStr, int nBuffLen, LPCSTR lpszUtf8)
{
	int utf8_len = strlen(lpszUtf8);
	int nLen = MultiByteToWideChar(CP_UTF8, 0, lpszUtf8, utf8_len, NULL, NULL);
	if (nBuffLen <= nLen)
		return -1;
	lpszWideStr[nLen] = '\0';
	return MultiByteToWideChar(CP_UTF8, 0, lpszUtf8, utf8_len, lpszWideStr, nLen);
}
