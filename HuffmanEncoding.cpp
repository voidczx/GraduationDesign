#include "HuffmanEncoding.h"

#include "qdebug.h"

HuffmanEncoding::HuffmanEncoding()
{
    Start = new FrequencyListNode();
    Root = nullptr;
    TreeIter = nullptr;
}

HuffmanEncoding::~HuffmanEncoding(){
    DeleteFrequencyList(Start);
    Start = nullptr;
    if (Root != nullptr){
        DeleteHuffmanTree(Root);
        Root = nullptr;
    }
    else{
        for (auto Iter = TreeNodeMap.begin(); Iter != TreeNodeMap.end(); Iter++){
            delete Iter->second;
            Iter->second = nullptr;
        }
    }
    TreeNodeMap.clear();
    TreeIter = nullptr;
}

void HuffmanEncoding::ClearAll(){
    DeleteFrequencyList(Start);
    Start = new FrequencyListNode();
    if (Root != nullptr){
        DeleteHuffmanTree(Root);
        Root = nullptr;
    }
    else{
        for (auto Iter = TreeNodeMap.begin(); Iter != TreeNodeMap.end(); Iter++){
            delete Iter->second;
            Iter->second = nullptr;
        }
    }
    TreeNodeMap.clear();
    TreeIter = nullptr;
    ProcessStack.clear();
    EncodingMap.clear();
    bTreeBuildCompleted = false;
    bBinaryBuildCompleted = false;
}

const std::unordered_map<char, int32_t> HuffmanEncoding::GenerateFrequencyMap(const std::string& InStr){

    std::unordered_map<char, int32_t> Output;
    for (const char& Ch : InStr){
        if (Output.count(Ch) == 0){
            Output.insert(std::pair<char, int32_t>(Ch, 1));
        }
        else{
            Output[Ch]++;
        }
    }
    for (const std::pair<const char, int32_t>& Pair : Output){
        std::string Word = "";
        Word.push_back(Pair.first);
        AddFrequencyNode(Word, Pair.second);
    }
    Output.clear();
    FrequencyListNode* FrequencyNode = Start->Next;
    while (FrequencyNode != nullptr){
        Output.emplace(FrequencyNode->Word[0], FrequencyNode->Value);
        FrequencyNode = FrequencyNode->Next;
    }
    return Output;
}

const std::vector<std::shared_ptr<HuffmanEncoding::Process>> HuffmanEncoding::StepForward(){

    std::vector<std::shared_ptr<HuffmanEncoding::Process>> Output;
    if (bBinaryBuildCompleted){
        return Output;
    }

    if (!bTreeBuildCompleted){
        // Tree Node Process
        if (Start->Next == nullptr){
            return Output;
        }

        FrequencyListNode* FirstNode = Start->Next;
        FrequencyListNode* SecondNode = FirstNode->Next;
        if (FirstNode == nullptr || SecondNode == nullptr){
            // Error!
            return Output;
        }

        // Add Process
        std::string AddWord = FirstNode->Word + "/" + SecondNode->Word;
        int32_t AddValue = FirstNode->Value + SecondNode->Value;
        std::shared_ptr<TreeNodeProcess> AddNodeProcess(new TreeNodeProcess());
        AddNodeProcess->Type = ProcessType::EAdd;
        AddNodeProcess->Str = AddWord;
        AddNodeProcess->Frequency = AddValue;
        Output.push_back(AddNodeProcess);

        // Remove Process
        std::shared_ptr<TreeNodeProcess> RemoveFirstNodeProcess(new TreeNodeProcess());
        RemoveFirstNodeProcess->Type = ProcessType::ERemove;
        RemoveFirstNodeProcess->Str = FirstNode->Word;
        Output.push_back(RemoveFirstNodeProcess);
        std::shared_ptr<TreeNodeProcess> RemoveSecondNodeProcess(new TreeNodeProcess());
        RemoveSecondNodeProcess->Type = ProcessType::ERemove;
        RemoveSecondNodeProcess->Str = SecondNode->Word;
        Output.push_back(RemoveSecondNodeProcess);

        // Add Nodes
        AddFrequencyNode(AddWord, AddValue);
        if (TreeNodeMap.count(AddWord) > 0){
            qDebug() << "Error: TreeNodeMap has AddWord " << AddWord.c_str();
        }
        HuffmanTreeNode* Parent = new HuffmanTreeNode();
        Parent->Word = AddWord;
        TreeNodeMap.emplace(AddWord, Parent);
        HuffmanTreeNode* LeftChild = nullptr;
        if (TreeNodeMap.count(FirstNode->Word) > 0){
            LeftChild = TreeNodeMap[FirstNode->Word];
        }
        else{
            LeftChild = new HuffmanTreeNode();
            TreeNodeMap.emplace(FirstNode->Word, LeftChild);
        }
        LeftChild->Word = FirstNode->Word;
        LeftChild->Parent = Parent;
        Parent->LeftChild = LeftChild;
        HuffmanTreeNode* RightChild = nullptr;
        if (TreeNodeMap.count(SecondNode->Word) > 0){
            RightChild = TreeNodeMap[SecondNode->Word];
        }
        else{
            RightChild = new HuffmanTreeNode();
            TreeNodeMap.emplace(SecondNode->Word, RightChild);
        }
        RightChild->Word = SecondNode->Word;
        RightChild->Parent = Parent;
        Parent->RightChild = RightChild;

        // Remove Nodes
        RemoveFrequencyNode(FirstNode->Word);
        FirstNode = nullptr;
        RemoveFrequencyNode(SecondNode->Word);
        SecondNode = nullptr;

        // Judge Completed
        if (Start->Next == nullptr){
            // Error!
            return Output;
        }
        if (Start->Next->Next == nullptr){
            bTreeBuildCompleted = true;
            Root = Parent;
        }
    }
    else{
        // Binary Process
        if (Root == nullptr || Root->LeftChild == nullptr || Root->RightChild == nullptr){
            // Error or Empty
            return Output;
        }

        // Encoding Current Node And Add Process
        if (TreeIter == nullptr){
            TreeIter = Root->LeftChild;
            TreeIter->EncodingResult = "0";
            std::shared_ptr<BinaryProcess> StartBinaryBuildProcess(new BinaryProcess());
            StartBinaryBuildProcess->Type = ProcessType::EAdd;
            StartBinaryBuildProcess->StrSource = Root->Word;
            StartBinaryBuildProcess->StrTarget = TreeIter->Word;
            StartBinaryBuildProcess->Value = "0";
            Output.push_back(StartBinaryBuildProcess);
        }
        else{
            if (TreeIter->Parent == nullptr){
                // Error
                return Output;
            }
            std::shared_ptr<BinaryProcess> AddLineProcess(new BinaryProcess());
            AddLineProcess->Type = ProcessType::EAdd;
            AddLineProcess->StrSource = TreeIter->Parent->Word;
            AddLineProcess->StrTarget = TreeIter->Word;
            if (TreeIter == TreeIter->Parent->LeftChild){
                TreeIter->EncodingResult = TreeIter->Parent->EncodingResult + "0";
                AddLineProcess->Value = "0";
            }
            else{
                TreeIter->EncodingResult = TreeIter->Parent->EncodingResult + "1";
                AddLineProcess->Value = "1";
            }
            Output.push_back(AddLineProcess);
        }

        // Find Next Node
        if (TreeIter->LeftChild != nullptr){
            TreeIter = TreeIter->LeftChild;
        }
        else{
            while (TreeIter->Parent != nullptr){
                TreeIter = TreeIter->Parent;
                if (TreeIter->RightChild != nullptr && TreeIter->RightChild->EncodingResult == ""){
                    TreeIter = TreeIter->RightChild;
                    break;
                }
            }
            if (TreeIter == Root){
                if (Root->RightChild->EncodingResult == ""){
                    TreeIter = Root->RightChild;
                }
                else{
                    bBinaryBuildCompleted = true;
                    TreeIter = nullptr;
                }
            }
        }
        if (bBinaryBuildCompleted){
            for (auto Iter = TreeNodeMap.begin(); Iter != TreeNodeMap.end(); Iter++){
                if (Iter->first.size() == 1){
                    EncodingMap.emplace(Iter->first, Iter->second->EncodingResult);
                }
            }
        }
    }
    ProcessStack.push_back(Output);
    return Output;

}

const std::vector<std::shared_ptr<HuffmanEncoding::Process>> HuffmanEncoding::StepBack(){

    std::vector<std::shared_ptr<HuffmanEncoding::Process>> Output;
    return Output;

}

bool HuffmanEncoding::RemoveFrequencyNode(const std::string& InStr){

    FrequencyListNode* ListNode = Start;
    if (ListNode == nullptr){
        return false;
    }
    while (ListNode != nullptr && ListNode->Word != InStr){
        ListNode = ListNode->Next;
    }
    if (ListNode == nullptr || ListNode->Front == nullptr){
        return false;
    }
    ListNode->Front->Next = ListNode->Next;
    if (ListNode->Next != nullptr){
        ListNode->Next->Front = ListNode->Front;
    }
    ListNode->Front = nullptr;
    ListNode->Next = nullptr;
    delete ListNode;
    return true;
}

bool HuffmanEncoding::AddFrequencyNode(const std::string& InStr, const int32_t& InFrequency){

    FrequencyListNode* ListNode = Start;
    if (ListNode == nullptr){
        return false;
    }
    while (ListNode->Next != nullptr && ListNode->Next->Value < InFrequency){
        ListNode = ListNode->Next;
    }
    FrequencyListNode* NewNode = new FrequencyListNode();
    NewNode->Word = InStr;
    NewNode->Value = InFrequency;
    NewNode->Next = ListNode->Next;
    if (NewNode->Next != nullptr){
        NewNode->Next->Front = NewNode;
    }
    NewNode->Front = ListNode;
    ListNode->Next = NewNode;
    return true;

}

bool HuffmanEncoding::DeleteFrequencyList(FrequencyListNode* InDeletedNode){

    if (InDeletedNode == nullptr){
        return false;
    }
    if (InDeletedNode->Next != nullptr){
        DeleteFrequencyList(InDeletedNode->Next);
    }
    if (InDeletedNode->Front != nullptr){
        InDeletedNode->Front->Next = nullptr;
    }
    delete InDeletedNode;
    return true;

}

bool HuffmanEncoding::DeleteHuffmanTree(HuffmanTreeNode* InDeletedNode){

    if (InDeletedNode == nullptr){
        return false;
    }
    if (InDeletedNode->LeftChild != nullptr){
        DeleteHuffmanTree(InDeletedNode->LeftChild);
    }
    if (InDeletedNode->RightChild != nullptr){
        DeleteHuffmanTree(InDeletedNode->RightChild);
    }
    if (InDeletedNode->Parent != nullptr){
        if (InDeletedNode->Parent->LeftChild == InDeletedNode){
            InDeletedNode->Parent->LeftChild = nullptr;
        }
        else if (InDeletedNode->Parent->RightChild == InDeletedNode){
            InDeletedNode->Parent->RightChild = nullptr;
        }
        else{
            // Error!
        }
    }
    delete InDeletedNode;
    return true;

}

