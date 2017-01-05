#ifndef HDR_KSCrashType_h
#define HDR_KSCrashType_h

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    KSCrashTypeMachException      = 0x01,
    KSCrashTypeSignal             = 0x02,
    KSCrashTypeCPPException       = 0x04,
    KSCrashTypeNSException        = 0x08,
    KSCrashTypeMainThreadDeadlock = 0x10,
    KSCrashTypeUserReported       = 0x20,
} KSCrashType;

#define KSCrashTypeAll              \
(                                   \
    KSCrashTypeMachException      | \
    KSCrashTypeSignal             | \
    KSCrashTypeCPPException       | \
    KSCrashTypeNSException        | \
    KSCrashTypeMainThreadDeadlock | \
    KSCrashTypeUserReported         \
)

#define KSCrashTypeAsyncSafe        \
(                                   \
    KSCrashTypeMachException      | \
    KSCrashTypeSignal               \
)
#define KSCrashTypeProductionSafe2 (KSCrashTypeAll)
#define KSCrashTypeNone 0

#ifdef __cplusplus
}
#endif

#endif // HDR_KSCrashType_h
