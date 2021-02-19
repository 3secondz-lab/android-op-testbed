#import <opencv2/core.hpp>
//#import <opencv2/imgcodecs/ios.h>
#import "transform.h"
#import <iostream>

cv::Mat cmat34f_ExtrinsicMatrix;
cv::Mat cmat33f_WarpMatrix;

static cv::Mat cmatf_Output = cv::Mat(128,256, CV_32FC(12), cv::Scalar(0));
static std::vector<float> concat(128*256*12);

//@implementation opencv
//
//+ (UIImage *) preprocessSupercomboImage:(UIImage *) image Intrinsic:(simd_float3x3) intrinsic {
//
//    cv::Mat inputImage;
//    UIImageToMat(image, inputImage, true);
//    cv::Mat cmat33f_intrinsic = (cv::Mat_<float>(3,3) << intrinsic.columns[0][0], intrinsic.columns[1][0], intrinsic.columns[2][0], intrinsic.columns[0][1], intrinsic.columns[1][1], intrinsic.columns[2][1], intrinsic.columns[0][2], intrinsic.columns[1][2], intrinsic.columns[2][2]);
////    std::cout << cmat33f_intrinsic << std::endl;
//    cv::Vec3f zerovec(0,0,0);
//    cv::Size Outsize(512,256);
//
//    cmat33f_WarpMatrix = (cv::Mat_<float>(3,3)<<
//                          910.0, 0.0, 582.0,
//                          0.0, 910.0, 437.0,
//                          0.0,   0.0,   1.0);
//
//    cv::Mat eon_intrinsic  = (cv::Mat_<float>(3,3)<<
//        910.0, 0.0, 582.0,
//        0.0, 910.0, 437.0,
//        0.0,   0.0,   1.0);
//
//    return MatToUIImage(transformImage(inputImage, zerovec, zerovec, cmat33f_intrinsic, Outsize));
////    return MatToUIImage(transformImage(inputImage, zerovec, zerovec, eon_intrinsic, Outsize));
//}
//
//+ (float *) testMultiarrayPointer {
////    std::vector<cv::Mat> outvec;
////    outvec.push_back(cmatf_OutputTensorPrev);
////    outvec.push_back(cmatf_OutputTensor);
////
//////    cv::Mat testMat = (cv::Mat_<float>(2,4) << 1,2,3,4,5,6,7,8);
//////    std::vector data = (float*) testMat.data;
//////    std::cout << data[2] << std::endl;
////
////    merge(outvec, cmatf_Output);
////    cmatf_OutputTensor.copyTo(cmatf_OutputTensorPrev);
//    std::copy(vec_OutputPrev.begin(), vec_OutputPrev.end(), concat.begin());
//    std::copy(vec_Output.begin(), vec_Output.end(), concat.begin()+6*128*256);
//    vec_OutputPrev.assign(vec_Output.begin(),vec_Output.end());
////    std::cout << (float)cmatf_Output.data[0] << std::endl;
////    return (float*)cmatf_Output.data;
//    return concat.data();
//}
//
//@end
