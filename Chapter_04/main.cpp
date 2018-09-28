/*****************************************************************************
*   Number Plate Recognition using SVM and Neural Networks
******************************************************************************
*   by David Millán Escrivá, 5th Dec 2012
*   http://blog.damiles.com
******************************************************************************
*   Ch5 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/

// Main entry code OpenCV

#include <cv.h>
#include <highgui.h>
#include <cvaux.h>
#include <ml.h>

#include <iostream>
#include <vector>

#include "DetectRegions.h"
#include "OCR.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

string getFilename(string s) {

    char sep = '/';
    char sepExt='.';

    #ifdef _WIN32
        sep = '\\';
    #endif

    size_t i = s.rfind(sep, s.length( ));
    if (i != string::npos) {
        string fn= (s.substr(i+1, s.length( ) - i));
        size_t j = fn.rfind(sepExt, fn.length( ));
        if (i != string::npos) {
            return fn.substr(0,j);
        }else{
            return fn;
        }
    }else{
        return "";
    }
}

int main ( int argc, char** argv )
{
    cout << "OpenCV Automatic Number Plate Recognition\n";
    char* filename;
    Mat input_image;

    //Check if user specify image to process
    if(argc >= 2 )
    {
        filename= argv[1];
        //load image  in gray level
        input_image=imread(filename,1);
    }else{
        printf("Use:\n\t%s image\n",argv[0]);
        return 0;
    }        

    string filename_whithoutExt=getFilename(filename);
    cout << "working with file: "<< filename_whithoutExt << "\n";
    //Detect posibles plate regions
    DetectRegions detectRegions;    
    detectRegions.setFilename(filename_whithoutExt);
    detectRegions.saveRegions=false;
    detectRegions.showSteps=false;
    vector<Plate> posible_regions= detectRegions.run( input_image );    

    //SVM for each plate region to get valid car plates
    //Read file storage.
    FileStorage fs;
    fs.open("SVM.xml", FileStorage::READ);
    Mat SVM_TrainingData;
    Mat SVM_Classes;
    fs["TrainingData"] >> SVM_TrainingData;
    fs["classes"] >> SVM_Classes;
    
    Ptr<SVM> svmClassifier = cv::ml::SVM::create();
    svmClassifier->setType(cv::ml::SVM::C_SVC);
    svmClassifier->setKernel(cv::ml::SVM::LINEAR);
    svmClassifier->setDegree(0.0);
    svmClassifier->setGamma(1.0);
    svmClassifier->setCoef0(0);
    svmClassifier->setC(1);
    svmClassifier->setNu(0.0);
    svmClassifier->setP(0);
    svmClassifier->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, 0.01));

    Ptr<TrainData> tdata= TrainData::create(SVM_TrainingData, ROW_SAMPLE, SVM_Classes);

    svmClassifier->train(tdata);

    //For each possible plate, classify with svm if it's a plate or no
    vector<Plate> plates;
    for(int i=0; i< posible_regions.size(); i++)
    {
        Mat img=posible_regions[i].plateImg;
        Mat p= img.reshape(1, 1);
        p.convertTo(p, CV_32FC1);

        int response = (int)svmClassifier->predict( p );
        if(response==1)
            plates.push_back(posible_regions[i]);
    }

    cout << "Num plates detected: " << plates.size() << "\n";
    //For each plate detected, recognize it with OCR
    OCR ocr("OCR.xml");    
    ocr.saveSegments=true;
    ocr.DEBUG=false;
    ocr.filename=filename_whithoutExt;
    for(int i=0; i< plates.size(); i++){
        Plate plate=plates[i];
        
        string plateNumber=ocr.run(&plate);
        string licensePlate=plate.str();
        cout << "================================================\n";
        cout << "License plate number: "<< licensePlate << "\n";
        cout << "================================================\n";
        rectangle(input_image, plate.position, Scalar(0,0,200));
        putText(input_image, licensePlate, Point(plate.position.x, plate.position.y), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,200),2);
        if(ocr.DEBUG){
            imshow("Plate Detected seg", plate.plateImg);
            cvWaitKey(0);
        }

    }
    	namedWindow("Plate Detected",WINDOW_NORMAL);
        imshow("Plate Detected", input_image);
       for(;;)
       {
       int c;
       c = cvWaitKey(10);
       if( (char) c == 27)
       break;
       }
    return 0;
}
