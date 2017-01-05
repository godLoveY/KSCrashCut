#ifndef HDR_KSCrashReportWriter_h
#define HDR_KSCrashReportWriter_h

#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>
#include <stdint.h>


typedef struct KSCrashReportWriter
{
    void* context;

} KSCrashReportWriter;

typedef void (*KSReportWriteCallback)(const KSCrashReportWriter* writer);


#ifdef __cplusplus
}
#endif

#endif // HDR_KSCrashReportWriter_h

