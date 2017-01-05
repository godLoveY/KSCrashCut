#import <Foundation/Foundation.h>

#import "KSCrashReportWriter.h"
#import "KSCrashType.h"

typedef enum
{
    KSCrashDemangleLanguageNone = 0,
    KSCrashDemangleLanguageCPlusPlus = 1,
    KSCrashDemangleLanguageSwift = 2,
    KSCrashDemangleLanguageAll = ~1
} KSCrashDemangleLanguage;

typedef enum
{
    KSCDeleteNever,
    KSCDeleteOnSucess,
    KSCDeleteAlways
} KSCDeleteBehavior;


@interface KSCrash : NSObject

@property(nonatomic,readwrite,assign) KSCrashType handlingCrashTypes;
@property(nonatomic,readwrite,assign) KSReportWriteCallback onCrash;

+ (KSCrash*) sharedInstance;
- (BOOL) install;

@end
