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

void Maze::ClearAllDynamic(){
    bSuccess = false;
    bFail = false;
    CurrentPoint.reset();
    for (int32_t Row = 0; Row < RowNum; Row++){
        for (int32_t Col = 0; Col < ColNum; Col++){
            MazeMap[Row][Col]->Value = -1;
            if (IsUnitDynamicType(MazeMap[Row][Col])){
                MazeMap[Row][Col]->Type = Maze::MazeUnitType::ENone;
            }
        }
    }
    OpenMap.clear();
    ProcessStack.clear();
}

void Maze::ClearAll(){

}

bool Maze::SetStartPoint(const int32_t& InRowNum, const int32_t& InColNum){
    std::shared_ptr<MazeUnit> NewStartPoint = MazeMap[InRowNum][InColNum];
    NewStartPoint->Type = MazeUnitType::EStartPoint;
    if (StartPoint){
        if (IsUnitEqual(StartPoint, NewStartPoint)){
            return false;
        }
        StartPoint->Type = MazeUnitType::ENone;
    }
    StartPoint = NewStartPoint;
    return true;
}

bool Maze::SetFinishPoint(const int32_t& InRowNum, const int32_t& InColNum){
    std::shared_ptr<MazeUnit> NewFinishPoint = MazeMap[InRowNum][InColNum];
    NewFinishPoint->Type = MazeUnitType::EFinishPoint;
    if (FinishPoint){
        if (IsUnitEqual(FinishPoint, NewFinishPoint)){
            return false;
        }
        FinishPoint->Type = MazeUnitType::ENone;
    }
    FinishPoint = NewFinishPoint;
    return true;
}

bool Maze::SetBlockPoint(const int32_t& InRowNum, const int32_t& InColNum){
    std::shared_ptr<MazeUnit> NewBlockPoint = MazeMap[InRowNum][InColNum];
    NewBlockPoint->Type = MazeUnitType::EBlock;
    return true;
}

bool Maze::ClearPoint(const int32_t& InRowNum, const int32_t& InColNum){
    std::shared_ptr<MazeUnit> WaitClearPoint = MazeMap[InRowNum][InColNum];
    WaitClearPoint->Type = MazeUnitType::ENone;
    if (IsUnitEqual(WaitClearPoint, StartPoint)){
        StartPoint.reset();
    }
    else if (IsUnitEqual(WaitClearPoint, FinishPoint)){
        FinishPoint.reset();
    }
    return true;
}

void Maze::EditComplete(std::vector<std::shared_ptr<Maze::Process>>& OutProcessArray){
    if (!StartPoint || !FinishPoint){
        return;
    }
    if (CurrentPoint){
        return;
    }
    CurrentPoint = StartPoint;
    FinishPoint->Value = 0;
    ProcessNeighbour(CurrentPoint->Row, CurrentPoint->Col, OutProcessArray);
}

Maze::MazeUnitType Maze::GetUnitType(const int32_t& InRow, const int32_t& InCol){
    if (MazeMap[InRow][InCol]){
        return MazeMap[InRow][InCol]->Type;
    }
    return Maze::MazeUnitType::ENone;
}

int32_t Maze::GetUnitValue(const int32_t& InRow, const int32_t& InCol){
    if (MazeMap[InRow][InCol] && MazeMap[InRow][InCol]->Type == Maze::MazeUnitType::EOpen){
        return MazeMap[InRow][InCol]->Value;
    }
    return -1;
}

bool Maze::GetStartPointPosition(int32_t& OutRow, int32_t& OutCol){
    if (!StartPoint){
        return false;
    }
    OutRow = StartPoint->Row;
    OutCol = StartPoint->Col;
    return true;
}

bool Maze::GetFinishPointPosition(int32_t& OutRow, int32_t& OutCol){
    if (!FinishPoint){
        return false;
    }
    OutRow = FinishPoint->Row;
    OutCol = FinishPoint->Col;
    return true;
}

bool Maze::GetCurrentPointPosition(int32_t& OutRow, int32_t& OutCol){
    if (!CurrentPoint){
        return false;
    }
    OutRow = CurrentPoint->Row;
    OutCol = CurrentPoint->Col;
    return true;
}

std::vector<std::shared_ptr<Maze::Process>> Maze::StepForward(){
    std::vector<std::shared_ptr<Maze::Process>> OutProcessArray;
    if (!StartPoint || !FinishPoint){
        return OutProcessArray;
    }
    if (bSuccess || bFail){
        return OutProcessArray;
    }
    if (!CurrentPoint){
       return OutProcessArray;
    }
    if (OpenMap.empty()){
        bFail = true;
        return OutProcessArray;
    }

    // Find Next Point
    std::shared_ptr<Maze::MazeUnit> NextPoint;
    NextPoint = OpenMap.begin()->second;

    if (NextPoint){
        OpenMap.erase(OpenMap.begin());
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
        int32_t PathRow = FinishPoint->ParentRow;
        int32_t PathCol = FinishPoint->ParentCol;
        std::shared_ptr<Maze::MazeUnit> PathUnit = MazeMap[PathRow][PathCol];
        while (PathRow >= 0 && PathCol >= 0 && !IsUnitEqual(PathUnit, StartPoint)){
            PathUnit->Type = Maze::MazeUnitType::EPath;
            PathRow = PathUnit->ParentRow;
            PathCol = PathUnit->ParentCol;
            PathUnit = MazeMap[PathRow][PathCol];
        }
        return OutProcessArray;
    }

    ProcessNeighbour(CurrentPoint->Row, CurrentPoint->Col, OutProcessArray);
    if (OpenMap.empty()){
        bFail = true;
        return OutProcessArray;
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

bool Maze::IsUnitDynamicType(const std::shared_ptr<Maze::MazeUnit>& InUnit){
    if (!InUnit){
        return false;
    }
    switch (InUnit->Type){
    case Maze::MazeUnitType::EOpen:
        return true;
        break;
    case Maze::MazeUnitType::EClose:
        return true;
        break;
    case Maze::MazeUnitType::EPath:
        return true;
        break;
    default:
        break;
    }
    return false;
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
    case Maze::MazeUnitType::EBlock:
        return false;
        break;
    default:
        break;
    }
    return true;
}

void Maze::ProcessNeighbour(const int32_t& InRow, const int32_t InCol, std::vector<std::shared_ptr<Maze::Process>>& OutProcessArray){

    if (InRow > 0){
        std::shared_ptr<Maze::MazeUnit> UpUnit = MazeMap[InRow - 1][InCol];
        if (IsUnitWalkable(UpUnit) && UpUnit->Value <= 0){
            UpUnit->Value = std::abs(FinishPoint->Row - UpUnit->Row) + std::abs(FinishPoint->Col - UpUnit->Col);
            UpUnit->ParentRow = InRow;
            UpUnit->ParentCol = InCol;
            OpenMap.emplace(UpUnit->Value, UpUnit);
            if (CanUnitChangeType(UpUnit)){
                UpUnit->Type = Maze::MazeUnitType::EOpen;
                std::shared_ptr<Maze::ChangeProcess> ChangeToOpenProcess(new Maze::ChangeProcess);
                ChangeToOpenProcess->Row = UpUnit->Row;
                ChangeToOpenProcess->Col = UpUnit->Col;
                ChangeToOpenProcess->Type = Maze::ChangeProcessType::EOpen;
                OutProcessArray.push_back(ChangeToOpenProcess);
            }
        }
    }
    if (InRow < RowNum - 1){
        std::shared_ptr<Maze::MazeUnit> DownUnit = MazeMap[InRow + 1][InCol];
        if (IsUnitWalkable(DownUnit) && DownUnit->Value <= 0){
            DownUnit->Value = std::abs(FinishPoint->Row - DownUnit->Row) + std::abs(FinishPoint->Col - DownUnit->Col);
            DownUnit->ParentRow = InRow;
            DownUnit->ParentCol = InCol;
            OpenMap.emplace(DownUnit->Value, DownUnit);
            if (CanUnitChangeType(DownUnit)){
                DownUnit->Type = Maze::MazeUnitType::EOpen;
                std::shared_ptr<Maze::ChangeProcess> ChangeToOpenProcess(new Maze::ChangeProcess);
                ChangeToOpenProcess->Row = DownUnit->Row;
                ChangeToOpenProcess->Col = DownUnit->Col;
                ChangeToOpenProcess->Type = Maze::ChangeProcessType::EOpen;
                OutProcessArray.push_back(ChangeToOpenProcess);
            }
        }
    }
    if (InCol > 0){
        std::shared_ptr<Maze::MazeUnit> LeftUnit = MazeMap[InRow][InCol - 1];
        if (IsUnitWalkable(LeftUnit) && LeftUnit->Value <= 0){
            LeftUnit->Value = std::abs(FinishPoint->Row - LeftUnit->Row) + std::abs(FinishPoint->Col - LeftUnit->Col);
            LeftUnit->ParentRow = InRow;
            LeftUnit->ParentCol = InCol;
            OpenMap.emplace(LeftUnit->Value, LeftUnit);
            if (CanUnitChangeType(LeftUnit)){
                LeftUnit->Type = Maze::MazeUnitType::EOpen;
                std::shared_ptr<Maze::ChangeProcess> ChangeToOpenProcess(new Maze::ChangeProcess);
                ChangeToOpenProcess->Row = LeftUnit->Row;
                ChangeToOpenProcess->Col = LeftUnit->Col;
                ChangeToOpenProcess->Type = Maze::ChangeProcessType::EOpen;
                OutProcessArray.push_back(ChangeToOpenProcess);
            }
        }
    }
    if (InCol < ColNum - 1){
        std::shared_ptr<Maze::MazeUnit> RightUnit = MazeMap[InRow][InCol + 1];
        if (IsUnitWalkable(RightUnit) && RightUnit->Value <= 0){
            RightUnit->Value = std::abs(FinishPoint->Row - RightUnit->Row) + std::abs(FinishPoint->Col - RightUnit->Col);
            RightUnit->ParentRow = InRow;
            RightUnit->ParentCol = InCol;
            OpenMap.emplace(RightUnit->Value, RightUnit);
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
}


