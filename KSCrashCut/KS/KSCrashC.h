#ifndef HDR_KSCrashC_h
#define HDR_KSCrashC_h

#ifdef __cplusplus
extern "C" {
#endif


#include "KSCrashType.h"
#include "KSCrashReportWriter.h"

#include <stdbool.h>

KSCrashType kscrash_install(const char* appName, const char* const installPath);
KSCrashType kscrash_setHandlingCrashTypes(KSCrashType crashTypes);

void kscrash_setCrashNotifyCallback(const KSReportWriteCallback onCrashNotify);
    
    void ks_reinstsallMach();

#ifdef __cplusplus
}
#endif

#endif // HDR_KSCrashC_h
