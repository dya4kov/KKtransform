#include "KKreader.h"

std::vector<std::string> &my_split(const std::string &s, 
								char delim, 
								std::vector<std::string> &elems) 
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> my_split(const std::string &s, 
								char delim) {
	std::vector<std::string> elems;
	my_split(s, delim, elems);
	return elems;
}

void KKInOut::readInput(std::string fileName) {
	std::ifstream infile(fileName.c_str(), std::ios::in);
	char* buffer = new char[BUF_SIZE];
	buffer[0] = '#';
	std::string line;
	std::string param;
	std::string value;
	//std::cout << filename << std::endl;
	while (!infile.eof()) {
		while (buffer[0] == '#' || buffer[0] == ' ') {
			infile.getline(buffer, BUF_SIZE);
		}
		line  = buffer;
		line  = my_split(line, '#')[0];
		param = my_split(line, ' ')[0];
		value = my_split(line, ' ')[1];
		if (!param.compare("format:")) {
			if (!value.compare("bin"))  format = bin;
			if (!value.compare("txt"))  format = txt;
		}
		if (!param.compare("infile:"))  inFileName      = value;
		if (!param.compare("outfile:")) outFileName     = value;
		if (!param.compare("Wmax:"))    Wmax       = atof(value.c_str());
		if (!param.compare("Npoints:")) NpointsOut = atoi(value.c_str());
		if (!param.compare("check:"))   check = true;
		if (!param.compare("eps:"))     eps = atof(value.c_str());
		infile.getline(buffer, BUF_SIZE);
	}
	std::cout << "Input parameters for KK:" << std::endl;
	std::cout << "Data format: " << (format == txt ? "txt" : "bin") << std::endl;
	std::cout << "Input data file: "  << inFileName << std::endl;
	std::cout << "Output data file: " << outFileName << std::endl;
	std::cout << "Wmax: "        << Wmax << std::endl;
	std::cout << "NpointsOut: "  << NpointsOut << std::endl;
	std::cout << "check: "       << (check ? "yes" : "no") << std::endl;
	std::cout << "eps: "         << eps << std::endl;
	return;
}

void KKInOut::readData() {
	if (format == bin) readBIN();
	if (format == txt) readTXT();
	return;
}

void KKInOut::writeData(Format fmt) {
	if (format == bin) writeBIN();
	if (format == txt) writeTXT();
	return;	
}

void KKInOut::readTXT() {
	std::ifstream infile(inFileName.c_str(), std::ios::in);
	double omega;
	double omegaOld;
	double sigma;
	// count items
	NpointsIn = 0;
	// AvSigma = 0;
	while (!infile.eof()) {
		infile >> omega;
		if (omega < 0) { 
			std::cout << "input is incorrect: omega = " 
			          << omega << " < 0, " << "Npoint = "
			          << NpointsIn << std::endl;
			exit(0);
		}
		if (omega < omegaOld) { 
			std::cout << "input is incorrect: omega = " 
			          << omega << " < previous omega = " << omegaOld 
			          << ", " << "Npoint = "
			          << NpointsIn << std::endl;
			exit(0);
		}
		omegaOld = omega;
		infile >> sigma;
		// AvSigma += sigma;
		NpointsIn++;
	}
	// AvSigma = AvSigma/NpointsIn;
	infile.close();
	NpointsIn++; // for omega = 0
	// allocate memory
	dataIn[0] = new double[NpointsIn];
	dataIn[1] = new double[NpointsIn];
	// read items
	infile.open(inFileName.c_str(), std::ios::in);
	NpointsIn = 1;
	while (!infile.eof()) {
		infile >> dataIn[0][NpointsIn];
		infile >> dataIn[1][NpointsIn];
		// dataIn[1][NpointsIn] /= AvSigma;
		dataIn[1][NpointsIn] = log10(dataIn[1][NpointsIn]);
		NpointsIn++;
	}
	infile.close();
	// omega = 0
	dataIn[0][0] = 0;
	// linear extrapolation for omega = 0
	dataIn[1][0] = dataIn[1][1] - (dataIn[1][2] - dataIn[1][1]);
	//dataIn[1][0] = 0.5*dataIn[1][1];
	return;
}

void KKInOut::readBIN() {
	return;
}

void KKInOut::writeTXT() {
	std::ofstream outfile(outFileName.c_str(), std::ios::out);
    for (int i = 0; i < NpointsOut; ++i) {
		outfile.setf(std::ios::scientific | std::ios::left);
    	outfile.width(15);
    	outfile.precision(6);
    	outfile << dataOut[0][i];
    	outfile << dataOut[1][i];
    	outfile << std::endl;
    }
	return;
}

void KKInOut::writeBIN() {
	return;
}

void KKInOut::prepareOut() {
	double WmaxNew;
	double dW;
	// define Wmax
	if (Wmax <= 0.0) {
		// 90% of maximal from input data
		WmaxNew = 0.9*dataIn[0][NpointsIn - 1];
	}
	else {
		WmaxNew = Wmax;
	}
	if (NpointsOut < 2) {
		dW = (dataIn[0][NpointsIn - 1] - dataIn[0][0])/(NpointsIn - 1);
		NpointsOut = (int) WmaxNew/dW;
		NpointsOut += 1;
	}
	else {
		dW = WmaxNew/(NpointsOut - 1);
	}
	Wmax = WmaxNew;
	dataOut[0] = new double[NpointsOut];
	dataOut[1] = new double[NpointsOut];
	dataOut[0][0] = 0;
	dataOut[1][0] = 0;
	for (int i = 1; i < NpointsOut; ++i) {
		dataOut[0][i] = dataOut[0][i - 1] + dW;
		dataOut[1][i] = 0;
	}
	return;
}