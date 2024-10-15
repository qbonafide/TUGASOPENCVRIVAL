#include <iostream>
#include<opencv4/opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(){
    VideoCapture kamera (0);
    if(kamera.isOpened()){
        cerr << "tidak bisa membuka" << endl;
    }
    cv::Mat frame;

    while(true){
        kamera >> frame;
        Mat hsv, lim_color;
        Mat frame_clone = frame.clone();

        cvtColor(frame, hsv, COLOR_BGR2HSV);
        inRange(hsv, Scalar(0, 100, 100), Scalar(10, 255, 255), lim_color);

        vector<vector<Point>> kontur;
        findContours(lim_color, kontur, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < kontur.size(); i++) {
            Rect box = cv::boundingRect(kontur[i]);
            rectangle(frame_clone, box, Scalar(255, 255, 255), 2);
        }

        imshow("kamera",frame_clone);
        if(waitKey(30)== 32){
            break;
        }
    }

}