//
// Author:   Peter Grofcik, xgrofc00
// Project:  Vigenere cipher password finder (KRY)
//

#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm>

#ifndef KRY_KASISK_H
#define KRY_KASISK_H

using namespace std;

vector<std::size_t> findOccurrences(string input, string search);
size_t findDivider(vector<size_t> differences);
unsigned int kasis(std::string input);

#endif //KRY_KASISK_H
