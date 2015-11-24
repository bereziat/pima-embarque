/*
 *  Classe permettant de représenter un Face détectée.
 */

/* TODO : ajouter une méthode toString pour l'envoi des données */
#ifndef FACE_HPP
#define FACE_HPP

#include <opencv2/opencv.hpp>

enum color{YELLOW_F=0, WHITE_F=1, GREEN_F=2, BLUE_F=3};

class Face
{
	public :
		inline Face(color c, cv::Rect boundingBox) : c_(c), boundingBox_(boundingBox){}
		inline color getColor() const {return c_;}
		inline cv::Rect getBoundingBox() const {return boundingBox_;}
		inline void setColor(color c) {c_ = c;}
		inline void setBoundingBox(cv::Rect bb){ boundingBox_=bb;}

	private :
		color c_;
		cv::Rect boundingBox_;

};

#endif