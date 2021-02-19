
#ifndef TRANSFORM_H
#define TRANSFORM_H

#define ALPHA 1.0
#define BETA  0.0
#define BLUR  0.0

#include <opencv2/imgproc.hpp>


cv::Mat cmat33f_YuvTransform = (cv::Mat_<float>(3,3) << 1.0, 0.0, 0.0,
                                                        0.0, 1.0, 0.0,
                                                        0.0, 0.0, 1.0);

cv::Mat cmat33f_GroundFromMedmodel = (cv::Mat_<float>(3,3) << 0.00000000e+00, 0.00000000e+00, 1.00000000e+00,
                                                            -1.09890110e-03, 0.00000000e+00, 2.81318681e-01,
                                                            -1.84808520e-20, 9.00738606e-04,-4.28751576e-02); 

cv::Mat cmat33f_MedmodelIntrinsicMatrix = (cv::Mat_<float>(3,3) << 910.0, 0.0, 256.0,
                                                                   0.0, 910.0, 47.6,
                                                                   0.0,   0.0,  1.0);

static cv::Mat cmatf_OutputTensorPrev = cv::Mat(128,256, CV_32FC(6), cv::Scalar(0));
static cv::Mat cmatf_OutputTensor = cv::Mat(128,256, CV_32FC(6), cv::Scalar(0));

static std::vector<float> vec_Output(128*256*6);
static std::vector<float> vec_OutputPrev(128*256*6);



cv::Mat prepareWarpMatrix(cv::Mat cmat34f_ExtrinsicMatrix,
                            cv::Mat cmat33f_IntrinsicMatrix,
                            cv::Mat cmat33f_GroundFromMedmodel){

    cv::Mat cmat34f_CameraFromRoad = cmat33f_IntrinsicMatrix * 
                                     cmat34f_ExtrinsicMatrix;
    cv::Mat cmat33f_CameraFromGround = (cv::Mat_<float>(3,3));

    cmat34f_CameraFromRoad.col(0).copyTo(cmat33f_CameraFromGround.col(0));
    cmat34f_CameraFromRoad.col(1).copyTo(cmat33f_CameraFromGround.col(1));
    cmat34f_CameraFromRoad.col(2).copyTo(cmat33f_CameraFromGround.col(2));

    return cmat33f_CameraFromGround * cmat33f_GroundFromMedmodel;
};

 

// Calculates rotation matrix given euler angles.
cv::Mat prepareRotationMatrix(cv::Vec3f &theta)
{
    cv::Mat R_x = (cv::Mat_<float>(3,3) <<
               1,       0,              0,
               0,       cos(theta[0]),   -sin(theta[0]),
               0,       sin(theta[0]),   cos(theta[0])
               );
    cv::Mat R_y = (cv::Mat_<float>(3,3) <<
               cos(theta[1]),    0,      sin(theta[1]),
               0,               1,      0,
               -sin(theta[1]),   0,      cos(theta[1])
               );
    cv::Mat R_z = (cv::Mat_<float>(3,3) <<
               cos(theta[2]),    -sin(theta[2]),      0,
               sin(theta[2]),    cos(theta[2]),       0,
               0,               0,                  1);
    cv::Mat R = R_z * R_y * R_x;
    
    return R;

};


cv::Mat normalizeImagePoints(cv::Mat cmatf_ImagePoints,
                            cv::Mat cmat33f_IntrinsicMatrix){
    cv::Size csize_InputShape = cmatf_ImagePoints.size();
    cv::Mat cmatf_ImagePointsAppend;
    cv::Mat cvec_Ones = cv::Mat::ones(csize_InputShape.height, 1, CV_32F);

    cv::hconcat(cmatf_ImagePoints, cvec_Ones, cmatf_ImagePointsAppend);
    cv::Mat cmatf_ImagePointsNormalized = cmatf_ImagePointsAppend * cmat33f_IntrinsicMatrix.inv().t();

    
    return cmatf_ImagePointsNormalized(cv::Rect(cv::Point(0,0),csize_InputShape));
};

cv::Mat preparePerspectiveMatrix(cv::Mat cmat33f_IntrinsicMatrix,
                                 cv::Mat cmat34f_Affine) {
    float f_Cy = cmat33f_IntrinsicMatrix.at<float>(1,2);
    float f_h = 1.22;
    int f_width = cmat33f_IntrinsicMatrix.at<float>(0,2) * 2;
    int f_height = cmat33f_IntrinsicMatrix.at<float>(1,2) * 2;
    cv::Mat cmat42f_QuadrAngle = (cv::Mat_<float>(4,2) << 0.0, f_Cy + 20.0,
                                                        f_width-1.0, f_Cy + 20.0,
                                                        0.0, f_height-1,
                                                        f_width-1.0, f_height-1.0);
    cv::Mat cvec_Ones = cv::Mat::ones(4, 1, CV_32F);
    cv::Mat cmat43f_QuadrAngleNormalized;
    cv::hconcat(normalizeImagePoints(cmat42f_QuadrAngle, cmat33f_IntrinsicMatrix),cvec_Ones, cmat43f_QuadrAngleNormalized);
    cv::Mat cmat44f_QuadrAngleWorld = (cv::Mat_<float>(4,4));
    
    cmat44f_QuadrAngleWorld.col(0) = (f_h*cmat43f_QuadrAngleNormalized.col(0)/cmat43f_QuadrAngleNormalized.col(1));
    cmat44f_QuadrAngleWorld.col(1) = (f_h*cvec_Ones);
    cmat44f_QuadrAngleWorld.col(2) = (f_h/cmat43f_QuadrAngleNormalized.col(1));
    cmat44f_QuadrAngleWorld.col(3) = cv::Mat::ones(4, 1, CV_32F);
    
    cv::Mat cmat34f_KE = cmat33f_MedmodelIntrinsicMatrix * cmat34f_Affine;
    // cv::hconcat(cmat33f_MedmodelIntrinsicMatrix, cv::Mat::zeros(3,1,CV_32F), cmat34f_KE);
    cv::Mat cmat43f_QuadrAngleFull = cmat44f_QuadrAngleWorld * cmat34f_KE.t();
    
    cv::Mat cmat42f_WarpedQuadrAngle = (cv::Mat_<float>(4,2));
    cmat42f_WarpedQuadrAngle.col(0) = cmat43f_QuadrAngleFull.col(0)/cmat43f_QuadrAngleFull.col(2);
    cmat42f_WarpedQuadrAngle.col(1) = cmat43f_QuadrAngleFull.col(1)/cmat43f_QuadrAngleFull.col(2);
    
    return cv::getPerspectiveTransform(cmat42f_QuadrAngle, cmat42f_WarpedQuadrAngle);
};

cv::Mat transformImage(cv::Mat cmati_InputImage,
                        cv::Vec3f cvec3f_AugmentTrans,
                        cv::Vec3f cvec3f_AugmentEulers,
                        cv::Mat cmat33f_FromIntrinsics,
                        cv::Size csize_OutputShape){
    
    cv::Size csize_InputShape = cmati_InputImage.size();
    cv::Mat cmat33f_RotationMatrix = prepareRotationMatrix(cvec3f_AugmentEulers);
//    std::cout << cmat33f_RotationMatrix << std::endl;
    cv::Mat cmat34f_AffineMatrix;
    cv::hconcat(cmat33f_RotationMatrix.t(), -cvec3f_AugmentTrans, cmat34f_AffineMatrix);
    cv::Mat cmat33f_PerspectiveMatrix = preparePerspectiveMatrix(cmat33f_FromIntrinsics, cmat34f_AffineMatrix);
//    std::cout << cmat33f_PerspectiveMatrix << std::endl;
    cv::Mat cmati_RGBOutputImage;
    cv::warpPerspective(cmati_InputImage, cmati_RGBOutputImage, cmat33f_PerspectiveMatrix,
                        csize_OutputShape);
    cv::Mat cmati_OutputImage;
    cv::cvtColor(cmati_RGBOutputImage, cmati_OutputImage,cv::COLOR_RGB2YUV_I420);
//    std::cout << cmati_OutputImage.size() << std::endl;
    
    int H = csize_OutputShape.height;
    int W = csize_OutputShape.width;
    int Hd = H/2;
    int Wd = W/2;
    
    cv::Mat cmati_Y1 = cv::Mat::zeros(Hd, Wd, CV_8UC1);
    cv::Mat cmati_Y2 = cv::Mat::zeros(Hd, Wd, CV_8UC1);
    cv::Mat cmati_Y3 = cv::Mat::zeros(Hd, Wd, CV_8UC1);
    cv::Mat cmati_Y4 = cv::Mat::zeros(Hd, Wd, CV_8UC1);
    cv::Mat cmati_U = cv::Mat::zeros(Hd, Wd, CV_8UC1);
    cv::Mat cmati_V = cv::Mat::zeros(Hd, Wd, CV_8UC1);
//    std::cout << cmati_OutputImage << std::endl;
    
    for(int r = 0; r < Hd; r++){
        if(r < Hd /2){
            cmati_OutputImage.row(H + r).colRange(0, Wd).copyTo(cmati_U.row(2*r));
            cmati_OutputImage.row(H + r).colRange(Wd, W).copyTo(cmati_U.row(2*r+1));
            cmati_OutputImage.row(H + Hd / 2 + r).colRange(0, Wd).copyTo(cmati_V.row(2*r));
            cmati_OutputImage.row(H + Hd / 2 + r).colRange(Wd, W).copyTo(cmati_V.row(2*r+1));
        };
        for(int c = 0; c < Wd; c++){
            cmati_Y1.at<uchar>(r,c) = cmati_OutputImage.at<uchar>(2*r, 2*c);
            cmati_Y2.at<uchar>(r,c) = cmati_OutputImage.at<uchar>(2*r+1, 2*c);
            cmati_Y3.at<uchar>(r,c) = cmati_OutputImage.at<uchar>(2*r, 2*c+1);
            cmati_Y4.at<uchar>(r,c) = cmati_OutputImage.at<uchar>(2*r+1, 2*c+1);
            
            vec_Output[r*Wd + c] = (float)cmati_Y1.at<uchar>(r,c)/128.0f - 1.0f;
            vec_Output[r*Wd + c + Wd*Hd] = (float)cmati_Y2.at<uchar>(r,c)/128.0f - 1.0f;
            vec_Output[r*Wd + c + 2*Wd*Hd] = (float)cmati_Y3.at<uchar>(r,c)/128.0f - 1.0f;
            vec_Output[r*Wd + c + 3*Wd*Hd] = (float)cmati_Y4.at<uchar>(r,c)/128.0f - 1.0f;
            vec_Output[r*Wd + c + 4*Wd*Hd] = (float)cmati_U.at<uchar>(r,c)/128.0f - 1.0f;
            vec_Output[r*Wd + c + 5*Wd*Hd] = (float)cmati_V.at<uchar>(r,c)/128.0f - 1.0f;
//
//            vec_Output[r*Wd + c] = (float)cmati_OutputImage.at<uchar>(2*r, 2*c)/128.0f -1.0f;
//            vec_Output[r*Wd + c + Wd*Hd] = (float)cmati_OutputImage.at<uchar>(2*r+1, 2*c)/128.0f - 1.0f;
//            Vec_Output[r*Wd + c + 2*(Wd*Hd)] = (float)cmati_OutputImage.at<uchar>(2*r, 2*c+1)/128.0f - 1.0f;
//            Vec_Output[r*Wd + c + 3*(Wd*Hd)] = cmati_OutputImage.at<uchar>(2*r+1, 2*c+1)/128.0f -1.0f;
//            Vec_Output[r*Wd + c + 4*(Wd*Hd)] =
        };
        
    }
    
//    for(int r = 0; r < Hd; r ++){
//        for(int c = 0; c < Wd; c++){
//            vec_Output[r*Wd + c] = (float)cmati_Y1.at<uchar>(r,c)/128.0f - 1.0f;
//            vec_Output[r*Wd + c + Wd*Hd] = (float)cmati_Y2.at<uchar>(r,c)/128.0f - 1.0f;
//            vec_Output[r*Wd + c + 2*Wd*Hd] = (float)cmati_Y3.at<uchar>(r,c)/128.0f - 1.0f;
//            vec_Output[r*Wd + c + 3*Wd*Hd] = (float)cmati_Y4.at<uchar>(r,c)/128.0f - 1.0f;
//            vec_Output[r*Wd + c + 4*Wd*Hd] = (float)cmati_U.at<uchar>(r,c)/128.0f - 1.0f;
//            vec_Output[r*Wd + c + 5*Wd*Hd] = (float)cmati_V.at<uchar>(r,c)/128.0f - 1.0f;
//        }
//    }
//    cv::flip(cmati_Y1, cmati_Y1, 1);
//    cv::flip(cmati_Y2, cmati_Y2, 1);
//    cv::flip(cmati_Y3, cmati_Y3, 1);
//    cv::flip(cmati_Y4, cmati_Y4, 1);
//    cv::flip(cmati_U, cmati_U, 1);
//    cv::flip(cmati_V, cmati_V, 1);
    
//    std::vector<cv::Mat> img_rearranged = {cmati_Y1.t(), cmati_Y2.t(), cmati_Y3.t(), cmati_Y4.t(), cmati_U.t(), cmati_V.t()};
//    std::vector<cv::Mat> img_rearranged = {cmati_Y1, cmati_Y2, cmati_Y3, cmati_Y4, cmati_U, cmati_V};
//    img_rearranged = img_rearranged/128.0f - 1.0f;

    
//    cv::Mat cmati_OutputTensor;
//    merge(img_rearranged, cmati_OutputTensor);
//    cv::Mat temp;
//    cmati_OutputTensor.convertTo(cmatf_OutputTensor,CV_32FC(6));
//    cv::Mat cmati_to_cmatf_OutputTensor = cmatf_OutputTensor/128.0 - 1.0;
//    cmati_to_cmatf_OutputTensor.copyTo(cmatf_OutputTensor);
//    std::cout << cmatf_OutputTensor.size() << std::endl;
//    std::cout << cmatf_OutputTensorPrev.size() << std::endl;
//    cv::Mat Channel[6];
//    cv::split(cmatf_OutputTensor,Channel);
    
    
    
    
//    std::cout << Channel[0] << std::endl;
    return cmati_RGBOutputImage;
};



#endif
