// Name: Peter Grofčík
// Project: Turbo coder/decoder for BMS
// Date: 24.11.2021

#ifndef BMS_DECODER_H
#define BMS_DECODER_H

#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include<iterator>
#include <math.h>
#include <iomanip>
#include "sisodecoder.h"


using namespace std;

void decode(vector<double> data, vector<int> interLeaver);
void SISOdecoder(vector<double> data, int La, vector<int> interLeaver);
double *metricCalculate(int y, int yp, int La);
TwoDimensions interleaved(TwoDimensions data, vector<int> interLeaver);
OneDimension interleave1D(OneDimension llr, vector<int> interLeaver);
OneDimension deInterleave1D(OneDimension llr, vector<int> interLeaver);
OneDimension plus1D(OneDimension llr1, OneDimension llr2);
bool checkIterSkip(OneDimension llr1, OneDimension llr2);
void printOut(OneDimension llr_ext, vector<int> interLeaver);



#endif //BMS_DECODER_H
