// Name: Peter Grofčík
// Project: Turbo coder/decoder for BMS
// Date: 24.11.2021

#include "coder.h"

using namespace std;

/**
 * Function implementing RSC functionality -> expecting:
 * @param rsc - old RCS register values in a form of vector<int>
 * @param data - whole data vector
 * @param i - index for input data
 * @return
 */
vector<int> getRSC(vector<int> rsc, vector<int> data, unsigned int i){
    vector<int> newRSC;
    newRSC.push_back(data[i]);
    newRSC.push_back(rsc[0] xor rsc[2]);
    newRSC.push_back(rsc[1]);
    return newRSC;
}

/**
 * Encode function that is cycling over input data, preparing input for new RSC values and assembling encoded output
 * @param data - input data
 * @param interLeaver - interleaver - vector with values to change order of data
 */
void encode(vector<int> data, vector<int> interLeaver){
    vector<int> dataInter;
    //interleaving for second RSC
    for (unsigned int i = 0; i < interLeaver.size(); ++i) {
        dataInter.push_back(data[interLeaver[i]]);
    }

    vector<int> rsc1, rsc2;
    for (int i = 0; i < 3; ++i) {
        rsc1.push_back(0);
        rsc2.push_back(0);
    }

    vector<int> output;
    vector<int> dataClear = {0,0};
    vector<int> dataClearOutput;
    //main cycle to compute encoded data using 2 RSC for input*3 -> output
    for (unsigned int i = 0; i < data.size(); ++i) {
        output.push_back(data[i]);
        rsc1 = getRSC(rsc1, data, i);
        output.push_back(rsc1[0] xor rsc1[2]);
        if (i+2 >= data.size()){
            dataClearOutput.push_back(rsc1[0] xor rsc1[2]);
        }
        rsc2 = getRSC(rsc2, dataInter, i);
        output.push_back(rsc2[0] xor rsc2[2]);
    }
    //register cleaning cycle for both RSC 2*3 -> output
    for (unsigned int i = 0; i < dataClear.size(); ++i) {
        output.push_back(dataClearOutput[i]);
        rsc1 = getRSC(rsc1, dataClear, i);
        output.push_back(rsc1[0] xor rsc1[2]);
        rsc2 = getRSC(rsc2, dataClear, i);
        output.push_back(rsc2[0] xor rsc2[2]);
    }

    for (unsigned int i = 0; i < output.size(); ++i) {
        cout << output[i];
    }
    cout << '\n';

}