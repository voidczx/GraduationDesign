#ifndef HUFFMANENCODING_H
#define HUFFMANENCODING_H

#include "unordered_map"
#include "stdint.h"
#include "string"
#include "vector"
#include "deque"

class HuffmanEncoding
{

public:

    enum ProcessType{
        ENone = 0,
        EAdd = 1,
        ERemove = 2,
    };

    struct FrequencyInfo{

        FrequencyInfo() : Value(-1), Next(std::string()) {}

        int32_t Value;
        std::string Next;
    };

    struct Process{

        Process() : Type(ProcessType::ENone), Str(std::string()), Frequency(-1) {}

        ProcessType Type;
        std::string Str;
        int32_t Frequency;
    };

public:

    HuffmanEncoding();

    const std::vector<Process> GenerateFrequencyMap(const std::string& InStr);
    const std::vector<Process> StepForward();
    const std::vector<Process> StepBack();


private:

    bool RemoveFrequency(const std::string& InStr);
    bool AddFrequency(const std::string& InStr, const int32_t& InFrequency);

    std::unordered_map<std::string, FrequencyInfo> FrequencyMap;
    std::deque<Process> ProcessStack;

};

#endif // HUFFMANENCODING_H
