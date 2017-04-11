#include "Utilities.h"

void showHelp()
{
	std::string tmp;
	std::string text;
	std::ifstream inp("readme.txt");

	if (!inp.is_open())
	{
		std::cout << "Help file 'Readme.txt' not found" << std::endl;
		return;
	}

	while (std::getline(inp, tmp))
		text += tmp + "\n";

	std::cout << text;
	getchar();
}

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		std::cout << "Not enough or incorrect arguments" << std::endl;
		showHelp();
		return -1;
	}
		
	std::cout << argv[1] << std::endl;
	if (std::string(argv[1]) == "-train")
	{
		std::string path = argv[2];
		cv::Ptr<cv::FaceRecognizer> model = cv::createEigenFaceRecognizer();
		training(path, model);
	}
	else if (std::string(argv[1]) == "-test")
	{
		if (argc < 4)
		{
			std::cout << "Not enough or incorrect arguments" << std::endl;
			showHelp();
			return -1;
		}

		std::string path = argv[2];
		cv::Ptr<cv::FaceRecognizer> model = cv::createEigenFaceRecognizer();
		model->load(std::string(argv[3]));
		testing(path, model);
	}

	//std::vector<traininfo> files;
	//std::string s = "training_and_test\\Training-Data";
	//cv::Ptr<cv::FaceRecognizer> model = cv::createEigenFaceRecognizer();
	///*s = "temp";
	//training_images(s, model);*/
	////training(s,model_load);
	//
	//
	//model->load("face-rec-model.yml");
	//std::string path = "training_and_test\\test-dataset1";
	//testing(path,model);

	getchar();
	return 0;
}
