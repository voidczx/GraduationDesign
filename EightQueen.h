#ifndef EIGHTQUEEN_H
#define EIGHTQUEEN_H

#include "vector"
#include "deque"
#include "stdint.h"
#include "unordered_map"

struct QueenChess;

class EightQueen{

    enum AutoProcess{
        Add = 0,
        Reduce = 1,
    };

public:

    EightQueen(const uint8_t& MapSize);

    bool IsPositionValid(const uint8_t& Row, const uint8_t& Col) const;
    bool TryAddQueenChess_Manually(const uint8_t& Row, const uint8_t& Col);
    bool TryAddQueenChess_Auto();
    bool TryReduceQueenChess_Manually();
    bool TryReduceQueenChess_Auto();

    void InitializeAutoState();
    void ClearAutoState();
    void StepBack_Auto();

    bool IsSuccess() const { return bSuccess; }
    bool IsFail() const { return bFail; }
    bool IsAutoTotallyFailed(){ return bFail && AutoChessStack.empty(); }

private:

    bool TryAddQueenChess(const uint8_t& Row, const uint8_t& Col);
    bool TryReduceQueenChess();

    void AfterAddQueenChess();
    void BeforeReduceQueenChess();

    std::vector<std::vector<bool>> Map;

    std::deque<QueenChess> ChessArray;
    std::unordered_map<uint8_t, uint8_t> AutoMap;
    std::deque<int32_t> AutoChessStack;
    std::deque<AutoProcess> ProcessStack;

    const uint8_t MapSize;
    bool bSuccess = false;
    bool bFail = false;
};

struct MapUnit{

    MapUnit() : Row(0), Col(0) {};
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

#endif // EIGHTQUEEN_H
