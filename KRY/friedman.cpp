//
// Author:   Peter Grofcik, xgrofc00
// Project:  Vigenere cipher password finder (KRY)
//

#include "friedman.h"

using namespace std;

/*
 * As well as kasisky with bigger password this gets more and more inaccurate,
 * as this approach stands only for approximate length of password -> it's output is only nearing the value of
 * a real length from the very beginning (small passwords) and gets more inaccurate as password length gets bigger,
 * but its IC (index of coincidence) is a good base for computation of real length when used properly
 */

float getIndexOfCoin(string input){
    unsigned int count[26] = { 0 };
    string alphabet = "abcdefghijklmnopqrstuvwxyz";

    for (unsigned int i = 0; i < alphabet.length(); ++i) {
        for (unsigned int j = 0; j < input.length(); ++j) {
            if (input[j] == alphabet[i]){
                count[i] = count[i] + 1;
            }
        }
    }

    float sum = 0.0;
    for (unsigned int i = 0; i < alphabet.length(); ++i) {
        if(count[i]){
            sum += count[i]*(count[i]-1.0);
        }
    }

    return sum/(input.length()*(input.length()-1.0));
}

float friedman(string input){
    return (0.0655 - (1.0/26.0))/(getIndexOfCoin(input) - (1.0/26.0));
}