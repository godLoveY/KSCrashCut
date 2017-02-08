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


#import <arpa/inet.h>
#import <err.h>
#import <ifaddrs.h>
#import <mach-o/dyld.h>
#import <mach-o/arch.h>
#import <net/if_dl.h>
#import <net/if_var.h>
#import <netdb.h>
#import <sys/sysctl.h>
#import <zlib.h>
//#import "TDCrypto.h"
//#import "TDHttpRequest.h"
//#import "TDReachability.h"
//#import "TDUniqueIdentifier.h"
#include <dlfcn.h>

#import "SeMonitorController.h"
#import "PMainThreadWatcher.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

const char*  getAllBinaryImage()
{
//    NSMutableString *ret = [NSMutableString string];
    NSMutableDictionary *resDic = [[NSMutableDictionary alloc]initWithCapacity:11];
    
    const uint32_t imageCount = _dyld_image_count();
    for(uint32_t iImg = 0; iImg < imageCount; iImg++)
    {
        @try {
            const char *dyld = _dyld_get_image_name(iImg);
            long slength = strlen(dyld);
            
            long j;
            for (j = slength - 1; j>= 0; --j) {
                if (dyld[j] == '/') {
                    break;
                }
            }
            
            ++j;
            char *name = strndup(dyld + j, slength - j);
            NSString *nameStr = [NSString stringWithFormat:@"%s",name];
//            [ret appendFormat:@"Process name: %s\r\n", name];
            free(name);
            
            const struct mach_header *header = _dyld_get_image_header(iImg);
            uint8_t *header_ptr = (uint8_t*)header;
            typedef struct load_command load_command;
            if (header->magic == MH_MAGIC_64 || header->magic == MH_CIGAM_64) {
                header_ptr += sizeof(struct mach_header_64);
            } else {
                header_ptr += sizeof(struct mach_header);
            }
            load_command *command = (load_command*)header_ptr;
            
            for (int i = 0; i < header->ncmds > 0; ++i) {
                if (command->cmd == LC_UUID) {
                    struct uuid_command ucmd = *(struct uuid_command*)header_ptr;
                    CFUUIDRef cuuid = CFUUIDCreateFromUUIDBytes(kCFAllocatorDefault, *((CFUUIDBytes*)ucmd.uuid));
                    CFStringRef suuid = CFUUIDCreateString(kCFAllocatorDefault, cuuid);
//                    [ret appendFormat:@"dSYM UUID: %@\r\n", (__bridge NSString*)suuid];
                    [resDic setValue: (__bridge NSString*)suuid forKey:nameStr];
                    CFRelease(cuuid);
                    CFRelease(suuid);
                    break;
                }
                
                header_ptr += command->cmdsize;
                command = (load_command*)header_ptr;
            }
            if (iImg==0) {
                const NXArchInfo *info = NXGetArchInfoFromCpuType(header->cputype, header->cpusubtype);
                [resDic setValue:[NSString stringWithFormat:@"%s",info->name] forKey:@"Architecture"];
            }
        }
        @catch (NSException *exception) {
        }
    }
    
    return [[resDic description] UTF8String];
}
const char* getBinaryImage()
{
    NSMutableString *ret = [NSMutableString string];
    @try {
        const char *dyld = _dyld_get_image_name(0);
        long slength = strlen(dyld);
        
        long j;
        for (j = slength - 1; j>= 0; --j) {
            if (dyld[j] == '/') {
                break;
            }
        }
        
        ++j;
        char *name = strndup(dyld + j, slength - j);
        [ret appendFormat:@"Process name: %s\r\n", name];
        free(name);
        
        const struct mach_header *header = _dyld_get_image_header(0);
        uint8_t *header_ptr = (uint8_t*)header;
        typedef struct load_command load_command;
        if (header->magic == MH_MAGIC_64 || header->magic == MH_CIGAM_64) {
            header_ptr += sizeof(struct mach_header_64);
        } else {
            header_ptr += sizeof(struct mach_header);
        }
        load_command *command = (load_command*)header_ptr;
        
        for (int i = 0; i < header->ncmds > 0; ++i) {
            if (command->cmd == LC_UUID) {
                struct uuid_command ucmd = *(struct uuid_command*)header_ptr;
                CFUUIDRef cuuid = CFUUIDCreateFromUUIDBytes(kCFAllocatorDefault, *((CFUUIDBytes*)ucmd.uuid));
                CFStringRef suuid = CFUUIDCreateString(kCFAllocatorDefault, cuuid);
                [ret appendFormat:@"dSYM UUID: %@\r\n", (__bridge NSString*)suuid];
                CFRelease(cuuid);
                CFRelease(suuid);
                break;
            }
            
            header_ptr += command->cmdsize;
            command = (load_command*)header_ptr;
        }
        
        const NXArchInfo *info = NXGetArchInfoFromCpuType(header->cputype, header->cpusubtype);
        [ret appendFormat:@"Architecture: %s\r\n", info->name];
        
        [ret appendFormat:@"Start Address: %p\r\n", header];
        
        intptr_t slide = _dyld_get_image_vmaddr_slide(0);
        [ret appendFormat:@"Slide Address: 0x%lX\r\n", slide];
    }
    @catch (NSException *exception) {
    }
    
    return [ret UTF8String];
}
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
//        [NSNumber numberWithUnsignedLong:(uintptr_t)info->dli_saddr]
//        [NSNumber numberWithUnsignedLong:(uintptr_t)info->dli_fbase]
//        [NSNumber numberWithUnsignedLong:context->parseResult.backtraceBuffer[i]]
        NSDictionary *tmpDic = @{
                                 @KSCrashField_InstructionAddr:[NSString stringWithFormat:@"0x%lx",context->parseResult.backtraceBuffer[i]],
                                  @KSCrashField_ObjectAddr:[NSString stringWithFormat:@"0x%lx",(uintptr_t)info->dli_fbase],
                                  @KSCrashField_SymbolAddr:[NSString stringWithFormat:@"0x%lx",(uintptr_t)info->dli_saddr],
                                  @KSCrashField_ObjectName:objName,
                                  @KSCrashField_SymbolName:symName
                                 };
        [arr addObject:tmpDic];
    }
    [resDic setValue:arr forKey:@"contents"];
    [resDic setValue:[NSString stringWithFormat:@"%s",getAllBinaryImage()] forKey:@"getBinaryImage"];
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

    NSNumber *crashType = [ArchObj unArchDataForKey:@"crashType"];
    NSNumber *time = [ArchObj unArchDataForKey:@"time"];
    NSDictionary *resDic = [ArchObj unArchDataForKey:@"resDic"];
    UIAlertView *alt = [[UIAlertView alloc]initWithTitle:@"crashType" message:[NSString stringWithFormat:@"type=%d\ntime = %ld\n%@",[crashType intValue],[time longValue],[resDic description]] delegate:nil cancelButtonTitle:@"yes" otherButtonTitles: nil];
    [alt show];
    
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        NSLog(@"开始卡顿检测 current thread = %@", [NSThread currentThread]);
        [[SeMonitorController sharedInstance] startMonitor];
        [[PMainThreadWatcher sharedInstance] startWatch];
    });
    
    return YES;
}


@end
