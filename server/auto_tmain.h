/*
auto_tmain.h : ʹ���ֱ���������_tmain .
Author: zyl910
Blog: http://www.cnblogs.com/zyl910
URL: http://www.cnblogs.com/zyl910/archive/2013/01/17/tcharall.html
Version: V1.00
Updata: 2013-01-17


Update
~~~~~~

[2013-01-17] V1.00
* V1.0����.


Manual
~~~~~~

���ܵ�ʹ_tmain����.
ֻ������Դ�ļ��м���һ�С���
#include "auto_tmain.h"


���� VC��GCC��BCB��

�ο��� https://github.com/coderforlife/mingw-unicode-main/blob/master/mingw-unicode.c

*/


#ifndef __AUTO_TMAIN_H_INCLUDED
#define __AUTO_TMAIN_H_INCLUDED

#if defined(__GNUC__) && defined(_UNICODE)

#ifndef __MSVCRT__
#error Unicode main function requires linking to MSVCRT
#endif

#include <wchar.h>
#include <stdlib.h>
#include "tchar.h"

#undef _tmain
#ifdef _UNICODE
#define _tmain wmain
#else
#define _tmain main
#endif


extern int _CRT_glob;
extern
#ifdef __cplusplus
"C"
#endif
void __wgetmainargs(int*, wchar_t***, wchar_t***, int, int*);

#ifdef MAIN_USE_ENVP
int wmain(int argc, wchar_t* argv[], wchar_t* envp[]);
#else
int wmain(int argc, wchar_t* argv[]);
#endif

int main(void)
{
    wchar_t** enpv, ** argv;
    int argc = 0, si = 0;
    __wgetmainargs(&argc, &argv, &enpv, _CRT_glob, &si); // this also creates the global variable __wargv
#ifdef MAIN_USE_ENVP
    return wmain(argc, argv, enpv);
#else
    return wmain(argc, argv);
#endif    // #ifdef MAIN_USE_ENVP
}

#endif    // #if defined(__GNUC__) && defined(_UNICODE)

#endif    // #ifndef __AUTO_TMAIN_H_INCLUDED