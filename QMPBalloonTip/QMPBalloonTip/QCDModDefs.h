//-----------------------------------------------------------------------------
//
// File:    QCDModDefs.h
//
// About:   Module definitions file.  Miscellanious definitions used by different
//          module types.  This file is published with the plugin SDKs.
//
// Authors: Written by Paul Quinn and Richard Carlson.
//
// Copyright:
//
//  Quintessential Player Plugin Development Kit
//
//  Copyright (C) 1997-2004 Quinnware
//
//  This code is free.  If you redistribute it in any form, leave this notice 
//  here.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//-----------------------------------------------------------------------------

#ifndef QCDMODDEFS_H
#define QCDMODDEFS_H

#include <windows.h>

#ifdef __cplusplus
#define PLUGIN_API extern "C" __declspec(dllexport)
#else
#define PLUGIN_API __declspec(dllexport)
#endif

// Current plugin version

// use this version for old style API calls (all returned text in native encoding)
#define PLUGIN_API_VERSION              250

// use this version for new style API calls (all returned text in UTF8 encoding on WinNT/2K/XP (native encoding on Win9x))
#define PLUGIN_API_VERSION_WANTUTF8     ((PLUGIN_API_WANTUTF8<<16)|PLUGIN_API_VERSION)
#define PLUGIN_API_WANTUTF8             100

//-----------------------------------------------------------------------------

typedef struct 
{
    char        *moduleString;                  // display string for plugin
    char        *moduleExtensions;              // file extensions (color delimited)
    char        *moduleCategory;                // audio/video
} QCDModInfo;

#define MODINFO_VALID_DEFAULT           0x0     // always true, moduleString and moduleExtensions are valid fields
#define MODINFO_VALID_CATEGORY          0x100   // moduleCategory is a valid field


//-----------------------------------------------------------------------------
// Services (ops) provided by the Player
// 
// Parameters to service ops are buffer, param1, param2.
// Relevant parameters for each op listed with op. Unmentioned
// parameters for an op are unused (leave 0).
// 
// For parameters defined as type (char*), the string should be a UTF8 string
// if you specified PLUGIN_API_VERSION_WANTUTF8 for the plugin version AND
// the current system is Windows NT (NT/2000/XP)
//-----------------------------------------------------------------------------
typedef enum 
{
//----Player and Window States-------------------------------------------------

    opGetPlayerVersion = 0,         // Returns version (high word = major version (eg 3.01 is 3), low word = minor (eg 3.01 = 1))
                                    //      param1 = 1, returns build number

    opGetPlayerInstance = 2,        // Returns HINSTANCE to player executable
    opGetPlayerState = 9,           // Returns current state of the player (1 = stopped, 2 = playing, 3 = paused, 0 = failed))

    opGetAlwaysOnTop = 22,          // Returns 1 if player is set to 'Always on Top'

    opGetParentWnd = 1,             // Returns HWND to player window
    opGetExtensionWnd = 30,         // Returns handle to the draggable window extension (only available on some skins)
                                    //      param1 = extension number (0 - 9), param2 unused. 

    opGetMusicBrowserWnd = 32,      // Returns HWND to music browser window (returns 0 if window is closed)
    opGetSkinPreviewWnd = 33,       // Returns HWND to the skin preview window (returns 0 if window is closed)
    opGetPropertiesWnd = 34,        // Returns HWND to preferences window (returns 0 if window is closed)
    opGetAboutWnd = 36,             // Returns HWND to the about window (returns 0 if window is closed)
    opGetSegmentsWnd = 37,          // Returns HWND to the segments window (returns 0 if window is closed)
    opGetEQPresetsWnd = 38,         // Returns HWND to the EQ presets window (returns 0 if window is closed)

    opGetExtVisWnd = 31,            // Returns HWND to the external visual window (returns 0 if window is closed)
    opGetVisTarget = 21,            // Returns where visual effect is being drawn (0 - internal to skin, 1 - external window, 2 - fullscreen)
    opGetVisDimensions = 50,        // Returns width and height of visual window (HEIGHT in high word, WIDTH in low word )
                                    //      param1 = -1 current vis window, 0 internal vis, 1 external vis, 2 fullscreen

    opGetVideoWnd = 39,             // Returns HWND to the video window (where the actual video appears, internal to skin or in extenal window)
    opGetVideoTarget = 40,          // Returns where video is being drawn (0 - internal to skin, 1 - external window, 2 - fullscreen)
    opShowVideoWindow = 55,         // Show or Close video window
                                    //      param1 = 0 - close, 1 - create, 2 - show (create if needed), 3 - fullscreen
									//      (when param1 == 2) param2 = 0 - default, 1 - force embedded video, 2 - force external video (returns 0 if cannot show)
									//      (when param1 == 3) param2 = 0 - disable fullscreen, 1 - enable fullscreen

//-----Internet Connection State-----------------------------------------------

    opGetOffline = 20,              // Returns 1 if client is in Offline Mode (0 - client is online)
    opGetProxyInfo = 202,           // Returns 1 if proxy is being used
                                    //      buffer = ProxyInfo*

//-----Playlist Info/Manipulation----------------------------------------------

    opGetNumTracks = 10,            // Returns number of tracks in playlist
    opGetCurrentIndex = 11,         // Returns index of current track in playlist (0 based)
    opGetNextIndex = 12,            // Returns index of next track to play (0 based)
                                    //      param1 = start index. -1 for after current
                                    //      param2 = (int*)didCycleFlag, set to 1 if returned index is start of repeated playlist

    opGetTrackNum = 13,             // Returns track number of index
                                    //      param1 = index of track in playlist, -1 for current
                                    //          The 'track number' is the number of the track in it's respective album, as opposed to playlist number
                                    //          The track number for digital files will be 1 if the tag is not set or the file is not identified

    opGetTrackLength = 14,          // Returns track length
                                    //      param1 = index of track in playlist, -1 for current
                                    //      param2 = 0 for seconds, 1 for milliseconds

    opGetTime = 15,                 // Returns time on player (in seconds)
                                    //      param1 = 0 for time displayed, 1 for track time, 2 for playlist time
                                    //      param2 = 0 for elapsed, 1 for remaining

    opGetOutputTime = 29,           // Returns position of current track (in milliseconds)

    opGetTrackState = 16,           // Returns whether track is selected
                                    //      param1 = index of track, -1 for current

    opGetPlaylistNum = 17,          // Returns playlist number of index
                                    //      param1 = index of track in playlist, -1 for current

    opGetIndexFromPLNum = 28,       // Returns index from playlist number
                                    //      param1 = playlist number

    opGetIndexFromFilename = 210,   // Returns the index of a filename that exists in current playlist (-1 if not in playlist)
                                    //      buffer = (char*)full path of file
                                    //      param1 = startindex (index to start searching on)

    opUpdateIndex = 89,             // Forces player to update a track's TrackExtents (ie: to recalc file length)
                                    //      param1 = index

    opDeleteIndex = 90,             // Delete index from playlist
                                    //      param1 = index

    opSelectIndex = 91,             // Set index as selected
                                    //      param1 = index
                                    //      param2 = 1 - set, 0 - unset

    opBlockIndex = 92,              // Set index as blocked
                                    //      param1 = index
                                    //      param2 = 1 - set, 0 - unset

    opSetPlayNext = 1009,           // Set the next index to be played
                                    //      param1 = index, -1 unsets playnext

    opSetPlaylist = 1006,           // Clear current playlist, add files to playlist or load playlist with new files 
                                    //      buffer = (char*)file list (each file in quotes, string null terminated) Eg; buffer="\"file1.mp3\" \"file2.mp3\"\0" - NULL to clear playlist
                                    //      param1 = (char*)originating path (can be NULL if paths included with files) 
                                    //      param2 = 0 - add files, 1 - clear playlist flag, 2 - add to top of playlist, 4 - add unique files only (options can be OR'd together)

    opInsertPlaylist = 1011,        // Insert tracks into playlist 
                                    //      buffer = (char*)file list (each file in quotes, string null terminated) Eg; buffer="\"file1.mp3\" \"file2.mp3\"\0"
                                    //      param1 = (char*)originating path (can be NULL if paths included with files) 
                                    //      param2 = index location to insert tracks (-1 to insert at end)

    opMovePlaylistTrack = 1012,     // Move a track in playlist 
                                    //      param1 = index of track to move
                                    //      param2 = destination index (move shifts tracks between param1 and param2)

    opSwapPlaylistTracks = 1013,    // Swap two tracks in playlist
                                    //      param1 = index of first track
                                    //      param2 = index of second track (swap only switches indecies param1 and param2)

    opGetQueriesComplete = 60,      // Returns status on whether all tracks in playlist have been queryied for their info (1 yes, 0 no)

//----Playback control---------------------------------------------------------

    opSetSeekPosition = 1100,       // Seek to position during playback
                                    //     param1 = position
                                    //     param2 = 0 - position is in seconds, 1 - position is in milliseconds, 2 - position is in percent (use (float)param1))

    opGetRepeatState = 23,          // Returns playlist repeat state: 0 - repeat off, 1 - repeat track, 2 - repeat all
    opGetShuffleState = 27,         // Returns playlist shuffle state: 0 - shuffle off, 1 - shuffle enabled

    opSetRepeatState = 1110,        // Set playlist repeat state
                                    //      param1 = 0 - off, 1 - repeat track, 2 - repeat playlist

    opSetShuffleState = 1111,       // Set playlist shuffle state
                                    //      param1 = 0 - off, 1 - on

    opGetTimerState = 24,           // Returns timer display state
                                    //      (low word: 0 - track ascend, 1 - playlist ascend, 2 - track descend, 3 - playlist descend)
                                    //      (hi word: 1 if 'show hours' is set)

    opGetVolume = 25,               // Returns player volume level (0 - 100)
                                    //      param1: 0 = combined, 1 = left, 2 = right

    opSetVolume = 26,               // Set player volume level
                                    //      param1: vol level 0 - 100
                                    //      param2: balance (-100 left, 0 center, 100 right)

//----Track Info---------------------------------------------------------------

    opGetMediaType = 18,            // Returns media type of track
                                    //      param1 = index if track in playlist, -1 for current
                                    //          See MediaTypes below for return values

    opGetAudioInfo = 19,            // Returns format info about currently playing track
                                    //      param1 = 0 for samplerate, 1 for bitrate, 2 for num channels

    opGetMediaInfo = 99,            // Returns the ICddbDisc* object for the index specified
                                    //      param1 = index of track, -1 for current
                                    //      param2 = (int*)that receives track value
                                    //          Do not release or deallocate returned pointer

    opGetMediaID = 115,             // Returns media identifier
                                    //      buffer = (char*)buffer for media id
                                    //      param1 = size of buffer in bytes
                                    //      param2 = index of track in playlist, -1 for current
                                    //          for CD's it's the TOC - for anything else, right now it's 0

    opGetTrackFile = 103,           // Returns filename of track in playlist
                                    //      buffer = (char*)buffer for filename
                                    //      param1 = size of buffer in bytes
                                    //      param2 = index of track in playlist, -1 for current

//-----Current Skin Info-------------------------------------------------------

    opGetSkinName = 104,            // Returns current skin name
                                    //      buffer = (char*)buffer for filename
                                    //      param1 = size of buffer in bytes

    opGetSkinFile = 109,            // Returns current skin filename
                                    //      buffer = (char*)buffer for filename
                                    //      param1 = size of buffer in bytes

    opGetSkinMode = 118,            // Returns current skin mode

//-----Folder Settings---------------------------------------------------------

    opGetPluginFolder = 105,        // Returns current plugin folder
                                    //      buffer = (char*)buffer for folder
                                    //      param1 = size of buffer in bytes
									//      param2 = 0 - default plugin folder, 1 - DSP plugin folder

    opGetPluginSettingsFile = 106,  // Returns settings file (plugins.ini) that plugin should save settings to
                                    //      buffer = (char*)buffer for filename
                                    //      param1 = size of buffer in bytes

    opGetPluginCacheFile = 107,     // Returns file that describes plugin validity, functions and names
                                    //      buffer = (char*)buffer for filename
                                    //      param1 = size of buffer in bytes

    opGetPlayerSettingsFile = 108,  // Returns settings file (qcd.ini) that player saves it settings to (should use for read-only)
                                    //      buffer = (char*)buffer for filename
                                    //      param1 = size of buffer in bytes

    opGetSettingsFolder = 110,      // Returns folder suitable for settings
                                    //      buffer = (char*)buffer for folder
                                    //      param1 = size of buffer in bytes

    opGetPlaylistFolder = 111,      // Returns current playlist folder
                                    //      buffer = (char*)buffer for folder
                                    //      param1 = size of buffer in bytes

    opGetSkinFolder = 112,          // Returns current skin folder
                                    //      buffer = (char*)buffer for folder
                                    //      param1 = size of buffer in bytes

    opGetCurrentPlaylist = 114,     // Returns full pathname of playlist currently loaded 
                                    //      buffer = (char*)buffer for filename
                                    //      param1 = size of buffer in bytes

    opGetSupportedExtensions = 116, // Returns file extensions supported by the player
                                    //      buffer = (char*)buffer for extensions
                                    //      param1 = size of buffer in bytes
                                    //      param2 = 0 - get all extensions, 1 - get registered extensions
                                    //          extensions will be colon delimited

//-----Menu Handles------------------------------------------------------------

    opShowMainMenu = 120,           // Display main menu
                                    //      buffer = POINT* - location to display menu

    opGetMainMenu = 121,            // Returns copy of HMENU handle to QCD Menu (use DestroyMenu on handle when complete)

    opShowQuickTrack = 125,         // Display QuickTrack menu
                                    //      buffer = POINT* - location to display menu

    opGetQuickTrack = 126,          // Returns copy of HMENU handle to QuickTrack menu (must use DestroyMenu on handle when complete)
                                    //      To use if QuickTrack item selected: PostMessage(hwndPlayer, WM_COMMAND, menu_id, 0);

//-----EQ State----------------------------------------------------------------

    opGetEQVals = 200,              // Returns current EQ levels/on/off
                                    //      buffer = EQInfo* (see EQInfo below)

    opSetEQVals = 201,              // Sets EQ levels/on/off
                                    //      buffer = EQInfo* (see EQInfo below)

    opGetPlaybackEQ = 203,          // Returns if EQ is to be handled by playback plugin or not
	                                //      returns 1 - playback plugin eq, 0 not playback plugin eq (handled elsewhere)

//-----Browser, Status and Audio Info msgs-------------------------------------

    opSetStatusMessage = 1000,      // Sets message for status area
                                    //      buffer = (char*)msg buffer
                                    //      param1 = TEXT_* flags (see definitions below)

    opSetBrowserUrl = 1001,         // Set music browser URL 
                                    //      buffer = (char*)url (buffer = 0 closes browser)
                                    //      param1 = 0 - normal browser behavior, 1 - force browser open

    opSetAudioInfo = 1002,          // set the current music bitrate/khz
                                    //      buffer = AudioInfo* (see AudioInfo below)

//----Track Metadata-----------------------------------------------------------

    opGetTrackName = 100,           // Returns track name
                                    //      buffer = (char*)buffer for name
                                    //      param1 = size of buffer in bytes
                                    //      param2 = index of track in playlist, -1 for current

    opGetArtistName = 101,          // Returns artist name
                                    //      buffer = (char*)buffer for name
                                    //      param1 = size of buffer in bytes
                                    //      param2 = index of track in playlist, -1 for current

    opGetDiscName = 102,            // Returns disc name
                                    //      buffer = (char*)buffer for name
                                    //      param1 = size of buffer in bytes
                                    //      param2 = index of track in playlist, -1 for current

    opGetPlaylistString = 117,      // Returns string for index as it appears in playlist
                                    //      buffer = (char*)buffer for string
                                    //      param1 = size of buffer in bytes
                                    //      param2 = index

    opSetTrackAlbum = 1003,         // Set track ablum name
                                    //      buffer = (char*)album
                                    //      param1 = (char*)file name
                                    //      param2 = MediaTypes

    opSetTrackTitle = 1004,         // Set track title
                                    //      buffer = (char*)title
                                    //      param1 = (char*)file name
                                    //      param2 = MediaTypes

    opSetTrackArtist = 1005,        // Set track artist name
                                    //      buffer = (char*)artist
                                    //      param1 = (char*)file name
                                    //      param2 = MediaTypes

//----Track Params-------------------------------------------------------------

    opSetTrackExtents = 1007,       // Set track TrackExtents info
                                    //      buffer = TrackExtents* (see TrackExtents below)
                                    //      param1 = (char*)filename

    opSetTrackSeekable = 1008,      // Set track seekable flag
                                    //      buffer = (char*)filename
                                    //      param1 = 1 - can seek, 0 not seekable

    opSetIndexFilename = 1010,      // Set the filename (or URL) that an index in the current playlist refers to
                                    //      buffer = (char*)filename
                                    //      param1 = index

//----ICddbDisc----------------------------------------------------------------

    opCreateDiscInfo = 1020,        // Returns pointer to new ICddbDisc object. Do not release or deallocate this pointer
    opSetDiscInfo = 1021,           // Set CddbDisc info record for track
                                    //      buffer = ICddbDisc*
                                    //      param1 = MediaInfo* (see MediaInfo below)
                                    //      param2 = track number (reference number of track in ICddbDisc)

//----Plugin Menu, Preferences, and Acclerators--------------------------------

    opSetPluginMenuItem = 2000,     // Set entry for plugin in Plugin Menu
                                    //      buffer = HINSTANCE of plugin
                                    //      param1 = item id
                                    //      param2 = (char*)string to display
                                    //          param2 = 0 to remove 'item id' from menu
                                    //          param1 = 0 and param2 = 0 to remove whole menu

    opSetPluginMenuState = 2001,    // Set state of entry in Plugin Menu
                                    //      buffer = HINSTANCE of plugin
                                    //      param1 = item id
                                    //      param2 = menu flags (same as windows menu flags - eg: MF_CHECKED)

    opSetMainMenuItem = 2010,       // Set entry for plugin in Main Menu
                                    //      buffer = HINSTANCE of plugin
                                    //      param1 = item id
                                    //      param2 = (char*)string to display
                                    //          param2 = 0 to remove 'item id' from menu
                                    //          param1 = 0 and param2 = 0 to remove whole menu

    opSetMainMenuState = 2011,      // Set state of entry in Main Menu
                                    //      buffer = HINSTANCE of plugin
                                    //      param1 = item id
                                    //      param2 = menu flags (same as windows menu flags - eg: MF_CHECKED)

    opSetPluginPage = 2020,         // Set page in preferences for plugin (returns page id)
                                    //      buffer = PluginPrefPage*

    opRemovePluginPage = 2021,      // Remove page in preferences for plugin
                                    //      buffer = HINSTANCE of plugin
                                    //      param1 = prefPage id (return value from opSetPluginPage)

    opShowPluginPage = 2022,        // Display page in preferences
                                    //      buffer = HINSTANCE of plugin
                                    //      param1 = prefPage id (return value from opSetPluginPage)

    opSetAccelerator = 2030,        // Sets a local keyboard accelerator
                                    //      buffer = HINSTANCE of plugin
                                    //      param1 = item id
                                    //      param2 = AccelInfo* (see AccelInfo below)
                                    //          param2 = 0 to remove 'item id' from menu
                                    //          param1 = 0 and param2 = 0 to remove all plugin's accelerators

//----Filename templates-------------------------------------------------------

    opShowTemplateEditor = 2100,    // Display template editor dialog
                                    //      param1 = (HWND)parent window
                                    //      param2 = 1 - modal, 0 modeless

    opLoadTemplate = 2101,          // Loads saved template
                                    //      buffer = (char*)template buffer
                                    //      param1 = size of buffer in bytes
                                    //      param2 = index of template (index < 0 for default formats, index >= 0 for user made formats)

    opRenderTemplate = 2102,        // Render string based on template and metadata
                                    //      buffer = (char*)template
                                    //      param1 = FormatMetaInfo*
                                    //      param2 = (char*)string buffer (min 260 bytes)

//----Language and Resource Loading--------------------------------------------------

    opGetCurLangID = 3000,          // Returns current LANGID of set language in player

    opLoadResString = 3001,         // Load string resource from plugin with same language id as player
                                    //      buffer = (char*)string buffer
                                    //      param1 - size of buffer in bytes
                                    //      param2 - ResInfo* (see ResInfo below)

    opLoadResDialog = 3002,         // Load dialog resource from plugin with same language id as player
                                    // Returns LPCDLGTEMPLATE of dialog (use with DialogBoxIndirect, or CreateDialogIndirect)
                                    //      buffer - ResInfo* (see ResInfo below)

    opLoadResMenu = 3003,           // Load menu resource from plugin with same language id as player
                                    // Returns HMENU of loaded menu (call DestroyMenu when finished with handle)
                                    //      buffer - ResInfo* (see ResInfo below)

//----Unicode-UTF8 conversion--------------------------------------------------

    opUTF8toUCS2 = 9000,            // Convert UTF8 string to UCS2 (Unicode) string
                                    //      buffer = null terminated utf8 string
                                    //      param1 = (WCHAR*)result string buffer
                                    //      param2 = size of result buffer (int WCHARs)

    opUCS2toUTF8 = 9001,            // Convert UCS2 (Unicode) string to UTF8 string
                                    //      buffer = null terminated ucs2 string
                                    //      param1 = (char*)result string buffer
                                    //      param2 = size of result buffer (in bytes)

    opMBtoUCS2 = 9002,              // Convert MultiByte string to UCS2 (Unicode) string
                                    // The multibyte string will be UTF8 encoded on NT systems, and ACP encoded on Win9x systems
                                    // (ACP refers to 'active code page')
                                    //      buffer = null terminated multibyte string
                                    //      param1 = (WCHAR*)result string buffer
                                    //      param2 = size of result buffer (int WCHARs)

    opUCS2toMB = 9003,              // Convert UCS2 (Unicode) string to MultiByte string
                                    // The multibyte string will be UTF8 encoded on NT systems, and ACP encoded on Win9x systems
                                    // (ACP refers to 'active code page')
                                    //      buffer = null terminated ucs2 string
                                    //      param1 = (char*)result string buffer
                                    //      param2 = size of result buffer (in bytes)

//----Miscellaneous------------------------------------------------------------------

    opSafeWait = 10000,             // Plugins can use this to wait on an object without worrying about deadlocking the player.
                                    // This should only be called by the thread that enters the plugin, not by any plugin-created threads

    opGetAccountType = 11000,       // Returns current user's account permissions
                                    //     1 = Guest
									//     2 = User
									//     3 = Power User
									//     4 = Administrator (this value always returned on Win9x systems)

    opGetNetworkLogin = 11100       // Shows Network Login dialog
                                    // Returns 1 on OK, 0 on Cancel
                                    //     buffer = (char*)result str buffer (result will be formatted username:password)
                                    //     param1 = sizeof result buffer (in bytes)
                                    //     param2 = (char*)null separated string of initialization values (double null terminated)
                                    //         hostname\0realm\0username\0password\0\0

} PluginServiceOp;


//-----------------------------------------------------------------------------
// Info services api provided by the Player, called by Plugin.
//-----------------------------------------------------------------------------
typedef long (*PluginServiceFunc)(PluginServiceOp op, void *buffer, long param1, long param2);

// Use to retrieve service func for DSP plugins (or other inproc process that doesn't have access to PluginServiceFunc)
// Eg: PluginServiceFunc Service = (PluginServiceFunc)SendMessage(hwndPlayer, WM_GETSERVICEFUNC, 0, 0);
// Set WPARAM = PLUGIN_API_WANTUTF8 for UTF8 string parameters
#define WM_GETSERVICEFUNC               (WM_USER+1)


//-----------------------------------------------------------------------------
// Types for QCD callbacks
//-----------------------------------------------------------------------------


//----WriteDataStruct----------------------------------------------------------

typedef struct              // for Output Plugin Write callback
{
    void    *data;          // pointer to valid data
    int     bytelen;        // length of data pointed to by 'data' in bytes
    UINT    numsamples;     // number of samples represented by 'data'
    UINT    bps;            // bits per sample
    UINT    nch;            // number of channels
    UINT    srate;          // sample rate

    UINT    markerstart;    // Marker position at start of data (marker is time value of data) 
                            // (set to WAVE_VIS_DATA_ONLY to not have data sent to output plugins)
    UINT	markerend;      // Marker position at end of data (not currently used, set to 0)

} WriteDataStruct;

//----TrackExtents-------------------------------------------------------------

typedef struct              // for GetTrackExtents Input Plugin callback
{
    UINT track;             // for CD's, set the track number. Otherwise set to 1.
    UINT start;             // for CD's or media that doesn't start at the beginning 
                            // of the file, set to start position. Otherwise set to 0.
    UINT end;               // set to end position of media.
    UINT unitpersec;        // whatever units are being used for this media, how many
                            // of them per second. 
                            // (Note: ((end - start) / unitpersecond) = file length
    UINT bytesize;          // size of file in bytes (if applicable, otherwise 0).

} TrackExtents;

//----AudioInfo----------------------------------------------------------------
typedef struct              // for opSetAudioInfo service
{
    long struct_size;       // sizeof(AudioInfo)
    long level;             // MPEG level (1 for MPEG1, 2 for MPEG2, 3 for MPEG2.5, 7 for MPEGpro)
    long layer;             // and layer (1, 2 or 3)
    long bitrate;           // audio bitrate in bits per second
    long frequency;         // audio freq in Hz
    long mode;              // 0 for stereo, 1 for joint-stereo, 2 for dual-channel, 3 for mono, 4 for multi-channel
    char text[8];           // up to eight characters to identify format (will override level and layer settings)

} AudioInfo;


//-----EQInfo------------------------------------------------------------------

typedef struct              // for EQ settings
{
    long struct_size;       // sizeof(EQInfo)
    char enabled;
    char preamp;            // -128 to 127, 0 is even
    char bands[10];         // -128 to 127, 0 is even

} EQInfo;

//----ProxyInfo----------------------------------------------------------------

typedef struct              // for opGetProxyInfo
{
    long struct_size;       // sizeof(ProxyInfo)
    char hostname[200];
    long port;
    char username[100];
    char password[100];
    long usePort80Only;

} ProxyInfo;

//----MediaTypes---------------------------------------------------------------

typedef enum                // for MediaInfo.mediaType
{
    UNKNOWN_MEDIA = 0,
    CD_AUDIO_MEDIA = 1,
    DIGITAL_FILE_MEDIA = 2,
    DIGITAL_AUDIOFILE_MEDIA = 2,
    DIGITAL_STREAM_MEDIA = 3,
    DIGITAL_VIDEOFILE_MEDIA = 4

} MediaTypes;

//----MediaInfo----------------------------------------------------------------

#define MAX_TOC_LEN         2048
typedef struct
{
    // media descriptors
    char        mediaFile[MAX_PATH];
    MediaTypes  mediaType;

    // cd audio media info
    char        cd_mediaTOC[MAX_TOC_LEN];
    int         cd_numTracks;
    int         cd_hasAudio;

    // operation info
    int         op_canSeek;

    // not used
    int         reserved[4];

} MediaInfo;

//----FormatMetaInfo-----------------------------------------------------------

typedef struct
{
    long        struct_size;        // sizeof(FormatMetaInfo)
    LPCWSTR     title;
    LPCWSTR     artalb;
    LPCWSTR     album;
    LPCWSTR     genre;
    LPCWSTR     year;
    LPCWSTR     tracknum;
    LPCWSTR     filename;
    LPCWSTR     arttrk;
    LPCWSTR     plnum;
    LPCWSTR     time;
    long        reserved;

} FormatMetaInfo;

//----PluginPrefPage-----------------------------------------------------------

typedef struct                      // for opSetPluginPage
{
    long        struct_size;        // sizeof(PluginPrefPage)
    HINSTANCE   hModule;            // plugin HINSTANCE
    LPCWSTR     lpTemplate;         // dialog resource template
    DLGPROC     lpDialogFunc;       // dialog window proc
    LPCWSTR     lpDisplayText;      // preference item display text
    long        nCategory;          // type of dialog (see PREFPAGE_CATEGORY_* below)

} PluginPrefPage;

// PluginPrefPage::nCategory values
#define PREFPAGE_CATEGORY_GENERAL				0x000	// use this value if unsure
#define PREFPAGE_CATEGORY_SYSTEM				0x100
#define PREFPAGE_CATEGORY_PLAYER				0x200
#define PREFPAGE_CATEGORY_ENCODER				0x300	

#define PREFPAGE_CATEGORY_SKINS					0x500
#define PREFPAGE_CATEGORY_SKINBROWSER			0x600

#define PREFPAGE_CATEGORY_PLUGINS				0x700
#define PREFPAGE_CATEGORY_PLUGINTYPES			0x800

#define PREFPAGE_CATEGORY_ENCODEFORMAT			0x1000	// use this value for encoder configuration pages

//----ResInfo------------------------------------------------------------------

typedef struct
{
    long        struct_size;        // sizeof(ResInfo)
    HINSTANCE   hModule;            // plugin HINSTANCE
    LPCWSTR     resID;              // resource id of resource
    long        langid;             // requested lang id of resource (0 for current player lang)
    long        reserved;  

} ResInfo;

//----AccelInfo----------------------------------------------------------------

typedef struct
{
    long        struct_size;        // sizeof(AccelInfo)
    short       modifiers;          // zero or more of the AI_* values below
    short       key;                // Specifies the accelerator key. This member can be either a virtual-key code or a character code.
	                                //     If virtual-key code, modifiers must include AI_VIRTKEY
	                                //     (See Windows SDK for virtual-key definitions)
} AccelInfo;

// AccelInfo:modifiers values
#define AI_VIRTKEY  0x01
#define AI_SHIFT    0x04
#define AI_CONTROL  0x08
#define AI_ALT      0x10

//-----------------------------------------------------------------------------
// When subclassing the parent window, a plugin can watch for these messages
// to react to events going on between plugins and player
// DO NOT SEND THESE MESSAGES - can only watch for them

// Plugin to Player Notifiers
#define WM_PN_PLAYBACKPROGRESS      (WM_USER+100) // playback progress updated
#define WM_PN_PLAYSTARTED           (WM_USER+101) // playback has started
#define WM_PN_PLAYSTOPPED           (WM_USER+102) // playback has stopped by user
#define WM_PN_PLAYPAUSED            (WM_USER+103) // playback has been paused
#define WM_PN_PLAYDONE              (WM_USER+104) // playback has finished (track completed)

#define WM_PN_MEDIAEJECTED          (WM_USER+105) // a CD was ejected (CDRom drive letter= 'A' + lParam)
#define WM_PN_MEDIAINSERTED         (WM_USER+106) // a CD was inserted (CDRom drive letter= 'A' + lParam)
#define WM_PN_INFOCHANGED           (WM_USER+107) // track information was updated (lParam = (LPCSTR)medianame)
#define WM_PN_TRACKCHANGED          (WM_USER+109) // current track playing has changed (relevant from CD plugin) (lParam = (LPCSTR)medianame)

#define WM_PN_ENCODEPROGRESS        (WM_USER+113) // encoding progress updated
#define WM_PN_ENCODESTARTED         (WM_USER+114) // encoding has started
#define WM_PN_ENCODESTOPPED         (WM_USER+115) // encoding has stopped by user
#define WM_PN_ENCODEPAUSED          (WM_USER+116) // encoding has been paused
#define WM_PN_ENCODEDONE            (WM_USER+117) // encoding has finished (track completed)

// Player to Plugin Notifiers
#define WM_PN_PLAYLISTCHANGED       (WM_USER+200) // playlist has changed in some way (add, delete, sort, shuffle, drag-n-drop, etc...)
#define WM_PN_DIALOGSAVE            (WM_USER+300) // sent to all dialogs set in Preferences. Indicates preferences should be saved (closing)

// For intercepting main menu display
// (so you can get handle, modify, and display your own)
#define WM_SHOWMAINMENU             (WM_USER+20)

// For intercepting skinned border window commands
#define WM_BORDERWINDOW             (WM_USER+26)
// WM_BORDERWINDOW	wParam's
#define BORDERWINDOW_NORMALSIZE     0x100000
#define BORDERWINDOW_DOUBLESIZE     0x200000
#define BORDERWINDOW_FULLSCREEN     0x400000

// send to border window to cause resize
// wParam = LPPOINT lpp; // point x-y is CLIENT area size of window
#define WM_SIZEBORDERWINDOW         (WM_USER+1)

//-----------------------------------------------------------------------------
// To shutdown player, send this command
#define WM_SHUTDOWN                 (WM_USER+5)

//-----------------------------------------------------------------------------
// opSetStatusMessage textflags
#define TEXT_DEFAULT                0x0         // message scrolls by in status window
#define TEXT_TOOLTIP                0x1         // message acts as tooltip in status window
#define TEXT_URGENT                 0x2         // forces message to appear even if no status window (using msg box)
#define TEXT_HOLD                   0x4         // tooltip message stays up (no fade out)
#define TEXT_UNICODE                0x10        // buffer contains a unicode string (multibyte string otherwise)


#endif //QCDMODDEFS_H