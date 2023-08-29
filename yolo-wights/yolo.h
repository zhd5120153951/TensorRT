#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace dnn;
using namespace std;

//����ṹ����
struct Net_config
{
	float confThreshold; // Confidence threshold--���Ŷ���ֵ
	float nmsThreshold;  // Non-maximum suppression threshold--�Ǽ���ֵ������ֵ
	int inpWidth;        // Width of network's input image--����ͼ���
	int inpHeight;		 // Height of network's input image--����ͼ���
	string classesFile;  //����ļ�
	string modelConfiguration;//ģ������
	string modelWeights;      //ģ��Ȩ��
	string netname;           //ģ������
};

//����Yolo��
class YOLO
{
	public:
		YOLO(Net_config config);
		void detect(Mat& frame);
	private:
		float confThreshold;
		float nmsThreshold;
		int inpWidth;
		int inpHeight;
		char netname[20];
		vector<string> classes;
		Net net;
		void postprocess(Mat& frame, const vector<Mat>& outs);
		void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame);
};

//����ģ�ͳ�ʼ������
Net_config yolo_nets[4] = 
{
	{0.5, 0.4, 416, 416,"classes/coco.names", "weights/yolov3/yolov3.cfg", "weights/yolov3/yolov3.weights", "yolov3"},
	{0.5, 0.4, 608, 608,"classes/coco.names", "weights/yolov4/yolov4.cfg", "weights/yolov4/yolov4.weights", "yolov4"},
	{0.5, 0.4, 320, 320,"classes/coco.names", "weights/yolo-fastest/yolo-fastest-xl.cfg", "weights/yolo-fastest/yolo-fastest-xl.weights", "yolo-fastest"},
	{0.5, 0.4, 320, 320,"classes/coco.names", "weights/yolobile/csdarknet53s-panet-spp.cfg", "weights/yolobile/yolobile.weights", "yolobile"}
};