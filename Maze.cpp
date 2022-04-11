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

float Maze::GetUnitValue(const int32_t& InRow, const int32_t& InCol){
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
            std::shared_ptr<Maze::ChangeProcess> NewChangeProcess(new Maze::ChangeProcess);
            NewChangeProcess->Col = CurrentPoint->Col;
            NewChangeProcess->Row = CurrentPoint->Row;
            NewChangeProcess->OldType = TransUnitTypeToChangeProcessType(CurrentPoint->Type);
            CurrentPoint->Type = Maze::MazeUnitType::EClose;
            NewChangeProcess->NewType = TransUnitTypeToChangeProcessType(CurrentPoint->Type);
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
        ProcessStack.push_back(OutProcessArray);
        return OutProcessArray;
    }

    ProcessNeighbour(CurrentPoint->Row, CurrentPoint->Col, OutProcessArray);
    if (OpenMap.empty()){
        bFail = true;
        ProcessStack.push_back(OutProcessArray);
        return OutProcessArray;
    }

    ProcessStack.push_back(OutProcessArray);
    return OutProcessArray;
}

std::vector<std::shared_ptr<Maze::Process>> Maze::StepBack(){
    std::vector<std::shared_ptr<Maze::Process>> OutProcessArray;
    if (bSuccess){
        return OutProcessArray;
    }
    if (ProcessStack.empty()){
        return OutProcessArray;
    }
    if (CurrentPoint == nullptr){
        return OutProcessArray;
    }
    std::vector<std::shared_ptr<Maze::Process>> LastProcessArray = ProcessStack.back();
    ProcessStack.pop_back();

    for (std::shared_ptr<Maze::Process> HistoryProcess : LastProcessArray){
        if (HistoryProcess->Class == Maze::ProcessClass::EMove){
            std::shared_ptr<Maze::MoveProcess> HistoryMoveProcess = std::static_pointer_cast<Maze::MoveProcess>(HistoryProcess);
            if (HistoryMoveProcess){
                std::shared_ptr<Maze::MoveProcess> StepBackMoveProcess(new Maze::MoveProcess());
                if (StepBackMoveProcess){
                    StepBackMoveProcess->FromRow = HistoryMoveProcess->ToRow;
                    StepBackMoveProcess->FromCol = HistoryMoveProcess->ToCol;
                    StepBackMoveProcess->ToRow = HistoryMoveProcess->FromRow;
                    StepBackMoveProcess->ToCol = HistoryMoveProcess->FromCol;
                    OutProcessArray.push_back(StepBackMoveProcess);
                }
                if (CurrentPoint->Value >= 0){
                    OpenMap.emplace(CurrentPoint->Value, CurrentPoint);
                }
                CurrentPoint = MazeMap[HistoryMoveProcess->FromRow][HistoryMoveProcess->FromCol];
            }
        }
        else if (HistoryProcess->Class == Maze::ProcessClass::EChange){
            std::shared_ptr<Maze::ChangeProcess> HistoryChangeProcess = std::static_pointer_cast<Maze::ChangeProcess>(HistoryProcess);
            if (HistoryChangeProcess){
                std::shared_ptr<Maze::ChangeProcess> StepBackChangeProcess(new Maze::ChangeProcess());
                if (StepBackChangeProcess){
                    StepBackChangeProcess->Row = HistoryChangeProcess->Row;
                    StepBackChangeProcess->Col = HistoryChangeProcess->Col;
                    StepBackChangeProcess->OldType = HistoryChangeProcess->NewType;
                    StepBackChangeProcess->NewType = HistoryChangeProcess->OldType;
                    OutProcessArray.push_back(StepBackChangeProcess);
                }
                std::shared_ptr<Maze::MazeUnit> HistoryUnit = MazeMap[HistoryChangeProcess->Row][HistoryChangeProcess->Col];
                if (HistoryUnit){
                    if (HistoryUnit->Type == Maze::MazeUnitType::EOpen){
                        RemoveFromOpenMap(HistoryUnit);
                    }
                    HistoryUnit->Type = TransChangeProcessTypeToUnitType(HistoryChangeProcess->OldType);
                }
            }
        }
    }

    if (!OpenMap.empty() && IsUnitEqual(FinishPoint, OpenMap.begin()->second)){
        OpenMap.erase(OpenMap.begin());
    }

    if (bFail){
        bFail = false;
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
    case Maze::MazeUnitType::EStartPoint:
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

Maze::ChangeProcessType Maze::TransUnitTypeToChangeProcessType(const Maze::MazeUnitType& InUnitType){
    switch (InUnitType) {
    case Maze::MazeUnitType::EClose:
        return Maze::ChangeProcessType::EClose;
        break;
    case Maze::MazeUnitType::EOpen:
        return Maze::ChangeProcessType::EOpen;
        break;
    case Maze::MazeUnitType::ENone:
        return Maze::ChangeProcessType::ENone;
        break;
    default:
        break;
    }
    return Maze::ChangeProcessType::ENone;
}

Maze::MazeUnitType Maze::TransChangeProcessTypeToUnitType(const Maze::ChangeProcessType& InChangeProcessType){
    switch (InChangeProcessType) {
    case Maze::ChangeProcessType::EClose:
        return Maze::MazeUnitType::EClose;
        break;
    case Maze::ChangeProcessType::EOpen:
        return Maze::MazeUnitType::EOpen;
        break;
    case Maze::ChangeProcessType::ENone:
        return Maze::MazeUnitType::ENone;
        break;
    default:
        break;
    }
    return Maze::MazeUnitType::ENone;
}

void Maze::ProcessNeighbour(const int32_t& InRow, const int32_t InCol, std::vector<std::shared_ptr<Maze::Process>>& OutProcessArray){

    if (InRow > 0){
        std::shared_ptr<Maze::MazeUnit> UpUnit = MazeMap[InRow - 1][InCol];
        if (IsUnitWalkable(UpUnit) && UpUnit->Type != Maze::MazeUnitType::EOpen){
            UpUnit->ParentRow = InRow;
            UpUnit->ParentCol = InCol;
            UpUnit->Value = CalculateUnitValue(UpUnit);
            OpenMap.emplace(UpUnit->Value, UpUnit);
            if (CanUnitChangeType(UpUnit)){
                std::shared_ptr<Maze::ChangeProcess> ChangeToOpenProcess(new Maze::ChangeProcess);
                ChangeToOpenProcess->Row = UpUnit->Row;
                ChangeToOpenProcess->Col = UpUnit->Col;
                ChangeToOpenProcess->OldType = TransUnitTypeToChangeProcessType(UpUnit->Type);
                UpUnit->Type = Maze::MazeUnitType::EOpen;
                ChangeToOpenProcess->NewType = TransUnitTypeToChangeProcessType(UpUnit->Type);
                OutProcessArray.push_back(ChangeToOpenProcess);
            }
        }
    }
    if (InRow < RowNum - 1){
        std::shared_ptr<Maze::MazeUnit> DownUnit = MazeMap[InRow + 1][InCol];
        if (IsUnitWalkable(DownUnit) && DownUnit->Type != Maze::MazeUnitType::EOpen){
            DownUnit->ParentRow = InRow;
            DownUnit->ParentCol = InCol;
            DownUnit->Value = CalculateUnitValue(DownUnit);
            OpenMap.emplace(DownUnit->Value, DownUnit);
            if (CanUnitChangeType(DownUnit)){
                std::shared_ptr<Maze::ChangeProcess> ChangeToOpenProcess(new Maze::ChangeProcess);
                ChangeToOpenProcess->Row = DownUnit->Row;
                ChangeToOpenProcess->Col = DownUnit->Col;
                ChangeToOpenProcess->OldType = TransUnitTypeToChangeProcessType(DownUnit->Type);
                DownUnit->Type = Maze::MazeUnitType::EOpen;
                ChangeToOpenProcess->NewType = TransUnitTypeToChangeProcessType(DownUnit->Type);
                OutProcessArray.push_back(ChangeToOpenProcess);
            }
        }
    }
    if (InCol > 0){
        std::shared_ptr<Maze::MazeUnit> LeftUnit = MazeMap[InRow][InCol - 1];
        if (IsUnitWalkable(LeftUnit) && LeftUnit->Type != Maze::MazeUnitType::EOpen){
            LeftUnit->ParentRow = InRow;
            LeftUnit->ParentCol = InCol;
            LeftUnit->Value = CalculateUnitValue(LeftUnit);
            OpenMap.emplace(LeftUnit->Value, LeftUnit);
            if (CanUnitChangeType(LeftUnit)){
                std::shared_ptr<Maze::ChangeProcess> ChangeToOpenProcess(new Maze::ChangeProcess);
                ChangeToOpenProcess->Row = LeftUnit->Row;
                ChangeToOpenProcess->Col = LeftUnit->Col;
                ChangeToOpenProcess->OldType = TransUnitTypeToChangeProcessType(LeftUnit->Type);
                LeftUnit->Type = Maze::MazeUnitType::EOpen;
                ChangeToOpenProcess->NewType = Maze::ChangeProcessType::EOpen;
                OutProcessArray.push_back(ChangeToOpenProcess);
            }
        }
    }
    if (InCol < ColNum - 1){
        std::shared_ptr<Maze::MazeUnit> RightUnit = MazeMap[InRow][InCol + 1];
        if (IsUnitWalkable(RightUnit) && RightUnit->Type != Maze::MazeUnitType::EOpen){
            RightUnit->ParentRow = InRow;
            RightUnit->ParentCol = InCol;
            RightUnit->Value = CalculateUnitValue(RightUnit);
            OpenMap.emplace(RightUnit->Value, RightUnit);
            if (CanUnitChangeType(RightUnit)){
                std::shared_ptr<Maze::ChangeProcess> ChangeToOpenProcess(new Maze::ChangeProcess);
                ChangeToOpenProcess->Row = RightUnit->Row;
                ChangeToOpenProcess->Col = RightUnit->Col;
                ChangeToOpenProcess->OldType = TransUnitTypeToChangeProcessType(RightUnit->Type);
                RightUnit->Type = Maze::MazeUnitType::EOpen;
                ChangeToOpenProcess->NewType = TransUnitTypeToChangeProcessType(RightUnit->Type);
                OutProcessArray.push_back(ChangeToOpenProcess);
            }
        }
    }
}

bool Maze::RemoveFromOpenMap(const std::shared_ptr<MazeUnit>& InUnit){
    auto RemoveIter = OpenMap.end();
    for (auto Iter = OpenMap.begin(); Iter != OpenMap.end(); Iter++){
        if (IsUnitEqual(Iter->second, InUnit)){
            RemoveIter = Iter;
            break;
        }
    }
    if (RemoveIter == OpenMap.end()){
        return false;
    }
    OpenMap.erase(RemoveIter);
    return true;
}

int32_t Maze::GetPathDepth(const std::shared_ptr<Maze::MazeUnit>& InUnit){
    std::shared_ptr<Maze::MazeUnit> CurUnit = InUnit;
    int32_t ResultDepth = 0;
    while (CurUnit->ParentCol >= 0 && CurUnit->ParentRow >= 0){
        std::shared_ptr<Maze::MazeUnit> ParentUnit = MazeMap[CurUnit->ParentRow][CurUnit->ParentCol];
        if (IsUnitEqual(ParentUnit, StartPoint)){
            break;
        }
        ResultDepth += 1;
        CurUnit = ParentUnit;
    }
    return ResultDepth;
}

float Maze::CalculateUnitValue(const std::shared_ptr<Maze::MazeUnit>& InUnit){
    return (std::abs(FinishPoint->Row - InUnit->Row) + std::abs(FinishPoint->Col - InUnit->Col)) * 1.1 + GetPathDepth(InUnit);
}


