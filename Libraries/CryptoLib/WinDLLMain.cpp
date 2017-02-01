/*
 *  WinDLLMain.cpp
 *  Daring CryptoLib
 *
 *  Created by Tom Weatherhead on Wed Aug 06 2003.
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
			hDLLInstance = HINSTANCE( hInst );
#if 0 //def _DEBUG
			std::cout << "CryptoLib attaching." << std::endl;
#endif
		case DLL_THREAD_ATTACH:
			return( TRUE );

		case DLL_PROCESS_DETACH:
#if 0 //def _DEBUG
			std::cout << "CryptoLib detaching." << std::endl;
#endif
		case DLL_THREAD_DETACH:
			return( TRUE );
	}

	return( FALSE );
} // DllMain()
#endif


// **** End of File ****
