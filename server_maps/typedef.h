#ifndef _TYPEDEF_H
#define _TYPEDEF_H

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned long long LWORD;

#define LEN_BYTE sizeof(BYTE)
#define LEN_WORD sizeof(WORD)
#define LEN_DWORD sizeof(DWORD)
#define LEN_LWORD sizeof(LWORD)

#define LEN_HEADER (LEN_BYTE+LEN_WORD)

#endif
