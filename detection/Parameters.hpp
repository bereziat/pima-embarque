#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

/*
 *  Classe permettant de gérer les parametres de l'algorithme
 *  ainsi que leur enregistrement dans un fichier texte
 */
class Parameters
{
	private :
		//contours
		static unsigned int minCanny;
		static unsigned int maxCanny;
		static unsigned int sizeCanny;

		//analyse 
		static unsigned int minSquare;
		static unsigned int maxSquare;
		static unsigned int minTriangle;
		static unsigned int maxTriangle;

	public :
		// input/output
		static void load();
		static void save();

		//contours
		inline static unsigned int getMinCanny(){return minCanny;}
		inline static unsigned int getMaxCanny(){return maxCanny;}
		inline static unsigned int getSizeCanny(){return sizeCanny;}
		inline static void setMinCanny(unsigned int min){minCanny = min;}
		inline static void setMaxCanny(unsigned int max){maxCanny = max;}
		inline static void setSizeCanny(unsigned int size){sizeCanny = size;}

		//analyse
		inline static unsigned int getMinSquare(){return minSquare;}
		inline static unsigned int getMaxSquare(){return maxSquare;}
		inline static unsigned int getMinTriangle(){return minTriangle;}
		inline static unsigned int getMaxTriangle(){return maxTriangle;}
		inline static void setMinSquare(unsigned int min){minSquare = min;}
		inline static void setMaxSquare(unsigned int max){maxSquare = max;}
		inline static void setMinTriangle(unsigned int min){minTriangle = min;}
		inline static void setMaxTriangle(unsigned int max){maxTriangle = max;}
};

#endif
