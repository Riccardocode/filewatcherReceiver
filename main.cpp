#include <iostream>    
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <vector>
#include "COM.h"

//Variabili per lettura COM
char COM[15];
HANDLE receiverCOM;
COMMTIMEOUTS timeout;
int nComRate = 57600;								// Baud (Bit) rate in bits/second 
int nComBits = 8;
const int BUFFERSIZE = 50;
std::vector<std::string> coords;

std::vector<std::string> split(std::string txt, char delimiter) {
	std::istringstream ss(txt);
	std::string token;
	std::vector<std::string> ret;
	int i = 0;
	while (std::getline(ss, token, delimiter) && (i < 4)) {
		ret.push_back(token);
		i++;
	}

	return ret;
}

int main() {
	int received = 0; //Conta il numero di coordinate ricevute
	std::string line;
	std::string sup_line;
	//Variabili per lettura parametri
	std::fstream parametri;
	const std::string pathParametri = "C:\\plotcoordinates\\fileWatcherReceiverParametri.txt";

	parametri.open(pathParametri, std::ios::in);
	if (parametri.is_open()) {
		getline(parametri, line);
		sup_line = "\\\\.\\" + line;
		for (int i = 0; i < sup_line.length(); i++) COM[i] = sup_line[i]; //Inizializza la variabile COM con il valore letto da file
	}
	else std::cout << "Errore apertura file" << std::endl;
	std::cout << sup_line << std::endl;
	
	//Inizializzazione COM
	createCOM(&receiverCOM, COM);  //questo e' un boolean quindi lo posso usare per biforcare il programma
	purgePort(&receiverCOM);
	SetComParms(&receiverCOM, nComRate, nComBits, timeout);
	purgePort(&receiverCOM);
	char msgIn[BUFFERSIZE];
	int i;
	std::fstream coordinats;   //variable for file open

	
	while (true) { 
		
		if (inputFromPort(&receiverCOM, msgIn, BUFFERSIZE) > 1) {	
			coords = split(msgIn, ',');
			if (coords.size() == 4) {
				if ((coords[0] == "0") || (coords[0] == "1")) {
					try {
						std::stof(coords[1]);
						std::stof(coords[2]);
						coordinats.open("C:\\plotcoordinates\\coordinates.txt", std::ios::app);
						coordinats << coords[0] << "," << coords[1] << "," << coords[2] << "\n";
						std::cout << msgIn << std::endl;
						coordinats.close();
					}
					catch (std::invalid_argument&) {
						std::cout << "the coordinates has been corrupted";
					}

				}
				else std::cout << "problema con esito plastica: " << coords[0] << "," << coords[1] << "," << coords[2] << "," << coords[3] << std::endl;

			}
			else {
				std::cout << "ricezione corrotta, dimensione coordinate: " << coords.size() <<" invece di 4." << std::endl;
			}
		
			
			received++;
			std::cout <<"Ricevuti: "<< received << std::endl;

		} 
		else {
			std::cout << "COM empty";
			//Sleep(50);
		}
		purgePort(&receiverCOM);
	}
}