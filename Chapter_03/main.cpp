//
// Created by roy_shilkrot on 8/27/18.
//

#include "opencv2/face.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <fstream>
#include <istream>
#include <vector>
#include <string>
#include <boost/format.hpp>

using namespace std;
using namespace cv;
using namespace cv::face;
using $ = boost::format;

void faceDetector(const Mat& image,
                  std::vector<Rect> &faces,
                  CascadeClassifier &face_cascade) {
    Mat gray;

    // The cascade classifier works best on grayscale images
    if (image.channels() > 1) {
        cvtColor(image, gray, COLOR_BGR2GRAY);
    } else {
        gray = image.clone();
    }

    // Histogram equalization generally aids in face detection
    equalizeHist(gray, gray);

    faces.clear();

    // Run the cascade classifier
    face_cascade.detectMultiScale(gray, faces, 1.4, 3, CASCADE_SCALE_IMAGE + CASCADE_FIND_BIGGEST_OBJECT);
}

// Read the annotation file for a frame in the 300-VW dataset: 68 facial landmarks
vector<Point2f> readAnnotationFile(const string& file) {
    ifstream in(file);
    string line;
    for (int i = 0; i < 3; ++i) { // Roll 3 lines for the header
        getline(in, line);
    }

    vector<Point2f> points;
    while (getline(in, line)) {
        stringstream l(line);
        Point2f p;
        l >> p.x >> p.y; // Each line contains a single point
        if (p.x != 0.0 and p.y != 0.0) {
            points.push_back(p);
        }
    }
    return points;
}

float MeanEuclideanDistance(const vector<Point2f>& A, const vector<Point2f>& B) {
    float med = 0.0f;
    for (int i = 0; i < A.size(); ++i) {
        med += cv::norm(A[i] - B[i]);
    }
    return med / (float)A.size();
}

vector<Point3f> objectPoints {
        {8.27412, 1.33849, 10.63490},    //left eye corner
        {-8.27412, 1.33849, 10.63490},   //right eye corner
        {0, -4.47894, 17.73010},         //nose tip
        {-4.61960, -10.14360, 12.27940}, //right mouth corner
        {4.61960, -10.14360, 12.27940},  //left mouth corner
};
vector<Point3f> objectPointsForReprojection {
        objectPoints[2],                   // nose
        objectPoints[2] + Point3f(0,0,15), // nose and Z-axis
        objectPoints[2] + Point3f(0,15,0), // nose and Y-axis
        objectPoints[2] + Point3f(15,0,0)  // nose and X-axis
};

// Facial landmarks IDs (from the 68) to match the 3D points
vector<int> landmarksIDsFor3DPoints {45, 36, 30, 48, 54}; // 0-index

int main(int argc, char **argv) {
    //Give the path to the directory containing all the files containing data
    CommandLineParser parser(
            argc, argv,
            "{ help h usage ?    |      | give the following arguments in following format }"
            "{ model_filename f  |      | (required) path to binary file storing the trained model which is to be loaded [example - /data/file.dat]}"
            "{ v vid_base        |      | (required) path to directory with video and facemark annotations.[example - /data/vid1/] }"
            "{ face_cascade c    |      | Path to the face cascade xml file which you want to use as a detector}"
    );
    // Read in the input arguments
    if (parser.has("help")) {
        parser.printMessage();
        cerr << "TIP: Use absolute paths to avoid any problems with the software!" << endl;
        return 0;
    }
    string filename(parser.get<string>("model_filename"));
    if (filename.empty()) {
        parser.printMessage();
        cerr << "The name  of  the model file to be loaded for detecting landmarks is not found" << endl;
        return -1;
    }
    string vid_base(parser.get<string>("vid_base"));
    if (vid_base.empty()) {
        parser.printMessage();
        cerr << "The video base dir name is not found" << endl;
        return -1;
    }
    string cascade_name(parser.get<string>("face_cascade"));
    if (cascade_name.empty()) {
        parser.printMessage();
        cerr << "The name of the cascade classifier to be loaded to detect faces is not found" << endl;
        return -1;
    }

    Mat img_orig;
    VideoCapture v(vid_base + "/vid.avi");

    if (not v.isOpened()) {
        cerr << "Cannot open video" << endl;
        return -1;
    }

    v >> img_orig;
    if (img_orig.empty()) {
        cerr << "Cannot read video" << endl;
        return -1;
    }

    cout << "original image size " << img_orig.size() << endl;

    //pass the face cascade xml file which you want to pass as a detector
    CascadeClassifier face_cascade;
    if (not face_cascade.load(cascade_name)) {
        cerr << "Cannot load cascade classifier: " << cascade_name << endl;
        return -1;
    }

    Ptr<Facemark> facemark = createFacemarkLBF();
    facemark->loadModel(filename);
    cout << "Loaded facemark LBF model" << endl;

    Size small_size(700, 700 * (float) img_orig.rows / (float) img_orig.cols);
    const float scaleFactor = 700.0f / img_orig.cols;

    const float w = small_size.width, h = small_size.height;
    Matx33f K {w, 0, w/2.0f,
               0, w, h/2.0f,
               0, 0, 1.0f};

    Mat img, img_out, img_out_dir;
    Mat rvec = Mat::zeros(3, 1, CV_64FC1);
    Mat tvec = Mat::zeros(3, 1, CV_64FC1);
    Mat R = Mat::eye(3,3,CV_64FC1);
    Rodrigues(R, rvec);

    for (;;) {
        v >> img_orig;
        if (img_orig.empty()) {
            break;
        }

        const uint32_t frameId = v.get(CAP_PROP_POS_FRAMES);
        const string filename = str($(vid_base + "/annot/%06d.pts") % frameId);
        const vector<Point2f> ground_truth = readAnnotationFile(filename);
        Mat(ground_truth) *= scaleFactor;

        resize(img_orig, img, small_size, 0, 0, INTER_LINEAR_EXACT);
        img.copyTo(img_out);
        img.copyTo(img_out_dir);

        // Draw the ground truth landmarks
        drawFacemarks(img_out, ground_truth, cv::Scalar(0, 255));

        vector<Rect> faces;
        faceDetector(img, faces, face_cascade);

        // Check if faces detected or not
        if (faces.size() != 0) {
            // We assume a single face so we look at the first only
            cv::rectangle(img_out, faces[0], Scalar(255, 0, 0), 2);

            vector<vector<Point2f> > shapes;

            if (facemark->fit(img, faces, shapes)) {
                // Draw the detected landmarks
                drawFacemarks(img_out, shapes[0], cv::Scalar(0, 0, 255));

                putText(img_out,
                        str($("MED: %.3f")%MeanEuclideanDistance(shapes[0], ground_truth)),
                        {10, 30},
                        FONT_HERSHEY_COMPLEX,
                        0.75,
                        Scalar(0,255,0), 2);

                // Collect 2D landmarks for 2D-3D correspondence
                vector<Point2f> points2d;
                for (int pId : landmarksIDsFor3DPoints) {
                    points2d.push_back(shapes[0][pId] / scaleFactor);
                }

                // Find object/camera transform
                solvePnP(objectPoints, points2d, K, Mat(), rvec, tvec, true);

                // Reproject the axes back onto the image
                vector<Point2f> projectionOutput(objectPointsForReprojection.size());
                projectPoints(objectPointsForReprojection, rvec, tvec, K, Mat(), projectionOutput);
                Mat(projectionOutput) *= scaleFactor;

                // Draw head direction axes
                arrowedLine(img_out_dir, projectionOutput[0], projectionOutput[1], Scalar(255,255,0), 2, 8, 0, 0.3);
                arrowedLine(img_out_dir, projectionOutput[0], projectionOutput[2], Scalar(0,255,255), 2, 8, 0, 0.3);
                arrowedLine(img_out_dir, projectionOutput[0], projectionOutput[3], Scalar(255,0,255), 2, 8, 0, 0.3);
            }
        } else {
            cout << "Faces not detected." << endl;
        }

        if (frameId % 10 == 0) {
            imwrite(($("orig%03d.jpg")%frameId).str(), img_orig);
            imwrite(($("out%03d.jpg")%frameId).str(), img_out);
            imwrite(($("dir%03d.jpg")%frameId).str(), img_out_dir);
        }

        imshow("input", img_out);
        if (waitKey(30) == 27) {
            break;
        }
    }

    v.release();

    return 0;
}