// Name: Peter Grofčík
// Project: Turbo coder/decoder for BMS
// Date: 24.11.2021

#include "decoder.h"

using namespace std;

/**
 * Main function for decoder -> iterating over whole data (20x default iterations for decoder to be sure) implementing referenced figure for SISO MAP decoder
 * -> 2x SISO decoders, Interleaver/Deinterleaver for both input and La data in each iteration
 * @param data input data -> for this function only required for SIZE value of multi-dim vectors
 * @param interLeaver - interleaver - vector with values to change order of data
 */
void decode(vector<double> data, vector<int> interLeaver){
    TwoDimensions data1(interLeaver.size()+2, OneDimension(3, 0)), data2(interLeaver.size()+2, OneDimension(3,0));
    for (unsigned int i = 0; i < data.size(); i+=3) {
        data1[i/3][0] = data[i];
        data1[i/3][1] = data[i+1];
        data2[i/3][0] = data[i];
        data2[i/3][1] = data[i+2];
    }
    data2 = interleaved(data2, interLeaver);

    sisodecoder siso1;
    siso1.initDecoder(data1);
    sisodecoder siso2;
    siso2.initDecoder(data2);

    OneDimension llr1(data1.size(), 0), llr2(data2.size(), 0), llr_ext(data2.size(), 0), llr_Interleaved(data2.size(), 0);

    for (unsigned int i = 0; i < 20; ++i) {
        for (unsigned int j = 0; j < data1.size(); ++j) {
            data1[j][2] = llr2[j];
        }

        llr1 = siso1.compute(data1);
        llr1 = plus1D(llr1, llr2);
        llr_Interleaved = interleave1D(llr1, interLeaver);

        for (unsigned int j = 0; j < data2.size(); ++j) {
            data2[j][2] = llr_Interleaved[j];
        }

        llr2 = llr_ext = siso2.compute(data2);
        llr2 = deInterleave1D(plus1D(llr2, llr_Interleaved), interLeaver);

//        siso1.coutOneDim(llr1);
//        -> shows decoders confidence about each decoded bit -> values should increase for positive and decrease for negative values in every iteration
//        -> positive can switch with negative if input is long enough to mislead decoder in few iterations, but once llr2 prediction is suggesting the same as llr1
//           next iterations can be skipped
        if (checkIterSkip(deInterleave1D(llr_Interleaved, interLeaver),llr2)){
            break;
        }

    }
    printOut(llr_ext, interLeaver);
}

/**
 * Function to print out calculated values in right order
 * @param llr_ext - finished llr value from second SISO decoder -> calculated value that was encrypted in input of decoder
 * @param interLeaver - interleaver to change order of calculated values
 */
void printOut(OneDimension llr_ext, vector<int> interLeaver){
    OneDimension llr = deInterleave1D(llr_ext, interLeaver);
    for (unsigned int i = 0; i < llr.size()-2; ++i) {
        llr[i] >= 0 ? cout << 1 : cout << 0;
    }
    cout << '\n';
}

/**
 * Function to check if llr from SISO1 is EQ to llr from SISO2
 * @param llr1 - llr from SISO1
 * @param llr2 - llr from SISO2
 * @return true if EQ/ false if nEQ
 */
bool checkIterSkip(OneDimension llr1, OneDimension llr2){
    vector<int> llr1I, llr2I;
    for (unsigned int i = 0; i < llr1.size(); ++i) {
        llr1I.push_back(llr1[i] >= 0 ? 1 : 0);
        llr2I.push_back(llr2[i] >= 0 ? 1 : 0);
        if (llr1I[i] != llr2I[i]){
            return false;
        }
    }
    return true;
}

/**
 * Calculating sum of two 1D vectors before Interleaving/Deinterleaving for new La vector of values
 * @param llr1 - llr from SISO1 or SISO2
 * @param llr2 - llr from SISO1 or SISO2
 * @return Sum of two 1D vectors (depending on order, bcs sum actually stands for - in this operation)
 */
OneDimension plus1D(OneDimension llr1, OneDimension llr2){
    OneDimension llr(llr1.size(), 0);
    for (unsigned int i = 0; i < llr.size(); ++i) {
        llr[i] = llr1[i] - llr2[i];
    }
    return llr;
}

/**
 * Deinterleaver for 1D data (llr vector)
 * @param llr - llr vector from SISO1/SISO2
 * @param interLeaver - interleaver to change order of calculated values
 * @return - changed La vector
 */
OneDimension deInterleave1D(OneDimension llr, vector<int> interLeaver){
    OneDimension llrNew(llr.size(), 0);
    for (unsigned int i = 0; i < llr.size(); ++i) {
        if(i < interLeaver.size()){
            llrNew[interLeaver[i]] = llr[i];
        }else{
            llrNew[i] = 0;
        }
    }
    return llrNew;
}

/**
 * Interleaver for 1D data (llr vector)
 * @param llr - llr vector from SISO1/SISO2
 * @param interLeaver - interleaver to change order of calculated values
 * @return - changed La vector
 */
OneDimension interleave1D(OneDimension llr, vector<int> interLeaver){
    OneDimension llrNew(llr.size(), 0);
    for (unsigned int i = 0; i < llr.size(); ++i) {
        if(i < interLeaver.size()){
            llrNew[i] = llr[interLeaver[i]];
        }else{
            llrNew[i] = 0;
        }
    }
    return llrNew;
}

/**
 * Interleaver for input data Y for SISO decoder no.2
 * @param data - input data
 * @param interLeaver - interleaver to change order of calculated values
 * @return - changed Y vector
 */
TwoDimensions interleaved(TwoDimensions data, vector<int> interLeaver){
    TwoDimensions dataInter(interLeaver.size()+2, OneDimension(3,0));
    for (unsigned int i = 0; i < data.size(); ++i) {
        if(i<interLeaver.size()){
            dataInter[i][0] = data[interLeaver[i]][0];
            dataInter[i][1] = data[i][0];
        }else{
            dataInter[i][0] = data[i][0];
            dataInter[i][1] = data[i][0];
        }
    }
    return dataInter;
}

