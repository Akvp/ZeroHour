#ifndef _LOG_H_
#define _LOG_H_

#include <cstdarg>

using namespace std;

	#define _DEBUG 1

	#if _DEBUG
		#include <iostream>
		void Log(...);
	#else
		#define NDEBUG
		#include <Windows.h>
		void Log(...);
	#endif

#endif