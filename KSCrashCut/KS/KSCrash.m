#import "KSCrash.h"
#import "KSCrashC.h"
#import "fishhook.h"
#import <mach/mach_types.h>

#if KSCRASH_HAS_UIKIT
#import <UIKit/UIKit.h>
#endif

@implementation KSCrash

@synthesize handlingCrashTypes = _handlingCrashTypes;
@synthesize onCrash = _onCrash;

static kern_return_t (*orig_mach_port_insert_right)
(
	ipc_space_t task,
	mach_port_name_t name,
	mach_port_t poly,
	mach_msg_type_name_t polyPoly
 );

kern_return_t my_mach_port_insert_right
(
	ipc_space_t task,
	mach_port_name_t name,
	mach_port_t poly,
	mach_msg_type_name_t polyPoly
 )
{
    if (polyPoly==999) {
        return orig_mach_port_insert_right(task,name,poly,MACH_MSG_TYPE_MAKE_SEND);
    }else{
        
        
        kern_return_t res = orig_mach_port_insert_right(task,name,poly,polyPoly);
        dispatch_async(dispatch_get_main_queue(), ^{
            ks_reinstsallMach();
        });
        return res;
    }
    
}


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
        rebind_symbols((struct rebinding[1]){{"mach_port_insert_right", my_mach_port_insert_right, (void *)&orig_mach_port_insert_right}}, 1);
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

