#include "Maze.h"

#include "qdebug.h"

Maze::Maze(const int32_t& InRowNum, const int32_t& InColNum) : RowNum(InRowNum), ColNum(InColNum)
{
    for (int32_t Row = 0; Row < RowNum; Row++){
        std::vector<std::shared_ptr<MazeUnit>> TempRow;
        for (int32_t Col = 0; Col < ColNum; Col++){
            std::shared_ptr<MazeUnit> NewUnit(new MazeUnit());
            NewUnit->Row = Row;
            NewUnit->Col = Col;
            NewUnit->Type = MazeUnitType::ENone;
            TempRow.push_back(NewUnit);
        }
        MazeMap.push_back(TempRow);
    }
}

void Maze::ClearAll(){

}

void Maze::SetStartPoint(const int32_t& InRowNum, const int32_t& InColNum){
    std::shared_ptr<MazeUnit> NewStartPoint = MazeMap[InRowNum][InColNum];
    NewStartPoint->Type = MazeUnitType::EStartPoint;
    if (StartPoint){
        if (IsUnitEqual(StartPoint, NewStartPoint)){
            return;
        }
        StartPoint->Type = MazeUnitType::ENone;
    }
    StartPoint = NewStartPoint;
}

void Maze::SetFinishPoint(const int32_t& InRowNum, const int32_t& InColNum){
    std::shared_ptr<MazeUnit> NewFinishPoint = MazeMap[InRowNum][InColNum];
    NewFinishPoint->Type = MazeUnitType::EFinishPoint;
    if (FinishPoint){
        if (IsUnitEqual(FinishPoint, NewFinishPoint)){
            return;
        }
        FinishPoint->Type = MazeUnitType::ENone;
    }
    FinishPoint = NewFinishPoint;
}

void Maze::SetBlockPoint(const int32_t& InRowNum, const int32_t& InColNum){
    std::shared_ptr<MazeUnit> NewBlockPoint = MazeMap[InRowNum][InColNum];
    NewBlockPoint->Type = MazeUnitType::EBlock;
}

void Maze::ClearPoint(const int32_t& InRowNum, const int32_t& InColNum){
    std::shared_ptr<MazeUnit> WaitClearPoint = MazeMap[InRowNum][InColNum];
    WaitClearPoint->Type = MazeUnitType::ENone;
    if (IsUnitEqual(WaitClearPoint, StartPoint)){
        StartPoint.reset();
    }
    else if (IsUnitEqual(WaitClearPoint, FinishPoint)){
        FinishPoint.reset();
    }
}

void Maze::EditComplete(std::vector<std::shared_ptr<Maze::Process>>& OutProcessArray, bool& OutHasWalkableNeighbour){
    if (!StartPoint || !FinishPoint){
        return;
    }
    if (CurrentPoint){
        return;
    }
    CurrentPoint = StartPoint;
    ProcessNeighbour(CurrentPoint->Row, CurrentPoint->Col, OutProcessArray, OutHasWalkableNeighbour);
}

std::vector<std::shared_ptr<Maze::Process>> Maze::StepForward(){
    std::vector<std::shared_ptr<Maze::Process>> OutProcessArray;
    if (!StartPoint || !FinishPoint){
        return OutProcessArray;
    }
    if (!CurrentPoint){
        bool bHasWalkableNeighbour = false;
        EditComplete(OutProcessArray, bHasWalkableNeighbour);
        if (!bHasWalkableNeighbour){
            bFail = false;
            return OutProcessArray;
        }
    }

    // Find Next Point
    bool bNeedTraceBack = true;
    std::shared_ptr<Maze::MazeUnit> NextPoint;
    int32_t LowestValue = -1;
    if (CurrentPoint->Row > 0){
        std::shared_ptr<Maze::MazeUnit> UpUnit = MazeMap[CurrentPoint->Row-1][CurrentPoint->Col];
        if (IsUnitWalkable(UpUnit)){
            if (UpUnit->Value > 0){
                if (LowestValue < 0){
                    LowestValue = UpUnit->Value;
                    NextPoint = UpUnit;
                    bNeedTraceBack = false;
                }
                else{
                    if (UpUnit->Value <  LowestValue){
                        LowestValue = UpUnit->Value;
                        NextPoint = UpUnit;
                        bNeedTraceBack = false;
                    }
                }
            }
        }
    }
    if (CurrentPoint->Row < RowNum - 1){
        std::shared_ptr<Maze::MazeUnit> DownUnit = MazeMap[CurrentPoint->Row+1][CurrentPoint->Col];
        if (IsUnitWalkable(DownUnit)){
            if (DownUnit->Value > 0){
                if (LowestValue < 0){
                    LowestValue = DownUnit->Value;
                    NextPoint = DownUnit;
                    bNeedTraceBack = false;
                }
                else{
                    if (DownUnit->Value <  LowestValue){
                        LowestValue = DownUnit->Value;
                        NextPoint = DownUnit;
                        bNeedTraceBack = false;
                    }
                }
            }
        }
    }
    if (CurrentPoint->Col > 0){
        std::shared_ptr<Maze::MazeUnit> LeftUnit = MazeMap[CurrentPoint->Row][CurrentPoint->Col-1];
        if (IsUnitWalkable(LeftUnit)){
            if (LeftUnit->Value > 0){
                if (LowestValue < 0){
                    LowestValue = LeftUnit->Value;
                    NextPoint = LeftUnit;
                    bNeedTraceBack = false;
                }
                else{
                    if (LeftUnit->Value <  LowestValue){
                        LowestValue = LeftUnit->Value;
                        NextPoint = LeftUnit;
                        bNeedTraceBack = false;
                    }
                }
            }
        }
    }
    if (CurrentPoint->Col < ColNum - 1){
        std::shared_ptr<Maze::MazeUnit> RightUnit = MazeMap[CurrentPoint->Row][CurrentPoint->Col+1];
        if (IsUnitWalkable(RightUnit)){
            if (RightUnit->Value > 0){
                if (LowestValue < 0){
                    LowestValue = RightUnit->Value;
                    NextPoint = RightUnit;
                    bNeedTraceBack = false;
                }
                else{
                    if (RightUnit->Value <  LowestValue){
                        LowestValue = RightUnit->Value;
                        NextPoint = RightUnit;
                        bNeedTraceBack = false;
                    }
                }
            }
        }
    }

    // Trace Back When Needed
    if (bNeedTraceBack){
        if (!PathStack.empty()){
            NextPoint = PathStack.back();
            PathStack.pop_back();
        }
    }
    else{
        PathStack.push_back(CurrentPoint);
    }

    // Add Move Process
    if (NextPoint){
        std::shared_ptr<Maze::MoveProcess> NewMoveProcess(new Maze::MoveProcess);
        NewMoveProcess->FromCol = CurrentPoint->Col;
        NewMoveProcess->FromRow = CurrentPoint->Row;
        NewMoveProcess->ToCol = NextPoint->Col;
        NewMoveProcess->ToRow = NextPoint->Row;
        OutProcessArray.push_back(NewMoveProcess);
    }

    // Add Change Process
    if (NextPoint){
        if (CanUnitChangeType(CurrentPoint)){
            CurrentPoint->Type = Maze::MazeUnitType::EClose;
            std::shared_ptr<Maze::ChangeProcess> NewChangeProcess(new Maze::ChangeProcess);
            NewChangeProcess->Col = CurrentPoint->Col;
            NewChangeProcess->Row = CurrentPoint->Row;
            NewChangeProcess->Type = Maze::ChangeProcessType::EClose;
            OutProcessArray.push_back(NewChangeProcess);
        }
    }

    CurrentPoint = NextPoint;
    if (IsUnitEqual(CurrentPoint, FinishPoint)){
        bSuccess = true;
        return OutProcessArray;
    }

    bool bHasWalkableNeighbour = false;
    ProcessNeighbour(CurrentPoint->Row, CurrentPoint->Col, OutProcessArray, bHasWalkableNeighbour);
    if (!bHasWalkableNeighbour){
        if (IsUnitEqual(CurrentPoint, StartPoint)){
            bFail = true;
            return OutProcessArray;
        }
    }

    return OutProcessArray;
}

bool Maze::IsUnitEqual(const std::shared_ptr<MazeUnit>& Left, const std::shared_ptr<MazeUnit>& Right){
    if (Left && Right){
        return Left->Col == Right->Col && Left->Row == Right->Row;
    }
    return false;
}

bool Maze::IsUnitWalkable(const std::shared_ptr<MazeUnit>& InUnit){
    if (!InUnit){
        return false;
    }
    switch (InUnit->Type){
    case Maze::MazeUnitType::EBlock:
        return false;
        break;
    case Maze::MazeUnitType::EClose:
        return false;
        break;
    default:
        break;
    }
    return true;
}

bool Maze::CanUnitChangeType(const std::shared_ptr<MazeUnit>& InUnit){
    if (!InUnit){
        return false;
    }
    switch (InUnit->Type) {
    case Maze::MazeUnitType::EStartPoint:
        return false;
        break;
    case Maze::MazeUnitType::EFinishPoint:
        return false;
        break;
    default:
        break;
    }
    return true;
}

void Maze::ProcessNeighbour(const int32_t& InRow, const int32_t InCol, std::vector<std::shared_ptr<Maze::Process>>& OutProcessArray, bool& OutHasWalkableNeighbour){
    OutHasWalkableNeighbour = false;
    if (InRow > 0){
        std::shared_ptr<Maze::MazeUnit> UpUnit = MazeMap[InRow - 1][InCol];
        if (IsUnitWalkable(UpUnit)){
            UpUnit->Value = std::abs(FinishPoint->Row - UpUnit->Row) + std::abs(FinishPoint->Col - UpUnit->Col);
            OutHasWalkableNeighbour = true;
        }
        if (CanUnitChangeType(UpUnit)){
            UpUnit->Type = Maze::MazeUnitType::EOpen;
            std::shared_ptr<Maze::ChangeProcess> ChangeToOpenProcess(new Maze::ChangeProcess);
            ChangeToOpenProcess->Row = UpUnit->Row;
            ChangeToOpenProcess->Col = UpUnit->Col;
            ChangeToOpenProcess->Type = Maze::ChangeProcessType::EOpen;
            OutProcessArray.push_back(ChangeToOpenProcess);
        }
    }
    if (InRow < RowNum - 1){
        std::shared_ptr<Maze::MazeUnit> DownUnit = MazeMap[InRow + 1][InCol];
        if (IsUnitWalkable(DownUnit)){
            DownUnit->Value = std::abs(FinishPoint->Row - DownUnit->Row) + std::abs(FinishPoint->Col - DownUnit->Col);
            OutHasWalkableNeighbour = true;
        }
        if (CanUnitChangeType(DownUnit)){
            DownUnit->Type = Maze::MazeUnitType::EOpen;
            std::shared_ptr<Maze::ChangeProcess> ChangeToOpenProcess(new Maze::ChangeProcess);
            ChangeToOpenProcess->Row = DownUnit->Row;
            ChangeToOpenProcess->Col = DownUnit->Col;
            ChangeToOpenProcess->Type = Maze::ChangeProcessType::EOpen;
            OutProcessArray.push_back(ChangeToOpenProcess);
        }
    }
    if (InCol > 0){
        std::shared_ptr<Maze::MazeUnit> LeftUnit = MazeMap[InRow][InCol - 1];
        if (IsUnitWalkable(LeftUnit)){
            LeftUnit->Value = std::abs(FinishPoint->Row - LeftUnit->Row) + std::abs(FinishPoint->Col - LeftUnit->Col);
            OutHasWalkableNeighbour = true;
        }
        if (CanUnitChangeType(LeftUnit)){
            LeftUnit->Type = Maze::MazeUnitType::EOpen;
            std::shared_ptr<Maze::ChangeProcess> ChangeToOpenProcess(new Maze::ChangeProcess);
            ChangeToOpenProcess->Row = LeftUnit->Row;
            ChangeToOpenProcess->Col = LeftUnit->Col;
            ChangeToOpenProcess->Type = Maze::ChangeProcessType::EOpen;
            OutProcessArray.push_back(ChangeToOpenProcess);
        }
    }
    if (InCol < ColNum - 1){
        std::shared_ptr<Maze::MazeUnit> RightUnit = MazeMap[InRow][InCol + 1];
        if (IsUnitWalkable(RightUnit)){
            RightUnit->Value = std::abs(FinishPoint->Row - RightUnit->Row) + std::abs(FinishPoint->Col - RightUnit->Col);
            OutHasWalkableNeighbour = true;
        }
        if (CanUnitChangeType(RightUnit)){
            RightUnit->Type = Maze::MazeUnitType::EOpen;
            std::shared_ptr<Maze::ChangeProcess> ChangeToOpenProcess(new Maze::ChangeProcess);
            ChangeToOpenProcess->Row = RightUnit->Row;
            ChangeToOpenProcess->Col = RightUnit->Col;
            ChangeToOpenProcess->Type = Maze::ChangeProcessType::EOpen;
            OutProcessArray.push_back(ChangeToOpenProcess);
        }
    }
}


