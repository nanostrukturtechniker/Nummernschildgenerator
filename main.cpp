#include <iostream>
#include <string>
#include <fstream>
#include "boost/program_options.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


using namespace std;
using namespace cv;
namespace po = boost::program_options;

class Character{
    public:
	char character;
	string filename;
	bool umlaut;
	bool number;
	bool allowedAsFirstCharacter;
	Mat imgScaled;
};


vector<Character> characters;

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


double scale = 0.1;

int main(int argc, char **argv) {
    
    string characterFilePath;
    string PNGFilePath;
    string zulassungsbezirkFilePath;
    string configFilePath;
    
    int i=-1;
   
    po::options_description options("Allowed options");
    options.add_options()
    ("help", "produce help message")
    ("characterFile", po::value<string>(&characterFilePath)->required(), "sets the path to the file containing the informations about the characters.")
    ("pathToPNGs", po::value<string>(&PNGFilePath)->required(), "sets the path to the PNG files.")
    ("zulassungsbezirkFile", po::value<string>(&zulassungsbezirkFilePath)->required(), "sets the path to the file containing the informations about the Zulassungsbezirke.")
    ("config,c", po::value<string>(&configFilePath)->default_value("config.ini"), "name of the configuration file")
    ("test,t", po::value<int>(&i)->default_value(666), "Test int")
    ;

    po::options_description config_file_options;
    config_file_options.add(options);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, options), vm);

    //We check for help first, to avoid exceptions of required parameters
    if (vm.count("help"))
    {
	std::cout << options << "\n";
	return false;
    }

   
    //Check if config file is readable
    ifstream ifs( vm["config"].as<string>().c_str());
    if (ifs)  po::store(po::parse_config_file(ifs, config_file_options), vm);

    po::notify(vm);    
    
    ifstream characterFile(characterFilePath.c_str());
    ifstream zulassungsbezirkFile(zulassungsbezirkFilePath.c_str());
    
    
    
    string s;	
    istringstream iss(); 
    string line;
    while(!characterFile.eof())
    {
	Character actChar;

	getline(characterFile, line);

	characterFile >> s;					//Read in the character
	actChar.character = s.c_str()[0];
	characterFile >> s;					//Read in the filename
	actChar.filename =  PNGFilePath + s;
	characterFile >> s;					//Read in the umlaut
	actChar.umlaut = (s.compare("true")) ? true : false;
	characterFile >> s;					//Read in the number
	actChar.number = (s.compare("true")) ? true : false;
	characterFile >> s;					//Read in the allowed as first char
	actChar.allowedAsFirstCharacter = (s.compare("true")) ? true : false;
	
	//cout << actChar.character<<endl;
	
	//Read in the file and scale it
	actChar.imgScaled = imread(actChar.filename);
	resize(actChar.imgScaled, actChar.imgScaled,Size(0,0),scale,scale,CV_INTER_CUBIC);
	characters.push_back(actChar);
	
    }
    
    return 0;
}
