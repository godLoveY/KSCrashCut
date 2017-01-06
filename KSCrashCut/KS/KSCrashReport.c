#include "KSCrashReport.h"

#include "KSBacktrace_Private.h"
#include "KSCrashReportWriter.h"
#include "KSDynamicLinker.h"
#include "KSCPU.h"
#include "KSMach.h"
#include "KSThread.h"
#include "KSSignalInfo.h"

#include <errno.h>
#include <fcntl.h>
#include <mach/mach.h>
#include <mach-o/dyld.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#ifdef __arm64__
    #include <sys/_types/_ucontext64.h>
    #define UC_MCONTEXT uc_mcontext64
    typedef ucontext64_t SignalUserContext;
#else
    #define UC_MCONTEXT uc_mcontext
    typedef ucontext_t SignalUserContext;
#endif


// ============================================================================
#pragma mark - Constants -
// ============================================================================

/** Maximum depth allowed for a backtrace. */
#define kMaxBacktraceDepth 150

/** Default number of objects, subobjects, and ivars to record from a memory loc */
#define kDefaultMemorySearchDepth 15

/** Length at which we consider a backtrace to represent a stack overflow.
 * If it reaches this point, we start cutting off from the top of the stack
 * rather than the bottom.
 */
#define kStackOverflowThreshold 200

/** Maximum number of lines to print when printing a stack trace to the console.
 */
#define kMaxStackTracePrintLines 40

/** How far to search the stack (in pointer sized jumps) for notable data. */
#define kStackNotableSearchBackDistance 20
#define kStackNotableSearchForwardDistance 10

/** How much of the stack to dump (in pointer sized jumps). */
#define kStackContentsPushedDistance 20
#define kStackContentsPoppedDistance 10
#define kStackContentsTotalDistance (kStackContentsPushedDistance + kStackContentsPoppedDistance)

/** The minimum length for a valid string. */
#define kMinStringLength 4


// ============================================================================
#pragma mark - Formatting -
// ============================================================================

#if defined(__LP64__)
    #define TRACE_FMT         "%-4d%-31s 0x%016lx %s + %lu"
    #define POINTER_FMT       "0x%016lx"
    #define POINTER_SHORT_FMT "0x%lx"
#else
    #define TRACE_FMT         "%-4d%-31s 0x%08lx %s + %lu"
    #define POINTER_FMT       "0x%08lx"
    #define POINTER_SHORT_FMT "0x%lx"
#endif


// ============================================================================
#pragma mark - Utility -
// ============================================================================

/** Get all parts of the machine state required for a dump.
 * This includes basic thread state, and exception registers.
 *
 * @param thread The thread to get state for.
 *
 * @param machineContextBuffer The machine context to fill out.
 */
static bool fetchMachineState(const thread_t thread, STRUCT_MCONTEXT_L* const machineContextBuffer)
{
    if(!kscpu_threadState(thread, machineContextBuffer))
    {
        return false;
    }

    if(!kscpu_exceptionState(thread, machineContextBuffer))
    {
        return false;
    }

    return true;
}

/** Get the machine context for the specified thread.
 *
 * This function will choose how to fetch the machine context based on what kind
 * of thread it is (current, crashed, other), and what kind of crash occured.
 * It may store the context in machineContextBuffer unless it can be fetched
 * directly from memory. Do not count on machineContextBuffer containing
 * anything. Always use the return value.
 *
 * @param crash The crash handler context.
 *
 * @param thread The thread to get a machine context for.
 *
 * @param machineContextBuffer A place to store the context, if needed.
 *
 * @return A pointer to the crash context, or NULL if not found.
 */
static STRUCT_MCONTEXT_L* getMachineContext(const KSCrash_SentryContext* const crash,
                                            const thread_t thread,
                                            STRUCT_MCONTEXT_L* const machineContextBuffer)
{
    if(thread == crash->offendingThread)
    {
        if(crash->crashType == KSCrashTypeSignal)
        {
            return ((SignalUserContext*)crash->signal.userContext)->UC_MCONTEXT;
        }
    }

    if(thread == ksthread_self())
    {
        return NULL;
    }

    if(!fetchMachineState(thread, machineContextBuffer))
    {
        KSLOG_ERROR("Failed to fetch machine state for thread %d", thread);
        return NULL;
    }

    return machineContextBuffer;
}

/** Get the backtrace for the specified thread.
 *
 * This function will choose how to fetch the backtrace based on machine context
 * availability andwhat kind of crash occurred. It may store the backtrace in
 * backtraceBuffer unless it can be fetched directly from memory. Do not count
 * on backtraceBuffer containing anything. Always use the return value.
 *
 * @param crash The crash handler context.
 *
 * @param thread The thread to get a machine context for.
 *
 * @param machineContext The machine context (can be NULL).
 *
 * @param backtraceBuffer A place to store the backtrace, if needed.
 *
 * @param backtraceLength In: The length of backtraceBuffer.
 *                        Out: The length of the backtrace.
 *
 * @param skippedEntries Out: The number of entries that were skipped due to
 *                             stack overflow.
 *
 * @return The backtrace, or NULL if not found.
 */
static uintptr_t* getBacktrace(const KSCrash_SentryContext* const crash,
                               const thread_t thread,
                               const STRUCT_MCONTEXT_L* const machineContext,
                               uintptr_t* const backtraceBuffer,
                               int* const backtraceLength,
                               int* const skippedEntries)
{
    if(thread == crash->offendingThread)
    {
        if(crash->stackTrace != NULL &&
           crash->stackTraceLength > 0 &&
           (crash->crashType & (KSCrashTypeCPPException | KSCrashTypeNSException | KSCrashTypeUserReported)))
        {
            *backtraceLength = crash->stackTraceLength;
            return crash->stackTrace;
        }
    }

    if(machineContext == NULL)
    {
        return NULL;
    }

    int actualSkippedEntries = 0;
    int actualLength = ksbt_backtraceLength(machineContext);
    if(actualLength >= kStackOverflowThreshold)
    {
        actualSkippedEntries = actualLength - *backtraceLength;
    }

    *backtraceLength = ksbt_backtraceThreadState(machineContext,
                                                 backtraceBuffer,
                                                 actualSkippedEntries,
                                                 *backtraceLength);
    if(skippedEntries != NULL)
    {
        *skippedEntries = actualSkippedEntries;
    }
    return backtraceBuffer;
}

/** Check if the stack for the specified thread has overflowed.
 *
 * @param crash The crash handler context.
 *
 * @param thread The thread to check.
 *
 * @return true if the thread's stack has overflowed.
 */
static bool isStackOverflow(const KSCrash_SentryContext* const crash,
                             const thread_t thread)
{
    STRUCT_MCONTEXT_L concreteMachineContext;
    STRUCT_MCONTEXT_L* machineContext = getMachineContext(crash,
                                                          thread,
                                                          &concreteMachineContext);
    if(machineContext == NULL)
    {
        return false;
    }

    return ksbt_isBacktraceTooLong(machineContext, kStackOverflowThreshold);
}

#pragma mark Thread-specific


/** Write information about a thread to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param crash The crash handler context.
 *
 * @param thread The thread to write about.
 *
 * @param index The thread's index relative to all threads.
 *
 * @param shouldWriteNotableAddresses If true, write any notable addresses found.
 *
 * @param searchThreadNames If true, search thread names as well.
 *
 * @param searchQueueNames If true, search queue names as well.
 */
static void writeThread(const KSCrashReportWriter* const writer,
                        const char* const key,
                        const KSCrash_SentryContext* const crash,
                        const thread_t thread,
                        const int index,
                        const bool shouldWriteNotableAddresses,
                        const bool searchThreadNames,
                        const bool searchQueueNames)
{
    //add by yao
    KSCrash_Context* context = writer->context;
    
    STRUCT_MCONTEXT_L machineContextBuffer;
    uintptr_t backtraceBuffer[kMaxBacktraceDepth];
    int backtraceLength = sizeof(backtraceBuffer) / sizeof(*backtraceBuffer);
    int skippedEntries = 0;

    STRUCT_MCONTEXT_L* machineContext = getMachineContext(crash, thread, &machineContextBuffer);

    uintptr_t* backtrace = getBacktrace(crash,
                                        thread,
                                        machineContext,
                                        context->parseResult.backtraceBuffer,
                                        &backtraceLength,
                                        &skippedEntries);
    context->parseResult.backtraceLength = backtraceLength;

//    writer->beginObject(writer, key);
//    {
        if(backtrace != NULL)
        {
            //add by yao
            if(backtraceLength > 0)
            {
//                Dl_info symbolicated[backtraceLength];
                ksbt_symbolicate(backtrace, context->parseResult.symbolicated, backtraceLength, skippedEntries);
                
//                for(int i = 0; i < backtraceLength; i++)
//                {
//                    writeBacktraceEntry(writer, NULL, backtrace[i], &symbolicated[i]);
//                }
            }
//            writeBacktrace(writer,
//                           NULL,
//                           backtrace,
//                           backtraceLength,
//                           skippedEntries);
        }
//        if(machineContext != NULL)
//        {
//            writeRegisters(writer,
//                           KSCrashField_Registers,
//                           machineContext,
//                           isCrashedThread);
//        }
//        writer->addIntegerElement(writer, KSCrashField_Index, index);
//        if(searchThreadNames)
//        {
//            if(ksthread_getThreadName(thread, nameBuffer, sizeof(nameBuffer)) && nameBuffer[0] != 0)
//            {
//                writer->addStringElement(writer, KSCrashField_Name, nameBuffer);
//            }
//        }
//        if (searchQueueNames) {
//            if(ksthread_getQueueName(thread, nameBuffer, sizeof(nameBuffer)) && nameBuffer[0] != 0)
//            {
//                writer->addStringElement(writer, KSCrashField_DispatchQueue, nameBuffer);
//            }
//        }
//        writer->addBooleanElement(writer, KSCrashField_Crashed, isCrashedThread);
//        writer->addBooleanElement(writer, KSCrashField_CurrentThread, thread == ksthread_self());
//        if(isCrashedThread && machineContext != NULL)
//        {
//            writeStackContents(writer,
//                               KSCrashField_Stack,
//                               machineContext,
//                               skippedEntries > 0);
//            if(shouldWriteNotableAddresses)
//            {
//                writeNotableAddresses(writer,
//                                      KSCrashField_NotableAddresses,
//                                      machineContext);
//            }
//        }
//    }
//    writer->endContainer(writer);
}
/** Record whether the crashed thread had a stack overflow or not.
 *
 * @param crashContext the context.
 */
static void updateStackOverflowStatus(KSCrash_Context* const crashContext)
{
    // TODO: This feels weird. Shouldn't be mutating the context.
    if(isStackOverflow(&crashContext->crash, crashContext->crash.offendingThread))
    {
        KSLOG_TRACE("Stack overflow detected.");
        crashContext->crash.isStackOverflow = true;
    }
}

/** add by yao: update result crash reason
 
 */
static void updateResCrashReason( KSCrash_SentryContext* const sentryContext)
{
    switch(sentryContext->crashType)
    {
        case KSCrashTypeMachException:
        {
            int machExceptionType = sentryContext->mach.type;
            kern_return_t machCode = (kern_return_t)sentryContext->mach.code;
            const char* machExceptionName = ksmach_exceptionName(machExceptionType);
            const char* machCodeName = machCode == 0 ? NULL : ksmach_kernelReturnCodeName(machCode);
            char tmpStr[1110];
            sprintf(tmpStr,"App crashed due to mach exception: [%s: %s] at 0x%08lx",
                            machExceptionName, machCodeName, sentryContext->faultAddress);
            sentryContext->resCrashReason = strdup(tmpStr);
            break;
        }
        case KSCrashTypeCPPException:
        {
            char tmpStr[1110];
            sprintf(tmpStr,"App crashed due to C++ exception: %s: %s",
                       sentryContext->CPPException.name,
                       sentryContext->crashReason);
            sentryContext->resCrashReason = strdup(tmpStr);
            break;
        }
        case KSCrashTypeNSException:
        {
            char tmpStr[1110];
            sprintf(tmpStr,"App crashed due to NSException: %s: %s",
                            sentryContext->NSException.name,
                            sentryContext->crashReason);
            sentryContext->resCrashReason = strdup(tmpStr);
            break;
        }
        case KSCrashTypeSignal:
        {
            int sigNum = sentryContext->signal.signalInfo->si_signo;
            int sigCode = sentryContext->signal.signalInfo->si_code;
            const char* sigName = kssignal_signalName(sigNum);
            const char* sigCodeName = kssignal_signalCodeName(sigNum, sigCode);
            char tmpStr[1110];
            sprintf(tmpStr,"App crashed due to signal: [%s, %s] at 0x%08lx",
                            sigName, sigCodeName, sentryContext->faultAddress);
            sentryContext->resCrashReason = strdup(tmpStr);
            break;
        }
        default:
            sentryContext->resCrashReason = strdup("can not parse");
            break;
    }
}
/** Write information about all threads to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param crash The crash handler context.
 */
void writeAllThreads(const KSCrashReportWriter* const writer,
                            const char* const key,
                            const KSCrash_SentryContext* const crash,
                            bool writeNotableAddresses,
                            bool searchThreadNames,
                            bool searchQueueNames)
{
    //add by yao
    KSCrash_Context* context = writer->context;
//    g_introspectionRules = &context->config.introspectionRules;
    updateStackOverflowStatus(context);
    
    
    const task_t thisTask = mach_task_self();
    thread_act_array_t threads;
    mach_msg_type_number_t numThreads;
    kern_return_t kr;

    if((kr = task_threads(thisTask, &threads, &numThreads)) != KERN_SUCCESS)
    {
        KSLOG_ERROR("task_threads: %s", mach_error_string(kr));
        return;
    }

    // Fetch info for all threads.
    
    for(mach_msg_type_number_t i = 0; i < numThreads; i++)
    {
        // add by yao
        if (threads[i] == crash->offendingThread) {
            writeThread(writer, NULL, crash, threads[i], (int)i, writeNotableAddresses, searchThreadNames, searchQueueNames);
        }
    }
    
    //add by yao
    
    updateResCrashReason(&context->crash);

    // Clean up.
    for(mach_msg_type_number_t i = 0; i < numThreads; i++)
    {
        mach_port_deallocate(thisTask, threads[i]);
    }
    vm_deallocate(thisTask, (vm_address_t)threads, sizeof(thread_t) * numThreads);
}

