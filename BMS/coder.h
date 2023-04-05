// Name: Peter Grofčík
// Project: Turbo coder/decoder for BMS
// Date: 24.11.2021

#ifndef BMS_CODER_H
#define BMS_CODER_H

#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include<iterator>
using namespace std;


vector<int> getRSC(vector<int> rsc, vector<int> data, unsigned int i);
void encode(vector<int> data, vector<int> interLeaver);

#endif //BMS_CODER_H

