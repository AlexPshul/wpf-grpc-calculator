#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    //the calc operations
    typedef enum
    {
        OP_ADD, OP_SUB, OP_MUL, OP_DIV
    } CalcOperations;

    typedef struct
    {
        double a;
        double b;
        CalcOperations operation;
    } OperationInfo;

    typedef enum
    {
        OP_SUCCESS,
        OP_ERROR_DIVIDE_BY_ZERO,
        OP_ERROR_UNKNOWN_OPERATION
    } OperationResultStatus;

    typedef struct
    {
        double calculationResult;
        OperationResultStatus resultStatus; 
    } OperationResult;

    OperationResult Calculate(OperationInfo operationInfo);

    typedef void (*CalcLoadCallback_t)(double rate);
    void RegisterForCallback(CalcLoadCallback_t callback); //for the demo we skip cleaning the callback logic 
#ifdef __cplusplus
}
#endif
