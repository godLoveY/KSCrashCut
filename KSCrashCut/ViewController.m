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

@interface ViewController ()

@property(nonatomic,strong)Crasher *crasher;
@property(nonatomic,strong)NSArray *arr;

@end

@implementation ViewController

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
//        [self onCrash2:bt];
    }
}

@end
