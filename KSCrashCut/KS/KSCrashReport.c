//
//  KSCrashReport.m
//
//  Created by Karl Stenerud on 2012-01-28.
//
//  Copyright (c) 2012 Karl Stenerud. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall remain in place
// in this source code.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "KSCrashReport.h"

#include "KSBacktrace_Private.h"
//#include "KSCrashReportFields.h"
#include "KSCrashReportWriter.h"
#include "KSDynamicLinker.h"
//#include "KSFileUtils.h"
//#include "KSJSONCodec.h"
#include "KSCPU.h"
#include "KSMach.h"
#include "KSThread.h"
//#include "KSObjC.h"
#include "KSSignalInfo.h"
//#include "KSZombie.h"
//#include "KSString.h"
//#include "KSCrashReportVersion.h"

//#define KSLogger_LocalLevel TRACE
//#include "KSLogger.h"

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
#pragma mark - JSON Encoding -
// ============================================================================

#define getJsonContext(REPORT_WRITER) ((KSJSONEncodeContext*)((REPORT_WRITER)->context))

/** Used for writing hex string values. */
//static const char g_hexNybbles[] =
//{
//    '0', '1', '2', '3', '4', '5', '6', '7',
//    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
//};

// ============================================================================
#pragma mark - Runtime Config -
// ============================================================================

//static KSCrash_IntrospectionRules* g_introspectionRules;


#pragma mark Callbacks
 
//typedef struct
//{
//    char buffer[1024];
//    int length;
//    int position;
//    int fd;
//} BufferedWriter;




// ============================================================================
#pragma mark - Utility -
// ============================================================================

/** Check if a memory address points to a valid null terminated UTF-8 string.
 *
 * @param address The address to check.
 *
 * @return true if the address points to a string.
 */
//static bool isValidString(const void* const address)
//{
//    if((void*)address == NULL)
//    {
//        return false;
//    }
//
//    char buffer[500];
//    if((uintptr_t)address+sizeof(buffer) < (uintptr_t)address)
//    {
//        // Wrapped around the address range.
//        return false;
//    }
//    if(ksmach_copyMem(address, buffer, sizeof(buffer)) != KERN_SUCCESS)
//    {
//        return false;
//    }
//    return ksstring_isNullTerminatedUTF8String(buffer, kMinStringLength, sizeof(buffer));
//}

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


// ============================================================================
#pragma mark - Console Logging -
// ============================================================================

/** Print the crash type and location to the log.
 *
 * @param sentryContext The crash sentry context.
 */
//static void logCrashType(const KSCrash_SentryContext* const sentryContext)
//{
//    switch(sentryContext->crashType)
//    {
//        case KSCrashTypeMachException:
//        {
//            int machExceptionType = sentryContext->mach.type;
//            kern_return_t machCode = (kern_return_t)sentryContext->mach.code;
//            const char* machExceptionName = ksmach_exceptionName(machExceptionType);
//            const char* machCodeName = machCode == 0 ? NULL : ksmach_kernelReturnCodeName(machCode);
//            KSLOGBASIC_INFO("App crashed due to mach exception: [%s: %s] at %p",
//                            machExceptionName, machCodeName, sentryContext->faultAddress);
//            break;
//        }
//        case KSCrashTypeCPPException:
//        {
//            KSLOG_INFO2("App crashed due to C++ exception: %s: %s",
//                       sentryContext->CPPException.name,
//                       sentryContext->crashReason);
//            break;
//        }
//        case KSCrashTypeNSException:
//        {
//            KSLOGBASIC_INFO("App crashed due to NSException: %s: %s",
//                            sentryContext->NSException.name,
//                            sentryContext->crashReason);
//            break;
//        }
//        case KSCrashTypeSignal:
//        {
//            int sigNum = sentryContext->signal.signalInfo->si_signo;
//            int sigCode = sentryContext->signal.signalInfo->si_code;
//            const char* sigName = kssignal_signalName(sigNum);
//            const char* sigCodeName = kssignal_signalCodeName(sigNum, sigCode);
//            KSLOGBASIC_INFO("App crashed due to signal: [%s, %s] at %08x",
//                            sigName, sigCodeName, sentryContext->faultAddress);
//            break;
//        }
//        case KSCrashTypeMainThreadDeadlock:
//        {
//            KSLOGBASIC_INFO("Main thread deadlocked");
//            break;
//        }
//        case KSCrashTypeUserReported:
//        {
//            KSLOG_INFO2("App crashed due to user specified exception: %s", sentryContext->crashReason);
//            break;
//        }
//    }
//}

/** Print a backtrace entry in the standard format to the log.
 *
 * @param entryNum The backtrace entry number.
 *
 * @param address The program counter value (instruction address).
 *
 * @param dlInfo Information about the nearest symbols to the address.
 */
//add by yao
//const char* ksfu_lastPathEntry(const char* const path)
//{
//    if(path == NULL)
//    {
//        return NULL;
//    }
//    
//    char* lastFile = strrchr(path, '/');
//    return lastFile == NULL ? path : lastFile + 1;
//}
//static void logBacktraceEntry(const int entryNum, const uintptr_t address, const Dl_info* const dlInfo)
//{
//    char faddrBuff[20];
//    char saddrBuff[20];
//
//    const char* fname = ksfu_lastPathEntry(dlInfo->dli_fname);
//    if(fname == NULL)
//    {
//        sprintf(faddrBuff, POINTER_FMT, (uintptr_t)dlInfo->dli_fbase);
//        fname = faddrBuff;
//    }
//
//    uintptr_t offset = address - (uintptr_t)dlInfo->dli_saddr;
//    const char* sname = dlInfo->dli_sname;
//    if(sname == NULL)
//    {
//        sprintf(saddrBuff, POINTER_SHORT_FMT, (uintptr_t)dlInfo->dli_fbase);
//        sname = saddrBuff;
//        offset = address - (uintptr_t)dlInfo->dli_fbase;
//    }
//
//    KSLOGBASIC_ALWAYS(TRACE_FMT, entryNum, fname, address, sname, offset);
//}

/** Print a backtrace to the log.
 *
 * @param backtrace The backtrace to print.
 *
 * @param backtraceLength The length of the backtrace.
 */
//static void logBacktrace(const uintptr_t* const backtrace, const int backtraceLength, const int skippedEntries)
//{
//    if(backtraceLength > 0)
//    {
//        Dl_info symbolicated[backtraceLength];
//        ksbt_symbolicate(backtrace, symbolicated, backtraceLength, skippedEntries);
//
//        for(int i = 0; i < backtraceLength; i++)
//        {
//            logBacktraceEntry(i, backtrace[i], &symbolicated[i]);
//        }
//    }
//}

/** Print the backtrace for the crashed thread to the log.
 *
 * @param crash The crash handler context.
 */
//static void logCrashThreadBacktrace(const KSCrash_SentryContext* const crash)
//{
//    thread_t thread = crash->offendingThread;
//    STRUCT_MCONTEXT_L concreteMachineContext;
//    uintptr_t concreteBacktrace[kMaxStackTracePrintLines];
//    int backtraceLength = sizeof(concreteBacktrace) / sizeof(*concreteBacktrace);
//
//    STRUCT_MCONTEXT_L* machineContext = getMachineContext(crash,
//                                                          thread,
//                                                          &concreteMachineContext);
//
//    int skippedEntries = 0;
//    uintptr_t* backtrace = getBacktrace(crash,
//                                        thread,
//                                        machineContext,
//                                        concreteBacktrace,
//                                        &backtraceLength,
//                                        &skippedEntries);
//
//    if(backtrace != NULL)
//    {
//        logBacktrace(backtrace, backtraceLength, skippedEntries);
//    }
//}


// ============================================================================
#pragma mark - Report Writing -
// ============================================================================

/** Write the contents of a memory location.
 * Also writes meta information about the data.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param address The memory address.
 *
 * @param limit How many more subreferenced objects to write, if any.
 */
//static void writeMemoryContents(const KSCrashReportWriter* const writer,
//                                const char* const key,
//                                const uintptr_t address,
//                                int* limit);




/** Write out ivar information about an unknown object.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param objectAddress The object's address.
 *
 * @param limit How many more subreferenced objects to write, if any.
 */


//static bool isRestrictedClass(const char* name)
//{
//    if(g_introspectionRules->restrictedClasses != NULL)
//    {
//        for(int i = 0; i < g_introspectionRules->restrictedClassesCount; i++)
//        {
//            if(strcmp(name, g_introspectionRules->restrictedClasses[i]) == 0)
//            {
//                return true;
//            }
//        }
//    }
//    return false;
//}

//static bool isValidPointer(const uintptr_t address)
//{
//    if(address == (uintptr_t)NULL)
//    {
//        return false;
//    }
//
//    if(ksobjc_isTaggedPointer((const void*)address))
//    {
//        if(!ksobjc_isValidTaggedPointer((const void*)address))
//        {
//            return false;
//        }
//    }
//    
//    return true;
//}



#pragma mark Backtrace

/** Write a backtrace entry to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param address The memory address.
 *
 * @param info Information about the nearest symbols to the address.
 */
//static void writeBacktraceEntry(const KSCrashReportWriter* const writer,
//                                const char* const key,
//                                const uintptr_t address,
//                                const Dl_info* const info)
//{
////    writer->beginObject(writer, key);
////    {
//        if(info->dli_fname != NULL)
//        {
////            writer->addStringElement(writer, KSCrashField_ObjectName, ksfu_lastPathEntry(info->dli_fname));
//        }
////        writer->addUIntegerElement(writer, KSCrashField_ObjectAddr, (uintptr_t)info->dli_fbase);
//        if(info->dli_sname != NULL)
//        {
//            const char* sname = info->dli_sname;
////            writer->addStringElement(writer, KSCrashField_SymbolName, sname);
//        }
////        writer->addUIntegerElement(writer, KSCrashField_SymbolAddr, (uintptr_t)info->dli_saddr);
////        writer->addUIntegerElement(writer, KSCrashField_InstructionAddr, address);
////    }
////    writer->endContainer(writer);
//}

/** Write a backtrace to the report.
 *
 * @param writer The writer to write the backtrace to.
 *
 * @param key The object key, if needed.
 *
 * @param backtrace The backtrace to write.
 *
 * @param backtraceLength Length of the backtrace.
 *
 * @param skippedEntries The number of entries that were skipped before the
 *                       beginning of backtrace.
 */
//static void writeBacktrace(const KSCrashReportWriter* const writer,
//                           const char* const key,
//                           const uintptr_t* const backtrace,
//                           const int backtraceLength,
//                           const int skippedEntries)
//{
////    writer->beginObject(writer, key);
////    {
////        writer->beginArray(writer, KSCrashField_Contents);
////        {
//            if(backtraceLength > 0)
//            {
//                Dl_info symbolicated[backtraceLength];
//                ksbt_symbolicate(backtrace, symbolicated, backtraceLength, skippedEntries);
//
//                for(int i = 0; i < backtraceLength; i++)
//                {
//                    writeBacktraceEntry(writer, NULL, backtrace[i], &symbolicated[i]);
//                }
//            }
////        }
////        writer->endContainer(writer);
////        writer->addIntegerElement(writer, KSCrashField_Skipped, skippedEntries);
////    }
////    writer->endContainer(writer);
//}

#pragma mark Stack



/** Write any notable addresses near the stack pointer (above and below).
 *
 * @param writer The writer.
 *
 * @param machineContext The context to retrieve the stack from.
 *
 * @param backDistance The distance towards the beginning of the stack to check.
 *
 * @param forwardDistance The distance past the end of the stack to check.
 */
//static void writeNotableStackContents(const KSCrashReportWriter* const writer,
//                                      const STRUCT_MCONTEXT_L* const machineContext,
//                                      const int backDistance,
//                                      const int forwardDistance)
//{
//    uintptr_t sp = kscpu_stackPointer(machineContext);
//    if((void*)sp == NULL)
//    {
//        return;
//    }
//
//    uintptr_t lowAddress = sp + (uintptr_t)(backDistance * (int)sizeof(sp) * kscpu_stackGrowDirection() * -1);
//    uintptr_t highAddress = sp + (uintptr_t)(forwardDistance * (int)sizeof(sp) * kscpu_stackGrowDirection());
//    if(highAddress < lowAddress)
//    {
//        uintptr_t tmp = lowAddress;
//        lowAddress = highAddress;
//        highAddress = tmp;
//    }
//    uintptr_t contentsAsPointer;
//    char nameBuffer[40];
//    for(uintptr_t address = lowAddress; address < highAddress; address += sizeof(address))
//    {
//        if(ksmach_copyMem((void*)address, &contentsAsPointer, sizeof(contentsAsPointer)) == KERN_SUCCESS)
//        {
//            sprintf(nameBuffer, "stack@%p", (void*)address);
//            writeMemoryContentsIfNotable(writer, nameBuffer, contentsAsPointer);
//        }
//    }
//}


#pragma mark Registers

/** Write the contents of all regular registers to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param machineContext The context to retrieve the registers from.
 */
//static void writeBasicRegisters(const KSCrashReportWriter* const writer,
//                                const char* const key,
//                                const STRUCT_MCONTEXT_L* const machineContext)
//{
//    char registerNameBuff[30];
//    const char* registerName;
//    writer->beginObject(writer, key);
//    {
//        const int numRegisters = kscpu_numRegisters();
//        for(int reg = 0; reg < numRegisters; reg++)
//        {
//            registerName = kscpu_registerName(reg);
//            if(registerName == NULL)
//            {
//                snprintf(registerNameBuff, sizeof(registerNameBuff), "r%d", reg);
//                registerName = registerNameBuff;
//            }
//            writer->addUIntegerElement(writer, registerName,
//                                       kscpu_registerValue(machineContext, reg));
//        }
//    }
//    writer->endContainer(writer);
//}

/** Write the contents of all exception registers to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param machineContext The context to retrieve the registers from.
 */
//static void writeExceptionRegisters(const KSCrashReportWriter* const writer,
//                                    const char* const key,
//                                    const STRUCT_MCONTEXT_L* const machineContext)
//{
//    char registerNameBuff[30];
//    const char* registerName;
//    writer->beginObject(writer, key);
//    {
//        const int numRegisters = kscpu_numExceptionRegisters();
//        for(int reg = 0; reg < numRegisters; reg++)
//        {
//            registerName = kscpu_exceptionRegisterName(reg);
//            if(registerName == NULL)
//            {
//                snprintf(registerNameBuff, sizeof(registerNameBuff), "r%d", reg);
//                registerName = registerNameBuff;
//            }
//            writer->addUIntegerElement(writer,registerName,
//                                       kscpu_exceptionRegisterValue(machineContext, reg));
//        }
//    }
//    writer->endContainer(writer);
//}




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
    
//    bool isCrashedThread = thread == crash->offendingThread;
//    char nameBuffer[128];
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
    

    // Clean up.
    for(mach_msg_type_number_t i = 0; i < numThreads; i++)
    {
        mach_port_deallocate(thisTask, threads[i]);
    }
    vm_deallocate(thisTask, (vm_address_t)threads, sizeof(thread_t) * numThreads);
}

/** Get the index of a thread.
 *
 * @param thread The thread.
 *
 * @return The thread's index, or -1 if it couldn't be determined.
 */
//static int threadIndex(const thread_t thread)
//{
//    int index = -1;
//    const task_t thisTask = mach_task_self();
//    thread_act_array_t threads;
//    mach_msg_type_number_t numThreads;
//    kern_return_t kr;
//
//    if((kr = task_threads(thisTask, &threads, &numThreads)) != KERN_SUCCESS)
//    {
//        KSLOG_ERROR("task_threads: %s", mach_error_string(kr));
//        return -1;
//    }
//
//    for(mach_msg_type_number_t i = 0; i < numThreads; i++)
//    {
//        if(threads[i] == thread)
//        {
//            index = (int)i;
//            break;
//        }
//    }
//
//    // Clean up.
//    for(mach_msg_type_number_t i = 0; i < numThreads; i++)
//    {
//        mach_port_deallocate(thisTask, threads[i]);
//    }
//    vm_deallocate(thisTask, (vm_address_t)threads, sizeof(thread_t) * numThreads);
//
//    return index;
//}

#pragma mark Global Report Data





/** Write information about the error leading to the crash to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param crash The crash handler context.
 */
//static void writeError(const KSCrashReportWriter* const writer,
//                       const char* const key,
//                       const KSCrash_SentryContext* const crash)
//{
//    int machExceptionType = 0;
//    kern_return_t machCode = 0;
//    kern_return_t machSubCode = 0;
//    int sigNum = 0;
//    int sigCode = 0;
//    const char* exceptionName = NULL;
//    const char* crashReason = NULL;
//
//    // Gather common info.
//    switch(crash->crashType)
//    {
//        case KSCrashTypeMainThreadDeadlock:
//            break;
//        case KSCrashTypeMachException:
//            machExceptionType = crash->mach.type;
//            machCode = (kern_return_t)crash->mach.code;
//            if(machCode == KERN_PROTECTION_FAILURE && crash->isStackOverflow)
//            {
//                // A stack overflow should return KERN_INVALID_ADDRESS, but
//                // when a stack blasts through the guard pages at the top of the stack,
//                // it generates KERN_PROTECTION_FAILURE. Correct for this.
//                machCode = KERN_INVALID_ADDRESS;
//            }
//            machSubCode = (kern_return_t)crash->mach.subcode;
//
//            sigNum = kssignal_signalForMachException(machExceptionType, machCode);
//            break;
//        case KSCrashTypeCPPException:
//            machExceptionType = EXC_CRASH;
//            sigNum = SIGABRT;
//            crashReason = crash->crashReason;
//            exceptionName = crash->CPPException.name;
//            break;
//        case KSCrashTypeNSException:
//            machExceptionType = EXC_CRASH;
//            sigNum = SIGABRT;
//            exceptionName = crash->NSException.name;
//            crashReason = crash->crashReason;
//            break;
//        case KSCrashTypeSignal:
//            sigNum = crash->signal.signalInfo->si_signo;
//            sigCode = crash->signal.signalInfo->si_code;
//            machExceptionType = kssignal_machExceptionForSignal(sigNum);
//            break;
//        case KSCrashTypeUserReported:
//            machExceptionType = EXC_CRASH;
//            sigNum = SIGABRT;
//            crashReason = crash->crashReason;
//            break;
//    }
//
//    const char* machExceptionName = ksmach_exceptionName(machExceptionType);
//    const char* machCodeName = machCode == 0 ? NULL : ksmach_kernelReturnCodeName(machCode);
//    const char* sigName = kssignal_signalName(sigNum);
//    const char* sigCodeName = kssignal_signalCodeName(sigNum, sigCode);
//
//    writer->beginObject(writer, key);
//    {
//        writer->beginObject(writer, KSCrashField_Mach);
//        {
//            writer->addUIntegerElement(writer, KSCrashField_Exception, (unsigned)machExceptionType);
//            if(machExceptionName != NULL)
//            {
//                writer->addStringElement(writer, KSCrashField_ExceptionName, machExceptionName);
//            }
//            writer->addUIntegerElement(writer, KSCrashField_Code, (unsigned)machCode);
//            if(machCodeName != NULL)
//            {
//                writer->addStringElement(writer, KSCrashField_CodeName, machCodeName);
//            }
//            writer->addUIntegerElement(writer, KSCrashField_Subcode, (unsigned)machSubCode);
//        }
//        writer->endContainer(writer);
//
//        writer->beginObject(writer, KSCrashField_Signal);
//        {
//            writer->addUIntegerElement(writer, KSCrashField_Signal, (unsigned)sigNum);
//            if(sigName != NULL)
//            {
//                writer->addStringElement(writer, KSCrashField_Name, sigName);
//            }
//            writer->addUIntegerElement(writer, KSCrashField_Code, (unsigned)sigCode);
//            if(sigCodeName != NULL)
//            {
//                writer->addStringElement(writer, KSCrashField_CodeName, sigCodeName);
//            }
//        }
//        writer->endContainer(writer);
//
//        writer->addUIntegerElement(writer, KSCrashField_Address, crash->faultAddress);
//        if(crashReason != NULL)
//        {
//            writer->addStringElement(writer, KSCrashField_Reason, crashReason);
//        }
//
//        // Gather specific info.
//        switch(crash->crashType)
//        {
//            case KSCrashTypeMainThreadDeadlock:
//                writer->addStringElement(writer, KSCrashField_Type, KSCrashExcType_Deadlock);
//                break;
//                
//            case KSCrashTypeMachException:
//                writer->addStringElement(writer, KSCrashField_Type, KSCrashExcType_Mach);
//                break;
//
//            case KSCrashTypeCPPException:
//            {
//                writer->addStringElement(writer, KSCrashField_Type, KSCrashExcType_CPPException);
//                writer->beginObject(writer, KSCrashField_CPPException);
//                {
//                    writer->addStringElement(writer, KSCrashField_Name, exceptionName);
//                }
//                writer->endContainer(writer);
//                break;
//            }
//            case KSCrashTypeNSException:
//            {
//                writer->addStringElement(writer, KSCrashField_Type, KSCrashExcType_NSException);
//                writer->beginObject(writer, KSCrashField_NSException);
//                {
//                    writer->addStringElement(writer, KSCrashField_Name, exceptionName);
//                    writeAddressReferencedByString(writer, KSCrashField_ReferencedObject, crashReason);
//                }
//                writer->endContainer(writer);
//                break;
//            }
//            case KSCrashTypeSignal:
//                writer->addStringElement(writer, KSCrashField_Type, KSCrashExcType_Signal);
//                break;
//
//            case KSCrashTypeUserReported:
//            {
//                writer->addStringElement(writer, KSCrashField_Type, KSCrashExcType_User);
//                writer->beginObject(writer, KSCrashField_UserReported);
//                {
//                    writer->addStringElement(writer, KSCrashField_Name, crash->userException.name);
//                    if(crash->userException.language != NULL)
//                    {
//                        writer->addStringElement(writer, KSCrashField_Language, crash->userException.language);
//                    }
//                    if(crash->userException.lineOfCode != NULL)
//                    {
//                        writer->addStringElement(writer, KSCrashField_LineOfCode, crash->userException.lineOfCode);
//                    }
//                    if(crash->userException.customStackTrace != NULL)
//                    {
//                        writer->addJSONElement(writer, KSCrashField_Backtrace, crash->userException.customStackTrace, true);
//                    }
//                }
//                writer->endContainer(writer);
//                break;
//            }
//        }
//    }
//    writer->endContainer(writer);
//}




// ============================================================================
#pragma mark - Main API -
// ============================================================================

//void kscrashreport_logCrash(const KSCrash_Context* const crashContext)
//{
//    const KSCrash_SentryContext* crash = &crashContext->crash;
//    logCrashType(crash);
//    logCrashThreadBacktrace(&crashContext->crash);
//}
