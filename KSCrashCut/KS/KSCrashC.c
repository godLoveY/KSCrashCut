#include "KSCrashC.h"
#include "KSCrashContext.h"//add by yao
#include "KSCrashSentry.h"//add by yao
#include "KSCrashReport.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>


// ============================================================================
#pragma mark - Globals -
// ============================================================================

/** True if KSCrash has been installed. */
static volatile sig_atomic_t g_installed = 0;

/** Single, global crash context. */
static KSCrash_Context g_crashReportContext =
{
    .config =
    {
        .handlingCrashTypes = KSCrashTypeProductionSafe2
    }
};

// ============================================================================
#pragma mark - Utility -
// ============================================================================

static inline KSCrash_Context* crashContext(void)
{
    return &g_crashReportContext;
}


// ============================================================================
#pragma mark - Callbacks -
// ============================================================================

/** Called when a crash occurs.
 *
 * This function gets passed as a callback to a crash handler.
 */
static void onCrash(void)
{
    KSLOG_DEBUG("Updating application state to note crash.");
    KSCrash_Context* context = crashContext();
    
    KSCrashReportWriter concreteWriter;
    KSCrashReportWriter* writer = &concreteWriter;
    writer->context = context;
    
    
    if(context->crash.crashedDuringCrashHandling){
    
        
    }else{
        writeAllThreads(writer,
                        NULL,
                        &context->crash,
                        0,
                        0,
                        0);
    }
    if(context->config.onCrashNotify != NULL){
        //回调函数
        context->config.onCrashNotify(writer);
    }
}


// ============================================================================
#pragma mark - API -
// ============================================================================
void ks_reinstsallMach()
{
    reInstsallMach();
}

KSCrashType kscrash_install(const char* appName, const char* const installPath)
{
    KSLOG_DEBUG("Installing crash reporter.");
    
    KSCrash_Context* context = crashContext();

    if(g_installed)
    {
        KSLOG_DEBUG("Crash reporter already installed.");
        return context->config.handlingCrashTypes;
    }
    g_installed = 1;

    KSCrashType crashTypes = kscrash_setHandlingCrashTypes(context->config.handlingCrashTypes);

    KSLOG_DEBUG("Installation complete.");
    return crashTypes;
}

KSCrashType kscrash_setHandlingCrashTypes(KSCrashType crashTypes)
{
    KSCrash_Context* context = crashContext();
    context->config.handlingCrashTypes = crashTypes;
    
    if(g_installed)
    {
        kscrashsentry_uninstall(~crashTypes);
        crashTypes = kscrashsentry_installWithContext(&context->crash, crashTypes, onCrash);
    }

    return crashTypes;
}
//设置回调函数
void kscrash_setCrashNotifyCallback(const KSReportWriteCallback onCrashNotify)
{
    KSLOG_TRACE("Set onCrashNotify to %p", onCrashNotify);
    crashContext()->config.onCrashNotify = onCrashNotify;
}
