//
//  AppDelegate.m
//  KSCrashCut
//
//  Created by 尧德仁 on 2016/12/29.
//  Copyright © 2016年 尧德仁123. All rights reserved.
//

#import "AppDelegate.h"
#import "KSCrash.h"
#import "ArchObj.h"
#import "KSCrashContext.h"
#import "KSCrashSentry_Context.h"

@interface AppDelegate ()

@end

@implementation AppDelegate
const char* ksfu_lastPathEntry2(const char* const path)
{
    if(path == NULL)
    {
        return NULL;
    }
    
    char* lastFile = strrchr(path, '/');
    return lastFile == NULL ? path : lastFile + 1;
}
static void onCrash(const KSCrashReportWriter* writer)
{
    KSCrash_Context* context = writer->context;
    
    NSTimeInterval dt = [[NSDate date] timeIntervalSince1970];
    [ArchObj archiData:[NSNumber numberWithLong:dt] forKey:@"time"];
    [ArchObj archiData:[NSNumber numberWithInt:context->crash.crashType] forKey:@"crashType"];
    NSLog(@"add_by_yao_type = %d,time = %f,count = %d",context->crash.crashType,dt,context->parseResult.backtraceLength);
    
    
    NSMutableDictionary *resDic = [[NSMutableDictionary alloc]initWithCapacity:11];
    NSMutableArray *arr = [[NSMutableArray alloc]initWithCapacity:11];

    int aaa = 0;
    for (int i = 0; i<context->parseResult.backtraceLength-aaa; i++) {
        const Dl_info* const info = &context->parseResult.symbolicated[i];
        id objName = [NSNull null];//否则可能出错
        id symName = [NSNull null];
        if(info->dli_fname != NULL){
            objName = [NSString stringWithUTF8String:ksfu_lastPathEntry2(info->dli_fname)];
            if (objName==NULL) {
            }
        }
        if(info->dli_sname != NULL){
            symName = [NSString stringWithUTF8String:info->dli_sname];
            if (symName==NULL) {
            }
        }
        NSDictionary *tmpDic = @{
                                 @KSCrashField_InstructionAddr:[NSNumber numberWithUnsignedLong:context->parseResult.backtraceBuffer[i]],
                                  @KSCrashField_ObjectAddr:[NSNumber numberWithUnsignedLong:(uintptr_t)info->dli_fbase],
                                  @KSCrashField_SymbolAddr:[NSNumber numberWithUnsignedLong:(uintptr_t)info->dli_saddr],
                                  @KSCrashField_ObjectName:objName,
                                  @KSCrashField_SymbolName:symName
                                 };
        [arr addObject:tmpDic];
    }
    [resDic setValue:arr forKey:@"contents"];
    [resDic setValue:[NSString stringWithFormat:@"%s",context->crash.resCrashReason] forKey:@"resCrashReason"];
    [ArchObj archiData:resDic forKey:@"resDic"];
    
//    NSLog(@"uuid = %s",context->config.crashID);
}

- (void) installCrashHandler
{
    KSCrash* handler = [KSCrash sharedInstance];
    handler.onCrash = onCrash;
    [handler install];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    [self installCrashHandler];
//
    NSNumber *crashType = [ArchObj unArchDataForKey:@"crashType"];
    NSNumber *time = [ArchObj unArchDataForKey:@"time"];
    NSDictionary *resDic = [ArchObj unArchDataForKey:@"resDic"];
    UIAlertView *alt = [[UIAlertView alloc]initWithTitle:@"crashType" message:[NSString stringWithFormat:@"type=%d\ntime = %ld\n%@",[crashType intValue],[time longValue],[resDic description]] delegate:nil cancelButtonTitle:@"yes" otherButtonTitles: nil];
    [alt show];
//
////    float res = 8.92+7.25+4.77+4.35+3.31+2.68+2.22+2.13+1.97+1.93+1.79+1.66+1.55+1.55+1.44+0.37+0.37+0.31+0.22;
////    NSLog(@"res = %f",res);
//    
////    NSLog(@"uuid = %@",)
    
    return YES;
}


@end
