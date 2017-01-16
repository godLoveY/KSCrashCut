#ifndef HDR_KSCrashReport_h
#define HDR_KSCrashReport_h

#ifdef __cplusplus
extern "C" {
#endif


#include "KSCrashContext.h"


void writeAllThreads(const KSCrashReportWriter* const writer,
                                const char* const key,
                                const KSCrash_SentryContext* const crash,
                                bool writeNotableAddresses,
                                bool searchThreadNames,
                            bool searchQueueNames);
    
    void printCallStack(thread_t thread);

#ifdef __cplusplus
}
#endif

#endif // HDR_KSCrashReport_h
