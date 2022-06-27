/*
prichar.h : �ַ����ĸ�ʽ�����ַ�.
Author: zyl910
Blog: http://www.cnblogs.com/zyl910
URL: http://www.cnblogs.com/zyl910/archive/2013/01/17/tcharall.html
Version: V1.00
Updata: 2013-01-17


���Թ��ı�����--
VC: 6, 2003, 2005, 2008, 2010, 2012.
BCB: 6.
GCC: 4.7.1(MinGW-w64), 4.7.0(Fedora 17), 4.6.2(MinGW), llvm-gcc-4.2(Mac OS X Lion 10.7.4, Xcode 4.4.1).


Update
~~~~~~

[2013-01-17] V1.00
* V1.0����.


Manual
~~~~~~

�ο���C99�ġ�inttypes.h����Ϊ�ַ�������Ƶĸ�ʽ�ַ�����

ǰ׺--
PRI: print, ���.
SCN: scan, ����.

��׺--
c: char, �ַ�.
s: string, �ַ���.

��׺--
A: char, խ�ַ���.
W: wchar_t, ���ַ���.
T: TCHAR, TCHAR��.


*/


#ifndef __PRICHAR_H_INCLUDED
#define __PRICHAR_H_INCLUDED


//#include "tchar.h"

#if defined __cplusplus
extern "C" {
#endif

    ////
    // char
    ////

#if defined(_MSC_VER)||defined(__BORLANDC__)
    // VC��BCB ��֧��hc/hs���Ǵ���խ�ַ�.
#define PRIcA    "hc"
#define PRIsA    "hs"
#elif defined(__GNUC__)||defined(_WIN32)||defined(_WIN64)
    // GCC��խ�溯����ʱ�޷�ʶ��hc/hs, ����溯������֧��hc/hs. ��������Windowsƽ̨�ı�����Ҳ������.
#if defined(_UNICODE)
#define PRIcA    "hc"
#define PRIsA    "hs"
#else
#define PRIcA    "c"
#define PRIsA    "s"
#endif
#else
    // �ٶ�����ƽֻ̨֧��c/s.
#define PRIcA    "c"
#define PRIsA    "s"
#endif


////
// wchar_t
////

// C99��׼�涨lc/ls���Ǵ�����ַ�.
#define PRIcW    "lc"
#define PRIsW    "ls"


////
// TCHAR
////

#if defined(_WIN32)||defined(_WIN64)||defined(_MSC_VER)
    // VC��BCB��MinGW��Windowsƽ̨�ϵı�����֧��cΪ����Ӧ, ����խ�ַ�������char, ���ڿ��ַ�������wchar_t.
#define PRIcT    "c"
#define PRIsT    "s"
#else
    // ����ƽ̨.
#if defined(_UNICODE)
#define PRIcT    PRIcW
#define PRIsT    PRIsW
#else
#define PRIcT    PRIcA
#define PRIsT    PRIsA
#endif
#endif


////
// SCN
////

#define SCNcA    PRIcA
#define SCNsA    PRIsA
#define SCNcW    PRIcW
#define SCNsW    PRIsW
#define SCNcT    PRIcT
#define SCNsT    PRIsT


#if defined __cplusplus
};
#endif

#endif    // #ifndef __PRICHAR_H_INCLUDED