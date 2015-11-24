/*
@author : ROUSSEAU Sylvain 
Code réalisé dans le cadre de l'UE de PIMA 
M1 Imagerie, Université Pierre et Marie Curie
Détection de balises dans une image.
contact : sylvain (dot) rousseau93 (at) gmail (dot) com
*/

#include <opencv2/opencv.hpp>
#include <vector>
#include "Parameters.hpp"
#include "Processing.hpp"
#include "Face.hpp"


/*****************************************************************************/
/*********************** FONCTIONS UTILITAIRES *******************************/
/*****************************************************************************/

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

void exemple(cv::VideoCapture capture)
{
	cv::Mat frame, copie;
	char key;
	
	while(key != 'q')
	{
        std::cout << "frame" << std::endl;
		std::vector<cv::Rect> triangles, squares;
		capture >> frame;
		copie = frame.clone();
		contours(copie);
		for(unsigned int i = 0; i < squares.size(); i++)
		{
			for(unsigned int j = 0; j < triangles.size(); j++)
			{
				if(contient(squares[i], triangles[j]))
                    std::cout << "Face détectée, coin haut gauche : " << squares[i].tl() << "coin bas droit : " << squares[i].br() << std::endl;
			}
		}
		key = cv::waitKey(1);
	}
	return;
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

	Parameters::load(); // charger les parametres depuis le fichier
    exemple(capture);   //trouver les faces (boucle principale)
    return EXIT_SUCCESS;
}
