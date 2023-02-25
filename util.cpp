#include <iostream>
#include <fstream>
#include <vector>

void smoothData(const std::string& inputFileName, const std::string& outputFileName)
{
    // open the input file for reading
    std::ifstream inputFile(inputFileName);
    if (!inputFile)
    {
        std::cerr << "Failed to open input file " << inputFileName << std::endl;
        return;
    }

    // read the data from the input file
    std::vector<double> data;
    double value;
    while (inputFile >> value)
    {
        data.push_back(value);
    }

    // open the output file for writing
    std::ofstream outputFile(outputFileName);
    if (!outputFile)
    {
        std::cerr << "Failed to open output file " << outputFileName << std::endl;
        return;
    }

    // smooth the data
    for (int i = 0; i < 3; ++i)
    {
        // copy the first 3 numbers from the original, negated data
        outputFile << -data[i] << std::endl;
    }

    for (int i = 3; i < data.size() - 3; ++i)
    {
        // apply the weighted average
        double smoothedValue = (data[i-3] + 2*data[i-2] + 3*data[i-1] + 3*data[i] + 3*data[i+1] + 2*data[i+2] + data[i+3]) / 15.0;
        outputFile << smoothedValue << std::endl;
    }

    for (int i = data.size() - 3; i < data.size(); ++i)
    {
        // copy the last 3 numbers from the original, negated data
        outputFile << -data[i] << std::endl;
    }
}

void detectPulses(const std::string& inputFileName, const std::string& outputFileName, double voltageThreshold)
{
    // open the input file for reading
    std::ifstream inputFile(inputFileName);
    if (!inputFile)
    {
        std::cerr << "Failed to open input file " << inputFileName << std::endl;
        return;
    }

    // read the data from the input file
    std::vector<double> data;
    double value;
    while (inputFile >> value)
    {
        data.push_back(value);
    }

    // open the output file for writing
    std::ofstream outputFile(outputFileName);
    if (!outputFile)
    {
        std::cerr << "Failed to open output file " << outputFileName << std::endl;
        return;
    }

    // detect pulses in the data
    bool inPulse = false;
    for (int i = 0; i < data.size() - 2; ++i)
    {
        double rise = data[i+2] - data[i];
        if (rise > voltageThreshold)
        {
            if (!inPulse)
            {
                // start of a new pulse
                inPulse = true;
                outputFile << i << std::endl;
            }
        }
        else if (inPulse)
        {
            // end of the current pulse
            inPulse = false;
            int j = i + 2;
            while (j < data.size() && data[j] >= data[j-1])
            {
                ++j;
            }
            outputFile << j-2 << std::endl;
        }
    }
}
