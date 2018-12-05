#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <android/log.h>

using namespace std;
using namespace cv;

#define LOGTAG "NativeAruco"

Ptr<aruco::Dictionary> dict = aruco::Dictionary::get(aruco::DICT_ARUCO_ORIGINAL);
Ptr<aruco::GridBoard> board = aruco::GridBoard::create(10,7,14.0f,9.2f, dict);
Mat_<float> cameraMatrix, distCoeffs;
Matx33d R;
vector< vector< vector< Point2f > > > allCorners;
vector< vector< int > > allIds;
vector< Mat > allImgs;
Size imgSize;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_packt_masteringopencv4_opencvarucoar_MainActivity_findMarker(JNIEnv *env, jclass type,
                                                                      jbyteArray grayImageAddr,
                                                                      jint h, jint w,
                                                                      jfloatArray poseOutput) {
    jbyte *data = env->GetByteArrayElements(grayImageAddr, NULL);
    Mat grayImage(h, w, CV_8UC1, data);

    vector< int > ids;
    vector< vector< Point2f > > corners, rejected;

    // detect markers and estimate pose
    try {
        aruco::detectMarkers(grayImage, dict, corners, ids, aruco::DetectorParameters::create(), rejected);
    } catch (cv::Exception& e) {
        __android_log_print(ANDROID_LOG_WARN, LOGTAG, "Cannot detect aruco markers: %s", e.what());
    }

    // image buffer no longer needed
    env->ReleaseByteArrayElements(grayImageAddr, data, 0);

    if (corners.size() == 0) {
        return false;
    }

    if (cameraMatrix.empty()) { // Initialize with dummy camera mat
        cameraMatrix = Mat_<float>(Matx33f(800,0,(1280-1)/2.0f, 0,800,(720-1)/2.0, 0,0,1));
    }

    const unsigned int numMarkers = ids.size();
    vector< Vec3d > rvecs, tvecs;
    if (numMarkers > 0) {
        try {
            aruco::estimatePoseSingleMarkers(corners,
                                             52.85f,
                                             cameraMatrix,
                                             distCoeffs,
                                             rvecs,
                                             tvecs);

            Rodrigues(rvecs[0], R);

            Matx44f P = Matx44f::eye();
            for (int i : {0,1,2}) {
                for (int j : {0,1,2}) {
                    P(i,j) = (float)(R(i,j));
                }
                P(i,3) = static_cast<float>(tvecs[0][i]);
            }
            Matx44f cvtogl(1,0,0,0,
                            0,-1,0,0,
                            0,0,-1,0,
                            0,0,0,1);

            env->SetFloatArrayRegion(poseOutput, 0, 16, (cvtogl * P).val);

            return true;

        } catch (cv::Exception &e) {
            __android_log_print(ANDROID_LOG_WARN, LOGTAG,
                                "Cannot estimate aruco markers pose: %s", e.what());
        }
    }

    return false;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_packt_masteringopencv4_opencvarucoar_MainActivity_rotate180(JNIEnv *env, jclass type,
                                                                      jbyteArray data_, jint h,
                                                                      jint w, jint cvtype) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    Mat img(h, w, cvtype, data);
    rotate(img, img, cv::ROTATE_180);
    env->ReleaseByteArrayElements(data_, data, 0);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_packt_masteringopencv4_opencvarucoar_CalibrationActivity_addCalibration8UImage(JNIEnv *env,
                                                                                        jclass type,
                                                                                        jbyteArray data_,
                                                                                        jint w,
                                                                                        jint h) {
    __android_log_write(ANDROID_LOG_DEBUG, LOGTAG, "native addCalibration8UImage");
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    Mat grayImage(h, w, CV_8UC1, data);

    vector< int > ids;
    vector< vector< Point2f > > corners, rejected;

    // detect markers
    aruco::detectMarkers(grayImage, dict, corners, ids, aruco::DetectorParameters::create(), rejected);
    __android_log_print(ANDROID_LOG_DEBUG, LOGTAG, "found %d markers", ids.size());

    aruco::refineDetectedMarkers(grayImage, board, corners, ids, rejected);
    __android_log_print(ANDROID_LOG_DEBUG, LOGTAG, "found %d markers after refinement", ids.size());

    {   // Save images to files for debug and visualization
        char buf[256];
        sprintf(buf, "/sdcard/calib_cap_orig%03d.png", allIds.size());
        imwrite(buf, grayImage);

        Mat out;
        cvtColor(grayImage, out, CV_GRAY2BGR);
        aruco::drawDetectedMarkers(out, corners, ids);
        sprintf(buf, "/sdcard/calib_cap_markers%03d.png", allIds.size());
        imwrite(buf, out);
    }

    allCorners.push_back(corners);
    allIds.push_back(ids);
    allImgs.push_back(grayImage.clone());
    imgSize = grayImage.size();

    __android_log_print(ANDROID_LOG_DEBUG, LOGTAG, "total %d calibration images", allImgs.size());

    env->ReleaseByteArrayElements(data_, data, 0);

    return allImgs.size();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_packt_masteringopencv4_opencvarucoar_CalibrationActivity_doCalibration(JNIEnv *env,
                                                                                jclass type) {

    vector< Mat > rvecs, tvecs;

    cameraMatrix = Mat::eye(3, 3, CV_64F);
    cameraMatrix.at< double >(0, 0) = 1.0;


    // prepare data for calibration
    vector< vector< Point2f > > allCornersConcatenated;
    vector< int > allIdsConcatenated;
    vector< int > markerCounterPerFrame;
    markerCounterPerFrame.reserve(allCorners.size());
    for(unsigned int i = 0; i < allCorners.size(); i++) {
        markerCounterPerFrame.push_back((int)allCorners[i].size());
        for(unsigned int j = 0; j < allCorners[i].size(); j++) {
            allCornersConcatenated.push_back(allCorners[i][j]);
            allIdsConcatenated.push_back(allIds[i][j]);
        }
    }

    // calibrate camera using aruco markers
    double arucoRepErr;
    arucoRepErr = aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated,
                                              markerCounterPerFrame, board, imgSize, cameraMatrix,
                                              distCoeffs, rvecs, tvecs, CALIB_FIX_ASPECT_RATIO);

    __android_log_print(ANDROID_LOG_DEBUG, LOGTAG, "calibration reprojection err: %.3f", arucoRepErr);
    stringstream ss;
    ss << cameraMatrix << endl << distCoeffs;
    __android_log_print(ANDROID_LOG_DEBUG, LOGTAG, "calibration result: %s", ss.str().c_str());

    cv::FileStorage fs("/sdcard/calibration.yml", FileStorage::WRITE);
    fs.write("cameraMatrix", cameraMatrix);
    fs.write("distCoeffs", distCoeffs);
    fs.release();
}