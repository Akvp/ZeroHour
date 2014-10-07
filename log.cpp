#include "log.h"

void Log(...)
{
	va_list arglist;
	va_start(arglist, arg);

#if _DEBUG
	cout << arg << ' ';
	va_end(arglist);
	cout << endl;

#else
	MessageBox(NULL, arg, "Error", MB_ICON_ERROR);
	va_end(arglist);
#endif
}