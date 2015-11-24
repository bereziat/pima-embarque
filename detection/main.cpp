/*
@author : ROUSSEAU Sylvain 
Code réalisé dans le cadre de l'UE de PIMA 
M1 Imagerie, Université Pierre et Marie Curie
Détection de balises dans une image.
contact : sylvain (dot) rousseau93 (at) gmail (dot) com
Il y a deux fenetres de configuration et une fenetre de valisation de l'algorithme
La premiere fenetre permet de configurer le filtre de canny (la détection de contours)
La seconde fenetre permet de configurer la taille minimale et maximale des triangles/carrés
La derniere fenetre est la fenetre de validation de l'algorithme
*/

#include <opencv2/opencv.hpp>
#include <vector>
#include "Parameters.hpp"
#include "Processing.hpp"
#include "Face.hpp"

#define WINDOW_NAME "Projet master IMA"
#define WINDOW_NAME2 "User interface"


/*****************************************************************************/
/*********************** FONCTIONS UTILITAIRES *******************************/
/*****************************************************************************/

//dessine les rectangles du tableau "squares" sur l'image "image" de couleur "color"
void drawRects(cv::Mat & image, std::vector<cv::Rect> & squares, cv::Scalar color)
{
	for(unsigned int i = 0; i < squares.size(); i++)
	{
		cv::rectangle(image, squares[i].tl(), squares[i].br(), color, 2, 1);
	}
}

//recupere le flux de la webcam
cv::VideoCapture getWebcam()
{
	cv::VideoCapture capture(1);
	if(!capture.isOpened())
	{
		std::cerr << "Impossible d'ouvrir le flux video" << std::endl;
	}
	return capture;
}

//retourne vrai si la boundingBox a contient la bounding box b
bool contient(cv::Rect a, cv::Rect b)
{
	if ((a.tl().x < b.tl().x) &&
		(a.tl().y < b.tl().y) &&
		(a.br().x > b.br().x) &&
		(a.br().y > b.br().y))
		return true;
	return false;
}

/*****************************************************************************/
/****************** ALGORITHMES DE TRAITEMENT D'IMAGES ***********************/
/*****************************************************************************/

//extrait les contours de l'image
void contours(cv::Mat & image)
{
	cv::Mat gray;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY); // couleur -> niveau de gris, possibilité de l'appliquer sur les trois caneaux
	cv::Canny(gray, gray, Parameters::getMinCanny(), Parameters::getMaxCanny(), Parameters::getSizeCanny());
	image = gray;
}

//extrait les rectangles et les triangles de l'image a partir de l'image des contours
void findRectangles(cv::Mat & gray, std::vector<cv::Rect > & squares, std::vector<cv::Rect > & triangles)
{
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	std::vector<cv::Point> approximation;
    //l'algorithme de suzuki pour l'analyse topologique d'images
	cv::findContours(gray, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++)
	{
        // approximation de polygones
		cv::approxPolyDP(cv::Mat(contours[i]), approximation, cv::arcLength(cv::Mat(contours[i]), true)*0.05, true);
		int nb_sides = approximation.size();
		if(nb_sides < 3 || nb_sides > 4)
			continue;
		unsigned int area = std::fabs(cv::contourArea(contours[i]));
		if(nb_sides == 4){
			if ( area < Parameters::getMinSquare() || area > Parameters::getMaxSquare())
				continue;
		}else{
			if ( area < Parameters::getMinTriangle() || area > Parameters::getMaxTriangle())
				continue;
		}
		if (nb_sides == 4)
		{
			squares.push_back(cv::boundingRect(contours[i]));
		}else{
			cv::Rect r = cv::boundingRect(contours[i]);
			float rapport = ((float)r.width/r.height);
			triangles.push_back(r);
		}
	}
}

/*****************************************************************************/
/***************** CONFIGURATION DE L'ALGORITHME *****************************/
/*****************************************************************************/

//configuration de la partie détection de contours
char configureContours(cv::VideoCapture capture)
{
	cv::Mat frame;
	int min(Parameters::getMinCanny());
	int max(Parameters::getMaxCanny());
	char key;

	cv::createTrackbar( "Seuil minimal", WINDOW_NAME2, &min, 700);
	cv::createTrackbar( "Seuil maximal", WINDOW_NAME2, &max, 700);

	while(key != 'q' && key != 'a' && key != 'z')
	{
		Parameters::setMinCanny(min);
		Parameters::setMaxCanny(max);
		capture >> frame;
		contours(frame);
		cv::imshow(WINDOW_NAME, frame);
		key = cv::waitKey(1);
	}
	cv::destroyWindow(WINDOW_NAME2);
	cv::namedWindow(WINDOW_NAME2, 0);
	return key;
}

//configuration de la partie détection de carrés et de triangles
char configureDetection(cv::VideoCapture capture)
{
	cv::Mat frame;
	int minT(Parameters::getMinTriangle());
	int maxT(Parameters::getMaxTriangle());
	int minS(Parameters::getMinSquare());
	int maxS(Parameters::getMaxSquare());
	char key;
	cv::createTrackbar( "Aire minimale (triangle)", WINDOW_NAME2, &minT, 1000);
	cv::createTrackbar( "Aire maximale (triangle)", WINDOW_NAME2, &maxT, 10000);
	cv::createTrackbar( "Aire minimale (carré)", WINDOW_NAME2, &minS, 1000);
	cv::createTrackbar( "Aire maximale (carré)", WINDOW_NAME2, &maxS, 100000);
	while(key != 'q' && key != 'a' && key != 'z')
	{
		std::vector<cv::Rect> triangles, squares;
		Parameters::setMinTriangle(minT);
		Parameters::setMaxTriangle(maxT);
		Parameters::setMinSquare(minS);
		Parameters::setMaxSquare(maxS);
		capture >> frame;
		cv::Mat gray = frame.clone();
		contours(gray);
		findRectangles(gray, squares, triangles);
		drawRects(frame, squares, cv::Scalar(255, 0, 255));
		drawRects(frame, triangles, cv::Scalar(255, 255, 0));
		cv::imshow(WINDOW_NAME, frame);
		key = cv::waitKey(1);
	}
	cv::destroyWindow(WINDOW_NAME2);
	cv::namedWindow(WINDOW_NAME2, 0);
	return key;

}

// validation de l'efficacité de l'algorithme
char validation(cv::VideoCapture capture)
{
	cv::destroyWindow(WINDOW_NAME2);
	cv::Mat frame, copie;
	char key;
	
	while(key != 'q' && key != 'a' && key != 'z')
	{
		std::vector<cv::Rect> triangles, squares;
		capture >> frame;
		copie = frame.clone();
		contours(copie);
		findRectangles(copie, squares, triangles);
		for(unsigned int i = 0; i < squares.size(); i++)
		{
			for(unsigned int j = 0; j < triangles.size(); j++)
			{
				if(contient(squares[i], triangles[j]))
					cv::rectangle(frame, squares[i].tl(), squares[i].br(), cv::Scalar(255, 255, 0), 3, 1);
			}
		}
		cv::imshow(WINDOW_NAME, frame);
		key = cv::waitKey(1);
	}
	cv::namedWindow(WINDOW_NAME2, 0);
	return key;
}


/*****************************************************************************/
/**************************** MAIN *******************************************/
/*****************************************************************************/
//les valeurs a configurer
unsigned int Parameters::minCanny = 400;
unsigned int Parameters::maxCanny = 180;
unsigned int Parameters::sizeCanny = 3;
unsigned int Parameters::minSquare = 600;
unsigned int Parameters::maxSquare = 23000;
unsigned int Parameters::minTriangle = 100;
unsigned int Parameters::maxTriangle = 4500;

int main(void)
{	
	cv::VideoCapture capture = getWebcam();
	cv::namedWindow(WINDOW_NAME, 1);
	cv::namedWindow(WINDOW_NAME2, 0);
	char key;
	int num_menu(0);
	const unsigned int nb_menus = 3;

	Parameters::load();
	while(key != 'q')
	{
		switch(num_menu)
		{
		case 0 :
			key = configureContours(capture);
			break;
		case 1 :
			key = configureDetection(capture);
			break;
		case 2 :
			key = validation(capture);
			break;
		default :
			num_menu = 0;
			break;
		}
		//touches clavier
		if(key == 'a'){
			num_menu++;
			num_menu%=nb_menus;
		}else if (key == 'z'){
			if (num_menu > 0){
				num_menu--;
			}
			else {
				num_menu = 2;
			}
		}else{ 
			key = 'q';
		}
	}
	Parameters::save();
    return EXIT_SUCCESS;
}
