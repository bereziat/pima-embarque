/*
 * Classe implémentant la détection de balises dans une image en niveaux de gris
 * une classe par canal de couleur, on permet ainsi la détection de l'orientation
 */

 //TODO : AJOUTER LES THREADS
#ifndef PROCESSING_HPP
#define PROCESSING_HPP


#include <opencv2/opencv.hpp>
#include <vector>

class Processing
{
	private :
		cv::Mat image;
		std::vector<cv::Vec4i> hierarchy;
		std::vector<cv::Rect> bbsquares;
		std::vector<cv::Rect> bbtriangles;
		std::vector<cv::Rect> bbfaces;

		void edgeDetection();
		void topologicalAnalysis();
		void findBeacon();
		bool isInside(cv::Rect a, cv::Rect b);

	public :
		inline Processing(cv::Mat img):image(img){}
		inline std::vector<cv::Rect> getface(){return bbfaces;}//todo : ref
		void processing();

};

#endif
