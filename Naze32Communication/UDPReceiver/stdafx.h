// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
//#include <stdio.h>
//#include <tchar.h>



// TODO: reference additional headers your program requires here
#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS 
	#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#endif

//#include <windows.h>
#include <string>
#include <thread>
#include <vector>
#include <assert.h>

using std::string;
using std::vector;


namespace network {
	enum {
		BUFFER_LENGTH = 1024,
	};
}

namespace common {
	namespace dbg {
		inline void Log(const char* fmt, ...)
		{
			// nothing
		}

		inline void ErrLog(const char* fmt, ...)
		{
			// nothing
		}
	}

	// elements를 회전해서 제거한다.
	template <class Seq>
	void rotatepopvector(Seq &seq, const unsigned int idx)
	{
		if ((seq.size() - 1) > idx)
			std::rotate(seq.begin() + idx, seq.begin() + idx + 1, seq.end());
		seq.pop_back();
	}
}


// 매크로 정의
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {if (p) { delete p; p=NULL;} }
#endif
#ifndef SAFE_DELETEA
#define SAFE_DELETEA(p) {if (p) { delete[] p; p=NULL;} }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) {if (p) { p->Release(); p=NULL;} }
#endif
#ifndef SAFE_RELEASE2
#define SAFE_RELEASE2(p) {if (p) { p->release(); p=NULL;} }
#endif
#ifndef DX_SAFE_RELEASE
#define DX_SAFE_RELEASE(p) {if (p) { p->Release(); p=NULL;} }
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#define RET(exp)		{if((exp)) return; }			// exp가 true이면 리턴
#define RET2(exp)		{if((exp)) {assert(0); return;} }			// exp가 true이면 리턴
#define RETV(exp,val)	{if((exp)) return val; }
#define RETV2(exp,val)	{if((exp)) {assert(0); return val;} }
#define ASSERT_RET(exp)		{assert(exp); RET(!(exp) ); }
#define ASSERT_RETV(exp,val)		{assert(exp); RETV(!(exp),val ); }
#define BRK(exp)		{if((exp)) break; }			// exp가 break


#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_
#endif

#ifndef _WINSOCK2API_
	#include <winsock2.h>
	#include <windows.h>
#endif

#include "../Common/timer.h"
#include "../Common/autocs.h"


#pragma comment( lib, "wsock32.lib" )
#pragma comment(lib, "Ws2_32.lib")

using namespace common;

