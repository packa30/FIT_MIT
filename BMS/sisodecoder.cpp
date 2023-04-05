// Name: Peter Grofčík
// Project: Turbo coder/decoder for BMS
// Date: 24.11.2021

#include "sisodecoder.h"

using namespace std;


///////////////////////////////////////////////     Initializations     ////////////////////////////////////////////////
/**
 * Siso decoder object initialization for metric/forward/backward paths with default "0" everywhere
 * @param data - input data -> for this function only required for SIZE value of multi-dim vectors
 */
void sisodecoder::initDecoder(TwoDimensions data){
    metricInit(data);
    this->forward = pathInit(data);
    this->backward = pathInit(data);
}

/**
 * Init for 2D vectors of forward/backward path -> each index of input data has it's own 1D vector for values
 * @param data - input data
 * @return either forward or backward path with "0"
 */
TwoDimensions sisodecoder::pathInit(TwoDimensions data){
    TwoDimensions init(data.size()+1, OneDimension(4, -INFINITY));
    for (unsigned int i = 0; i < data.size()+1; ++i) {
        init[i][0] = 0;
    }
    return init;
}

/**
 * Metric initialization for 3D vector -> each index of input data has it's own 2D vector for metric between states
 * @param data - input data
 */
void sisodecoder::metricInit(TwoDimensions data){
    ThreeDimensions init(data.size(), TwoDimensions(4, OneDimension(4, 0)));
    this->metric = init;
}


////////////////////////////////////////////////     Computations     //////////////////////////////////////////////////
/**
 * Main function for SISO decoder -> computing one iteration
 * @param data - input data
 * @return llr value (La value) for one iteration over input one input index
 */
vector<double> sisodecoder::compute(TwoDimensions data){
    metricCalculate(data);
    forwardP(data);
    backwardP(data);
    vector<double> llr = LLR(data);
    return llr;
//    coutThreeDim(this->metric);
//    coutTwoDim(this->metric[0]);
//    coutTwoDim(this->forward);
//    coutTwoDim(this->backward);
//    coutOneDim(llr);
}

/**
 * LLR computation from pre-computed forward/backward paths and metric in each iteration
 * Metric is changed based on actual La for SISO decoder and so are both path vectors based on new metric
 * @param data - input data (2D) - each input index has it's own (X,Xp,La) value
 * @return whole vector of llr values for one iteration over whole input data
 */
vector<double> sisodecoder::LLR(TwoDimensions data){
    vector<double> llr;
    for (unsigned int i = 0; i < data.size(); ++i) {
        vector<double> posMAX, negMax;
        for (unsigned int j = 0; j < 4; ++j) {
            for (unsigned int k = 0; k < 4; ++k) {
                if (input[j][k] == 1){
                    posMAX.push_back(this->forward[i][j] + this->metric[i][j][k] + this->backward[data.size()-i-1][k]);
                }else if(input[j][k] == -1){
                    negMax.push_back(this->forward[i][j] + this->metric[i][j][k] + this->backward[data.size()-i-1][k]);
                }else{}
            }
        }
        llr.push_back(*max_element(posMAX.begin(), posMAX.end()) - *max_element(negMax.begin(), negMax.end()));
    }
    return llr;
}

/**
 * Forward path calculation based on changed metric -> 4 values for each state where step might have ended
 * (values of paths from 4 states where it started -> choosing max value from 2 paths for each state )
 * @param data - input data (2D) - each input index has it's own (X,Xp,La) value
 */
void sisodecoder::forwardP(TwoDimensions data){
    for (unsigned int i = 0; i < data.size(); ++i) {
        vector<double> alpha;
        for (unsigned int j = 0; j < 4; ++j) {
            double maximum = -INFINITY;
            for (unsigned int k = 0; k < 4; ++k) {
                if (input[k][j] != 0){
                    maximum = max(maximum, this->forward[i][k] + this->metric[i][k][j]);
                }
            }
            alpha.push_back(maximum);
        }
        this->forward[i+1] = alpha;
    }
}

/**
 * Backward path calculation based on changed metric -> 4 values for each state where step might have started
 * (values of paths from 4 states where it ended -> choosing max value from 2 paths for each state )
 * @param data - input data (2D) - each input index has it's own (X,Xp,La) value
 */
void sisodecoder::backwardP(TwoDimensions data){
    for (unsigned int i = 0; i < data.size(); ++i) {
        vector<double> beta;
        for (unsigned int j = 0; j < 4; ++j) {
            double maximum = -INFINITY;
            for (unsigned int k = 0; k < 4; ++k) {
                if (input[j][k] != 0){
                    maximum = max(maximum, this->backward[i][k] + this->metric[data.size()-i-1][j][k]);
                }
            }
            beta.push_back(maximum);
        }
        this->backward[i+1] = beta;
    }
}

/**
 * Metric calculation for new La input
 * @param data - input data (2D) - each input index has it's own (X,Xp,La) value
 */
void sisodecoder::metricCalculate(TwoDimensions data){
    for (unsigned int i = 0; i < data.size(); ++i) {
        for (unsigned int j = 0; j < 4; ++j) {
            for (unsigned int k = 0; k < 4; ++k) {
                if(this->input[j][k]!=0){
                    this->metric[i][j][k] = this->input[j][k]*data[i][0]+this->output[j][k]*data[i][1]+this->input[j][k]*data[i][2];
                }
            }
        }
    }
}


//////////////////////////////////////     Debug outputs for multi dim vectors     /////////////////////////////////////
void sisodecoder::coutThreeDim(ThreeDimensions data){
    for (unsigned int i = 0; i < data.size(); ++i) {
        coutTwoDim(data[i]);
    }
}

void sisodecoder::coutTwoDim(TwoDimensions data){
    for (unsigned int i = 0; i < data.size(); ++i) {
        coutOneDim(data[i]);
    }
    cout << "\n";
}

void sisodecoder::coutOneDim(OneDimension data){
    std::cout << std::fixed;
    std::cout << std::setprecision(16);
    cout << "[ ";
    for (unsigned int i = 0; i < data.size(); ++i) {
        if (i!=data.size()-1){
            cout << data[i] << ", ";
        }else{
            cout << data[i] << " ";
        }
    }
    cout << " ]\n";
}


