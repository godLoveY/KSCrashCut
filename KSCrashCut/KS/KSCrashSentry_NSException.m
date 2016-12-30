//
//  KSCrashSentry_NSException.m
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


#import "KSCrashSentry_NSException.h"
#include "KSCrashSentry_Context.h"
#import "KSCrashSentry_Private.h"
#include "KSThread.h"
#import <Foundation/Foundation.h>

//#define KSLogger_LocalLevel TRACE
//#import "KSLogger.h"


// ============================================================================
#pragma mark - Globals -
// ============================================================================

/** Flag noting if we've installed our custom handlers or not.
 * It's not fully thread safe, but it's safer than locking and slightly better
 * than nothing.
 */
static volatile sig_atomic_t g_installed = 0;

/** The exception handler that was in place before we installed ours. */
static NSUncaughtExceptionHandler* g_previousUncaughtExceptionHandler;

/** Context to fill with crash information. */
static KSCrash_SentryContext* g_context;


// ============================================================================
#pragma mark - Callbacks -
// ============================================================================

/** Our custom excepetion handler.
 * Fetch the stack trace from the exception and write a report.
 *
 * @param exception The exception that was raised.
 */
static void handleException(NSException* exception)
{
    KSLOG_DEBUG2(@"Trapped exception %@", exception);
    if(g_installed)
    {
        bool wasHandlingCrash = g_context->handlingCrash;
        kscrashsentry_beginHandlingCrash(g_context);

        KSLOG_DEBUG2(@"Exception handler is installed. Continuing exception handling.");

        if(wasHandlingCrash)
        {
            KSLOG_INFO(@"Detected crash in the crash reporter. Restoring original handlers.");
            g_context->crashedDuringCrashHandling = true;
            kscrashsentry_uninstall(KSCrashTypeAll);
        }

        KSLOG_DEBUG2(@"Suspending all threads.");
        kscrashsentry_suspendThreads();

        KSLOG_DEBUG2(@"Filling out context.");
        NSArray* addresses = [exception callStackReturnAddresses];
        NSUInteger numFrames = [addresses count];
        uintptr_t* callstack = malloc(numFrames * sizeof(*callstack));
        for(NSUInteger i = 0; i < numFrames; i++)
        {
            callstack[i] = [[addresses objectAtIndex:i] unsignedLongValue];
        }

        g_context->crashType = KSCrashTypeNSException;
        g_context->offendingThread = ksthread_self();
        g_context->registersAreValid = false;
        g_context->NSException.name = strdup([[exception name] UTF8String]);
        g_context->crashReason = strdup([[exception reason] UTF8String]);
        g_context->stackTrace = callstack;
        g_context->stackTraceLength = (int)numFrames;


        KSLOG_DEBUG2(@"Calling main crash handler.");
        g_context->onCrash();


        KSLOG_DEBUG2(@"Crash handling complete. Restoring original handlers.");
        kscrashsentry_uninstall(KSCrashTypeAll);

        if (g_previousUncaughtExceptionHandler != NULL)
        {
            KSLOG_DEBUG2(@"Calling original exception handler.");
            g_previousUncaughtExceptionHandler(exception);
        }
    }
}


// ============================================================================
#pragma mark - API -
// ============================================================================

bool kscrashsentry_installNSExceptionHandler(KSCrash_SentryContext* const context)
{
    KSLOG_DEBUG2(@"Installing NSException handler.");
    if(g_installed)
    {
        KSLOG_DEBUG2(@"NSException handler already installed.");
        return true;
    }
    g_installed = 1;

    g_context = context;

    KSLOG_DEBUG2(@"Backing up original handler.");
    g_previousUncaughtExceptionHandler = NSGetUncaughtExceptionHandler();

    KSLOG_DEBUG2(@"Setting new handler.");
    NSSetUncaughtExceptionHandler(&handleException);

    return true;
}

void kscrashsentry_uninstallNSExceptionHandler(void)
{
    KSLOG_DEBUG2(@"Uninstalling NSException handler.");
    if(!g_installed)
    {
        KSLOG_DEBUG2(@"NSException handler was already uninstalled.");
        return;
    }

    KSLOG_DEBUG2(@"Restoring original handler.");
    NSSetUncaughtExceptionHandler(g_previousUncaughtExceptionHandler);
    g_installed = 0;
}
