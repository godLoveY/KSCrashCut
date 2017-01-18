//
//  ViewController.m
//  testPAD_xcode7
//
//  Created by 尧德仁 on 2016/10/31.
//  Copyright © 2016年 尧德仁. All rights reserved.
//

#import "ViewController.h"
//#import "TalkingData.h"
#import <signal.h>
#import "Crasher.h"
#import "KSCrashReport.h"
#import <mach/mach.h>
#include <pthread.h>
//#import "BSBacktraceLogger.h"

@interface ViewController ()

@property(nonatomic,strong)Crasher *crasher;
@property(nonatomic,strong)NSArray *arr;

@end

static mach_port_t main_thread_id;
@implementation ViewController

+ (void)load {
    main_thread_id = mach_thread_self();
}
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


- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    _arr = @[@"throwUncaughtNSException",@"dereferenceBadPointer",@"dereferenceNullPointer",@"useCorruptObject",@"spinRunloop",@"causeStackOverflow",@"doAbort",@"doDiv0",@"doIllegalInstruction",@"accessDeallocatedObject",@"accessDeallocatedPtrProxy",@"zombieNSException",@"corruptMemory",@"deadlock",@"pthreadAPICrash",@"userDefinedCrash",@"throwUncaughtCPPException",@"",@"",@""];
    
    int x = 0;
    int y = 0;
    for (int i =0 ; i<20; i++) {
        y = i;
        x = 0;
        if (i>=10) {
            x = 210;
            y = i-10;
        }
        int tag = i+101;
        UIButton *button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
        button.backgroundColor = [UIColor grayColor];
        NSString *title = _arr[i];
        if (title.length==0) {
            title = [NSString stringWithFormat:@"%d",tag];
        }
        [button setTitle:title forState:UIControlStateNormal];
        [button setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        button.frame = CGRectMake(x, 64+55*y, 200, 50);
        button.tag = tag;
        [button addTarget:self action:@selector(buttonAction:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:button];
    }
    self.crasher = [[Crasher alloc] init];
    
    //test runloop mode
//    UITableView *tabView = [[UITableView alloc]initWithFrame:CGRectMake(10, 620, 200, 95)];
//    tabView.backgroundColor = [UIColor redColor];
//    [self.view addSubview:tabView];
}
- (void) onCrash1:(__unused id) sender
{
    char* ptr = (char*)-1;
    *ptr = 10;
}

- (void)onCrash2:(__unused id) sender
{
    NSString *str = @"2";
    [str substringFromIndex:11];
}

- (void) onCrash3:(__unused id) sender
{
    NSString *str = [[NSString alloc]initWithFormat:@"123"];
    __weak NSString *weakStr = str;
    
    dispatch_async(dispatch_get_main_queue(), ^
                   {
                       NSLog(@"weakStr = %@",weakStr);
                   });
}

- (void)onCrash4:(id)sender
{
//    dispatch_queue_t gQueue =dispatch_get_global_queue (DISPATCH_QUEUE_PRIORITY_DEFAULT,0);
    dispatch_queue_t gQueue = dispatch_queue_create("com.lai.www", DISPATCH_QUEUE_CONCURRENT);
    for (int i = 0; i<1; i++) {
        dispatch_async(gQueue, ^{
            NSLog(@"thread = %@",[NSThread currentThread]);
            [NSTimer scheduledTimerWithTimeInterval:2.0f target:self selector:@selector(timerAction) userInfo:nil repeats:YES];
           [[NSRunLoop currentRunLoop] run];
        });
    }
    
//    [NSTimer scheduledTimerWithTimeInterval:2.0f target:self selector:@selector(timerAction) userInfo:nil repeats:YES];
}
- (void)onCrash5:(id)sender
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
        printCallStack(bs_machThreadFromNSThread([NSThread currentThread]));
//        BSLOG_MAIN;
    });
    [self foo];
}

- (void)timerAction
{
//    NSLog(@"time_thread = %@",[NSThread currentThread]);
    NSLog(@"---");
}
- (void)foo {
    [self bar];
}

- (void)bar {
    while (true) {
        ;
    }
}
- (void)buttonAction:(UIButton*)bt
{
    NSString *str = _arr[bt.tag-101];
    if (str.length>0) {
        SEL select = NSSelectorFromString(str);
        if ([_crasher respondsToSelector:select]) {
            IMP imp = [_crasher methodForSelector:select];
            void (*func)(id, SEL) = (void *)imp;
            func(_crasher, select);
        }
    }else{
//        [self onCrash4:bt];
    }
}

@end
