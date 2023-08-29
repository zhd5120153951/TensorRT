// trt8_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#define HAVE_STRUCT_TIMESPEC
#include "yolov5_builder.hpp"
#include "yolov5_detector.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <windows.h>
#include <algorithm>
#include <string>
#include <thread>
#include <Windows.h>
#pragma once

#include "pthread.h"
//using namespace std;
//using namespace cv;
#pragma comment(lib,"pthreadVC2.lib")

//typedef struct
//{
//    yolov5::Detector& detector;
//    std::string url;
//    std::string num;
//}ThreadArg;


pthread_t thread[3];
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;
//pthread_mutex_t mutex4;
//pthread_mutex_t mutex5;
int mark[4] = {0};


cv::Mat  mat1;
std::vector<yolov5::Detection> detectionsout1;
std::vector<std::vector<yolov5::Detection>> detectionBatchOut1;//批处理输出
cv::Mat  mat2;
std::vector<yolov5::Detection> detectionsout2;
std::vector<std::vector<yolov5::Detection>> detectionBatchOut2;//批处理输出
cv::Mat  mat3;
std::vector<yolov5::Detection> detectionsout3;
std::vector<std::vector<yolov5::Detection>> detectionBatchOut3;//批处理输出

//cv::Mat  mat4;
//std::vector<yolov5::Detection> detectionsout4;
//cv::Mat  mat5;
//std::vector<yolov5::Detection> detectionsout5;

int picid1=0, picid2=0, picid3=0, picid4=0, picid5=0,picid6 = 0;

class TicToc
{
public:
    TicToc()
    {
        tic();
    }

    void tic()
    {
       start = std::chrono::system_clock::now();
    }

    double toc()
    {
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        return elapsed_seconds.count() * 1000;
    }

private:
    std::chrono::time_point<std::chrono::system_clock> start, end;
};

//线程1--执行函数
void* thread1(void*)
{
    yolov5::Detector detector;
    detector.init();
    detector.loadEngine("yolov5s-fire.engine");//同一模型文件，多线陈可以同时读吗？

    cv::Mat frame1;//这个帧由主线程获取视频帧后传入
    std::vector<cv::Mat> vecFrame1;

    std::vector<yolov5::Detection>::iterator iter;


    while (true)
    {
        
       /* if (picid1 != picid5)
        { */  
            printf(" 线程1在运行！%d %d\n ", picid1, picid5);
            pthread_mutex_lock(&mutex1);
            if (!mat1.empty())
            {
                mat1.copyTo(frame1);
                mat1.release();
                mark[0] = 2;
            }
            else
            {
                printf(" 线程1无数据处理！");
            }
            pthread_mutex_unlock(&mutex1);
            if (!frame1.empty())
            {
                //detectionsout1.clear();
                //detector.detect(frame1, &detectionsout1);//batch=1
                
                for (int j = 0; j < 3; j++)
                {
                    cv::Rect rect1(j * 640, 0, 640, 640);
                    vecFrame1.emplace_back(frame1(rect1).clone());
                    cv::Rect rect2(j * 640, 440, 640, 640);
                    vecFrame1.emplace_back(frame1(rect2).clone());
                }
                detector.detectBatch(vecFrame1, &detectionBatchOut1, 0);//batch=6
                
                frame1.release();
                mark[0] = 3;
                picid1= picid5;
            }
        //}
    }

    return 0;
}


void* thread2(void*)
{
    yolov5::Detector detector;
    detector.init();
    detector.loadEngine("yolov5sdc.engine");

    cv::Mat frame1;

    std::vector<yolov5::Detection>::iterator iter;


    while(true)
    {
        /*if (picid2 != picid5)
        {*/
            printf(" 线程2在运行！%d %d\n ", picid2, picid5);
            pthread_mutex_lock(&mutex2);
            if (!mat2.empty())
            {
                mat2.copyTo(frame1);
                mat2.release();
                mark[1] = 2;
            }
            else
            {
                printf(" 线程2无数据处理！");
            }
            pthread_mutex_unlock(&mutex2);
            if (!frame1.empty())
            {
                //detectionsout1.clear();
                detector.detect(frame1, &detectionsout2);
                frame1.release();
                mark[1] = 3;
                picid2 = picid5;
            }
        //}
    }

    return 0;
}


void* thread3(void*)
{
    yolov5::Detector detector;
    detector.init();
    detector.loadEngine("yolov5sdc.engine");

    cv::Mat frame1;

    std::vector<yolov5::Detection>::iterator iter;


    while(true)
    {
       /* if (picid3 != picid5)
        {*/
            printf(" 线程3在运行！%d %d\n ", picid3, picid5);
            pthread_mutex_lock(&mutex3);
            if (!mat3.empty())
            {
                mat3.copyTo(frame1);
                mat3.release();
                mark[2] = 2;
            }
            else
            {
                printf(" 线程3无数据处理！");
            }
            pthread_mutex_unlock(&mutex3);
            if (!frame1.empty())
            {
                //detectionsout1.clear();
                detector.detect(frame1, &detectionsout3);
                frame1.release();
                mark[2] = 3;
                picid3 = picid5;
            }
        //}
    }

    return 0;
}


//void* thread4(void*)
//{
//    yolov5::Detector detector;
//    detector.init();
//    detector.loadEngine("yolov5sdc.engine");
//
//    cv::Mat frame1;
//
//    std::vector<yolov5::Detection>::iterator iter;
//
//
//    while(true)
//    {
//     /*   if (picid4 != picid5)
//        {*/
//            printf(" 线程4在运行！%d %d\n ", picid4, picid5);
//            pthread_mutex_lock(&mutex4);
//            if (!mat4.empty())
//            {
//                mat4.copyTo(frame1);
//                mat4.release();
//                mark[3] = 2;
//            }
//            else
//            {
//                printf(" 线程4无数据处理！");
//            }
//            pthread_mutex_unlock(&mutex4);
//            if (!frame1.empty())
//            {
//                //detectionsout1.clear();
//                detector.detect(frame1, &detectionsout4);
//                frame1.release();
//                mark[3] = 3;
//                picid4 = picid5;
//            }
//        //}
//    }
//
//    return 0;
//}


//void* thread5(void*)
//{
//    yolov5::Detector detector;
//    detector.init();
//    detector.loadEngine("yolov5sdc.engine");
//
//    cv::Mat frame1;
//
//    std::vector<yolov5::Detection>::iterator iter;
//
//
//    while (true)
//    {
//        /*   if (picid4 != picid5)
//           {*/
//        printf(" 线程5在运行！%d %d\n ", picid6, picid5);
//        pthread_mutex_lock(&mutex5);
//        if (!mat5.empty())
//        {
//            mat5.copyTo(frame1);
//            mat5.release();
//           
//        }
//        else
//        {
//            printf(" 线程5无数据处理！");
//        }
//        pthread_mutex_unlock(&mutex5);
//        if (!frame1.empty())
//        {
//            //detectionsout1.clear();
//            detector.detect(frame1, &detectionsout5);
//            frame1.release();
//          
//            picid6 = picid5;
//        }
//        //}
//    }
//
//    return 0;
//}

//线程创建函数
void  thread_create()
{
    
    int  temp;
    memset(&thread, 0, sizeof(thread));
    if ((temp = pthread_create(&thread[0], NULL, thread1, NULL)) != 0)
        printf(" 线程1创建失败！\n ");
    else
        printf(" 线程1被创建！\n ");
    if ((temp = pthread_create(&thread[1], NULL, thread2, NULL)) != 0)
        printf(" 线程2创建失败！\n ");
    else
        printf(" 线程2被创建！\n ");
    if ((temp = pthread_create(&thread[2], NULL, thread3, NULL)) != 0)
        printf(" 线程3创建失败！\n ");
    else
        printf(" 线程3被创建！\n ");
    /*if ((temp = pthread_create(&thread[3], NULL, thread4, NULL)) != 0)
        printf(" 线程4创建失败！\n ");
    else
        printf(" 线程4被创建！\n ");
    if ((temp = pthread_create(&thread[4], NULL, thread5, NULL)) != 0)
        printf(" 线程5创建失败！\n ");
    else
        printf(" 线程5被创建！\n ");*/
}


//多线程推理
int threadrtsp()
{

    // rtsp地址变量
        // 一般main 主码流，sub 子码流
    std::string rtsp1 = "rtsp://admin:jiankong123@192.168.23.10:554/Streaming/Channels/101";

    // CAP_FFMPEG：使用ffmpeg解码
    cv::VideoCapture stream1 = cv::VideoCapture(rtsp1, cv::CAP_FFMPEG);

    if (!stream1.isOpened())
    {
        std::cout << "1摄像头有视频流未打开" << std::endl;
        return -1;
    }


    std::string rtsp2 = "rtsp://admin:jiankong123@192.168.23.13:554/Streaming/Channels/101";

    // CAP_FFMPEG：使用ffmpeg解码
    cv::VideoCapture stream2 = cv::VideoCapture(rtsp2, cv::CAP_FFMPEG);


    if (!stream2.isOpened())
    {
        std::cout << "2摄像头有视频流未打开" << std::endl;
        return -1;
    }

    std::string rtsp3 = "rtsp://admin:jiankong123@192.168.23.14:554/Streaming/Channels/101";

    // CAP_FFMPEG：使用ffmpeg解码
    cv::VideoCapture stream3 = cv::VideoCapture(rtsp3, cv::CAP_FFMPEG);


    if (!stream3.isOpened())
    {
        std::cout << "3摄像头有视频流未打开" << std::endl;
        return -1;
    }


    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex3, NULL);
    //pthread_mutex_init(&mutex4, NULL);
    //pthread_mutex_init(&mutex5, NULL);
    thread_create();


    Sleep(3000);
    cv::Mat frame1;
    cv::Mat frame2;
    TicToc timer;

    // // 使用namedWindow创建窗口，WINDOW_AUTOSIZE：自动调整窗口大小
    // cv::namedWindow("rtsp_demo", cv::WINDOW_AUTOSIZE);
    std::vector<yolov5::Detection> detections;
    std::vector<yolov5::Detection>::iterator iter;

    bool finishflag[4] = { false };

    std::vector<yolov5::Detection> detectionsout1_tmp;

    std::vector<yolov5::Detection> detectionsout2_tmp;

    std::vector<yolov5::Detection> detectionsout3_tmp;

    std::vector<yolov5::Detection> detectionsout4_tmp;

    std::vector<yolov5::Detection> detectionsout5_tmp;
    picid5 = 1000;
    while (true)
    {
        timer.tic();
        if (!stream1.read(frame1))
        {
            std::cout << "1有视频流未读取" << std::endl;
            continue;
        }
        if (!stream2.read(frame2))
        {
            std::cout << "2有视频流未读取" << std::endl;
            continue;
        }



        picid5++;

        /* 
        detectionsout1.clear();
        detectionsout2.clear();
        detectionsout3.clear();
        detectionsout4.clear();
        */

        pthread_mutex_lock(&mutex1);
        if (mat1.empty())
        {
            mat1.release();
            frame1.copyTo(mat1);
            
        }
        pthread_mutex_unlock(&mutex1);

        pthread_mutex_lock(&mutex2);
        if (mat2.empty())
        {
            mat2.release();
            frame2.copyTo(mat2);

        }
        pthread_mutex_unlock(&mutex2);

        pthread_mutex_lock(&mutex3);
        if (mat3.empty())
        {
            mat3.release();
            frame1.copyTo(mat3);

        }
        pthread_mutex_unlock(&mutex3);

        /*pthread_mutex_lock(&mutex4);
        if (mat4.empty())
        {
            mat4.release();
            frame1.copyTo(mat4);

        }
        pthread_mutex_unlock(&mutex4);

        pthread_mutex_lock(&mutex5);
        if (mat5.empty())
        {
            mat5.release();
            frame1.copyTo(mat5);

        }
        pthread_mutex_unlock(&mutex5);*/

        

        //while(true)
        //{
        //    Sleep(10);
        //    if (mark[0] == 3)
        //        finishflag[0] = true;
        //    if (mark[1] == 3)
        //        finishflag[1] = true;
        //    if (mark[2] == 3)
        //        finishflag[2] = true;
        //    if (mark[3] == 3)
        //        finishflag[3] = true;
        //    if (finishflag[0] && finishflag[1] && finishflag[2] && finishflag[3])
        //        break;
        //}
    /*  bool judge = true;
        do 
        {
            if(picid1 == picid5 && picid2 == picid5 && picid3 == picid5 && picid4 == picid5)
                judge = false;
        } while (judge);*/

        detectionsout1_tmp = detectionsout1;
        //detectionsout1.clear();
        detectionsout2_tmp = detectionsout2;
        //detectionsout2.clear();
        detectionsout3_tmp = detectionsout3;
        //detectionsout3.clear();
        //detectionsout4_tmp = detectionsout4;
        ////detectionsout4.clear();
        //detectionsout5_tmp = detectionsout5;

        std::cout<<timer.toc()<<std::endl;

        for (int i = 0; i < detectionsout1_tmp.size(); i++)
            if (detectionsout1_tmp[i].classId() == 1)
            {
                cv::rectangle(frame1, detectionsout1_tmp[i].boundingBox(), cv::Scalar((255, 0, 255)), 3);
            }
        cv::imshow("rtsp_demo1", frame1);
        if (cv::waitKey(1) == 27)
        {
            break;
        }

        for (int i = 0; i < detectionsout2_tmp.size(); i++)
            if (detectionsout2_tmp[i].classId() == 0)
                cv::rectangle(frame2, detectionsout2_tmp[i].boundingBox(), cv::Scalar((255, 0, 255)), 3);

        cv::imshow("rtsp_demo2", frame2);

        if (cv::waitKey(1) == 27)
        {
            break;
        }
    }

    return 0;
}

//推理单张RTSP流视频
int readrtsp(yolov5::Detector &detector)
{

    // rtsp地址变量:一般main 主码流，sub 子码流
    //std::string rtsp1 = "rtsp://admin:great123@192.168.8.204:554/cam/realmonitor?channel=1&subtype=0";
    //std::string rtsp1 = "rtsp://admin:1QAZ@wsx@192.168.1.251:554/Streaming/Channels/101";
    //std::string rtsp1 = "rtsp://192.168.1.102:554/stream1";
    
    //std::string rtsp1 = "test.mp4";
    
    std::string rtsp1 = "rtsp://admin:jiankong123@192.168.23.10:554/Streaming/Channels/101";

    // std::string rtsp1 = "rtsp://localhost:8554/live1.sdp";


    // CAP_FFMPEG：使用ffmpeg解码
    cv::VideoCapture stream1 = cv::VideoCapture(rtsp1, cv::CAP_FFMPEG);


    if (!stream1.isOpened())
    {
        std::cout << "有视频流未打开" << std::endl;
        return -1;
    }

    cv::Mat frame1;
    std::vector<cv::Mat>vecFrame;

    // // 使用namedWindow创建窗口，WINDOW_AUTOSIZE：自动调整窗口大小
    // cv::namedWindow("rtsp_demo", cv::WINDOW_AUTOSIZE);
    std::vector<yolov5::Detection> detections;
    std::vector<std::vector<yolov5::Detection>> detectionsBatch;
    std::vector<yolov5::Detection>::iterator iter;

    int seq = 1;
    float thresh0 = 0.15;
    float thresh1 = 0.7;
    bool hasperson = false;
    while (true)
    {
        seq++;
        if (!stream1.read(frame1))
        {
            std::cout << "有视频流未读取" << std::endl;
            continue;
        }
        
        if (seq % 20 != 0)//每隔多少帧取一张图推理
            continue;
      

        detector.detect(frame1, &detections);
        for (int i = 0; i < detections.size(); i++)
        {
            
            if (detections[i].classId() == 0 && detections[i].score()> thresh0)
            {
                hasperson = true;
                cv::rectangle(frame1, detections[i].boundingBox(), cv::Scalar((255, 0, 0)), 1);
                std::cout << detections[i].score() << std::endl;
            }
            
            /*if (detections[i].classId() == 1 && detections[i].score() > thresh1)
            {
                cv::rectangle(frame1, detections[i].boundingBox(), cv::Scalar((0, 255, 0)), 5);
                std::cout << detections[i].score() << std::endl;
            }*/
            //cv::imwrite("savepic/" + std::to_string(seq) + ".jpg", frame1);
        }

        /*
        vecFrame.push_back(frame1.clone());
        detector.detectBatch(vecFrame, &detectionsBatch, 0);
        for (int i = 0; i < detectionsBatch.size(); i++)
        {
            for (int j = 0; j < detectionsBatch[0].size(); j++)
            {
                if (detectionsBatch[i][j].classId() == 0 && detectionsBatch[i][j].score() > thresh0)
                {
                    hasperson = true;
                    cv::rectangle(vecFrame[i], detectionsBatch[i][j].boundingBox(), cv::Scalar((255, 0, 0)), 1);
                    std::cout << detectionsBatch[i][j].score() << std::endl;
                }
            }
        }
        */
        //保存
        if (hasperson)
        {
            cv::imwrite("save/" + std::to_string(seq) + ".jpg", frame1);
            //cv::imwrite("save/" + std::to_string(seq) + ".jpg", vecFrame[0]);
        }
        hasperson = false;
        //vecFrame.clear();

        cv::imshow("rtsp_demo", frame1);

        if (cv::waitKey(1) == 27)
        {
            break;
        }
    }

    return 0;
}

//循环读取文件夹中的图像--这个不可以直接返回，因为要深拷贝
void ReadImg(cv::String path,std::vector<cv::Mat>& vecimg)
{
    std::vector<cv::String> fn;
    cv::glob(path, fn, false);//获取路径下的每个图像的路径放在fn中

    for (size_t i = 0; i < fn.size(); i++)
    {
        vecimg.emplace_back(cv::imread(fn[i]));
    }
}

//推理本地batchsize=9
int readimg(yolov5::Detector& detector)
{
    std::vector<cv::Mat>vecFrame;

    // 使用namedWindow创建窗口，WINDOW_AUTOSIZE：自动调整窗口大小
    // cv::namedWindow("rtsp_demo", cv::WINDOW_AUTOSIZE);
    std::vector<std::vector<yolov5::Detection>> detectionsBatch;

    float thresh0 = 0.15;
    float thresh1 = 0.7;
    bool hasperson = false;
    
    ReadImg("E:\\Source\\WorkSpace\\yolov5-tensorrt\\test\\*.jpg", vecFrame); 
    std::vector<cv::Mat>vecSubFrame;//每张图被切割成6张---输入是1920*1080的，小于这个尺寸不行
    for (int i = 0; i < vecFrame.size(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            cv::Rect rect1(j * 640, 0, 640, 640);
            vecSubFrame.emplace_back(vecFrame[i](rect1).clone());
            cv::Rect rect2(j * 640, 440, 640, 640);
            vecSubFrame.emplace_back(vecFrame[i](rect2).clone());
        }        

        detector.detectBatch(vecSubFrame, &detectionsBatch, 0);
        for (int i = 0; i < detectionsBatch.size(); i++)
        {
            for (int j = 0; j < detectionsBatch[i].size(); j++)
            {
                if (detectionsBatch[i][j].classId() == 0 && detectionsBatch[i][j].score() > thresh0)
                {
                    hasperson = true;
                    cv::imwrite("source/" + std::to_string(i + 1) + ".jpg", vecSubFrame[i]);
                    cv::rectangle(vecSubFrame[i], detectionsBatch[i][j].boundingBox(), cv::Scalar((0, 0, 255)), 1);
                    std::cout << detectionsBatch[i][j].score() << std::endl;
                }
                //保存
                if (hasperson)
                {
                    cv::imwrite("save/" + std::to_string(i + 1) + ".jpg", vecSubFrame[i]);
                }
                hasperson = false;
            }
        }
        vecSubFrame.clear();
    }
    vecFrame.clear();
    return 0;
}

//切割图像
void CutPics(std::string path, std::string fileTarget, std::string fileName)
{
    cv::Mat srcImg = cv::imread(path);

    std::vector<cv::Mat> ceilImg;

    int height = srcImg.rows;
    int width = srcImg.cols;

    int cut_rows, cut_cols;
    int ceil_height = (int)(height / cut_rows);
    int ceil_width = (int)(width / cut_cols);
    int ceil_down_height = height - (cut_rows - 1) * ceil_height;
    int ceil_right_width = width - (cut_cols - 1) * ceil_width;

    for (int i = 0; i < cut_rows - 1; i++)
        for (int j = 0; j < cut_cols; j++)
        {
            if (j < cut_cols - 1)
            {
                cv::Rect rect(j * ceil_width, i * ceil_height, ceil_width, ceil_height);
                ceilImg.push_back(srcImg(rect));

            }
            else
            {
                cv::Rect rect((cut_cols - 1) * ceil_width, i * ceil_height, ceil_right_width, ceil_height);
                ceilImg.push_back(srcImg(rect));
            }
        }

    for (int i = 0; i < cut_cols; i++)
    {
        if (i < cut_cols - 1)
        {
            cv::Rect rect(i * ceil_width, (cut_rows - 1) * ceil_height, ceil_width, ceil_down_height);
            ceilImg.push_back(srcImg(rect));
        }
        else   //右下角这个图像块
        {
            cv::Rect rect((cut_cols - 1) * ceil_width, (cut_rows - 1) * ceil_height, ceil_right_width, ceil_down_height);
            ceilImg.push_back(srcImg(rect));
        }
    }



    std::cout << "分块个数：" << ceilImg.size() << std::endl;
    cv::Mat dst;



    for (int i = 0; i < ceilImg.size(); i++)
    {
        dst = ceilImg[i];

        /*imwrite( "F:/pic/00.jpg", dst);*/
        //imwrite("F:/" + fileTarget + "/" + fileName + "-" + num2str(i + 1) + ".jpg", dst);
        cv::imshow("dst", dst);

        /*waitKey(0);*/
    }
}

//合并图像
void MergePics()
{

}

//推理RTSP视频流batch-size=9(一个张图切分为9张)
int readrtspBatch(yolov5::Detector& detector)
{
    //std::string rtsp1 = "Thursday.mp4";
    //std::string rtsp1 = "728.mp4";
    std::string rtsp1 = "816.mp4";
    //std::string rtsp1 = "rtsp://admin:jiankong123@192.168.23.10:554/Streaming/Channels/101";

    // CAP_FFMPEG：使用ffmpeg解码
    cv::VideoCapture stream1 = cv::VideoCapture(rtsp1, cv::CAP_FFMPEG);


    if (!stream1.isOpened())
    {
        std::cout << "有视频流未打开" << std::endl;
        return -1;
    }

    cv::Mat frame1;
    std::vector<cv::Mat>vecFrame;

    // // 使用namedWindow创建窗口，WINDOW_AUTOSIZE：自动调整窗口大小
    // cv::namedWindow("rtsp_demo", cv::WINDOW_AUTOSIZE);
    std::vector<yolov5::Detection> detections;
    std::vector<std::vector<yolov5::Detection>> detectionsBatch;
    std::vector<yolov5::Detection>::iterator iter;

    int fps = 0;
    int index = 1;
    float thresh0 = 0.25;
    float thresh1 = 0.7;
    bool hasperson = false;
    cv::Point pt(0, 0);
    while (true)
    {
        fps++;
        if (!stream1.read(frame1))
        {
            std::cout << "有视频流未读取" << std::endl;
            continue;
        }
    
        if (fps % 5 != 0)//每隔多少帧取一张图推理
            continue;

        
        //for (int j = 0; j < 3; j++)//列--宽
        //{
        //    cv::Rect rect1(j * 640, 0, 640, 640);
        //    vecFrame.emplace_back(frame1(rect1).clone());
        //    cv::Rect rect2(j * 640, 440, 640, 640);
        //    vecFrame.emplace_back(frame1(rect2).clone());
        //    //vecFrame.emplace_back(frame1(cv::Rect(j * 640, i * 360, (j + 1) * 640, (i + 1) * 360)));
        //}
        
       
        for (int i = 0; i < 3; i++)//行--高
        {
            for (int j = 0; j < 3; j++)//列--宽
            {
                cv::Rect rect(j * 640, i * 360, 640, 360);
                vecFrame.emplace_back(frame1(rect).clone());
                //vecFrame.emplace_back(frame1(cv::Rect(j * 640, i * 360, (j + 1) * 640, (i + 1) * 360)));
            }
        }
        detector.detectBatch(vecFrame, &detectionsBatch, 0);
        for (int i = 0; i < detectionsBatch.size(); i++)
        {
            for (int j = 0; j < detectionsBatch[i].size(); j++)
            {
                if (detectionsBatch[i][j].classId() == 0 && detectionsBatch[i][j].score() > thresh0)
                {
                    hasperson = true;
                    cv::imwrite("source/" + std::to_string(index) + ".jpg", vecFrame[i]);

                    cv::rectangle(vecFrame[i], detectionsBatch[i][j].boundingBox(), cv::Scalar(0, 0, 255), 1);
                    /*pt.x = detectionsBatch[i][j].boundingBox().x;
                    pt.y = detectionsBatch[i][j].boundingBox().y-2;
                    cv::putText(vecFrame[i], detectionsBatch[i][j].className(), pt, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 1);
                    pt.x += 4;
                    cv::putText(vecFrame[i], detectionsBatch[i][j].score_str(), pt, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 1);*/
                    std::cout << detectionsBatch[i][j].score() << std::endl;

                    //保存
                    if (hasperson)
                    {
                        //cv::imwrite("save/" + std::to_string(seq) + ".jpg", frame1);
                        cv::imwrite("save/" + std::to_string(index) + ".jpg", vecFrame[i]);
                        index++;
                    }
                }
                
            }
            //合并
        }
        
        hasperson = false;
        vecFrame.clear();

        //cv::imshow("rtsp_batch", frame1);

        if (cv::waitKey(1) == 27)
        {
            break;
        }
    }

    return 0;
}

//推理RTSP视频流batch-size=9(一个张图切分为9张)--带参数
int readrtspBatchThread(int& num)
{
    std::string rtspUrl;
    yolov5::Detector detector;
    switch (num)
    {
    case 1:
        rtspUrl = "rtsp://admin:jiankong123@192.168.23.10:554/Streaming/Channels/101";
        detector.init();
        detector.loadEngine("yolov5s-fire.engine");
        break;
    case 2:
        rtspUrl = "rtsp://admin:jiankong123@192.168.23.13:554/Streaming/Channels/101";
        detector.init();
        detector.loadEngine("yolov5s-fire.engine");
        break;
    case 3:
        rtspUrl = "rtsp://admin:jiankong123@192.168.23.15:554/Streaming/Channels/101";
        detector.init();
        detector.loadEngine("yolov5s-fire.engine");
        break;
    default:
        break;
    }
    //std::string rtsp1 = "test.mp4";

    //std::string rtsp1 = "rtsp://admin:jiankong123@192.168.23.10:554/Streaming/Channels/101";

    // std::string rtsp1 = "rtsp://localhost:8554/live1.sdp";


    // CAP_FFMPEG：使用ffmpeg解码
    //cv::VideoCapture stream1 = cv::VideoCapture(rtsp1, cv::CAP_FFMPEG);
    cv::VideoCapture stream1 = cv::VideoCapture(rtspUrl, cv::CAP_FFMPEG);

    if (!stream1.isOpened())
    {
        std::cout << "有视频流未打开" << std::endl;
        return -1;
    }

    cv::Mat frame1;
    std::vector<cv::Mat>vecFrame;

    // // 使用namedWindow创建窗口，WINDOW_AUTOSIZE：自动调整窗口大小
    // cv::namedWindow("rtsp_demo", cv::WINDOW_AUTOSIZE);
    std::vector<yolov5::Detection> detections;
    std::vector<std::vector<yolov5::Detection>> detectionsBatch;
    std::vector<yolov5::Detection>::iterator iter;

    int fps = 0;
    int index = 1;
    float thresh0 = 0.15;
    float thresh1 = 0.7;
    bool hasperson = false;
    cv::Point pt(0, 0);
    while (true)
    {
        fps++;
        if (!stream1.read(frame1))
        {
            std::cout << "有视频流未读取" << std::endl;
            continue;
        }

        if (fps % 25 != 0)//每隔多少帧取一张图推理
            continue;


        for (int j = 0; j < 3; j++)//列--宽
        {
            cv::Rect rect1(j * 640, 0, 640, 640);
            vecFrame.emplace_back(frame1(rect1).clone());
            cv::Rect rect2(j * 640, 440, 640, 640);
            vecFrame.emplace_back(frame1(rect2).clone());
            //vecFrame.emplace_back(frame1(cv::Rect(j * 640, i * 360, (j + 1) * 640, (i + 1) * 360)));
        }


        //for (int i = 0; i < 3; i++)//行--高
        //{
        //    for (int j = 0; j < 3; j++)//列--宽
        //    {
        //        cv::Rect rect(j * 640, i * 360, 640, 360);
        //        vecFrame.emplace_back(frame1(rect).clone());
        //        //vecFrame.emplace_back(frame1(cv::Rect(j * 640, i * 360, (j + 1) * 640, (i + 1) * 360)));
        //    }
        //}
        //detector.detectBatch(vecFrame, &detectionsBatch, 0);
        detector.detectBatch(vecFrame, &detectionsBatch, 0);
        for (int i = 0; i < detectionsBatch.size(); i++)
        {
            for (int j = 0; j < detectionsBatch[i].size(); j++)
            {
                if (detectionsBatch[i][j].classId() == 0 && detectionsBatch[i][j].score() > thresh0)
                {
                    hasperson = true;
                    cv::imwrite("source-" + std::to_string(num) + "/" + std::to_string(index) + ".jpg", vecFrame[i]);

                    cv::rectangle(vecFrame[i], detectionsBatch[i][j].boundingBox(), cv::Scalar(0, 0, 255), 1);
                    /*pt.x = detectionsBatch[i][j].boundingBox().x;
                    pt.y = detectionsBatch[i][j].boundingBox().y - 2;
                    cv::putText(vecFrame[i], detectionsBatch[i][j].className(), pt, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 1);
                    pt.x += 4;
                    cv::putText(vecFrame[i], detectionsBatch[i][j].score_str(), pt, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 1);*/
                    std::cout << detectionsBatch[i][j].score() << std::endl;

                    //保存
                    if (hasperson)
                    {
                        //cv::imwrite("save/" + std::to_string(seq) + ".jpg", frame1);
                        cv::imwrite("save-" + std::to_string(num) + "/" + std::to_string(index) + ".jpg", vecFrame[i]);
                        index++;
                    }
                }

            }
            //合并
        }

        hasperson = false;
        vecFrame.clear();

        //cv::imshow("rtsp_batch", frame1);

        if (cv::waitKey(1) == 27)
        {
            break;
        }
    }

    return 0;
}


//当前处理的每张图没有切分，可以进行批量推理。
int main()
{
    std::cout << "start inference....\n";
    
    //yolov5::Builder builder;
    //builder.init();
    //builder.buildEngine("yolov5s.onnx", "yolov5s.engine");
    //builder.buildEngine("yolov5-fire-727.onnx", "yolov5-fire-727.engine");

    yolov5::Detector detector;
    detector.init();
    //detector.loadEngine("yolov5s-fire.engine");
    detector.loadEngine("yolov5-fire-727.engine");
    //detector.loadEngine("yolov5s.engine");

    //单线程执行
    //readrtsp(detector);//batch=1
    //readimg(detector);

    readrtspBatch(detector);//batch=6/9

    //多线程执行
    //threadrtsp();//创建子线程
    /*int num1, num2, num3;
    num1 = 1;
    num2 = 2;
    num3 = 3;
    std::thread th1(readrtspBatchThread, std::ref(num1));
    th1.join();
    std::thread th2(readrtspBatchThread, std::ref(num2));
    th2.join();
    std::thread th3(readrtspBatchThread, std::ref(num3));
    th3.join();*/
    

    return 0;
}



