#ifndef HUFFMANENCODING_H
#define HUFFMANENCODING_H

#include "unordered_map"
#include "stdint.h"
#include "string"
#include "vector"
#include "deque"
#include "memory"

class HuffmanEncoding
{

public:

    enum class ProcessClass{
        ENone = 0,
        ETreeNode = 1,
        EBinary = 2,
    };

    enum class ProcessType{
        ENone = 0,
        EAdd = 1,
        ERemove = 2,
    };

    struct HuffmanTreeNode{

        HuffmanTreeNode() : Word(), Parent(nullptr), LeftChild(nullptr), RightChild(nullptr), EncodingResult() {}

        std::string Word;
        HuffmanTreeNode* Parent;
        HuffmanTreeNode* LeftChild;
        HuffmanTreeNode* RightChild;
        std::string EncodingResult;
    };

    struct FrequencyListNode{

        FrequencyListNode() : Word(), Value(-1), Next(nullptr), Front(nullptr) {}

        std::string Word;
        int32_t Value;
        FrequencyListNode* Next;
        FrequencyListNode* Front;
        HuffmanTreeNode* TreeNode;
    };

    struct Process{

        Process() : Class(ProcessClass::ENone) {}
        Process(ProcessClass InClass) : Class(InClass) {}

        ProcessClass Class;
    };

    struct TreeNodeProcess : public Process{

        TreeNodeProcess() : Process(ProcessClass::ETreeNode), Type(ProcessType::ENone), Str(), Frequency(-1) {}

        ProcessType Type;
        std::string Str;
        int32_t Frequency;
    };

    struct BinaryProcess : public Process{

        BinaryProcess() : Process(ProcessClass::EBinary), Type(ProcessType::ENone), StrSource(), StrTarget(), Value() {}

        ProcessType Type;
        std::string StrSource;
        std::string StrTarget;
        std::string Value;
    };

public:

    HuffmanEncoding();
    ~HuffmanEncoding();

    bool IsTreeBuildCompleted() const { return bTreeBuildCompleted; }
    bool IsBinaryBuildCompleted() const { return bBinaryBuildCompleted; }
    std::string GetEncodingResult(const std::string& InStr) { return EncodingMap.count(InStr) > 0 ? EncodingMap[InStr] : ""; }

    void ClearAll();

    const std::unordered_map<char, int32_t> GenerateFrequencyMap(const std::string& InStr);
    const std::vector<std::shared_ptr<Process>> StepForward();
    const std::vector<std::shared_ptr<Process>> StepBack();


private:

    bool RemoveFrequencyNode(const std::string& InStr);
    bool AddFrequencyNode(const std::string& InStr, const int32_t& InFrequency);

    bool DeleteFrequencyList(FrequencyListNode* InDeletedNode);
    bool DeleteHuffmanTree(HuffmanTreeNode* InDeletedNode);

    FrequencyListNode* Start;
    HuffmanTreeNode* Root;
    HuffmanTreeNode* TreeIter;
    std::deque<std::vector<std::shared_ptr<Process>>> ProcessStack;
    std::unordered_map<std::string, std::string> EncodingMap;
    std::unordered_map<std::string, HuffmanTreeNode*> TreeNodeMap;
    bool bTreeBuildCompleted = false;
    bool bBinaryBuildCompleted = false;

};

#endif // HUFFMANENCODING_H
