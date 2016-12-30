//
//  ArchObj.h
//  KSCrashCut
//
//  Created by 尧德仁 on 2016/12/29.
//  Copyright © 2016年 尧德仁123. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface ArchObj : NSObject

+ (instancetype) sharedInstance;
+ (void)archiData:(id)obj forKey:(NSString*)key;
+ (id)unArchDataForKey:(NSString *)key;

@end
