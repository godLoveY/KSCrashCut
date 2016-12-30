//
//  KSThread.c
//
//  Created by Karl Stenerud on 2012-01-29.
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


#include "KSThread.h"

//#include "KSSystemCapabilities.h"

//#define KSLogger_LocalLevel TRACE
//#include "KSLogger.h"

#include <dispatch/dispatch.h>
#include <mach/mach.h>
#include <pthread.h>
#include <sys/sysctl.h>


thread_t ksthread_self()
{
    thread_t thread_self = mach_thread_self();
    mach_port_deallocate(mach_task_self(), thread_self);
    return thread_self;
}

bool ksthread_getThreadName(const thread_t thread, char* const buffer, int bufLength)
{
    // WARNING: This implementation is no longer async-safe!
    
    const pthread_t pthread = pthread_from_mach_thread_np(thread);
    return pthread_getname_np(pthread, buffer, (unsigned)bufLength) == 0;
}

bool ksthread_getQueueName(const thread_t thread, char* const buffer, int bufLength)
{
    // WARNING: This implementation is no longer async-safe!
    
    integer_t infoBuffer[THREAD_IDENTIFIER_INFO_COUNT] = {0};
    thread_info_t info = infoBuffer;
    mach_msg_type_number_t inOutSize = THREAD_IDENTIFIER_INFO_COUNT;
    kern_return_t kr = 0;
    
    kr = thread_info(thread, THREAD_IDENTIFIER_INFO, info, &inOutSize);
    if(kr != KERN_SUCCESS)
    {
        KSLOG_TRACE("Error getting thread_info with flavor THREAD_IDENTIFIER_INFO from mach thread : %s", mach_error_string(kr));
        return false;
    }
    
    thread_identifier_info_t idInfo = (thread_identifier_info_t)info;
    dispatch_queue_t* dispatch_queue_ptr = (dispatch_queue_t*)idInfo->dispatch_qaddr;
    //thread_handle shouldn't be 0 also, because
    //identifier_info->dispatch_qaddr =  identifier_info->thread_handle + get_dispatchqueue_offset_from_proc(thread->task->bsd_info);
    if(dispatch_queue_ptr == NULL || idInfo->thread_handle == 0 || *dispatch_queue_ptr == NULL)
    {
        KSLOG_TRACE("This thread doesn't have a dispatch queue attached : %p", thread);
        return false;
    }
    
    dispatch_queue_t dispatch_queue = *dispatch_queue_ptr;
    const char* queue_name = dispatch_queue_get_label(dispatch_queue);
    if(queue_name == NULL)
    {
        KSLOG_TRACE("Error while getting dispatch queue name : %p", dispatch_queue);
        return false;
    }
    KSLOG_TRACE("Dispatch queue name: %s", queue_name);
    int length = (int)strlen(queue_name);
    
    // Queue label must be a null terminated string.
    int iLabel;
    for(iLabel = 0; iLabel < length + 1; iLabel++)
    {
        if(queue_name[iLabel] < ' ' || queue_name[iLabel] > '~')
        {
            break;
        }
    }
    if(queue_name[iLabel] != 0)
    {
        // Found a non-null, invalid char.
        KSLOG_TRACE("Queue label contains invalid chars");
        return false;
    }
    bufLength = MIN(length, bufLength - 1);//just strlen, without null-terminator
    strncpy(buffer, queue_name, bufLength);
    buffer[bufLength] = 0;//terminate string
    KSLOG_TRACE("Queue label = %s", buffer);
    return true;
}

// ============================================================================
#pragma mark - Utility -
// ============================================================================

#if 1//KSCRASH_HAS_THREADS_API
static inline bool isThreadInList(thread_t thread, thread_t* list, int listCount)
{
    for(int i = 0; i < listCount; i++)
    {
        if(list[i] == thread)
        {
            return true;
        }
    }
    return false;
}
#endif

bool ksthread_suspendAllThreads(void)
{
    return ksthread_suspendAllThreadsExcept(NULL, 0);
}

#if 1//KSCRASH_HAS_THREADS_API
bool ksthread_suspendAllThreadsExcept(thread_t* exceptThreads, int exceptThreadsCount)
{
    kern_return_t kr;
    const task_t thisTask = mach_task_self();
    const thread_t thisThread = ksthread_self();
    thread_act_array_t threads;
    mach_msg_type_number_t numThreads;
    
    if((kr = task_threads(thisTask, &threads, &numThreads)) != KERN_SUCCESS)
    {
        KSLOG_ERROR("task_threads: %s", mach_error_string(kr));
        return false;
    }
    
    for(mach_msg_type_number_t i = 0; i < numThreads; i++)
    {
        thread_t thread = threads[i];
        if(thread != thisThread && !isThreadInList(thread, exceptThreads, exceptThreadsCount))
        {
            if((kr = thread_suspend(thread)) != KERN_SUCCESS)
            {
                KSLOG_ERROR("thread_suspend (%08x): %s",
                            thread, mach_error_string(kr));
                // Don't treat this as a fatal error.
            }
        }
    }
    
    for(mach_msg_type_number_t i = 0; i < numThreads; i++)
    {
        mach_port_deallocate(thisTask, threads[i]);
    }
    vm_deallocate(thisTask, (vm_address_t)threads, sizeof(thread_t) * numThreads);
    
    return true;
}
#else
bool ksthread_suspendAllThreadsExcept(__unused thread_t* exceptThreads, __unused int exceptThreadsCount)
{
    return false;
}
#endif

bool ksthread_resumeAllThreads(void)
{
    return ksthread_resumeAllThreadsExcept(NULL, 0);
}

#if 1//KSCRASH_HAS_THREADS_API
bool ksthread_resumeAllThreadsExcept(thread_t* exceptThreads, int exceptThreadsCount)
{
    kern_return_t kr;
    const task_t thisTask = mach_task_self();
    const thread_t thisThread = ksthread_self();
    thread_act_array_t threads;
    mach_msg_type_number_t numThreads;
    
    if((kr = task_threads(thisTask, &threads, &numThreads)) != KERN_SUCCESS)
    {
        KSLOG_ERROR("task_threads: %s", mach_error_string(kr));
        return false;
    }
    
    for(mach_msg_type_number_t i = 0; i < numThreads; i++)
    {
        thread_t thread = threads[i];
        if(thread != thisThread && !isThreadInList(thread, exceptThreads, exceptThreadsCount))
        {
            if((kr = thread_resume(thread)) != KERN_SUCCESS)
            {
                KSLOG_ERROR("thread_resume (%08x): %s",
                            thread, mach_error_string(kr));
                // Don't treat this as a fatal error.
            }
        }
    }
    
    for(mach_msg_type_number_t i = 0; i < numThreads; i++)
    {
        mach_port_deallocate(thisTask, threads[i]);
    }
    vm_deallocate(thisTask, (vm_address_t)threads, sizeof(thread_t) * numThreads);
    
    return true;
}
#else
bool ksthread_resumeAllThreadsExcept(__unused thread_t* exceptThreads, __unused int exceptThreadsCount)
{
    return false;
}
#endif
