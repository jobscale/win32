///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

char * GetNextArgument(char *source,char *destvar,int varlength,char *destval,int vallength)
{
	char prevchar='\0';
	int destlen=0;

	if (!source || !destvar || !destval)
	{
		return((char *)0);
	}

	if (!*source)
	{
		return((char *)0);
	}

	*destvar = '\0';
	*destval = '\0';

	// length = maximum size of dest
	while (*source && !(*source == '&' && prevchar != '\\') && !(*source == '=' && prevchar != '\\') && destlen<varlength)
	{
		if (*source!='\\' || (*source == '\\' && prevchar == '\\'))
		{
			*destvar = *source;
			destvar++;
			destlen++;
		}

		if (*source == '\\' && prevchar == '\\')
		{
			prevchar=0;
		} else {
			prevchar=*source;
		}

		source++;
	}
	*destvar='\0';

	destlen=0;

	if (*source == '=')
	{
		// We have a value. Excellent.
		source++;
	
		while (*source && !(*source == '&' && prevchar != '\\') && destlen<vallength) {
			if (*source!='\\' || (*source == '\\' && prevchar == '\\')) {
				*destval=*source;
				destval++;
				destlen++;
			}

			if (*source == '\\' && prevchar == '\\') {
				prevchar = 0;
			} else {
				prevchar = *source;
			}
			source++;
		}
		*destval = '\0';
	}

	// Return our position in the new string, or null for end of string.
	if (*source) {
		source++;
	}

	return source;
}
