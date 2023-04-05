//
// Author:   Peter Grofcik, xgrofc00
// Project:  Vigenere cipher password finder (KRY)
//

#include "kasisk.h"

using namespace std;

/*
 * This approach is relevant with small password lengths <= 41, (might be different based on length of input text, I used around 10k characters)
 * everything above some limit that most likely ends up not computing divider correctly,
 * because there are many differences to find a correct most frequent divider
 */

/*
 * Function to find positions of every occurrence of provided substring
 */
vector<std::size_t> findOccurrences(string input, string search){
    vector<std::size_t> occurrences;
    size_t pos = input.find(search,0);
    while(pos != string::npos){
        occurrences.push_back(pos);
        pos = input.find(search,pos+1);
    }
    return  occurrences;
}

/*
 * Finding biggest divider for all offered differences in a form of a vector
 * Max possible divider is a value of max element from provided differences
 */
size_t findDivider(vector<size_t> differences){
    unsigned int divider = 1;

    for (unsigned int i = 2; i <= *max_element(differences.begin(), differences.end()); ++i) {
        bool isDivider = true;
        for (unsigned int j = 0; j < differences.size(); ++j) {
            if(differences[j] % i != 0){
                isDivider = false;
            }
        }
        if(isDivider){
            divider = i;
        }
    }
    return divider;
}

/*
 * Functions searches for each trigram+ that is repeated in a input and calculates differences between each occurrence of it
 * and simply returns most frequent biggest mutual divider computed by findDivider
 */
unsigned int kasis (string input) {

    string found;
    vector<std::string> searched;
    size_t dividers[input.length()];

    for (unsigned int i = 0; i < input.length(); ++i) {
        dividers[i] = 0;
    }

    for (unsigned int i = 0; i < input.length(); ++i) {
        unsigned int k = 3;
        string search = input.substr(i, k);
        string end = input.substr(i + k);
        size_t pos = 1;
        while(pos != string::npos){
            search = input.substr(i, k);
            end = input.substr(i + k);
            pos = end.find(search,0);
            k++;
        }
        search = input.substr(i, k - 1);

        if(!std::count(searched.begin(), searched.end(), search)){
            searched.push_back(search);
            vector<std::size_t> newOC = findOccurrences(input,search);
            vector<std::size_t> differences;
            if (newOC.size() > 2){
                for (unsigned int j = 1; j < newOC.size(); ++j) {
                    differences.push_back((newOC[j] - newOC[j-1]));
                }
                dividers[findDivider(differences)]++;
            }
        }
        if((i+k+3) >= input.length()){
            break;
        }else{
            i += k;
        }
    }

    size_t MFDivider = 0;
    unsigned int divider = 0;
    for (unsigned int i = 5; i < input.length(); ++i) {
        if (MFDivider <= dividers[i]){
            MFDivider = dividers[i];
            divider = i;
        }
    }

    return divider;
}