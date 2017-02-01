/*
 *  ExAPMinMax.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sun Jun 29 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPMINMAX_H_
#define _EXAPMINMAX_H_

// These functions require only operator<() to be implemented.


//template<class T> const T & min( const T & a, const T & b )


template<class T> T min( T a, T b )
{
	return( ( a < b ) ? a : b );
}


template<class T> T max( T a, T b )
{
	return( ( a < b ) ? b : a );
}


#endif // #ifndef _EXAPMINMAX_H_


// **** End of File ****
