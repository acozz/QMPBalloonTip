//-----------------------------------------------------------------------------
//
// File:	QCDGeneralDLL.cpp
//
// About:	See QCDGeneralDLL.h
//
// Authors:	Written by Paul Quinn and Richard Carlson.
//
//	QCD multimedia player application Software Development Kit Release 1.0.
//
//	Copyright (C) 1997-2002 Quinnware
//
//	This code is free.  If you redistribute it in any form, leave this notice 
//	here.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//-----------------------------------------------------------------------------

// Balloon Notification Plugin
// Created by Anthony Cozzolino
// Copyright 2005

#define UNICODE					// Use Unicode
#define WIN32_LEAN_AND_MEAN		// Disable rarely used Windows APIs
#define _WIN32_IE 0x0501		// Requires IE 6.0 or later functionality

#include "QCDGeneralDLL.h"
#include <shellapi.h>
#include "resource.h"

/**
 * Encapsulated variables used to configure the plugin.
 */
struct MessageOptions
{
	/**
	 * If true, the plugin parses the custom template, otherwise the playlist string
	 * is used for display.
	 */
	bool useCustomMessage;

	/**
	 * User specified template which must be parsed to create the display message 
	 * in the balloon tool tip.
	 */
	TCHAR customTemplate[255];

	/**
	 * The maximum size of the custom template.
	 */
	const static int templateSize = 255;

	/**
	 * User specified timeout value for the balloon tip.
	 */
	unsigned int timeout;
};

void Convert(const char* utf8String, TCHAR* buffer, unsigned int bufferSize);
void GetPluginPropertyFile(TCHAR* fileNameBuffer, unsigned int size);
BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, 
							INT iValue, LPCTSTR lpFileName);
void LoadOptions();
void SaveOptions();
void SubclassPlayer();
void UnsubclassPlayer();
void ShowBalloonTip(const TCHAR* text);
void GenerateMessage(TCHAR* message, unsigned int size, int trackIndex);
LRESULT CALLBACK SubclassProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI TimeoutProc(LPVOID parameters);
INT_PTR CALLBACK AboutProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ConfigProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE		hInstance;
HWND			hwndPlayer;
QCDModInitGen	*QCDCallbacks;
WNDPROC QMPCallback;
MessageOptions options;
const TCHAR* pluginName = TEXT("Balloon Notification");
HANDLE timeoutThreadHandle = NULL;
unsigned int threadID = 0;

//-----------------------------------------------------------------------------

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID pRes)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		hInstance = hInst;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------

PLUGIN_API BOOL GENERALDLL_ENTRY_POINT(QCDModInitGen *ModInit, QCDModInfo *ModInfo)
{
	ModInit->version = PLUGIN_API_VERSION_WANTUTF8;
	char pluginName[] = "Balloon Notification v0.3";
	ModInfo->moduleString = pluginName;

	ModInit->toModule.ShutDown			= ShutDown;
	ModInit->toModule.About				= About;
	ModInit->toModule.Configure			= Configure;

	QCDCallbacks = ModInit;

	hwndPlayer = (HWND)QCDCallbacks->Service(opGetParentWnd, 0, 0, 0);

	char inifile[MAX_PATH];
	QCDCallbacks->Service(opGetPluginSettingsFile, inifile, MAX_PATH, 0);

	//
	// TODO: all your plugin initialization here
	//

	// Load options
	LoadOptions();

	// Subclass the player
	SubclassPlayer();

	// return TRUE for successful initialization
	return TRUE;
}

//----------------------------------------------------------------------------

void ShutDown(int flags) 
{
	//
	// TODO : Cleanup
	//

	// Unsubclass the player
	UnsubclassPlayer();

	// Close the thread handle
	if(timeoutThreadHandle)
	{
		CloseHandle(timeoutThreadHandle);
		timeoutThreadHandle = NULL;
	}
}

//-----------------------------------------------------------------------------

void Configure(int flags)
{
	//
	// TODO : Show your "configuration" dialog.
	//
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_CONFIG), hwndPlayer, ConfigProc);
}

//-----------------------------------------------------------------------------

void About(int flags)
{
	//
	// TODO : Show your "about" dialog.
	//
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUT), hwndPlayer, AboutProc);
}

//-----------------------------------------------------------------------------
void Convert(const char* utf8String, TCHAR* buffer, unsigned int bufferSize)
{
#ifdef UNICODE
	QCDCallbacks->Service(opUTF8toUCS2, (char*)utf8String, (long)buffer, bufferSize);
#else
	lstrcpyn(buffer, utf8String, bufferSize);
#endif
}

void GetPluginPropertyFile(TCHAR* fileNameBuffer, unsigned int size)
{
	// Get the plugin ini file name
	char pluginPropFile[MAX_PATH];
	int byteSize = sizeof(pluginPropFile);
	QCDCallbacks->Service(opGetPluginSettingsFile, pluginPropFile, byteSize, 0);
	Convert(pluginPropFile, fileNameBuffer, size);
}


BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, 
							INT iValue, LPCTSTR lpFileName)
{
	// Convert the integer to a string
	TCHAR number[10];
	wsprintf(number, TEXT("%d"), iValue);

	// Write the value
	return WritePrivateProfileString(lpAppName, lpKeyName, number, lpFileName);
}

void LoadOptions()
{
	const TCHAR* defaultTemplate = TEXT("%A%N%T%N%D");

	// Get the plugin property file
	TCHAR pluginPropFile[MAX_PATH];
	GetPluginPropertyFile(pluginPropFile, MAX_PATH);

	// Load the options
	options.useCustomMessage = 
		GetPrivateProfileInt(pluginName, TEXT("useCustomMessage"), 0, pluginPropFile) != 0;
	GetPrivateProfileString(pluginName, TEXT("customTemplate"), defaultTemplate,
		options.customTemplate, options.templateSize, pluginPropFile);
	options.timeout = GetPrivateProfileInt(pluginName, TEXT("timeout"), 5000, pluginPropFile);
}

void SaveOptions()
{
	// Get the plugin property file
	TCHAR pluginPropFile[MAX_PATH];
	GetPluginPropertyFile(pluginPropFile, MAX_PATH);

	// Save the options
	WritePrivateProfileInt(pluginName, TEXT("useCustomMessage"), 
		options.useCustomMessage, pluginPropFile);
	WritePrivateProfileString(pluginName, TEXT("customTemplate"), 
		options.customTemplate, pluginPropFile);
	WritePrivateProfileInt(pluginName, TEXT("timeout"), options.timeout, pluginPropFile);
}

void SubclassPlayer()
{
	QMPCallback = (WNDPROC)SetWindowLongPtr(hwndPlayer, GWLP_WNDPROC, (LONG_PTR)SubclassProc);
}

void UnsubclassPlayer()
{
	SetWindowLongPtr(hwndPlayer, GWLP_WNDPROC, (LONG_PTR)QMPCallback);
}

void ShowBalloonTip(const TCHAR* text, const TCHAR* title)
{
	NOTIFYICONDATA iconData = {0};
	iconData.cbSize = sizeof(NOTIFYICONDATA);
	iconData.hWnd = hwndPlayer;
	iconData.uFlags = NIF_INFO;
	iconData.uID = 32771;
    iconData.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND;
	lstrcpy(iconData.szInfo, text);
	lstrcpy(iconData.szInfoTitle, title);

	Shell_NotifyIcon(NIM_MODIFY, &iconData);
}

void GenerateMessage(TCHAR* message, unsigned int messageSize, int trackIndex)
{
	if(messageSize == 0)
	{
		return;
	}

	// Workaround for opGetPlaylistString not accepting -1
	if(trackIndex == -1)
	{
        trackIndex = QCDCallbacks->Service(opGetCurrentIndex, 0, 0, 0);
	}

	// Clear the message buffer
    lstrcpy(message, TEXT(""));
	
	// Parse the custom template, if necessary
	if(options.useCustomMessage)
	{
		int templateLength = lstrlen(options.customTemplate);
		unsigned int templateOffset = 0;
		unsigned int messageLength = 0;
		while(templateOffset < templateLength)
		{
			TCHAR currentChar = options.customTemplate[templateOffset];

			if(currentChar == '%' && templateOffset < templateLength - 1 &&
				(options.customTemplate[templateOffset + 1] == 'A' ||
				options.customTemplate[templateOffset + 1] == 'T' ||
				options.customTemplate[templateOffset + 1] == 'D' ||
				options.customTemplate[templateOffset + 1] == 'P' ||
				options.customTemplate[templateOffset + 1] == 'N'))
			{
				TCHAR nextChar = options.customTemplate[templateOffset + 1];

				char metaData[100];
				TCHAR tMetaData[100];
				unsigned int sizeCount = 100;

				switch(nextChar)
				{
				case 'A':	// Artist
					{
						// Get the artist
						QCDCallbacks->Service(opGetArtistName, metaData, sizeCount, trackIndex);
												
						break;
					}

				case 'T':	// Track
					{
						// Get the track
						QCDCallbacks->Service(opGetTrackName, metaData, sizeCount, trackIndex);

						break;
					}

				case 'D':	// Album
					{
						// Get the disc name
						QCDCallbacks->Service(opGetDiscName, metaData, sizeCount, trackIndex);

						break;
					}

				case 'P':	// Playlist String
					{
						// Get the playlist string
						QCDCallbacks->Service(opGetPlaylistString, metaData, sizeCount, trackIndex);

						break;
					}

				case 'N':	// New Line
					{
						strcpy_s(metaData, "\r\n");

						break;
					}
				}

				// Convert if necessary
				Convert(metaData, tMetaData, sizeCount);

				// Add the artist to the message
				lstrcpyn(&message[messageLength], tMetaData, (messageSize - messageLength));
				messageLength = lstrlen(message);

				// Increment the template offset to skip the letter after the %
				++templateOffset;
			}
			else
			{
				if(messageLength < messageSize - 1)
				{
					TCHAR singleChar[2] = {currentChar, 0};
					lstrcpyn(&message[messageLength], singleChar, (messageSize - messageLength));
					++messageLength;
				}
			}
			
			++templateOffset;
		}
	}
	// Just get the playlist string
	else
	{
		char playlistString[255];
		int byteSize = sizeof(playlistString);
		QCDCallbacks->Service(opGetPlaylistString, playlistString, byteSize, trackIndex);
		Convert(playlistString, message, messageSize);
	}
}

void TrackUpdate(int trackIndex)
{
	TCHAR message[255];
	GenerateMessage(message, 255, trackIndex);
	ShowBalloonTip(TEXT(""), TEXT(""));
	ShowBalloonTip(message, TEXT("Quintessential Player Now Playing..."));
	if(options.timeout != 0)
	{
		++threadID;
		DWORD tempID = 0;
		if(timeoutThreadHandle)
		{
			CloseHandle(timeoutThreadHandle);
			timeoutThreadHandle = NULL;
		}
		timeoutThreadHandle = 
			CreateThread(NULL, 0, TimeoutProc, (LPVOID)&threadID, 0, &tempID);
	}
}

LRESULT CALLBACK SubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	// CD Track Changed Notification
	case WM_PN_TRACKCHANGED:
		{
			// Get the file name of the track
			const char* fileName = (const char*)lParam;

			// Get the index of this file
			int index = QCDCallbacks->Service(opGetIndexFromFilename, (void*)fileName, 0, 0);

			// Update
			TrackUpdate(index);

			break;
		}

	// New Non-CD Track Started Notification
	case WM_PN_PLAYSTARTED:
		{
			// Update
			TrackUpdate(-1);
			break;
		}

	// Track Information Changed Notification
	case WM_PN_INFOCHANGED:
		{
			// Only update if the player is playing and this is a stream
			if(QCDCallbacks->Service(opGetPlayerState, 0, 0, 0) == 2 &&
				QCDCallbacks->Service(opGetMediaType, 0, -1, 0) == DIGITAL_STREAM_MEDIA)
			{
				// Update
				TrackUpdate(-1);
			}
			break;
		}
	}

	return CallWindowProc(QMPCallback, hWnd, msg, wParam, lParam);
}

DWORD WINAPI TimeoutProc(LPVOID parameters)
{
	if(options.timeout != 0)
	{
		unsigned int* idPtr = (unsigned int*)parameters;
		unsigned int id = *idPtr;
		Sleep(options.timeout);
		if(threadID == id)
			ShowBalloonTip(TEXT(""), TEXT(""));
	}

	return 0;
}

INT_PTR CALLBACK AboutProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_BUTTON_ABOUTOK:
				{
					EndDialog(hWnd, TRUE);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


INT_PTR CALLBACK ConfigProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			// Set the controls to reflect the current options
			int radioToCheck = IDC_RADIO_CONFIGPLAYLIST;
			if(options.useCustomMessage)
				radioToCheck = IDC_RADIO_CONFIGCUSTOM;
            CheckRadioButton(hWnd, IDC_RADIO_CONFIGPLAYLIST, 
				IDC_RADIO_CONFIGCUSTOM, radioToCheck);
			SetDlgItemText(hWnd, IDC_EDIT_CONFIGCUSTOM, options.customTemplate);
			SetDlgItemInt(hWnd, IDC_EDIT_CONFIGTIMEOUT, options.timeout, FALSE);
			
			// Return
			return TRUE;
		}

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_BUTTON_CONFIGOK:
				{
					EndDialog(hWnd, TRUE);
					options.useCustomMessage = 
						IsDlgButtonChecked(hWnd, IDC_RADIO_CONFIGCUSTOM) == BST_CHECKED;
					GetDlgItemText(hWnd, IDC_EDIT_CONFIGCUSTOM, 
						options.customTemplate, options.templateSize);
					options.timeout = 
						GetDlgItemInt(hWnd, IDC_EDIT_CONFIGTIMEOUT, NULL, FALSE);
					SaveOptions();
					return TRUE;
				}
			case IDC_BUTTON_CONFIGCANCEL:
				{
					EndDialog(hWnd, FALSE);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}