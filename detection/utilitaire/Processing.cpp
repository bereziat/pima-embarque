#include "Processing.hpp"
#include "Parameters.hpp"

bool Processing::isInside(cv::Rect a, cv::Rect b)
{
	if ((a.tl().x < b.tl().x) &&
		(a.tl().y < b.tl().y) &&
		(a.br().x > b.br().x) &&
		(a.br().y > b.br().y))
		return true;
	return false;
}


void Processing::edgeDetection()
{
	cv::Canny(	this->image,
		 		this->image, 
		 		Parameters::getMinCanny(), 
		 		Parameters::getMaxCanny(), 
		 		Parameters::getSizeCanny());
}

void Processing::topologicalAnalysis()
{
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Point> approximation;
	//analyse topologique, algorithme suzuky85
	cv::findContours(	this->image, 
						contours,
						hierarchy, 
						cv::RETR_LIST, 
						cv::CHAIN_APPROX_SIMPLE );

	for (int i = 0; i < contours.size(); i++)
	{
		//algorithme d'approximation de polygones
		cv::approxPolyDP(	cv::Mat(contours[i]), 
							approximation, 
							cv::arcLength(cv::Mat(contours[i]), true)*0.05,
							true);
		int nb_sides = approximation.size();
		if(nb_sides < 3 || nb_sides > 4)
			continue;
		unsigned int area = std::fabs(cv::contourArea(contours[i]));
		if(nb_sides == 4){
			if ( area < Parameters::getMinSquare() ||
				 area > Parameters::getMaxSquare())
				continue;
		}else{
			if ( area < Parameters::getMinTriangle() || 
				 area > Parameters::getMaxTriangle())
				continue;
		}
		if (nb_sides == 4)
		{
			if(hierarchy[i][2] < 0) 
			bbsquares.push_back(cv::boundingRect(contours[i]));
		}else{
			if(hierarchy[i][3] > 0) 
				bbtriangles.push_back(cv::boundingRect(contours[i]));
		}
	}
}

void Processing::findBeacon()
{
	for(unsigned int i = 0; i < bbtriangles.size(); i++)
	{
		for(unsigned int j = 0; j < bbsquares.size(); j++)
		{
			if(isInside(bbtriangles[i], bbsquares[j]))
			{
				bbfaces.push_back(bbsquares[i]);
			}
		}
	}
	return;
}

void Processing::processing()
{
	edgeDetection();
	topologicalAnalysis();
	findBeacon();
}
