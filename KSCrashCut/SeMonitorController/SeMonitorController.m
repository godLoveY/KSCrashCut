//
//  SeMonitorController.m
//  RunloopMonitorDemo
//
//  Created by game3108 on 16/4/14.
//  Copyright © 2016年 game3108. All rights reserved.
//

#import "SeMonitorController.h"
#import <libkern/OSAtomic.h>
#import <execinfo.h>

#import "KSCrashReport.h"
#import <mach/mach.h>
#include <pthread.h>

@interface SeMonitorController(){
    CFRunLoopObserverRef _observer;
    dispatch_semaphore_t _semaphore;
    CFRunLoopActivity _activity;
    NSInteger _countTime;
    NSMutableArray *_backtrace;
}

@end
static mach_port_t main_thread_id;
@implementation SeMonitorController

thread_t bs_machThreadFromNSThread(NSThread *nsthread) {
    char name[256];
    mach_msg_type_number_t count;
    thread_act_array_t list;
    task_threads(mach_task_self(), &list, &count);
    
    NSTimeInterval currentTimestamp = [[NSDate date] timeIntervalSince1970];
    NSString *originName = [nsthread name];
    [nsthread setName:[NSString stringWithFormat:@"%f", currentTimestamp]];
    
    if ([nsthread isMainThread]) {
        return (thread_t)main_thread_id;
    }
    
    for (int i = 0; i < count; ++i) {
        pthread_t pt = pthread_from_mach_thread_np(list[i]);
        if ([nsthread isMainThread]) {
            if (list[i] == main_thread_id) {
                return list[i];
            }
        }
        if (pt) {
            name[0] = '\0';
            pthread_getname_np(pt, name, sizeof name);
            if (!strcmp(name, [nsthread name].UTF8String)) {
                [nsthread setName:originName];
                return list[i];
            }
        }
    }
    
    [nsthread setName:originName];
    return mach_thread_self();
}

+ (instancetype) sharedInstance{
    static dispatch_once_t once;
    static id sharedInstance;
    dispatch_once(&once, ^{
        sharedInstance = [[self alloc] init];
    });
    return sharedInstance;
}

- (void) startMonitor{
    [self registerObserver];
}

- (void) endMonitor{
    if (!_observer) {
        return;
    }
    CFRunLoopRemoveObserver(CFRunLoopGetMain(), _observer, kCFRunLoopCommonModes);
    CFRelease(_observer);
    _observer = NULL;
}

- (void) printLogTrace{
    NSLog(@"====================堆栈\n %@ \n",_backtrace);
}

static void runLoopObserverCallBack(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info)
{
    SeMonitorController *instrance = [SeMonitorController sharedInstance];
    instrance->_activity = activity;
    // 发送信号
    dispatch_semaphore_t semaphore = instrance->_semaphore;
    dispatch_semaphore_signal(semaphore);
//    switch (activity) {
//            //The entrance of the run loop, before entering the event processing loop.
//            //This activity occurs once for each call to CFRunLoopRun and CFRunLoopRunInMode
//        case kCFRunLoopEntry:
//            NSLog(@"run loop entry");
//            break;
//            //Inside the event processing loop before any timers are processed
//        case kCFRunLoopBeforeTimers:
//            NSLog(@"run loop before timers");
//            break;
//            //Inside the event processing loop before any sources are processed
//        case kCFRunLoopBeforeSources:
//            NSLog(@"run loop before sources");
//            break;
//            //Inside the event processing loop before the run loop sleeps, waiting for a source or timer to fire.
//            //This activity does not occur if CFRunLoopRunInMode is called with a timeout of 0 seconds.
//            //It also does not occur in a particular iteration of the event processing loop if a version 0 source fires
//        case kCFRunLoopBeforeWaiting:{
////            _waitStartTime = 0;
//            NSLog(@"run loop before waiting---睡");
//            break;
//        }
//            //Inside the event processing loop after the run loop wakes up, but before processing the event that woke it up.
//            //This activity occurs only if the run loop did in fact go to sleep during the current loop
//        case kCFRunLoopAfterWaiting:{
////            _waitStartTime = [[NSDate date] timeIntervalSince1970];
//            NSLog(@"run loop after waiting----起");
//            break;
//        }
//            //The exit of the run loop, after exiting the event processing loop.
//            //This activity occurs once for each call to CFRunLoopRun and CFRunLoopRunInMode
//        case kCFRunLoopExit:
//            NSLog(@"run loop exit");
//            break;
//            /*
//             A combination of all the preceding stages
//             case kCFRunLoopAllActivities:
//             break;
//             */
//        default:
//            break;
//    }
}

- (void)registerObserver
{
    CFRunLoopObserverContext context = {0,(__bridge void*)self,NULL,NULL};
    _observer = CFRunLoopObserverCreate(kCFAllocatorDefault,
                                                            kCFRunLoopAllActivities,
                                                            YES,
                                                            0,
                                                            &runLoopObserverCallBack,
                                                            &context);
    CFRunLoopAddObserver(CFRunLoopGetMain(), _observer, kCFRunLoopCommonModes);
    
    // 创建信号
    _semaphore = dispatch_semaphore_create(0);
    
    // 在子线程监控时长
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        while (YES)
        {
            // 假定连续5次超时50ms认为卡顿(当然也包含了单次超时250ms)
            long st = dispatch_semaphore_wait(_semaphore, dispatch_time(DISPATCH_TIME_NOW, 3.2*NSEC_PER_MSEC));
            if (st != 0)
            {
                if (_activity==kCFRunLoopBeforeSources || _activity==kCFRunLoopAfterWaiting)
                {
                    if (++_countTime < 5)
                        continue;
                    [self logStack];
                    NSLog(@"something lag");
                    
//                    printCallStack(bs_machThreadFromNSThread([NSThread mainThread]));
//                    NSLog(@"-----------");
//                    break;
                }
            }
            _countTime = 0;
        }
    });
}

- (void)logStack{
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    char **strs = backtrace_symbols(callstack, frames);
    int i;
    _backtrace = [NSMutableArray arrayWithCapacity:frames];
    for ( i = 0 ; i < frames ; i++ ){
        [_backtrace addObject:[NSString stringWithUTF8String:strs[i]]];
    }
    free(strs);
}

@end
