//
//  ArchObj.m
//  KSCrashCut
//
//  Created by 尧德仁 on 2016/12/29.
//  Copyright © 2016年 尧德仁123. All rights reserved.
//

#import "ArchObj.h"

@implementation ArchObj

static ArchObj *sharedInstance = nil;
+ (instancetype) sharedInstance
{
    static dispatch_once_t onceToken;
    
    dispatch_once(&onceToken, ^{
        sharedInstance = [[ArchObj alloc] init];
    });
    return sharedInstance;
}

+ (void)archiData:(id)obj forKey:(NSString *)key
{
    NSString *homeDirectory = NSHomeDirectory();
    NSString *fileName = @"deren123";//[[NSUserDefaults standardUserDefaults] objectForKey:@"userId"];
    
    NSString *filePath = [homeDirectory stringByAppendingPathComponent:[NSString stringWithFormat:@"%@%@",@"Documents/",fileName]];
    
    //
    NSMutableData *unArchiData = [[NSMutableData alloc]initWithContentsOfFile:filePath];
    NSMutableDictionary *all = NULL;
    if (unArchiData != NULL) {
        NSKeyedUnarchiver *unArchi = [[NSKeyedUnarchiver alloc]initForReadingWithData:unArchiData];
        all = [unArchi decodeObjectForKey:@"all"];
    }
    if (all==NULL) {
        all = [[NSMutableDictionary alloc]initWithCapacity:1];
    }
    //
    NSMutableData *archiData = [[NSMutableData alloc]init];
    NSKeyedArchiver *archi = [[NSKeyedArchiver alloc]initForWritingWithMutableData:archiData];
    [all setValue:obj forKey:key];
    [archi encodeObject:all forKey:@"all"];
    [archi finishEncoding];
    if([archiData writeToFile:filePath atomically:1])
    {
        NSLog(@"-archiData--success");
    }
}

+ (id)unArchDataForKey:(NSString *)key
{
    NSString *homeDirectory = NSHomeDirectory();
    NSString *fileName = @"deren123";//[[NSUserDefaults standardUserDefaults] objectForKey:@"userId"];
    
    NSString *filePath = [homeDirectory stringByAppendingPathComponent:[NSString stringWithFormat:@"%@%@",@"Documents/",fileName]];
    //    NSLog(@"filePath=%@",filePath);
    
    id obj = NULL;
    NSMutableData *unArchiData = [[NSMutableData alloc]initWithContentsOfFile:filePath];
    NSMutableDictionary *all = NULL;
    if (unArchiData != NULL) {
        NSKeyedUnarchiver *unArchi = [[NSKeyedUnarchiver alloc]initForReadingWithData:unArchiData];
        all = [unArchi decodeObjectForKey:@"all"];
    }
    if (all != NULL) {
        obj = [all objectForKey:key];
        return obj;
    }
    NSLog(@"解归档失败");
    return obj;
}

@end
