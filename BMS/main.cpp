// Name: Peter Grofčík
// Project: Turbo coder/decoder for BMS
// Date: 24.11.2021

#include "coder.h"
#include "decoder.h"

using namespace std;
bool coder = false;
bool decoder = false;

/**
 * Simple main function -> gathering inputs to 1D vectors of type int/double depending on -i -e arg expecting only correct inputs
 */
int main(int argc, char* argv[]) {
    int c;
    vector<int> interLeaver;

    while ((c = getopt(argc, argv, "i:ed")) != -1) {
        switch (c) {
            case 'e':
                coder = true;
                break;
            case 'd':
                decoder = true;
                break;
            case 'i': {
                std::stringstream input(optarg);
                while (input.good()) {
                    string substr;
                    getline(input, substr, ',');
                    interLeaver.push_back(atoi(substr.c_str()));
                }
            }
                break;
            default:
                std::cout << "Unknown parameter" << '\n';
                exit(1);
                break;
        }
    }

    if (coder){
        vector<int> data;
        char n;
        while (cin.get(n)) {
            if ((n - 48) >= 0) {
                data.push_back(n - 48);
            }
        }
        encode(data, interLeaver);
    }else if(decoder){
        vector<double> data;
        while(cin.good()){
            string substr;
            getline(cin, substr, ',');
            data.push_back(atof(substr.c_str()));
        }
        decode(data, interLeaver);
    }
}