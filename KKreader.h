#pragma once

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>

#define BUF_SIZE 100

enum Format {
	bin,
	txt
};

std::vector<std::string> &my_split(const std::string &s, 
								char delim, 
								std::vector<std::string> &elems);

std::vector<std::string> my_split(const std::string &s, char delim); 

class KKInOut {
public:
	  void  readInput(std::string infile);
	  void prepareOut();
	  void   readData();
	  void  writeData(Format fmt);

	std::string  inFileName;
	std::string outFileName;

	double Wmax;
	double eps;
	// double AvSigma;
	   int NpointsOut;
	   int NpointsIn;
	  bool check;
	Format format;
	double* dataIn[2];
	double* dataOut[2];
private:
	  void  readTXT();
	  void  readBIN();
	  void writeTXT();
	  void writeBIN();
};