//==============================================================================
/*
	Basic macro for logging (can be extended for other target builds; i.e, using
	NSLog for OS X / iOS). Could also be modified to log to a file instead of
	console.

	3/11/2014
    SDLTutorials.com
    Tim Jones
*/
//==============================================================================

#pragma once

#ifndef __LOG_H__
	#define __LOG_H__

	#ifndef _DEBUG
	#define _DEBUG 1
	#endif

	#include <cassert>
		#include <cstdio>

		#include <cstdarg>
		#include <iostream>
		#define Log(...) printf(__VA_ARGS__); printf("\n\n");
	#if !_DEBUG
		#define NDEBUG
	#endif

#endif
