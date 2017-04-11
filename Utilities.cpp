#include "Utilities.h"

int open_train_directory(std::string s, std::vector<traininfo>* filenames)
{
	int label_ctr = -1;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(s.c_str())) != NULL)
	{
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			DIR *tempdir;
			if (ent->d_name[0] != '.')
			{
				std::string temp(ent->d_name);
				temp = s + "\\" + temp;
				//std::cout << temp << std::endl;
				if ((tempdir = opendir(temp.c_str())) != NULL)
				{
					traininfo tempinfo; //temp
					label_ctr++;
					tempinfo.label = label_ctr;
					while ((ent = readdir(tempdir)) != NULL)
					{

						if (ent->d_name[0] != '.')
						{
							std::string tempimagename(ent->d_name);

							if (tempimagename == "Data.csv")
							{
								tempimagename = temp + "\\" + tempimagename;
								//(*csv).push_back(tempimagename);
								tempinfo.csvname = tempimagename; //temp
							}

							else
							{
								tempimagename = temp + "\\" + tempimagename;
								//(*filenames).push_back(tempimagename);
								tempinfo.imagenames.push_back(tempimagename); //temp
								//std::cout << tempimagename << std::endl;

							}

						}
					}
					(*filenames).push_back(tempinfo);
					closedir(tempdir);
				}
				else
				{
					/* could not open directory */
					perror("Could not open directory");
					return EXIT_FAILURE;
				}
			}
		}

		closedir(dir);
	}
	else
	{
		/* could not open directory */
		perror("Could not open directory");
		return EXIT_FAILURE;
	}
	return 0;
}


int traindata(std::string s)
{
	std::vector<traininfo> files;
	open_train_directory(s, &files);
	read_images(&files);
	return 0;
}

int read_images(std::vector<traininfo>* directories)
{
	cv::CascadeClassifier face_cascade;
	std::string face_cascade_name = "haarcascade_frontalface_alt.xml";
	if (!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading\n"); return -1; };
	int counter = 0;
	std::ofstream outfile;
	outfile.open("labels.csv", std::ios::out);
	for (int d = 0; d < (*directories).size(); d++)
	{
		std::vector<cv::Rect> faces;
		std::vector<cv::Mat> final_faces;
		std::vector<int> final_labels;
		std::ifstream file((*directories)[d].csvname);
		int i = 0;
		CSVRow row;
		while (file >> row)
		{
			if (row.size() < 5)
			{
				perror("Not enough information");
				return EXIT_FAILURE;

			}
			else
			{
				cv::Mat tempimg = cv::imread((*directories)[d].imagenames[i], CV_LOAD_IMAGE_GRAYSCALE);
				i++;
				cv::equalizeHist(tempimg, tempimg);
				face_cascade.detectMultiScale(tempimg, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
				for (int j = 0; j < faces.size(); j++)
				{
					if (stoi(row[1])> faces[j].x && stoi(row[3]) > faces[j].x && stoi(row[1]) < faces[j].x + faces[j].width && stoi(row[3]) < faces[j].x + faces[j].width && stoi(row[2]) > faces[j].y && stoi(row[4]) > faces[j].y && stoi(row[2]) < faces[j].y + faces[j].height && stoi(row[4]) < faces[j].y + faces[j].height)
					{
						if (faces[j].x >= 0 && faces[j].width >= 0 && faces[j].x + faces[j].width <= tempimg.cols && faces[j].y >= 0 && faces[j].height >= 0 && faces[j].y + faces[j].height <= tempimg.rows)
						{
							counter++;
							tempimg = tempimg(cv::Rect(faces[j].x, faces[j].y, faces[j].width, faces[j].height));
							cv::resize(tempimg, tempimg, cv::Size(32, 32));
							cv::imwrite("temp\\" + std::to_string(counter) + ".jpg", tempimg);
							outfile << counter << ',' << (*directories)[d].label << std::endl;
							final_faces.push_back(tempimg);
							final_labels.push_back((*directories)[d].label);
							std::cout << counter << std::endl;
						}
					}


				}

			}

		}
	}

	return 0;
}

int training_images(std::string path, cv::Ptr<cv::FaceRecognizer> model)
{
	if (!path.empty())
	{
		path = path + "\\";
		std::vector<cv::Mat>images;
		std::vector<int> labels;

		std::ifstream file(path + "labels.csv");
		CSVRow row;
		while (file >> row)
		{
			images.push_back(cv::imread(path + row[0] + ".jpg")); //change to row[0] and add path
			labels.push_back(stoi(row[1]));
			std::cout << stoi(row[1]) << std::endl;
		}

		model->train(images, labels);
		std::string saveModelPath = "face-rec-model.yml";
		std::cout << "Saving the trained model to " << saveModelPath << std::endl;
		model->save(saveModelPath);
		return 0;
	}
	else
	{
		perror("Directory is empty");
		return EXIT_FAILURE;
	}

}

//int open_test_directory(std::string path, std::vector<cv::Mat>* test_images)
//{
//		DIR *dir;
//		struct dirent *ent;
//		
//		if ((dir = opendir(path.c_str())) != NULL)
//		{
//			cv::CascadeClassifier face_cascade;
//			std::string face_cascade_name = "haarcascade_frontalface_alt.xml";
//			if (!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading\n"); return -1; };
//			std::vector<cv::Rect> face;
//			cv::Mat tempimage;
//			int counter = 0;
//			while ((ent = readdir(dir)) != NULL) 
//			{
//				if (ent->d_name[0] != '.')
//				{
//					std::string temp(ent->d_name);
//					if (temp.substr(temp.find_last_of('.')+1) == "png")
//					{
//						temp = path + "\\" + temp;
//						if (!temp.empty())
//						{
//							tempimage = cv::imread(temp, CV_LOAD_IMAGE_GRAYSCALE);
//							cv::equalizeHist(tempimage, tempimage);
//							face_cascade.detectMultiScale(tempimage, face, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
//							std::cout << "dimagh ka dahi" << std::endl;
//							for (int j = 0; j < face.size(); j++)
//							{
//								if (face[j].x >= 0 && face[j].width >= 0 && face[j].x + face[j].width <= tempimage.cols && face[j].y >= 0 && face[j].height >= 0 && face[j].y + face[j].height <= tempimage.rows)
//								{
//									tempimage = tempimage(cv::Rect(face[j].x, face[j].y, face[j].width, face[j].height));
//									cv::resize(tempimage, tempimage, cv::Size(32, 32));
//									(*test_images).push_back(tempimage);
//									//cv::imwrite("test\\" + std::to_string(counter) + ".jpg", tempimage);
//									counter++;
//								}
//							}
//
//						}
//						else
//						{
//							perror("Image not found");
//							return EXIT_FAILURE;
//						}
//					}
//					
//				}
//				
//			}
//			closedir(dir);
//		}
//		else
//		{
//			/* could not open directory */
//			perror("Could not open directory");
//			return EXIT_FAILURE;
//		}
//		return 0;
//
//}

int open_test_directory(std::string path, std::vector<cv::Mat>* test_images)
{
	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		cv::CascadeClassifier face_cascade;
		std::string face_cascade_name = "haarcascade_frontalface_alt.xml";
		if (!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading\n"); return -1; };
		std::vector<cv::Rect> face;
		cv::Mat tempimage;
		int counter = 0;
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] != '.')
			{
				std::string temp(ent->d_name);
				if (temp.substr(temp.find_last_of('.') + 1) == "png")
				{
					temp = path + "\\" + temp;
					if (!temp.empty())
					{
						tempimage = cv::imread(temp, CV_LOAD_IMAGE_GRAYSCALE);
						cv::equalizeHist(tempimage, tempimage);
						(*test_images).push_back(tempimage);
						counter++;
					}
					else
					{
						perror("Image not found");
						return EXIT_FAILURE;
					}
				}

			}

		}
		closedir(dir);
	}
	else
	{
		/* could not open directory */
		perror("Could not open directory");
		return EXIT_FAILURE;
	}
	return 0;

}

int testing_images(cv::Ptr<cv::FaceRecognizer> model, std::vector<cv::Mat>* test_images)
{
	cv::CascadeClassifier face_cascade;
	std::string face_cascade_name = "haarcascade_frontalface_alt.xml";
	if (!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading\n"); return -1; };
	std::vector<cv::Rect> face;
	int counter = 0;
	int predicted_label;

	for (int i = 0; i < (*test_images).size(); i++)
	{
		face_cascade.detectMultiScale((*test_images)[i], face, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
		//std::cout << "dimagh ka dahi" << std::endl;
		for (int j = 0; j < face.size(); j++)
		{
			if (face[j].x >= 0 && face[j].width >= 0 && face[j].x + face[j].width <= (*test_images)[i].cols && face[j].y >= 0 && face[j].height >= 0 && face[j].y + face[j].height <= (*test_images)[i].rows)
			{
				cv::Mat tempimage;
				tempimage = (*test_images)[i](cv::Rect(face[j].x, face[j].y, face[j].width, face[j].height));
				cv::resize(tempimage, tempimage, cv::Size(32, 32));
				predicted_label = model->predict(tempimage);
				std::cout << predicted_label << std::endl;
				cv::rectangle((*test_images)[i], cv::Point(face[j].x, face[j].y), cv::Point(face[j].x + face[j].width, face[j].y + face[j].height), (255, 0, 0), 2);
				std::string name = "Person" + std::to_string(predicted_label);
				cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
				imshow(name, (*test_images)[i]);
				cv::waitKey(0);

				counter++;
			}
		}
	}
	return 0;

}

int testing(std::string path, cv::Ptr<cv::FaceRecognizer> model)
{
	std::vector<cv::Mat> test_images;
	open_test_directory(path, &test_images);
	testing_images(model, &test_images);
	return 0;
}

int training(std::string path, cv::Ptr<cv::FaceRecognizer> model)
{
	traindata(path);
	path = "temp";
	training_images(path,model);
	return 0;
}