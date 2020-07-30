clang -arch armv7 -S aaa.c -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS10.2.sdk

weex compile ./src/foo.vue ./js -w

http://proxy.paic.com.cn/proxyforwebx.pac


1，class-dump -H ./SysSecInfo.app -o ./headFile
2，svn ci '05SDK/iOS集成SDK 文档1103/ios_sdk_开发集成说明1103.html' -m "集成CIO需求"
3，svn co svn://10.20.8.33/SkyEyeDoc
4,-mllvm –ecs -mllvm -cxf -mllvm -equ -mllvm -fcf -mllvm -chenxify-prob=10 -mllvm -falsify-loop=2 -mllvm -falsify-prob=10

5,-mllvm -cxf -mllvm -equ -mllvm -fcf -mllvm -chenxify-prob=10 -mllvm -falsify-loop=2 -mllvm -falsify-prob=10

6,lipo -info libTalkingData.a
    armv7 armv7s i386 x86_64 arm64
7,lipo -create 1.a 2.a -output 12.a
8,otool -arch armv7 -l libTalkingData.a | grep __bitcode | wc -l
有数字输出就是bitcode
0就是无bitcode

9，grep -r advertisingIdentifier . cd到对应的静态库目录执行
显示Mac隐藏文件的命令：defaults write com.apple.finder AppleShowAllFiles  YES
隐藏Mac隐藏文件的命令：defaults write com.apple.finder AppleShowAllFiles  NO

10.git clone ssh://git@git.palife.com:2222/m3-iOS/sky-eye-ios.git  密码：Pa888888

git status
git add .
git commit -m "first ci"
git push

11.

git push origin branch1  创建远程分支
git push origin :Branch1 删除分支  (分支名前的冒号代表删除)
//测试拉取另一个分支内容（先拉取仓库，默认拉到的是master，然后git checkout branch1切换就可以）
C02R4DDFFVH5:testGitLab yaoderen131$ git branch
* master
C02R4DDFFVH5:testGitLab yaoderen131$ git branch -r
  origin/HEAD -> origin/master
  origin/branch1
  origin/master
C02R4DDFFVH5:testGitLab yaoderen131$ git checkout branch1
Branch branch1 set up to track remote branch branch1 from origin.
Switched to a new branch 'branch1'
C02R4DDFFVH5:testGitLab yaoderen131$ git branch
* branch1
  master

//git hub
…or create a new repository on the command line

echo "# KSCrashCut" >> README.md
git init
git add README.md
git commit -m "first commit"
git remote add origin https://github.com/godLoveY/KSCrashCut.git
git push -u origin master

…or push an existing repository from the command line

git remote add origin https://github.com/godLoveY/KSCrashCut.git
git push -u origin master

release 模式
xcrun atos -o ./KSCrashCut -arch arm64 -l 0x1000a4000 0x1000aeedc
-[Crasher dereferenceBadPointer] (in KSCrashCut) (Crasher.mm:92)

系统函数符号化（一般需要找对系统对应的库）

///Users/yaoderen131/Library/Developer/Xcode/iOS DeviceSupport/10.0.2 (14A456)/Symbols/System/Library/Frameworks/UIKit.framework

 atos -o ./UIKit -arch arm64 -l 0x18c857000 0x18c89bd30
 dwarfdump --uuid ./Flutter 

系统库的解压（教程地址：https://gold.xitu.io/entry/57e0eb1dd203090069f55b55）
步骤：	1，下载https://opensource.apple.com/tarballs/dyld/ 下载360.22压缩包
	2，解压，然后cd dyld-360.22/launch-cache ，修改dsc_extractor.cpp(将main函数之前的#if0改成#if 1)

	3，clang dsc_extractor.cpp dsc_iterator.cpp -lstdc++ -o dsc_extractor

系统库解压命令：
./dsc_extractor ./dyld_shared_cache_arm64 ./decachDyld/

看云相关资料
android src:
http://fcloud.paic.com.cn/f/6b186d24f5/
 
ios src:
http://fcloud.paic.com.cn/f/95da886ca4/
 
doc:
http://fcloud.paic.com.cn/f/79f1b69d22/

mixPannel A/B testing不能进入可先埋一个点 然后进去看看
 
去掉错误
2>/dev/null


{"type":"snapshot_request","payload":{"config":{"enums":[{"name":"UILayoutConstraintAxis","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Horizontal"},{"value":1,"display_name":"Vertical"}]},{"name":"UIControlState","flag_set":true,"base_type":"NSUInteger","values":[{"value":0,"display_name":"Normal"},{"value":1,"display_name":"Highlighted"},{"value":2,"display_name":"Disabled"},{"value":4,"display_name":"Selected"}]},{"name":"UIControlContentVerticalAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Center"},{"value":1,"display_name":"Top"},{"value":2,"display_name":"Bottom"},{"value":3,"display_name":"Fill"}]},{"name":"UIControlContentHorizontalAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Center"},{"value":1,"display_name":"Left"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Fill"}]},{"name":"NSLayoutRelation","base_type":"NSInteger","flag_set":false,"values":[{"value":-1,"display_name":"LessThanOrEqual"},{"value":0,"display_name":"Equal"},{"value":1,"display_name":"GreaterThanOrEqual"}]},{"name":"NSLayoutAttribute","base_type":"NSInteger","flag_set":false,"values":[{"value":1,"display_name":"Left"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Top"},{"value":4,"display_name":"Bottom"},{"value":5,"display_name":"Leading"},{"value":6,"display_name":"Trailing"},{"value":7,"display_name":"Width"},{"value":8,"display_name":"Height"},{"value":9,"display_name":"CenterX"},{"value":10,"display_name":"CenterY"},{"value":11,"display_name":"Baseline"},{"value":0,"display_name":"NotAnAttribute"}]},{"name":"UIControlEvents","base_type":"NSUInteger","flag_set":true,"values":[{"value":1,"display_name":"TouchDown"},{"value":2,"display_name":"TouchDownRepeat"},{"value":4,"display_name":"TouchDragInside"},{"value":8,"display_name":"TouchDragOutside"},{"value":16,"display_name":"TouchDragEnter"},{"value":32,"display_name":"TouchDragExit"},{"value":64,"display_name":"TouchUpInside"},{"value":128,"display_name":"TouchUpOutside"},{"value":256,"display_name":"TouchCancel"},{"value":4096,"display_name":"ValueChanged"},{"value":65536,"display_name":"EditingDidBegin"},{"value":131072,"display_name":"EditingChanged"},{"value":262144,"display_name":"EditingDidEnd"},{"value":524288,"display_name":"EditingDidEndOnExit"},{"value":4095,"display_name":"AllTouchEvents"},{"value":983040,"display_name":"AllEditingEvents"},{"value":251658240,"display_name":"ApplicationReserved"},{"value":4026531840,"display_name":"SystemReserved"},{"value":4294967295,"display_name":"AllEvents"}]},{"name":"UIBarButtonItemStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Plain"},{"value":1,"display_name":"Bordered"},{"value":2,"display_name":"Done"}]},{"name":"NSTextAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Left"},{"value":1,"display_name":"Center"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Justified"},{"value":4,"display_name":"Natural"}]},{"name":"NSLineBreakMode","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"WordWrapping"},{"value":1,"display_name":"CharWrapping"},{"value":2,"display_name":"Clipping"},{"value":3,"display_name":"TruncatingHead"},{"value":4,"display_name":"TruncatingTail"},{"value":5,"display_name":"TruncatingMiddle"}]},{"name":"UIBaselineAdjustment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"AlignBaselines"},{"value":1,"display_name":"AlignCenters"},{"value":2,"display_name":"None"}]},{"name":"UIScrollViewIndicatorStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Default"},{"value":1,"display_name":"Black"},{"value":2,"display_name":"White"}]},{"name":"UITableViewStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"UITableViewStylePlain"},{"value":1,"display_name":"UITableViewStyleGrouped"}]}],"classes":[{"name":"NSObject","superclass":null,"properties":[]},{"name":"UIResponder","superclass":"NSObject","properties":[]},{"name":"UIScreen","superclass":"NSObject","properties":[{"name":"bounds","type":"CGRect","readonly":true},{"name":"applicationFrame","type":"CGRect","readonly":true},{"name":"scale","type":"CGFloat","readonly":true}]},{"name":"UIStoryboardSegueTemplate","superclass":"NSObject","properties":[{"name":"identifier","type":"NSString","readonly":true},{"name":"viewController","type":"UIViewController"},{"name":"performOnViewLoad","type":"BOOL"}]},{"name":"UIView","superclass":"UIResponder","properties":[{"name":"userInteractionEnabled","type":"BOOL"},{"name":"tag","type":"NSInteger"},{"name":"center","type":"CGPoint"},{"name":"alpha","type":"CGFloat"},{"name":"opaque","type":"BOOL"},{"name":"hidden","type":"BOOL"},{"name":"frame","type":"CGRect"},{"name":"bounds","type":"CGRect"},{"name":"superview","type":"UIView"},{"name":"window","type":"UIWindow"},{"name":"subviews","type":"NSArray"},{"name":"se_fingerprintVersion","type":"NSArray","use_kvc":false},{"name":"se_varA","type":"NSString","use_kvc":false},{"name":"se_varB","type":"NSString","use_kvc":false},{"name":"se_varC","type":"NSString","use_kvc":false},{"name":"se_varSetD","type":"NSArray","use_kvc":false},{"name":"se_varE","type":"NSString","use_kvc":false},{"name":"se_clickableState","type":"NSDictionary","use_kvc":false}]},{"name":"UIImageView","superclass":"UIView","properties":[]},{"name":"UIControl","superclass":"UIView","properties":[{"name":"enabled","type":"BOOL"},{"name":"subviews","type":"NSArray","nofollow":true}]},{"name":"UITabBar","superclass":"UIView","properties":[],"delegateImplements":[{"selector":"tabBar:didSelectItem:"}]},{"name":"UIScrollView","superclass":"UIView","properties":[]},{"name":"UICollectionReusableView","superclass":"UIView","properties":[]},{"name":"UITextView","superclass":"UIScrollView","properties":[]},{"name":"UICollectionView","superclass":"UIScrollView","properties":[{"name":"subviews","type":"NSArray","nofollow":false}],"delegateImplements":[{"selector":"collectionView:didSelectItemAtIndexPath:"}]},{"name":"UICollectionViewCell","superclass":"UICollectionReusableView","properties":[{"name":"subviews","type":"NSArray","nofollow":false},{"name":"cellIndexPath","type":"NSString","use_kvc":false}]},{"name":"UITableView","superclass":"UIScrollView","properties":[{"name":"subviews","type":"NSArray","nofollow":false}],"delegateImplements":[{"selector":"tableView:didSelectRowAtIndexPath:"}]},{"name":"UITableViewCell","superclass":"UIView","properties":[{"name":"subviews","type":"NSArray","nofollow":false},{"name":"cellIndexPath","type":"NSString","use_kvc":false}]},{"name":"UIButton","superclass":"UIControl","properties":[]},{"name":"UIWindow","superclass":"UIView","properties":[{"name":"keyWindow","type":"BOOL","readonly":true},{"name":"rootViewController","type":"UIViewController"},{"name":"screen","type":"UIScreen","readonly":true}]},{"name":"UILabel","superclass":"UIView","properties":[{"name":"text","type":"NSString"},{"name":"attributedText","type":"NSAttributedString"}]},{"name":"UIViewController","superclass":"UIResponder","properties":[{"name":"isViewLoaded","type":"BOOL","readonly":true},{"name":"view","type":"UIView","predicate":"self.isViewLoaded == YES"},{"name":"title","type":"NSString"},{"name":"parentViewController","type":"UIViewController"},{"name":"presentedViewController","type":"UIViewController"},{"name":"presentingViewController","type":"UIViewController"},{"name":"childViewControllers","type":"NSArray"},{"name":"_storyboardSegueTemplates","type":"NSArray"}]}]}}}




{"type":"snapshot_request","payload":{"config":{"enums":[{"name":"UILayoutConstraintAxis","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Horizontal"},{"value":1,"display_name":"Vertical"}]},{"name":"UIControlState","flag_set":true,"base_type":"NSUInteger","values":[{"value":0,"display_name":"Normal"},{"value":1,"display_name":"Highlighted"},{"value":2,"display_name":"Disabled"},{"value":4,"display_name":"Selected"}]},{"name":"UIControlContentVerticalAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Center"},{"value":1,"display_name":"Top"},{"value":2,"display_name":"Bottom"},{"value":3,"display_name":"Fill"}]},{"name":"UIControlContentHorizontalAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Center"},{"value":1,"display_name":"Left"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Fill"}]},{"name":"NSLayoutRelation","base_type":"NSInteger","flag_set":false,"values":[{"value":-1,"display_name":"LessThanOrEqual"},{"value":0,"display_name":"Equal"},{"value":1,"display_name":"GreaterThanOrEqual"}]},{"name":"NSLayoutAttribute","base_type":"NSInteger","flag_set":false,"values":[{"value":1,"display_name":"Left"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Top"},{"value":4,"display_name":"Bottom"},{"value":5,"display_name":"Leading"},{"value":6,"display_name":"Trailing"},{"value":7,"display_name":"Width"},{"value":8,"display_name":"Height"},{"value":9,"display_name":"CenterX"},{"value":10,"display_name":"CenterY"},{"value":11,"display_name":"Baseline"},{"value":0,"display_name":"NotAnAttribute"}]},{"name":"UIControlEvents","base_type":"NSUInteger","flag_set":true,"values":[{"value":1,"display_name":"TouchDown"},{"value":2,"display_name":"TouchDownRepeat"},{"value":4,"display_name":"TouchDragInside"},{"value":8,"display_name":"TouchDragOutside"},{"value":16,"display_name":"TouchDragEnter"},{"value":32,"display_name":"TouchDragExit"},{"value":64,"display_name":"TouchUpInside"},{"value":128,"display_name":"TouchUpOutside"},{"value":256,"display_name":"TouchCancel"},{"value":4096,"display_name":"ValueChanged"},{"value":65536,"display_name":"EditingDidBegin"},{"value":131072,"display_name":"EditingChanged"},{"value":262144,"display_name":"EditingDidEnd"},{"value":524288,"display_name":"EditingDidEndOnExit"},{"value":4095,"display_name":"AllTouchEvents"},{"value":983040,"display_name":"AllEditingEvents"},{"value":251658240,"display_name":"ApplicationReserved"},{"value":4026531840,"display_name":"SystemReserved"},{"value":4294967295,"display_name":"AllEvents"}]},{"name":"UIBarButtonItemStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Plain"},{"value":1,"display_name":"Bordered"},{"value":2,"display_name":"Done"}]},{"name":"NSTextAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Left"},{"value":1,"display_name":"Center"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Justified"},{"value":4,"display_name":"Natural"}]},{"name":"NSLineBreakMode","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"WordWrapping"},{"value":1,"display_name":"CharWrapping"},{"value":2,"display_name":"Clipping"},{"value":3,"display_name":"TruncatingHead"},{"value":4,"display_name":"TruncatingTail"},{"value":5,"display_name":"TruncatingMiddle"}]},{"name":"UIBaselineAdjustment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"AlignBaselines"},{"value":1,"display_name":"AlignCenters"},{"value":2,"display_name":"None"}]},{"name":"UIScrollViewIndicatorStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Default"},{"value":1,"display_name":"Black"},{"value":2,"display_name":"White"}]},{"name":"UITableViewStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"UITableViewStylePlain"},{"value":1,"display_name":"UITableViewStyleGrouped"}]}],"classes":[{"name":"NSObject","superclass":null,"properties":[]},{"name":"UIResponder","superclass":"NSObject","properties":[]},{"name":"UIScreen","superclass":"NSObject","properties":[{"name":"bounds","type":"CGRect","readonly":true},{"name":"applicationFrame","type":"CGRect","readonly":true},{"name":"scale","type":"CGFloat","readonly":true}]},{"name":"UIStoryboardSegueTemplate","superclass":"NSObject","properties":[{"name":"identifier","type":"NSString","readonly":true},{"name":"viewController","type":"UIViewController"},{"name":"performOnViewLoad","type":"BOOL"}]},{"name":"UIView","superclass":"UIResponder","properties":[{"name":"userInteractionEnabled","type":"BOOL"},{"name":"tag","type":"NSInteger"},{"name":"center","type":"CGPoint"},{"name":"alpha","type":"CGFloat"},{"name":"opaque","type":"BOOL"},{"name":"hidden","type":"BOOL"},{"name":"frame","type":"CGRect"},{"name":"bounds","type":"CGRect"},{"name":"superview","type":"UIView"},{"name":"window","type":"UIWindow"},{"name":"subviews","type":"NSArray"},{"name":"se_fingerprintVersion","type":"NSArray","use_kvc":false},{"name":"se_varA","type":"NSString","use_kvc":false},{"name":"se_varB","type":"NSString","use_kvc":false},{"name":"se_varC","type":"NSString","use_kvc":false},{"name":"se_varSetD","type":"NSArray","use_kvc":false},{"name":"se_varE","type":"NSString","use_kvc":false}]},{"name":"UIImageView","superclass":"UIView","properties":[]},{"name":"UIControl","superclass":"UIView","properties":[{"name":"enabled","type":"BOOL"},{"name":"subviews","type":"NSArray","nofollow":true}]},{"name":"UITabBar","superclass":"UIView","properties":[],"delegateImplements":[{"selector":"tabBar:didSelectItem:"}]},{"name":"UIScrollView","superclass":"UIView","properties":[]},{"name":"UITextView","superclass":"UIScrollView","properties":[]},{"name":"UITableView","superclass":"UIScrollView","properties":[{"name":"subviews","type":"NSArray","nofollow":false}],"delegateImplements":[{"selector":"tableView:didSelectRowAtIndexPath:"}]},{"name":"UITableViewCell","superclass":"UIView","properties":[{"name":"subviews","type":"NSArray","nofollow":false},{"name":"cellIndexPath","type":"NSString","use_kvc":false}]},{"name":"UIButton","superclass":"UIControl","properties":[]},{"name":"UIWindow","superclass":"UIView","properties":[{"name":"keyWindow","type":"BOOL","readonly":true},{"name":"rootViewController","type":"UIViewController"},{"name":"screen","type":"UIScreen","readonly":true}]},{"name":"UIViewController","superclass":"UIResponder","properties":[{"name":"isViewLoaded","type":"BOOL","readonly":true},{"name":"view","type":"UIView","predicate":"self.isViewLoaded == YES"},{"name":"title","type":"NSString"},{"name":"parentViewController","type":"UIViewController"},{"name":"presentedViewController","type":"UIViewController"},{"name":"presentingViewController","type":"UIViewController"},{"name":"childViewControllers","type":"NSArray"},{"name":"_storyboardSegueTemplates","type":"NSArray"}]}]}}}

