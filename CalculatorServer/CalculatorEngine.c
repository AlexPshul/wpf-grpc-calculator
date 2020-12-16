#include "CalculatorEngine.h"
#include "time.h"
#include <Windows.h>

static double numberOfOperations;
static HANDLE hTimerQueue, hTimer;
static clock_t startTime;

static CalcLoadCallback_t loadCallBack = NULL;
void CreateLoadTimer(int intervalInSeconds);

OperationResult Calculate(OperationInfo operationInfo)
{
    OperationResult result;
    result.resultStatus = OP_SUCCESS;
    result.calculationResult = 0;
    ++numberOfOperations;

    switch (operationInfo.operation)
    {
    case OP_ADD:
        result.calculationResult = operationInfo.a + operationInfo.b;
        break;

    case OP_SUB:
        result.calculationResult = operationInfo.a - operationInfo.b;
        break;

    case OP_MUL:
        result.calculationResult = operationInfo.a * operationInfo.b;
        break;

    case OP_DIV:
        if (operationInfo.b == 0.0)
        {
            result.resultStatus = OP_ERROR_DIVIDE_BY_ZERO;
        }
        else
        {
            result.calculationResult = operationInfo.a / operationInfo.b;
        }
        break;

    default:  // NOLINT(clang-diagnostic-covered-switch-default)
        result.resultStatus = OP_ERROR_UNKNOWN_OPERATION;
    }
    return result;
}

//for the demo we skip cleaning the callback logic
void RegisterForCallback(CalcLoadCallback_t callback, int intervalInSeconds)
{
    loadCallBack = callback;
    CreateLoadTimer(intervalInSeconds);
}

void CALLBACK OnLoadReport(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    const double timeTaken = (clock() - startTime) / (double)CLOCKS_PER_SEC;
    loadCallBack(numberOfOperations / timeTaken);
}

void CreateLoadTimer(int intervalInSeconds)
{
    startTime = clock();
    SECURITY_ATTRIBUTES sa;
    memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);

    hTimerQueue = CreateTimerQueue();
    if (hTimerQueue == NULL)
    {
        //demo, skip error handling
    }

    if (!CreateTimerQueueTimer(&hTimer, hTimerQueue,
        (WAITORTIMERCALLBACK)OnLoadReport, 0, 1000, intervalInSeconds * 1000, WT_EXECUTEDEFAULT))
    {
        //demo, skip error handling
    }

}
