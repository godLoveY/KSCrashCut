{"type":"snapshot_request","payload":{"config":{"enums":[{"name":"UILayoutConstraintAxis","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Horizontal"},{"value":1,"display_name":"Vertical"}]},{"name":"UIControlState","flag_set":true,"base_type":"NSUInteger","values":[{"value":0,"display_name":"Normal"},{"value":1,"display_name":"Highlighted"},{"value":2,"display_name":"Disabled"},{"value":4,"display_name":"Selected"}]},{"name":"UIControlContentVerticalAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Center"},{"value":1,"display_name":"Top"},{"value":2,"display_name":"Bottom"},{"value":3,"display_name":"Fill"}]},{"name":"UIControlContentHorizontalAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Center"},{"value":1,"display_name":"Left"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Fill"}]},{"name":"NSLayoutRelation","base_type":"NSInteger","flag_set":false,"values":[{"value":-1,"display_name":"LessThanOrEqual"},{"value":0,"display_name":"Equal"},{"value":1,"display_name":"GreaterThanOrEqual"}]},{"name":"NSLayoutAttribute","base_type":"NSInteger","flag_set":false,"values":[{"value":1,"display_name":"Left"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Top"},{"value":4,"display_name":"Bottom"},{"value":5,"display_name":"Leading"},{"value":6,"display_name":"Trailing"},{"value":7,"display_name":"Width"},{"value":8,"display_name":"Height"},{"value":9,"display_name":"CenterX"},{"value":10,"display_name":"CenterY"},{"value":11,"display_name":"Baseline"},{"value":0,"display_name":"NotAnAttribute"}]},{"name":"UIControlEvents","base_type":"NSUInteger","flag_set":true,"values":[{"value":1,"display_name":"TouchDown"},{"value":2,"display_name":"TouchDownRepeat"},{"value":4,"display_name":"TouchDragInside"},{"value":8,"display_name":"TouchDragOutside"},{"value":16,"display_name":"TouchDragEnter"},{"value":32,"display_name":"TouchDragExit"},{"value":64,"display_name":"TouchUpInside"},{"value":128,"display_name":"TouchUpOutside"},{"value":256,"display_name":"TouchCancel"},{"value":4096,"display_name":"ValueChanged"},{"value":65536,"display_name":"EditingDidBegin"},{"value":131072,"display_name":"EditingChanged"},{"value":262144,"display_name":"EditingDidEnd"},{"value":524288,"display_name":"EditingDidEndOnExit"},{"value":4095,"display_name":"AllTouchEvents"},{"value":983040,"display_name":"AllEditingEvents"},{"value":251658240,"display_name":"ApplicationReserved"},{"value":4026531840,"display_name":"SystemReserved"},{"value":4294967295,"display_name":"AllEvents"}]},{"name":"UIBarButtonItemStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Plain"},{"value":1,"display_name":"Bordered"},{"value":2,"display_name":"Done"}]},{"name":"NSTextAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Left"},{"value":1,"display_name":"Center"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Justified"},{"value":4,"display_name":"Natural"}]},{"name":"NSLineBreakMode","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"WordWrapping"},{"value":1,"display_name":"CharWrapping"},{"value":2,"display_name":"Clipping"},{"value":3,"display_name":"TruncatingHead"},{"value":4,"display_name":"TruncatingTail"},{"value":5,"display_name":"TruncatingMiddle"}]},{"name":"UIBaselineAdjustment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"AlignBaselines"},{"value":1,"display_name":"AlignCenters"},{"value":2,"display_name":"None"}]},{"name":"UIScrollViewIndicatorStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Default"},{"value":1,"display_name":"Black"},{"value":2,"display_name":"White"}]},{"name":"UITableViewStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"UITableViewStylePlain"},{"value":1,"display_name":"UITableViewStyleGrouped"}]}],"classes":[{"name":"NSObject","superclass":null,"properties":[]},{"name":"UIResponder","superclass":"NSObject","properties":[]},{"name":"UIScreen","superclass":"NSObject","properties":[{"name":"bounds","type":"CGRect","readonly":true},{"name":"applicationFrame","type":"CGRect","readonly":true},{"name":"scale","type":"CGFloat","readonly":true}]},{"name":"UIStoryboardSegueTemplate","superclass":"NSObject","properties":[{"name":"identifier","type":"NSString","readonly":true},{"name":"viewController","type":"UIViewController"},{"name":"performOnViewLoad","type":"BOOL"}]},{"name":"UIView","superclass":"UIResponder","properties":[{"name":"userInteractionEnabled","type":"BOOL"},{"name":"tag","type":"NSInteger"},{"name":"center","type":"CGPoint"},{"name":"alpha","type":"CGFloat"},{"name":"opaque","type":"BOOL"},{"name":"hidden","type":"BOOL"},{"name":"frame","type":"CGRect"},{"name":"bounds","type":"CGRect"},{"name":"superview","type":"UIView"},{"name":"window","type":"UIWindow"},{"name":"subviews","type":"NSArray"},{"name":"se_fingerprintVersion","type":"NSArray","use_kvc":false},{"name":"se_varA","type":"NSString","use_kvc":false},{"name":"se_varB","type":"NSString","use_kvc":false},{"name":"se_varC","type":"NSString","use_kvc":false},{"name":"se_varSetD","type":"NSArray","use_kvc":false},{"name":"se_varE","type":"NSString","use_kvc":false},{"name":"se_clickableState","type":"NSDictionary","use_kvc":false}]},{"name":"UIImageView","superclass":"UIView","properties":[]},{"name":"UIControl","superclass":"UIView","properties":[{"name":"enabled","type":"BOOL"},{"name":"subviews","type":"NSArray","nofollow":true}]},{"name":"UITabBar","superclass":"UIView","properties":[],"delegateImplements":[{"selector":"tabBar:didSelectItem:"}]},{"name":"UIScrollView","superclass":"UIView","properties":[]},{"name":"UICollectionReusableView","superclass":"UIView","properties":[]},{"name":"UITextView","superclass":"UIScrollView","properties":[]},{"name":"UICollectionView","superclass":"UIScrollView","properties":[{"name":"subviews","type":"NSArray","nofollow":false}],"delegateImplements":[{"selector":"collectionView:didSelectItemAtIndexPath:"}]},{"name":"UICollectionViewCell","superclass":"UICollectionReusableView","properties":[{"name":"subviews","type":"NSArray","nofollow":false},{"name":"cellIndexPath","type":"NSString","use_kvc":false}]},{"name":"UITableView","superclass":"UIScrollView","properties":[{"name":"subviews","type":"NSArray","nofollow":false}],"delegateImplements":[{"selector":"tableView:didSelectRowAtIndexPath:"}]},{"name":"UITableViewCell","superclass":"UIView","properties":[{"name":"subviews","type":"NSArray","nofollow":false},{"name":"cellIndexPath","type":"NSString","use_kvc":false}]},{"name":"UIButton","superclass":"UIControl","properties":[]},{"name":"UIWindow","superclass":"UIView","properties":[{"name":"keyWindow","type":"BOOL","readonly":true},{"name":"rootViewController","type":"UIViewController"},{"name":"screen","type":"UIScreen","readonly":true}]},{"name":"UILabel","superclass":"UIView","properties":[{"name":"text","type":"NSString"},{"name":"attributedText","type":"NSAttributedString"}]},{"name":"UIViewController","superclass":"UIResponder","properties":[{"name":"isViewLoaded","type":"BOOL","readonly":true},{"name":"view","type":"UIView","predicate":"self.isViewLoaded == YES"},{"name":"title","type":"NSString"},{"name":"parentViewController","type":"UIViewController"},{"name":"presentedViewController","type":"UIViewController"},{"name":"presentingViewController","type":"UIViewController"},{"name":"childViewControllers","type":"NSArray"},{"name":"_storyboardSegueTemplates","type":"NSArray"}]}]}}}




{"type":"snapshot_request","payload":{"config":{"enums":[{"name":"UILayoutConstraintAxis","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Horizontal"},{"value":1,"display_name":"Vertical"}]},{"name":"UIControlState","flag_set":true,"base_type":"NSUInteger","values":[{"value":0,"display_name":"Normal"},{"value":1,"display_name":"Highlighted"},{"value":2,"display_name":"Disabled"},{"value":4,"display_name":"Selected"}]},{"name":"UIControlContentVerticalAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Center"},{"value":1,"display_name":"Top"},{"value":2,"display_name":"Bottom"},{"value":3,"display_name":"Fill"}]},{"name":"UIControlContentHorizontalAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Center"},{"value":1,"display_name":"Left"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Fill"}]},{"name":"NSLayoutRelation","base_type":"NSInteger","flag_set":false,"values":[{"value":-1,"display_name":"LessThanOrEqual"},{"value":0,"display_name":"Equal"},{"value":1,"display_name":"GreaterThanOrEqual"}]},{"name":"NSLayoutAttribute","base_type":"NSInteger","flag_set":false,"values":[{"value":1,"display_name":"Left"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Top"},{"value":4,"display_name":"Bottom"},{"value":5,"display_name":"Leading"},{"value":6,"display_name":"Trailing"},{"value":7,"display_name":"Width"},{"value":8,"display_name":"Height"},{"value":9,"display_name":"CenterX"},{"value":10,"display_name":"CenterY"},{"value":11,"display_name":"Baseline"},{"value":0,"display_name":"NotAnAttribute"}]},{"name":"UIControlEvents","base_type":"NSUInteger","flag_set":true,"values":[{"value":1,"display_name":"TouchDown"},{"value":2,"display_name":"TouchDownRepeat"},{"value":4,"display_name":"TouchDragInside"},{"value":8,"display_name":"TouchDragOutside"},{"value":16,"display_name":"TouchDragEnter"},{"value":32,"display_name":"TouchDragExit"},{"value":64,"display_name":"TouchUpInside"},{"value":128,"display_name":"TouchUpOutside"},{"value":256,"display_name":"TouchCancel"},{"value":4096,"display_name":"ValueChanged"},{"value":65536,"display_name":"EditingDidBegin"},{"value":131072,"display_name":"EditingChanged"},{"value":262144,"display_name":"EditingDidEnd"},{"value":524288,"display_name":"EditingDidEndOnExit"},{"value":4095,"display_name":"AllTouchEvents"},{"value":983040,"display_name":"AllEditingEvents"},{"value":251658240,"display_name":"ApplicationReserved"},{"value":4026531840,"display_name":"SystemReserved"},{"value":4294967295,"display_name":"AllEvents"}]},{"name":"UIBarButtonItemStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Plain"},{"value":1,"display_name":"Bordered"},{"value":2,"display_name":"Done"}]},{"name":"NSTextAlignment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Left"},{"value":1,"display_name":"Center"},{"value":2,"display_name":"Right"},{"value":3,"display_name":"Justified"},{"value":4,"display_name":"Natural"}]},{"name":"NSLineBreakMode","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"WordWrapping"},{"value":1,"display_name":"CharWrapping"},{"value":2,"display_name":"Clipping"},{"value":3,"display_name":"TruncatingHead"},{"value":4,"display_name":"TruncatingTail"},{"value":5,"display_name":"TruncatingMiddle"}]},{"name":"UIBaselineAdjustment","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"AlignBaselines"},{"value":1,"display_name":"AlignCenters"},{"value":2,"display_name":"None"}]},{"name":"UIScrollViewIndicatorStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"Default"},{"value":1,"display_name":"Black"},{"value":2,"display_name":"White"}]},{"name":"UITableViewStyle","base_type":"NSInteger","flag_set":false,"values":[{"value":0,"display_name":"UITableViewStylePlain"},{"value":1,"display_name":"UITableViewStyleGrouped"}]}],"classes":[{"name":"NSObject","superclass":null,"properties":[]},{"name":"UIResponder","superclass":"NSObject","properties":[]},{"name":"UIScreen","superclass":"NSObject","properties":[{"name":"bounds","type":"CGRect","readonly":true},{"name":"applicationFrame","type":"CGRect","readonly":true},{"name":"scale","type":"CGFloat","readonly":true}]},{"name":"UIStoryboardSegueTemplate","superclass":"NSObject","properties":[{"name":"identifier","type":"NSString","readonly":true},{"name":"viewController","type":"UIViewController"},{"name":"performOnViewLoad","type":"BOOL"}]},{"name":"UIView","superclass":"UIResponder","properties":[{"name":"userInteractionEnabled","type":"BOOL"},{"name":"tag","type":"NSInteger"},{"name":"center","type":"CGPoint"},{"name":"alpha","type":"CGFloat"},{"name":"opaque","type":"BOOL"},{"name":"hidden","type":"BOOL"},{"name":"frame","type":"CGRect"},{"name":"bounds","type":"CGRect"},{"name":"superview","type":"UIView"},{"name":"window","type":"UIWindow"},{"name":"subviews","type":"NSArray"},{"name":"se_fingerprintVersion","type":"NSArray","use_kvc":false},{"name":"se_varA","type":"NSString","use_kvc":false},{"name":"se_varB","type":"NSString","use_kvc":false},{"name":"se_varC","type":"NSString","use_kvc":false},{"name":"se_varSetD","type":"NSArray","use_kvc":false},{"name":"se_varE","type":"NSString","use_kvc":false}]},{"name":"UIImageView","superclass":"UIView","properties":[]},{"name":"UIControl","superclass":"UIView","properties":[{"name":"enabled","type":"BOOL"},{"name":"subviews","type":"NSArray","nofollow":true}]},{"name":"UITabBar","superclass":"UIView","properties":[],"delegateImplements":[{"selector":"tabBar:didSelectItem:"}]},{"name":"UIScrollView","superclass":"UIView","properties":[]},{"name":"UITextView","superclass":"UIScrollView","properties":[]},{"name":"UITableView","superclass":"UIScrollView","properties":[{"name":"subviews","type":"NSArray","nofollow":false}],"delegateImplements":[{"selector":"tableView:didSelectRowAtIndexPath:"}]},{"name":"UITableViewCell","superclass":"UIView","properties":[{"name":"subviews","type":"NSArray","nofollow":false},{"name":"cellIndexPath","type":"NSString","use_kvc":false}]},{"name":"UIButton","superclass":"UIControl","properties":[]},{"name":"UIWindow","superclass":"UIView","properties":[{"name":"keyWindow","type":"BOOL","readonly":true},{"name":"rootViewController","type":"UIViewController"},{"name":"screen","type":"UIScreen","readonly":true}]},{"name":"UIViewController","superclass":"UIResponder","properties":[{"name":"isViewLoaded","type":"BOOL","readonly":true},{"name":"view","type":"UIView","predicate":"self.isViewLoaded == YES"},{"name":"title","type":"NSString"},{"name":"parentViewController","type":"UIViewController"},{"name":"presentedViewController","type":"UIViewController"},{"name":"presentingViewController","type":"UIViewController"},{"name":"childViewControllers","type":"NSArray"},{"name":"_storyboardSegueTemplates","type":"NSArray"}]}]}}}

