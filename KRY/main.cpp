//
// Author:   Peter Grofcik, xgrofc00
// Project:  Vigenere cipher password finder (KRY)
//

#include <iostream>
#include <string.h>
#include "friedman.h"
#include "kasisk.h"
#include <chrono>

using namespace std;

/*
 * Frequency of english alphabet in a alphabetic order (a...z)
 */
float EnglishFrequency[] ={
        0.08167, 0.01492, 0.02782, 0.04253, 0.12702,
        0.02228, 0.02015, 0.06094, 0.06966, 0.00153,
        0.00772, 0.04025, 0.02406, 0.06749, 0.07507,
        0.01929, 0.00095, 0.05987, 0.06327, 0.09056,
        0.02758, 0.00978, 0.02360, 0.00150, 0.01974,
        0.00074,
};

/*
 * Load input offered and stores it as a simple string ignoring white space and special characters. (selecting only alphabet)
 */
std::string load_input(){
    char c;
    string loaded;
    while(cin.get(c)){
        if(isalpha(c)){
            loaded += tolower(c);
        }
    }
    return loaded;
}

/*
 * Function to find real length of password
 * As base it uses IC used in friedman computation as well as approximate length computed by friedman
 * Search of length is between 2 and 10*friedman length, because in extreme cases i tested friedman ended up with
 * length of +-10 with password over a hundred or even length of +- 450 when used password "abcdefghijklmnopqrstuvwxyz" (length 26)
 * As from forum info 2 - 200 should be more then enough for testing
 * Function itself regroups characters to blocks and computes IC for each block (with addition of comparing it to English IC)
 * after computation ends it returns length that had minimal AVG IC (computed password length)
 * */
unsigned int find_real(string input, unsigned int keyLenAprox){

    unsigned int finalLength = 0;
    float minCIndex = 1.0;

    for (unsigned int i = 2; i < keyLenAprox*10; ++i) {

        string blocks[i];
        float sum = 0.0;
        float avg;

        for (unsigned int j = 0; j < input.length(); ++j) {
            blocks[j % i] += input[j];
        }

        for (unsigned int j = 0; j < i; ++j) {
            sum += abs(getIndexOfCoin(blocks[j]) - 0.0655); //compare with a english IC
        }

        avg = sum/i;
        if(minCIndex > avg){
            minCIndex = avg;
            finalLength = i;
        }
    }
    return finalLength;
}

/*
 * Computes chi-squared test output based on frequency of letters in provided input compared to frequency of letters in english alphabet
 */
float getCosetIndex(string input){
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
            sum += (((float)count[i]/input.length() - EnglishFrequency[i]) * ((float)count[i]/input.length() - EnglishFrequency[i]))/EnglishFrequency[i];
        }
    }
    return sum;
}

/*
 * Left shift for alphabet to rotate input string (simulates rotation in Caesar cipher)
 */
std::string leftShift(string input){
    string shifted;
    int a;
    for (unsigned int i = 0; i < input.length(); ++i) {
        a = input[i];
        if(a == 97){
            a = 122;
        }else{
            a--;
        }
        shifted += a;
    }
    return shifted;
}

/*
 * Regroups input to blocks of count of computed length of password
 * Each block is shifted *alphabet to find lowest IC compared to English language
 * (meaning IC closest to English language)
 * Then a number of shifts required is the number of right shifts used to encrypt open text (that number stands for a single character from alphabet)
 * Final character == number is one char from password used to encrypt whole input
 */
std::string crackPass(string input, unsigned int lengthOfPass){
    string blocks[lengthOfPass];
    string alphabet = "abcdefghijklmnopqrstuvwxyz";
    string password;
    for (unsigned int j = 0; j < input.length(); ++j) {
        blocks[j % lengthOfPass] += input[j];
    }

    for (unsigned int i = 0; i < lengthOfPass; ++i) {
        string shifted = blocks[i];
        float indexC = getCosetIndex(shifted);
        unsigned int shiftNum = 0;
        for (unsigned int j = 1; j < 26; ++j) {
            shifted = leftShift(shifted);
            float indexCN = getCosetIndex(shifted);
            if(indexCN < indexC){
                indexC = indexCN;
                shiftNum = j;
            }
        }

        password += alphabet[shiftNum];
    }
    return password;
}

int main() {

//    auto p1 = std::chrono::high_resolution_clock::now();

    string input = load_input();

    float friedmanL = friedman(input);
    unsigned int kasisL = kasis(input);
    unsigned int findReal = find_real(input, (unsigned int)friedmanL);
    string password = crackPass(input, findReal);

    cout    << friedmanL << ';'
            << kasisL << ';'
            << findReal << ';'
            << password << '\n';

//    auto p2 = std::chrono::high_resolution_clock::now();

//    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(p2 - p1);
//    cout << ms_int.count() << '\n';

    return 0;
}
