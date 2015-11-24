#include "Parameters.hpp"
#include <iostream>
#include <fstream>

void Parameters::save()
{
	std::ofstream file("data", std::ios::out | std::ios::trunc);
	if(file)
	{
		file << minCanny << "\n";
		file << maxCanny << "\n";
		file << sizeCanny << "\n";
		file << minSquare << "\n";
		file << maxSquare << "\n";
		file << minTriangle << "\n";
		file << maxTriangle;


		file.close();
	}else
	{
		std::cerr << "Impossible d'ouvrir le fichier de sauvegarde." << std::endl;
	}
}
void Parameters::load()
{
	std::ifstream file("data", std::ios::in);
	if(file)
	{
		file >> minCanny;
		file >> maxCanny;
		file >> sizeCanny;
		file >> minSquare;
		file >> maxSquare;
		file >> minTriangle;
		file >> maxTriangle;

		file.close();
	}else
	{
		std::cerr << "Impossible d'ouvrir le fichier de sauvegarde." << std::endl;
	}
}