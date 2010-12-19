#ifndef _TYPEDEF_H
#define _TYPEDEF_H

typedef BYTE unsigned char;
typedef WORD unsigned short;
typedef DWORD unsigned int;
typedef LWORD unsigned long long;

#define LEN_BYTE sizeof(BYTE)
#define LEN_WORD sizeof(WORD)
#define LEN_DWORD sizeof(DWORD)
#define LEN_LWORD sizeof(LWORD)

#define LEN_HEADER (LEN_BYTE+LEN_WORD)

#endif
