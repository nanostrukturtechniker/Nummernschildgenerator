#include <iostream>
#include <string>
#include <fstream>
#include "opencv2/core/core.hpp"


using namespace std;
using namespace cv;


class Character{
    char character;
    string filename;
    bool umlaut;
    bool number;
    bool allowedAsFirstCharacter;
};


//All sizes are in tenth of mm
const int widthNumberplate=5200;
const int heightNumberplate=1100;
const int heightCharacters=750;
const int witdhCharacters=405;
const int witdhNumbers=385;

const int startOfFirstCharX=540;
const int distOfCharsX=90;

const int widthDienstsiegel=655;
const int widthSpacer=270;




int main(int argc, char **argv) {
  
    ifstream characterFile(argv[1]);
    string pathToPNGs = argv[2];
    ifstream zulassungsbezirkFile(argv[3]);
    
    
    
    return 0;
}
