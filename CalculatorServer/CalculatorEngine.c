#include "CalculatorEngine.h"
#include "time.h"
#include <Windows.h>

static double numberOfOperations;
static HANDLE hTimer;
static clock_t startTime;

static CalcLoadCallback_t loadCallBack = NULL;
void CreateLoadTimer(void);

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
void RegisterForCallback(CalcLoadCallback_t callback)
{
    loadCallBack = callback;
    CreateLoadTimer();
}

void APIENTRY OnLoadReport(
    LPVOID lpArgToCompletionRoutine,
    DWORD dwTimerLowValue,
    DWORD dwTimerHighValue
    )
{
    const double timeTaken = (clock() - startTime) / (double)CLOCKS_PER_SEC;
    loadCallBack(numberOfOperations / timeTaken);
}

void CreateLoadTimer()
{
    startTime = clock();
    SECURITY_ATTRIBUTES sa;
    memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);

    hTimer = CreateWaitableTimer(&sa, FALSE, NULL);
    if (hTimer == NULL)
    {
        //demo, skip error handling
    }
    if (!SetWaitableTimer(hTimer, 0, 1000, OnLoadReport, 0, TRUE))
    {
        //demo, skip error handling
    }

}
