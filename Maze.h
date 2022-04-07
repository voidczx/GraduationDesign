#ifndef MAZE_H
#define MAZE_H

#include "stdint.h"
#include "vector"
#include "memory"
#include "deque"
#include "map"

class Maze
{

public:

    enum class MazeUnitType{
        ENone = 0,
        EStartPoint = 1,
        EFinishPoint = 2,
        EBlock = 3,
        EClose = 4,
        EOpen = 5,
        ENum,
    };

    struct MazeUnit{
        MazeUnitType Type = MazeUnitType::ENone;
        int32_t Row = -1;
        int32_t Col = -1;
        int32_t Value = -1;
        int32_t ParentRow = -1;
        int32_t ParentCol = -1;
    };

    enum class ProcessClass{
        ENone = 0,
        EMove = 1,
        EChange = 2,
        ENum,
    };

    struct Process{
        Process() : Class(ProcessClass::ENone) {}
        Process(ProcessClass InClass) : Class(InClass) {}
        ProcessClass Class;
    };

    struct MoveProcess : public Process{
        MoveProcess() : Process(ProcessClass::EMove) {}
        int32_t FromRow = -1;
        int32_t ToRow = -1;
        int32_t FromCol = -1;
        int32_t ToCol = -1;
    };

    enum class ChangeProcessType{
        ENone = 0,
        EOpen = 1,
        EClose = 2,
        ENum,
    };

    struct ChangeProcess : public Process{
        ChangeProcess() : Process(ProcessClass::EChange) {}
        int32_t Row = -1;
        int32_t Col = -1;
        ChangeProcessType Type = ChangeProcessType::ENone;
    };

    Maze(const int32_t& InRowNum, const int32_t& InColNum);

    int32_t GetRowNum() const { return RowNum; }
    int32_t GetColNum() const { return ColNum; }
    bool IsSuccessful() const { return bSuccess; }
    bool IsFail() const { return bFail; }

    void ClearAllDynamic();
    void ClearAll();

    bool SetStartPoint(const int32_t& InRowNum, const int32_t& InColNum);
    bool SetFinishPoint(const int32_t& InRowNum, const int32_t& InColNum);
    bool SetBlockPoint(const int32_t& InRowNum, const int32_t& InColNum);
    bool ClearPoint(const int32_t& InRowNum, const int32_t& InColNum);
    void EditComplete(std::vector<std::shared_ptr<Process>>& OutProcessArray);

    MazeUnitType GetUnitType(const int32_t& InRow, const int32_t& InCol);
    bool GetStartPointPosition(int32_t& OutRow, int32_t& OutCol);
    bool GetFinishPointPosition(int32_t& OutRow, int32_t& OutCol);
    bool GetCurrentPointPosition(int32_t& OutRow, int32_t& OutCol);

    std::vector<std::shared_ptr<Process>> StepForward();

private:

    bool IsUnitEqual(const std::shared_ptr<MazeUnit>& Left, const std::shared_ptr<MazeUnit>& Right);
    bool IsUnitWalkable(const std::shared_ptr<MazeUnit>& InUnit);
    bool IsUnitDynamicType(const std::shared_ptr<MazeUnit>& InUnit);
    bool CanUnitChangeType(const std::shared_ptr<MazeUnit>& InUnit);
    void ProcessNeighbour(const int32_t& InRow, const int32_t InCol, std::vector<std::shared_ptr<Process>>& OutProcessArray);

    int32_t RowNum = -1;
    int32_t ColNum = -1;
    bool bSuccess = false;
    bool bFail = false;
    std::shared_ptr<MazeUnit> StartPoint;
    std::shared_ptr<MazeUnit> FinishPoint;
    std::shared_ptr<MazeUnit> CurrentPoint;
    std::map<int32_t, std::shared_ptr<MazeUnit>> OpenMap;
    std::deque<std::vector<std::shared_ptr<Process>>> ProcessStack;
    std::vector<std::vector<std::shared_ptr<MazeUnit>>> MazeMap;


};

#endif // MAZE_H
