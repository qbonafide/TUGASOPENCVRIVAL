#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <cstdlib>
#include <ctime>  

using namespace std;
using namespace cv;

struct Lingkaran {
    Point posisi;   
    Point kecepatan;   
    int radius;  
};

int main() {
    VideoCapture kamera(0);  
    if (!kamera.isOpened()) {
        cerr << "Tidak bisa membuka kamera" << endl;
        return -1;
    }

    srand(time(0));  
    cv::Mat frame;
    int score = 0;
    
    vector<Lingkaran> lingkarans;

    auto generateLingkaran = [&](int frameWidth, int frameHeight) {
        Lingkaran lingkaran;
        lingkaran.radius = 20;  

        int randomCorner = rand() % 4;
        if (randomCorner == 0) {  
            lingkaran.posisi = Point(0, 0);
        } else if (randomCorner == 1) {  
            lingkaran.posisi = Point(frameWidth - lingkaran.radius, 0);
        } else if (randomCorner == 2) {  
            lingkaran.posisi = Point(0, frameHeight - lingkaran.radius);
        } else {  
            lingkaran.posisi = Point(frameWidth - lingkaran.radius, frameHeight - lingkaran.radius);
        }

        
        lingkaran.kecepatan = Point(rand() % 15 + 10, rand() % 15 + 10);  
        return lingkaran;
    };

    
    lingkarans.push_back(generateLingkaran(640, 480));  

    while (true) {
        kamera >> frame;  
        if (frame.empty()) {
            cerr << "Tidak ada frame yang diambil dari kamera" << endl;
            break;
        }

        Mat hsv, lim_color;
        cvtColor(frame, hsv, COLOR_BGR2HSV);  

        inRange(hsv, Scalar(0, 100, 100), Scalar(10, 255, 255), lim_color);
        Mat lower_red;
        inRange(hsv, Scalar(160, 100, 100), Scalar(180, 255, 255), lower_red);
        lim_color += lower_red;  

        vector<vector<Point>> kontur;
        findContours(lim_color, kontur, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < lingkarans.size();) {
            Lingkaran& lingkaran = lingkarans[i];

            circle(frame, lingkaran.posisi, lingkaran.radius, Scalar(255, 0, 0), -1);  

            bool lingkaranMenyentuhMerah = false;
            for (size_t j = 0; j < kontur.size(); j++) {
                Rect box = boundingRect(kontur[j]);
                rectangle(frame, box, Scalar(255, 255, 255), 2);  

                if ((lingkaran.posisi.x + lingkaran.radius > box.x && lingkaran.posisi.x - lingkaran.radius < box.x + box.width) &&
                    (lingkaran.posisi.y + lingkaran.radius > box.y && lingkaran.posisi.y - lingkaran.radius < box.y + box.height)) {
                    lingkaranMenyentuhMerah = true;
                    score++;  
                    cout << "Skor: " << score << endl;
                    break;
                }
            }

            if (lingkaranMenyentuhMerah) {
                lingkarans.erase(lingkarans.begin() + i);
                lingkarans.push_back(generateLingkaran(frame.cols, frame.rows));  
                continue;  
            }

            lingkaran.posisi += lingkaran.kecepatan;
            if (lingkaran.posisi.x - lingkaran.radius < 0 || lingkaran.posisi.x + lingkaran.radius > frame.cols) {
                lingkaran.kecepatan.x = -lingkaran.kecepatan.x;  
            }
            if (lingkaran.posisi.y - lingkaran.radius < 0 || lingkaran.posisi.y + lingkaran.radius > frame.rows) {
                lingkaran.kecepatan.y = -lingkaran.kecepatan.y;  
            }

            i++;  
        }

        putText(frame, "Score = " + to_string(score), Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

        imshow("Kamera", frame);

        if (waitKey(10) == 32) {
            break;
        }
    }

    return 0;
}

