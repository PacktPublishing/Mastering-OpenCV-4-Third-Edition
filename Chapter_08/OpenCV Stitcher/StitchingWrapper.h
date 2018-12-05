//
//  StitchingWrapper.h
//  OpenCV Stitcher
//
//  Created by roy_shilkrot on 10/25/18.
//  Copyright © 2018 roy_shilkrot. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface StitchingWrapper : NSObject

+ (NSString *)openCVVersionString;

+ (UIImage* _Nullable)stitch:(NSMutableArray*) images;

@end

NS_ASSUME_NONNULL_END
