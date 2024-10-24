#pragma once

#include <windows.h>

#if _MSC_VER<1400
#pragma warning(disable: 4616)  //warning C4616: #pragma warning : warning number '6255' out of range, must be between '4001' and '4999'
#endif

// Define SAL macros to be empty if some old Visual Studio used
#ifndef __reserved 
#define __reserved
#endif
#ifndef __in
#define __in
#endif
#ifndef __in_z
#define __in_z
#endif
#ifndef __in_opt
#define __in_opt
#endif
#ifndef __out_opt
#define __out_opt
#endif
#ifndef __out_ecount_z
#define __out_ecount_z(x)
#endif
#ifndef __deref_out
#define __deref_out
#endif
