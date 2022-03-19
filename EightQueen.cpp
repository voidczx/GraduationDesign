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

bool EightQueen::TryAddQueenChess_Auto(uint8_t& OutRow, uint8_t& OutCol){
    for (uint8_t Row = 0; Row < MapSize; Row++){
        if (AutoMap[Row] < MapSize){
            const uint8_t Col = AutoMap[Row];
            if (TryAddQueenChess(Row, Col)){
                    OutRow = Row;
                    OutCol = Col;
                    AutoChessStack.push_back(Row * MapSize + Col);
                    RefreshAutoMap();
                    Process AddProcess(ProcessType::EAdd);
                    ProcessStack.push_back(AddProcess);
                    return true;
            }
            else{
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
    const int32_t ReduceIndex = AutoChessStack.back();
    const uint8_t Row = ReduceIndex / MapSize;
    const uint8_t Col = ReduceIndex % MapSize;
    if (TryReduceQueenChess()){
        AutoChessStack.pop_back();
        RefreshAutoMap();
        AutoMap[Row] = MapSize;
        uint8_t NextCol = Col;
        while (++NextCol < MapSize){
            if (Map[Row][NextCol]){
                AutoMap[Row] = NextCol;
                break;
            }
        }
        if (AutoMap[Row] >= MapSize){
            bAutoFail = true;
        }
        Process ReduceProcess(ProcessType::EReduce);
        ReduceProcess.OptionalInt1 = ReduceIndex;
        if (LastAutoReduceUnit.Row >= 255 || LastAutoReduceUnit.Col >= 255){
            ReduceProcess.OptionalInt2 = -1;
        }
        else{
            ReduceProcess.OptionalInt2 = LastAutoReduceUnit.Row * MapSize + LastAutoReduceUnit.Col;
        }
        LastAutoReduceUnit = MapUnit(Row, Col);
        ProcessStack.push_back(ReduceProcess);
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
        uint8_t Index = MapSize;
        for (int Col = 0; Col < MapSize; Col++){
            if (Map[Row][Col]){
                Index = Col;
                break;
            }
        }
        AutoMap.emplace(Row, Index);
    }
    bAutoFail = false;
}

void EightQueen::ClearAutoState(){
    AutoMap.clear();
    AutoChessStack.clear();
    bAutoFail = false;
}

void EightQueen::RefreshAutoMap(){
    for (int Row = 0; Row < MapSize; Row++){
        AutoMap[Row] = MapSize;
        for (int Col = 0; Col < MapSize; Col++){
            if (Map[Row][Col]){
                AutoMap[Row] = Col;
                break;
            }
        }
    }
}

EightQueen::Process EightQueen::StepBack_Auto(){
    if (ProcessStack.empty()){
        return Process();
    }
    Process LastProcess = ProcessStack.back();
    if (LastProcess.Type == ProcessType::EAdd){
        const int32_t LastAddIndex = AutoChessStack.back();
        const uint8_t LastAddRow = LastAddIndex / MapSize;
        const uint8_t LastAddCol = LastAddIndex % MapSize;
        if (TryReduceQueenChess()){
            if (bAutoFail){
                bAutoFail = false;
            }
            AutoChessStack.pop_back();
            RefreshAutoMap();
            AutoMap[LastAddRow] = LastAddCol;
            ProcessStack.pop_back();
            Process ShowProcess(ProcessType::EReduce);
            return ShowProcess;
        }
    }
    else if (LastProcess.Type == ProcessType::EReduce){
        const int32_t LastReduceIndex = LastProcess.OptionalInt1;
        const uint8_t LastReduceRow = LastReduceIndex / MapSize;
        const uint8_t LastReduceCol = LastReduceIndex % MapSize;
        if (TryAddQueenChess(LastReduceRow, LastReduceCol)){
            bAutoFail = true;
            AutoChessStack.push_back(LastReduceIndex);
            RefreshAutoMap();
            if (LastProcess.OptionalInt2 > 0){
                LastAutoReduceUnit = MapUnit(LastProcess.OptionalInt2 / MapSize, LastProcess.OptionalInt2 % MapSize);
            }
            else{
                LastAutoReduceUnit = MapUnit();
            }
            ProcessStack.pop_back();
            Process ShowProcess(ProcessType::EAdd);
            ShowProcess.OptionalInt1 = LastReduceIndex;
            return ShowProcess;
        }
    }
    else{

    }
    return Process();
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
    if (bAutoFail){
        bAutoFail = false;
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

