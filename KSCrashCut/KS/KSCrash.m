//
//  KSCrash.m
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


#import "KSCrash.h"

#import "KSCrashC.h"
//#import "KSCrashDoctor.h"
//#import "KSCrashReportFields.h"
//#import "KSCrashReportFixer.h"
//#import "KSCrashReportStore.h"
//#import "KSCrashState.h"
//#import "KSJSONCodecObjC.h"
//#import "NSError+SimpleConstructor.h"
//#import "KSCrashReportWriter.h"//add by yao


#if KSCRASH_HAS_UIKIT
#import <UIKit/UIKit.h>
#endif


// ============================================================================
#pragma mark - Globals -
// ============================================================================

@interface KSCrash ()

//@property(nonatomic,readwrite,retain) NSString* bundleName;
//@property(nonatomic,readwrite,retain) NSString* basePath;

@end


//static NSString* getBundleName()
//{
//    NSString* bundleName = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleName"];
//    if(bundleName == nil)
//    {
//        bundleName = @"Unknown";
//    }
//    return bundleName;
//}
//
//static NSString* getBasePath()
//{
//    NSArray* directories = NSSearchPathForDirectoriesInDomains(NSCachesDirectory,
//                                                               NSUserDomainMask,
//                                                               YES);
//    if([directories count] == 0)
//    {
//        KSLOG_ERROR2(@"Could not locate cache directory path.");
//        return nil;
//    }
//    NSString* cachePath = [directories objectAtIndex:0];
//    if([cachePath length] == 0)
//    {
//        KSLOG_ERROR2(@"Could not locate cache directory path.");
//        return nil;
//    }
//    NSString* pathEnd = [@"KSCrash" stringByAppendingPathComponent:getBundleName()];
//    return [cachePath stringByAppendingPathComponent:pathEnd];
//}


@implementation KSCrash

// ============================================================================
#pragma mark - Properties -
// ============================================================================

//@synthesize sink = _sink;
//@synthesize userInfo = _userInfo;
//@synthesize deleteBehaviorAfterSendAll = _deleteBehaviorAfterSendAll;
@synthesize handlingCrashTypes = _handlingCrashTypes;
//@synthesize deadlockWatchdogInterval = _deadlockWatchdogInterval;
//@synthesize printTraceToStdout = _printTraceToStdout;
@synthesize onCrash = _onCrash;
//@synthesize bundleName = _bundleName;
//@synthesize basePath = _basePath;
//@synthesize searchThreadNames = _searchThreadNames;
//@synthesize searchQueueNames = _searchQueueNames;
//@synthesize introspectMemory = _introspectMemory;
//@synthesize catchZombies = _catchZombies;
//@synthesize doNotIntrospectClasses = _doNotIntrospectClasses;
//@synthesize demangleLanguages = _demangleLanguages;
//@synthesize redirectConsoleLogToFile = _redirectConsoleLogToFile;


// ============================================================================
#pragma mark - Lifecycle -
// ============================================================================

+ (instancetype) sharedInstance
{
    static KSCrash *sharedInstance = nil;
    static dispatch_once_t onceToken;
    
    dispatch_once(&onceToken, ^{
        sharedInstance = [[KSCrash alloc] init];
    });
    return sharedInstance;
}

- (id) init
{
    if((self = [super init]))
    {
//        self.bundleName = getBundleName();
//        self.basePath = getBasePath();
//        if(self.basePath == nil)
//        {
//            KSLOG_ERROR2(@"Failed to initialize crash handler. Crash reporting disabled.");
//            return nil;
//        }
    }
    return self;
}
- (void) setOnCrash:(KSReportWriteCallback) onCrash
{
    _onCrash = onCrash;
    kscrash_setCrashNotifyCallback(onCrash);
}


- (BOOL) install
{
//    _handlingCrashTypes = kscrash_install(self.bundleName.UTF8String,
//                                          self.basePath.UTF8String);
    _handlingCrashTypes = kscrash_install("","");
    if(self.handlingCrashTypes == 0)
    {
        return false;
    }
    return true;
}

@end


////! Project version number for KSCrashFramework.
//const double KSCrashFrameworkVersionNumber = 1.112;
//
////! Project version string for KSCrashFramework.
//const unsigned char KSCrashFrameworkVersionString[] = "1.11.2";
