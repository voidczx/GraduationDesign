#ifndef EIGHTQUEEN_H
#define EIGHTQUEEN_H

#include "vector"
#include "stdint.h"

struct QueenChess;

class EightQueen{

public:

    EightQueen(const uint8_t& MapSize);


private:

    std::vector<std::vector<bool>> Map;
    std::vector<QueenChess> ChessArray;
};

struct QueenChess{
    uint8_t Row;
    uint8_t Col;
};

#endif // EIGHTQUEEN_H
