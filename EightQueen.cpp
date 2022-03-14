#include "EightQueen.h"

EightQueen::EightQueen(const uint8_t& MapSize)
    : Map(MapSize, std::vector<bool>(MapSize, true)), MapSize(MapSize){

}

bool EightQueen::IsPositionValid(const uint8_t& Row, const uint8_t& Col) const{
    if (Row >= MapSize || Col >= MapSize){
        return false;
    }
    if (!Map[Row][Col]){
        return false;
    }
    return true;
}

bool EightQueen::TryAddQueenChess(const uint8_t& Row, const uint8_t& Col){
    if (!IsPositionValid(Row, Col)){
        return false;
    }
    QueenChess NewQueenChess(Row, Col);
    ChessArray.push_back(NewQueenChess);
    AfterAddQueenChess();
    return true;
}

bool EightQueen::TryReduceQueenChess(){
    if (ChessArray.size() <= 0){
        return false;
    }
    BeforeReduceQueenChess();
    ChessArray.pop_back();
    return true;
}

void EightQueen::AfterAddQueenChess(){
    uint8_t Row = ChessArray.back().Row;
    uint8_t Col = ChessArray.back().Col;
    const int32_t BiasFlag = Row - Col;
    const int32_t AntiBiasFlag = Row + Col;
    uint8_t ResPositionNum = 0;
    for (uint8_t RowIndex = 0; RowIndex < MapSize; RowIndex++){
        for (uint8_t ColIndex = 0; ColIndex < MapSize; ColIndex++){
            if (RowIndex == Row || ColIndex == Col || RowIndex - ColIndex == BiasFlag || RowIndex + ColIndex == AntiBiasFlag){
                if (Map[RowIndex][ColIndex]){
                    Map[RowIndex][ColIndex] = false;
                    MapUnit NewMapUnit(RowIndex, ColIndex);
                    ChessArray.back().EffectUnitArray.push_back(NewMapUnit);
                }
            }
            if (Map[RowIndex][ColIndex]){
                ResPositionNum++;
            }
        }
    }
    if (ChessArray.size() >= MapSize){
        bSuccess = true;
        return;
    }
    if (ResPositionNum <= 0){
        bFail = true;
        return;
    }
}

void EightQueen::BeforeReduceQueenChess(){
    for (const MapUnit& Unit : ChessArray.back().EffectUnitArray){
        Map[Unit.Row][Unit.Col] = true;
    }
    Map[ChessArray.back().Row][ChessArray.back().Col] = true;
}

