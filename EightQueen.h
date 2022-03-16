#ifndef EIGHTQUEEN_H
#define EIGHTQUEEN_H

#include "vector"
#include "deque"
#include "stdint.h"

struct QueenChess;

class EightQueen{

public:

    EightQueen(const uint8_t& MapSize);

    bool IsPositionValid(const uint8_t& Row, const uint8_t& Col) const;
    bool TryAddQueenChess_Manually(const uint8_t& Row, const uint8_t& Col);
    bool TryAddQueenChess_Auto();
    bool TryReduceQueenChess_Manually();
    bool TryReduceQueenChess_Auto();

    bool IsSuccess() const { return bSuccess; }
    bool IsFail() const { return bFail; }

private:

    bool TryAddQueenChess(const uint8_t& Row, const uint8_t& Col);
    bool TryReduceQueenChess();

    void AfterAddQueenChess();
    void BeforeReduceQueenChess();

    std::vector<std::vector<bool>> Map;
    std::deque<QueenChess> ChessArray;

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
