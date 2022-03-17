#include "EightQueen.h"

EightQueen::EightQueen(const uint8_t& MapSize)
    : Map(MapSize, std::vector<bool>(MapSize, true)),
      MapSize(MapSize){

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

bool EightQueen::TryAddQueenChess_Manually(const uint8_t& Row, const uint8_t& Col){
    return TryAddQueenChess(Row, Col);
}

bool EightQueen::TryAddQueenChess_Auto(){
    for (uint8_t Row = 0; Row < MapSize; Row++){
        if (AutoMap[Row] > 0){
            const uint8_t Col = AutoMap[Row];
            if (TryAddQueenChess(Row, Col)){
                AutoChessStack.push_back(Row * MapSize + Col);
                AutoMap[Row] = -1;
                ProcessStack.push_back(AutoProcess::Add);
                return true;
            }
            else{
                // Something Wrong
                return false;
            }
        }
    }

    return false;
}

bool EightQueen::TryReduceQueenChess_Manually(){
    return TryReduceQueenChess();
}

bool EightQueen::TryReduceQueenChess_Auto(){
    if (AutoChessStack.empty()){
        return false;
    }
    const int32_t RemoveIndex = AutoChessStack.back();
    const uint8_t Row = RemoveIndex / MapSize;
    const uint8_t Col = RemoveIndex % MapSize;
    if (TryReduceQueenChess()){
        AutoChessStack.pop_back();
        AutoMap[Row] = -1;
        while (Col < MapSize){
            if (Map[Row][Col]){
                AutoMap[Row] = Col;
            }
        }
        ProcessStack.push_back(AutoProcess::Reduce);
        return true;
    }
    return false;
}

void EightQueen::InitializeAutoState(){
    if (!AutoMap.empty()){
        // Something Wrong
        return;
    }
    if (!AutoChessStack.empty()){
        // Something Wrong
        return;
    }
    for (int Row = 0; Row < MapSize; Row++){
        uint8_t Index = -1;
        for (int Col = 0; Col < MapSize; Col++){
            if (Map[Row][Col]){
                Index = Col;
                break;
            }
        }
        AutoMap.emplace(Row, Index);
    }
}

void EightQueen::ClearAutoState(){
    AutoMap.clear();
    AutoChessStack.clear();
}

void EightQueen::StepBack_Auto(){

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
    if (bSuccess){
        bSuccess = false;
    }
    if (bFail){
        bFail = false;
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

