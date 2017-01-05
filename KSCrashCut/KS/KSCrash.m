#import "KSCrash.h"
#import "KSCrashC.h"

#if KSCRASH_HAS_UIKIT
#import <UIKit/UIKit.h>
#endif

@implementation KSCrash

@synthesize handlingCrashTypes = _handlingCrashTypes;
@synthesize onCrash = _onCrash;

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
    _handlingCrashTypes = kscrash_install("","");
    if(self.handlingCrashTypes == 0)
    {
        return false;
    }
    return true;
}

@end

