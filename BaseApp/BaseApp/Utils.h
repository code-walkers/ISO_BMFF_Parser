#ifndef _UTILS_H_
#define _UTILS_H_
#include <wx/wfStream.h>

void DEBUG_PRINT(wxFileOutputStream *out,const char *_format, ...);
int buff4ToInt(char *pbuff);
int buff2ToShort(char *pbuff);
long long int buff8ToInt64(char *pbuff);


#endif
