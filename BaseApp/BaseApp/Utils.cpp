#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Utils.h"

//#define WRITE_TO_STDERR (1)


void DEBUG_PRINT(wxFileOutputStream *out,const char *_format, ...)
{
#if defined(WRITE_TO_STDERR)
	va_list argptr;
	va_start(argptr, _format);
	vprintf(_format, argptr);
	va_end(argptr);
#else
	//Write to specific file
	char buffer[200];
	va_list argptr;
	va_start(argptr, _format);
	vsprintf(buffer,_format, argptr);
	va_end(argptr);

	out->Write(buffer, strlen(buffer));
#endif
	return;
}




int buff4ToInt(char *pbuff)
{
	int ret = 0;
	ret = ((pbuff[0] << 24) & 0xFF000000) |
		((pbuff[1] << 16) & 0x00FF0000) |
		((pbuff[2] << 8) & 0x0000FF00) |
		((pbuff[3]) & 0x000000FF);
	return ret;
}


int buff2ToShort(char *pbuff)
{
	short ret = 0;
	ret = ((pbuff[0] << 8) & 0x0000FF00) |
		((pbuff[1]) & 0x000000FF);
	return ret;
}


long long int buff8ToInt64(char *pbuff)
{
	long long int ret = 0;

	ret = ((pbuff[0] << 56) & 0xFF00000000000000) |
		((pbuff[1] << 48) & 0x00FF000000000000) |
		((pbuff[2] << 40) & 0x0000FF0000000000) |
		((pbuff[3] << 32) & 0x000000FF00000000) |
		((pbuff[4] << 24) & 0x00000000FF000000) |
		((pbuff[5] << 16) & 0x0000000000FF0000) |
		((pbuff[6] << 8) & 0x000000000000FF00) |
		((pbuff[7]) & 0x00000000000000FF);

	return ret;
}