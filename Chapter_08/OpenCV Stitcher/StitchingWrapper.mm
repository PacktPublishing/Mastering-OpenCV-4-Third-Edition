//
//  StitchingWrapper.m
//  OpenCV Stitcher
//
//  Created by roy_shilkrot on 10/25/18.
//  Copyright © 2018 roy_shilkrot. All rights reserved.
//

#import "StitchingWrapper.h"

@implementation StitchingWrapper

+ (NSString *)openCVVersionString {
    return [NSString stringWithFormat:@"OpenCV Version %s",  CV_VERSION];
}

+ (UIImage* _Nullable)stitch:(NSMutableArray*) images {
    using namespace cv;
    using namespace cv::detail;
    
    NSLog(@"got array of #images %lu",[images count]);
    
    std::vector<cv::Mat> imgs;
    
    for (UIImage* img in images) {
        cv::Mat mat;
        UIImageToMat(img, mat);
        if ([img imageOrientation] == UIImageOrientationRight) {
            cv::rotate(mat, mat, cv::ROTATE_90_CLOCKWISE);
        }
        NSLog(@"Image size %d x %d, channels %d", mat.cols, mat.rows, mat.channels());
        cvtColor(mat, mat, cv::COLOR_BGRA2BGR);
        imgs.push_back(mat);
    }

    //![stitching]
    Mat pano;
    Stitcher::Mode mode = Stitcher::PANORAMA;
    Ptr<Stitcher> stitcher = Stitcher::create(mode, false);
    NSLog(@"Created stitcher");
    try {
        Stitcher::Status status = stitcher->stitch(imgs, pano);
        NSLog(@"After stitching");

        if (status != Stitcher::OK)
        {
            NSLog(@"Can't stitch images, error code = %d", status);
            return NULL;
        }
    } catch (const cv::Exception& e) {
        NSLog(@"Error %s", e.what());
        return NULL;
    }
    
    NSLog(@"stitching completed successfully");
    return MatToUIImage(pano);
}

@end
