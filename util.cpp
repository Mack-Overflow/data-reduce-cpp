#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>

struct Params {
    int vt;
    int width;
    int pulse_delta;
    double drop_ratio;
    int below_drop_ratio;
};

Params readParams(const std::string& filename) {
    std::map<std::string, std::string> iniData;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '#') continue; // ignore comments
        std::size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            iniData[key] = value;
        }
    }

    file.close();

    // Check for required parameters
    if (iniData.count("vt") == 0 || iniData.count("width") == 0 || iniData.count("pulse_delta") == 0 || iniData.count("drop_ratio") == 0 || iniData.count("below_drop_ratio") == 0) {
        std::cerr << "Missing required parameter in " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    Params params;
    params.vt = std::stoi(iniData["vt"]);
    params.width = std::stoi(iniData["width"]);
    params.pulse_delta = std::stoi(iniData["pulse_delta"]);
    params.drop_ratio = std::stod(iniData["drop_ratio"]);
    params.below_drop_ratio = std::stoi(iniData["below_drop_ratio"]);
    return params;
}

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
        outputFile << data[i] << std::endl;
    }

    for (int i = 3; i < data.size() - 3; i++)
    {
        // apply the weighted average
        double smoothedValue = (data[i-3] + 2*data[i-2] + 3*data[i-1] + 3*data[i] + 3*data[i+1] + 2*data[i+2] + data[i+3]) / 15.0;
        outputFile << smoothedValue << std::endl;
    }

    for (int i = data.size() - 3; i < data.size(); i++)
    {
        // copy the last 3 numbers from the original, negated data
        outputFile << data[i] << std::endl;
    }
}

void detectPulses(const std::string& inputFileName, Params params)
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

    std::vector<int> pulseStarts;
    std::vector<double> pulseHeights;
    std::vector<int> pulsePeaks;

// Find pulse starts and heights
    int i = 0;
    while (i < data.size() - 2) {
        if (data[i + 2] - data[i] > params.vt) {
            pulseStarts.push_back(i + 2);
            int j = i + 2;
            while (j < data.size() - 1 && data[j + 1] > data[j]) {
                j++;
            }
            pulsePeaks.push_back(j);
            pulseHeights.push_back(data[j] - data[i + 2]);
            i = j;
        } else {
            i++;
        }
    }

    // Check for overlapping pulses and omit pulses that don't meet the drop ratio criterion
    std::vector<int> pulsesToRemove;
    for (int i = 0; i < pulseStarts.size() - 1; i++) {
        int j = i + 1;
        while (j < pulseStarts.size() && pulseStarts[j] - pulseStarts[i] <= params.pulse_delta) {
            double peakHeight = pulseHeights[i];
            int peakPos = pulsePeaks[i];
            int startPos = pulseStarts[i] + 1;
            int endPos = pulseStarts[j];
            int numBelowDropRatio = 0;
            for (int k = startPos; k < endPos; k++) {
                if (data[k] < peakHeight * params.drop_ratio) {
                    numBelowDropRatio++;
                }
            }
            if (numBelowDropRatio > params.below_drop_ratio) {
                pulsesToRemove.push_back(i);
                break;
            }
            j++;
        }
    }

    // Remove pulses that didn't meet the drop ratio criterion
    for (int i = pulsesToRemove.size() - 1; i >= 0; i--) {
        int pulseIndex = pulsesToRemove[i];
        pulseStarts.erase(pulseStarts.begin() + pulseIndex);
        pulseHeights.erase(pulseHeights.begin() + pulseIndex);
        pulsePeaks.erase(pulsePeaks.begin() + pulseIndex);
    }

    // Compute pulse areas
    std::vector<double> pulseAreas;
    for (int i = 0; i < pulseStarts.size(); i++) {
        double area = 0.0;
        int start = pulseStarts[i];
        int end = start + params.width;
        if (end > data.size()) {
            end = data.size();
        }
        for (int j = start; j < end; j++) {
            area += (data[j] - data[start]) / params.width;
        }
        pulseAreas.push_back(area);
    }

    // Print pulse info
    for (int i = 0; i < pulseStarts.size(); i++) {
        std::cout << "Pulse start: " << pulseStarts[i] << ", pulse height: " << pulseHeights[i] << ", pulse area: " << pulseAreas[i] << std::endl;
    }
}

void detectPulsesUsingAlgo(const std::string& inputFileName, Params params) {
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
    
    auto pulseStartIter = std::adjacent_find(data.begin(), data.end(),
    [params](double a, double b) { return (b - a) > params.vt; });
}