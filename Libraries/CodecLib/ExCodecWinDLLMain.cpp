/*
 *  ExCodecWinDLLMain.cpp
 *  Daring CodecLib
 *
 *  Created by Tom Weatherhead on Wed Jul 09 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#define DLL_VER		1

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#ifdef DLL_VER
HINSTANCE hDLLInstance;			// Handle to this instance of the DLL.


BOOL APIENTRY DllMain( HANDLE hInst, DWORD dwReason, LPVOID lpReserved )
{
	//UNUSED( lpReserved );

	switch( dwReason )
	{
		case DLL_PROCESS_ATTACH:
			hDLLInstance = static_cast<HINSTANCE>( hInst );
#if 0 //def _DEBUG
			std::cout << "CodecLib attaching." << std::endl;
#endif
		case DLL_THREAD_ATTACH:
			return( TRUE );

		case DLL_PROCESS_DETACH:
#if 0 //def _DEBUG
			std::cout << "CodecLib detaching." << std::endl;
#endif
		case DLL_THREAD_DETACH:
			return( TRUE );
	}

	return( FALSE );
} // DllMain()
#endif


// **** End of File ****
