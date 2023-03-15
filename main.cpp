#include "util.cpp"
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <cstring>
#include <sstream>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <ini_file>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string iniFile = argv[1];
    Params params = readParams(iniFile);
    

    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(".")) != nullptr)
    {
        while ((ent = readdir(dir)) != nullptr)
        {
            std::string fileName = ent->d_name;
            if (fileName.size() > 4 && fileName.substr(fileName.size() - 4) == ".dat")
            {
                std::string smoothedFileName = fileName.substr(0, fileName.size() - 4) + "_smoothed.dat";
                std::cout << fileName << ":" << std::endl;
                smoothData(fileName, smoothedFileName);
                // std::string pulseFileName = fileName.substr(0, fileName.size() - 4) + "_pulses.dat";
                detectPulses(smoothedFileName, params);
                std::remove(smoothedFileName.c_str());
            }
        }
        closedir(dir);
    }
    else
    {
        std::cerr << "Error: could not open current directory" << std::endl;
        return 1;
    }

    return 0;
}