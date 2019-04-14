//-----------------------------------------------------------------------------
//
// File:	QCDModGeneral
//
// About:	General plugin module interface.  This file is published with the 
//			General plugin SDK.
//
// Authors:	Written by Paul Quinn and Richard Carlson.
//
// Copyright:
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

#ifndef QCDMODGENERAL_H
#define QCDMODGENERAL_H

#include "QCDModDefs.h"

// name of the DLL export for output plugins
#define GENERALDLL_ENTRY_POINT	QGeneralModule

//-----------------------------------------------------------------------------

typedef struct 
{
	UINT				size;			// size of init structure
	UINT				version;		// plugin structure version (set to PLUGIN_API_VERSION)
	PluginServiceFunc	Service;		// player supplied services callback

	struct
	{
		void *Reserved[2];
	} toPlayer;

	struct 
	{
		void (*ShutDown)(int flags);
		void (*Configure)(int flags);
		void (*About)(int flags);
		void *Reserved[2];
	} toModule;
} QCDModInitGen;

#endif //QCDMODGENERAL_H