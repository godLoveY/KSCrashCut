//
//  Crasher.h
//
//  Created by Karl Stenerud on 2012-01-28.
//

#import <Foundation/Foundation.h>

@interface Crasher : NSObject

- (void) throwUncaughtNSException;//差不多

- (void) dereferenceBadPointer;/* 这里pad确实会少一条
                                ks:   堆栈不显示、atos正常
                                pad:  堆栈不显示、atos显示signalHandler
                                友盟:  堆栈不显示、atos解析紊乱居然到ks的文件
                                */

- (void) dereferenceNullPointer;/*
                                 ks:   堆栈不显示、atos正常
                                 pad:  堆栈不显示、atos显示signalHandler
                                 友盟:  堆栈不显示、atos解析紊乱居然到ks的文件
                                 */

- (void) useCorruptObject;

- (void) spinRunloop;

- (void) causeStackOverflow;//递归调用，死循环

- (void) doAbort;//PAD 收集不到（PAD自己屏蔽了），友盟有问题（基地址大于异常的 堆栈中的那个地址 ），ks的atos正常

- (void) doDiv0;//

- (void) doIllegalInstruction;//PAD 还是只atos显示signalHandler，ks的第一条记录显示也不太正常（typeinfo name for MyException (in testPAD_xcode7) + 4）原生xcode的解析结果也是这样，友盟的地址网页端显示错误（通过友盟的解析工具能解析出但也不能解析到最后一条）

- (void) accessDeallocatedObject;////nsexcept 错误，但友盟网页显示的地址还是有问题

- (void) accessDeallocatedPtrProxy;//PAD 还是只atos显示signalHandler ,ks正常，友盟最后一条解析不正常，倒数第二条可以（这里的倒数第二相当于ks的倒数第一）

- (void) zombieNSException;//基本同上

- (void) corruptMemory;//PAD、友盟正常，KS居然捕捉不到*******官方的异常也发不出来

- (void) deadlock;//

- (void) pthreadAPICrash;//友盟出现卡死，其它正常

- (void) userDefinedCrash;

- (void) throwUncaughtCPPException;//PAD 捕捉不到,友盟好像也拦截不到，ks正常

@end
