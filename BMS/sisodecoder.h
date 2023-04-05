// Name: Peter Grofčík
// Project: Turbo coder/decoder for BMS
// Date: 24.11.2021

#ifndef BMS_SISODECODER_H
#define BMS_SISODECODER_H

#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include<iterator>
#include <math.h>
#include <iomanip>
#include <algorithm>

using namespace std;

typedef std::vector<double>OneDimension;
typedef std::vector<OneDimension>TwoDimensions;
typedef std::vector<TwoDimensions>ThreeDimensions;

class sisodecoder {
private:
    /**
     * For metric calculation 1 = 1 & -1 = 0 -> value 0 actually stands for not defined path
     */
    int input[4][4] =   {{-1,  0,  1, 0 },
                         { 1,  0, -1, 0 },
                         { 0, -1,  0, 1 },
                         { 0,  1,  0,-1 }};

    int output[4][4] =  {{ -1, 0,  1, 0 },
                         { -1, 0,  1, 0 },
                         { 0,  -1, 0,  1 },
                         { 0,  -1, 0,  1 }};

    TwoDimensions data;
    ThreeDimensions metric;
    TwoDimensions forward;
    TwoDimensions backward;
    void metricCalculate(TwoDimensions data);
    void metricInit(TwoDimensions data);
    TwoDimensions pathInit(TwoDimensions data);
    void forwardP(TwoDimensions data);
    void backwardP(TwoDimensions data);
    vector<double> LLR(TwoDimensions data);

public:
    void coutMetric();
    vector<double> compute(TwoDimensions data);
    void initDecoder(TwoDimensions data);

    void coutOneDim(OneDimension data);
    void coutTwoDim(TwoDimensions data);
    void coutThreeDim(ThreeDimensions data);
};


#endif //BMS_SISODECODER_H
