#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "CSVRow.h"


int open_train_directory(std::string s, std::vector<traininfo>* filenames);
int read_images(std::vector<traininfo>* directories);
int traindata(std::string s);
int training_images(std::string path, cv::Ptr<cv::FaceRecognizer> model);
int open_test_directory(std::string path, std::vector<cv::Mat>* test_images);
int testing_images(cv::Ptr<cv::FaceRecognizer> model, std::vector<cv::Mat>* test_images);
int training(std::string path, cv::Ptr<cv::FaceRecognizer> model);
int testing(std::string path, cv::Ptr<cv::FaceRecognizer> model);