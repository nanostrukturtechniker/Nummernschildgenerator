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
vector<string> zulassungsbezirke;


//All sizes are in tenth of mm
const int widthNumberplate=5200;
const int heightNumberplate=1100;
const int heightCharacters=750;
const int witdhCharacters=405;
const int heightNumbers=750;
const int widthNumbers=385;

const int startOfFirstCharX=540;
const int distOfCharsX=90;

const int widthDienstsiegel=655;
const int widthSpacer=270;

const int maxCharactersOnPlate=7;

double scale = 0.1;


string getCharWithoutUmlaut()
{
    Character actChar;
    do{
	actChar = characters[rand()%characters.size()];
    }while (actChar.umlaut||actChar.number);
    return string(1,actChar.character);
}

string getNumber(bool firstNumber=false)
{
    Character actNumber;
    do{
	do{
	    actNumber = characters[rand()%characters.size()];
	}while (actNumber.number==false);
    }while((actNumber.allowedAsFirstCharacter==false)&&(firstNumber));
    return string(1,actNumber.character);
}


string getErkennungsnummer()
{
    string actErkennungsnummer;
    if (rand()%2==0) // one character
    {
	actErkennungsnummer = getCharWithoutUmlaut();
    }else{//Two characters
	actErkennungsnummer = getCharWithoutUmlaut();
	actErkennungsnummer += getCharWithoutUmlaut();
    }
    return actErkennungsnummer;
}


string getErkennungsziffer(int maxLength)
{
    //Get the max length and limit it to 4.
    maxLength=rand()%maxLength;
    maxLength=(maxLength>4)? 4: maxLength;

    string actErkennungsziffer=getNumber(true);
    for(int i=1;i<maxLength;i++) actErkennungsziffer+=getNumber(false);
    return actErkennungsziffer;
}


void copyMatToPosXY(Mat bigImage, Mat smallImage, int x, int y)
{
    Rect roi( Point( x, y ), cv::Size( smallImage.cols, smallImage.rows));
    Mat destinationROI = bigImage( roi );
    smallImage.copyTo( destinationROI );
}


Mat getBitmapForChar(char c)
{
    for (int i=0; i<characters.size();i++)
    {
	if (c==characters[i].character) return characters[i].imgScaled;
    }
}

int main(int argc, char **argv) {
    
    string characterFilePath;
    string PNGFilePath;
    string zulassungsbezirkFilePath;
    string configFilePath;
    string backgroundImgPath;
    string imagesPath;
  
    int images=-1;
   
    po::options_description options("Allowed options");
    options.add_options()
    ("help", "produce help message")
    ("characterFile", po::value<string>(&characterFilePath)->required(), "sets the path to the file containing the informations about the characters.")
    ("pathToPNGs", po::value<string>(&PNGFilePath)->required(), "sets the path to the PNG files.")
    ("zulassungsbezirkFile", po::value<string>(&zulassungsbezirkFilePath)->required(), "sets the path to the file containing the informations about the Zulassungsbezirke.")
    ("config,c", po::value<string>(&configFilePath)->default_value("config.ini"), "name of the configuration file")
    ("background,b", po::value<string>(&backgroundImgPath)->required(), "The image used as background, normally containing an empty licence plate.")
    ("imageFolder", po::value<string>(&imagesPath)->required(), "The image used as background, normally containing an empty licence plate.")
    ("images,i", po::value<int>(&images)->required(), "The amount of images to be produced.")
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
    
    //Read in all possible characters, empty lines are allowed
    string s;	
    string line;
    while(!characterFile.eof())
    {
	Character actChar;
	//Get the new line and split it via the stringstream
	getline(characterFile, line);
	istringstream iss(line);
	//Skip if the line is too short for anything
	iss >> s;					//Read in the character
	actChar.character = s.c_str()[0];
	iss >> s;					//Read in the filename
	actChar.filename =  PNGFilePath + s;
	iss >> s;					//Read in the umlaut
	actChar.umlaut = (s.compare("true")==0);
	iss >> s;					//Read in the number
	actChar.number = (s.compare("true")==0);
	iss >> s;					//Read in the allowed as first char
	actChar.allowedAsFirstCharacter = (s.compare("true")==0);

	//Read in the file and scale it
	actChar.imgScaled = imread(actChar.filename);
	//Resize and add only if read was successfull
	if (actChar.imgScaled.cols>0)
	{
	    //Check if number or notify
	    if (actChar.number)
	    {
		resize(actChar.imgScaled, actChar.imgScaled,Size(scale*widthNumbers,scale*heightNumbers));
	    }else{
		resize(actChar.imgScaled, actChar.imgScaled,Size(scale*witdhCharacters,scale*heightCharacters));
	    }
	    characters.push_back(actChar);
	}

    }
    
 
    ifstream zulassungsbezirkFile(zulassungsbezirkFilePath.c_str());
  
    //Read in the Zulassungsbezirke, They only contain a short string
    while(!zulassungsbezirkFile.eof())
    {
	Character actChar;
	//Same as above
	getline(zulassungsbezirkFile, line);
	//Skip if the line is too short for anything
	if (line.length()>=1)
	{
	    istringstream iss(line);
	    iss >> s;			//Read in the Zulassungsbezirk, skip the rest.
	    zulassungsbezirke.push_back(s);
	}
    }
    
    
    
    
    //Now, we should have everything and we can open our backgrund image and scale it.
    Mat imgBackground = imread(backgroundImgPath);
    resize(imgBackground,imgBackground,Size(scale*widthNumberplate,scale*heightNumberplate));
    
    
    //initialize Random numbers
    srand( time(0) );
    
    //Create each Numberplate
    for(int i=0;i<images;i++)
    {
	//Get the Zulassungsbezirk
	string actZulassungsbezirk = zulassungsbezirke[rand() % zulassungsbezirke.size()];
	string actErkennungsnummer = getErkennungsnummer();
	int maxPossibleNumberDigits = maxCharactersOnPlate-actZulassungsbezirk.length()-actErkennungsnummer.length();
	string actErkennungsziffer = getErkennungsziffer(maxPossibleNumberDigits);
	
	string actPlate = actZulassungsbezirk+"-"+actErkennungsnummer+actErkennungsziffer;
	
	cout << actPlate<<endl;
	
	//OK, we have now everything we need and we can build up the Image
	Mat img = imgBackground.clone();
	
	//Put the Zulassungsbezirk on it
	int xCoordinate=45 + 80 + 450;  //Border + EU Bar + Space
	int yCoordinate=45 + 130 ;  //black border + distance
	
	//First Letter
	copyMatToPosXY(img,getBitmapForChar(actZulassungsbezirk.c_str()[0]),xCoordinate*scale,yCoordinate*scale);
	
	//Second letter
	if (actZulassungsbezirk.size()>1)
	{
	    xCoordinate+=485;
	    copyMatToPosXY(img,getBitmapForChar(actZulassungsbezirk.c_str()[1]),xCoordinate*scale,yCoordinate*scale);
	}

	//Third letter
	if (actZulassungsbezirk.size()>2)
	{
	    xCoordinate+=485;
	    copyMatToPosXY(img,getBitmapForChar(actZulassungsbezirk.c_str()[2]),xCoordinate*scale,yCoordinate*scale);
	}
	
	//Spacer for Stamps
	xCoordinate+=485+655;
	
	
	//Erkennungsnummer
	//First Letter
	copyMatToPosXY(img,getBitmapForChar(actErkennungsnummer.c_str()[0]),xCoordinate*scale,yCoordinate*scale);

	//Second letter
	if (actErkennungsnummer.size()>1)
	{
	    xCoordinate+=485;
	    copyMatToPosXY(img,getBitmapForChar(actErkennungsnummer.c_str()[1]),xCoordinate*scale,yCoordinate*scale);
	}
	
	
	//Spacer between chars and digits
	xCoordinate+=485 + 260;
	
	//Numbers are right aligned, so we have to calculate the position first
	xCoordinate+=455*(maxPossibleNumberDigits-actErkennungsziffer.size());
	
	//First Digit
	copyMatToPosXY(img,getBitmapForChar(actErkennungsziffer.c_str()[0]),xCoordinate*scale,yCoordinate*scale);
	
	//Second digit
	if (actErkennungsziffer.size()>1)
	{
	    xCoordinate+=455;
	    copyMatToPosXY(img,getBitmapForChar(actErkennungsziffer.c_str()[1]),xCoordinate*scale,yCoordinate*scale);
	}
	
	//Third digit
	if (actErkennungsziffer.size()>2)
	{
	    xCoordinate+=455;
	    copyMatToPosXY(img,getBitmapForChar(actErkennungsziffer.c_str()[2]),xCoordinate*scale,yCoordinate*scale);
	}
	
	
	imshow("LP", img);
	waitKey(50);
	
    }
    waitKey();
    return 0;
}
