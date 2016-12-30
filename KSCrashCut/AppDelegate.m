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

static void onCrash(const KSCrashReportWriter* writer)
{
    KSCrash_Context* context = writer->context;
    NSLog(@"type = %d",context->crash.crashType);
    
    [ArchObj archiData:[NSNumber numberWithInt:context->crash.crashType] forKey:@"crashType"];
    
}

- (void) installCrashHandler
{
    KSCrash* handler = [KSCrash sharedInstance];
    handler.onCrash = onCrash;
    [handler install];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    [self installCrashHandler];
    
    NSNumber *crashType = [ArchObj unArchDataForKey:@"crashType"];
    UIAlertView *alt = [[UIAlertView alloc]initWithTitle:@"crashType" message:[NSString stringWithFormat:@"%d",[crashType intValue]] delegate:nil cancelButtonTitle:@"yes" otherButtonTitles: nil];
    [alt show];
    return YES;
}


@end
