#ifndef EIGHTQUEEN_H
#define EIGHTQUEEN_H

#include "vector"
#include "deque"
#include "stdint.h"
#include "unordered_map"

class EightQueen{

public:

    enum ProcessType{
        ENone = 0,
        EAdd = 1,
        EReduce = 2,
    };

    struct Process{
        Process() : Type(ProcessType::ENone){}
        Process(const ProcessType& InType) : Type(InType){}

        ProcessType Type;
        int32_t OptionalInt1 = -1;
        int32_t OptionalInt2 = -1;
    };

    struct MapUnit{

        MapUnit() : Row(255), Col(255) {};
        MapUnit(const uint8_t& InRow, const uint8_t& InCol) : Row(InRow), Col(InCol) {};

        uint8_t Row;
        uint8_t Col;
    };

    struct QueenChess{

        QueenChess() : Row(0), Col(0) {};
        QueenChess(const uint8_t& InRow, const uint8_t& InCol) : Row(InRow), Col(InCol) {};

        uint8_t Row;
        uint8_t Col;
        std::vector<MapUnit> EffectUnitArray;
    };

public:

    EightQueen(const uint8_t& MapSize);

    bool IsPositionValid(const uint8_t& Row, const uint8_t& Col) const;
    bool TryAddQueenChess_Manually(const uint8_t& Row, const uint8_t& Col);
    bool TryAddQueenChess_Auto(uint8_t& OutRow, uint8_t& OutCol);
    bool TryReduceQueenChess_Manually();
    bool TryReduceQueenChess_Auto();

    void InitializeAutoState();
    void ClearAutoState();
    void RefreshAutoMap();
    Process StepBack_Auto();

    bool IsSuccess() const { return bSuccess; }
    bool IsFail() const { return bFail; }
    bool IsAutoTotallyFailed(){ return bAutoFail && AutoChessStack.empty(); }
    bool IsAutoFailed() { return bAutoFail; }
    const MapUnit& GetLastAutoReduceUnit() const { return LastAutoReduceUnit; }

private:

    bool TryAddQueenChess(const uint8_t& Row, const uint8_t& Col);
    bool TryReduceQueenChess();

    void AfterAddQueenChess();
    void BeforeReduceQueenChess();

    std::vector<std::vector<bool>> Map;

    std::deque<QueenChess> ChessArray;
    std::unordered_map<uint8_t, uint8_t> AutoMap;
    std::deque<int32_t> AutoChessStack;
    MapUnit LastAutoReduceUnit;
    std::deque<Process> ProcessStack;

    const uint8_t MapSize;
    bool bSuccess = false;
    bool bFail = false;
    bool bAutoFail = false;
};

#endif // EIGHTQUEEN_H
