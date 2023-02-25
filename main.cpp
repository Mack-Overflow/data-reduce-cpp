#include "util.cpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <voltage_threshold>" << std::endl;
        return 1;
    }

    std::string inputFileName = argv[1];
    double voltageThreshold = std::stod(argv[2]);

    smoothData(inputFileName, "smoothed.dat");
    detectPulses("smoothed.dat", "pulses.dat", voltageThreshold);

    return 0;
}