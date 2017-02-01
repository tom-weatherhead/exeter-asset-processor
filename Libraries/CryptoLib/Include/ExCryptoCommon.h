// ExCryptoCommon.h - Derived from Helix
// Copyright (c) 2002 by Tom Weatherhead.  All rights reserved.
// Started November 28, 2002


#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


// Standard includes.

//#include <string>
#include <vector>
#include <ctime>


// Custom includes.

#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
#include "UtilLib\\ExUtilFileWriterBaseInterface.h"
#include "UtilLib\\ExUtilVersionInterface.h"
//#include "Exception.h"
//#include "ExCryptoBigNumInterface.h"
//#include "ExCryptoBigNumVariableLength.h"
#include "ExCryptoBigNumFixedLength.h"
#include "ExCryptoRSAKey.h"
#include "ExCryptoUUCode.h"
//#include "ExCryptoHelixApp.h"


// Using directives.

using std::vector;


// **** End of File ****
