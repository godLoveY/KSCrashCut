#ifndef HDR_KSCrashContext_h
#define HDR_KSCrashContext_h

#ifdef __cplusplus
extern "C" {
#endif


#include "KSCrashType.h"
#include "KSCrashSentry_Context.h"
#include "KSCrashReportWriter.h"

#include <stdbool.h>
#include <dlfcn.h>

typedef struct
{
    KSCrashType handlingCrashTypes;
    KSReportWriteCallback onCrashNotify;
} KSCrash_Configuration;

//add by yao
typedef struct
{
    uintptr_t backtraceBuffer[150];
    Dl_info symbolicated[150];
    int backtraceLength;
} Crash_ParseResult;

typedef struct
{
    KSCrash_Configuration config;
    KSCrash_SentryContext crash;
    Crash_ParseResult parseResult;
} KSCrash_Context;


#ifdef __cplusplus
}
#endif

#endif // HDR_KSCrashContext_h
