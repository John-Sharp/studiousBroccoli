#include "../studiousBroccoli.h"
#include "../utils/testMallocTracker/testMallocTracker.h"
#include <stdio.h>
#include <stdlib.h>

bool mallocFailsFirstTime()
{
    return true;
}

// create SBStateMachine and then destroy and verify that bytes allocated
// equals bytes freed
// verify that when memory allocation fails null pointer is
// returned from create function
void createSBStateMachineTest()
{
    allocatorTrackerReset();
    SBStateMachine * sm = createSBStateMachine(NULL);

    SBStateMachineDestroy(sm);

    if (allocatorTrackerGetBytesAllocated() != allocatorTrackerGetBytesFreed())
    {
        printf("FAILED: createSBStateMachineTest(), for straightforward create then delete\n\n");
        exit(1);
    }

    mallocShouldFail = mallocFailsFirstTime;

    allocatorTrackerReset();
    sm = createSBStateMachine(NULL);

    if (sm)
    {
        printf("FAILED: createSBStateMachineTest(), when malloc fails to allocate memory,"
                " returns non-null pointer\n\n");
        exit(1);
    }
    mallocShouldFail = NULL;
}

juint testTransitionFunction(SBStateMachine * stateMachine, juint token)
{
    return 4;
}

void addOneState()
{
    allocatorTrackerReset();
    SBStateMachine * sm = createSBStateMachine(NULL);
    juint testState = 4;
    juint testToken = 8;

    juint ret =  SBStateMachineAddState(sm, testState, 1, testToken, 
        testTransitionFunction);

    if (ret != SB_STATE_MACHINE_OK)
    {
        printf("FAILED: addOneState(), fails to return `SB_STATE_MACHINE_OK` "
                "after adding one state\n\n");
        exit(1);
    }

    SBStateMachineDestroy(sm);

    if (allocatorTrackerGetBytesAllocated() != allocatorTrackerGetBytesFreed())
    {
        printf("FAILED: addOneState(), when adding one state "
                "memory allocated != memory freed\n\n");
        exit(1);
    }
}

void addOneStateInitialMallocFails()
{
    allocatorTrackerReset();
    SBStateMachine * sm = createSBStateMachine(NULL);

    mallocShouldFail = mallocFailsFirstTime;
    juint testState = 4;
    juint testToken = 8;

    juint ret =  SBStateMachineAddState(sm, testState, 1, testToken, 
        testTransitionFunction);

    if (ret != SB_STATE_MACHINE_MALLOC_ERROR)
    {
        printf("FAILED: addOneStateInitialMallocFails(), fails to return `SB_STATE_MACHINE_MALLOC_ERROR` "
                "when initial malloc fails\n\n");
        exit(1);
    }

    SBStateMachineDestroy(sm);

    if (allocatorTrackerGetBytesAllocated() != allocatorTrackerGetBytesFreed())
    {
        printf("FAILED: addOneStateInitialMallocFails(), when adding one state "
                "when initial malloc fails memory allocated != memory freed\n\n");
        exit(1);
    }
    mallocShouldFail = NULL;
}

bool mallocFailsSecondTime()
{
    static juint i = 0;
    i++;
    if (i==2)
    {
        return true;
    }

    return false;
}

void addOneStateSecondMallocFails()
{
    allocatorTrackerReset();
    SBStateMachine * sm = createSBStateMachine(NULL);

    mallocShouldFail = mallocFailsSecondTime;
    juint testState = 4;
    juint testToken = 8;

    juint ret =  SBStateMachineAddState(sm, testState, 1, testToken, 
        testTransitionFunction);

    if (ret != SB_STATE_MACHINE_MALLOC_ERROR)
    {
        printf("FAILED: addOneStateSecondMallocFails(), fails to return `SB_STATE_MACHINE_MALLOC_ERROR` "
                "when second malloc fails\n\n");
        exit(1);
    }

    SBStateMachineDestroy(sm);

    if (allocatorTrackerGetBytesAllocated() != allocatorTrackerGetBytesFreed())
    {
        printf("FAILED: addOneStateSecondMallocFails(), when adding one state "
                "when second malloc fails memory allocated != memory freed\n\n");
        exit(1);
    }
    mallocShouldFail = NULL;
}

void addThreeStates()
{
    allocatorTrackerReset();
    SBStateMachine * sm = createSBStateMachine(NULL);

    juint i;
    for (i = 1; i <= 3; i++)
    {
        juint testToken = 6;
        juint ret =  SBStateMachineAddState(sm, i, 1, testToken, 
                testTransitionFunction);

        if (ret != SB_STATE_MACHINE_OK)
        {
            printf("FAILED: addThreeStates(), fails to return `SB_STATE_MACHINE_OK` "
                    "when adding state %u out of 3\n\n", i);
            exit(1);
        }
    }

    SBStateMachineDestroy(sm);

    if (allocatorTrackerGetBytesAllocated() != allocatorTrackerGetBytesFreed())
    {
        printf("FAILED: addThreeStates(), when adding 3 states "
                "memory allocated != memory freed\n\n");
        exit(1);
    }
}

void setGetState()
{
    SBStateMachine * sm = createSBStateMachine(NULL);
    juint recvdState;

    juint ret = SBStateMachineGetCurrentState(sm, &recvdState);

    if (ret != SB_STATE_MACHINE_NOT_INITIALIZED)
    {
        printf("FAILED: setGetState(), when getting current state "
                "when no state has been set incorrect error msg returned\n\n");
        exit(1);
    }

    ret = SBStateMachineSetCurrentState(sm, 22);
    if (ret != SB_STATE_MACHINE_STATE_NOT_FOUND)
    {
        printf("FAILED: setGetState(), when setting current state "
                "that state machine does not know about,"
                " incorrect error msg returned\n\n");
        exit(1);
    }


    juint testStateId = 4;
    juint testToken = 8;
    SBStateMachineAddState(sm, testStateId, 1, testToken, 
        testTransitionFunction);
    ret = SBStateMachineSetCurrentState(sm, testStateId);

    if (ret != SB_STATE_MACHINE_OK)
    {
        printf("FAILED: setGetState(), when setting current state "
                " incorrect return msg returned\n\n");
        exit(1);
    }

    ret = SBStateMachineGetCurrentState(sm, &recvdState);
    if (ret != SB_STATE_MACHINE_OK)
    {
        printf("FAILED: setGetState(), when getting current state "
                "after initial state has been set, incorrect return"
                " msg returned\n\n");
        exit(1);
    }

    if (recvdState != testStateId)
    {
        printf("FAILED: setGetState(), when getting current state "
                "after initial state has been set, incorrect stateId"
                " returned\n\n");
        exit(1);
    }

    SBStateMachineDestroy(sm);
}

juint testTransitionFunctionRetVal = 0;
struct transitionFunctionStateCallStatus
{
    SBStateMachineTransitionFunction addrCalled;
    SBStateMachine * stateMachineArg;
    juint tokenArg;
};

struct transitionFunctionStateCallStatus transitionFunctionOneStateCallStatus = {
    .addrCalled = NULL,
    .stateMachineArg = NULL,
    .tokenArg =0
};

juint testTransitionFunctionOneState(SBStateMachine * stateMachine, juint token)
{
    transitionFunctionOneStateCallStatus.addrCalled = testTransitionFunctionOneState;
    transitionFunctionOneStateCallStatus.stateMachineArg = stateMachine;
    transitionFunctionOneStateCallStatus.tokenArg = token;
    return testTransitionFunctionRetVal;
}

void testOneStateStateMachine()
{
    juint context = 8897;
    SBStateMachine * sm = createSBStateMachine(&context);

    juint stateId = 4;
    testTransitionFunctionRetVal = stateId;
    juint testToken = 6;
    SBStateMachineAddState(sm, stateId, 1, testToken, 
            testTransitionFunctionOneState);
    SBStateMachineSetCurrentState(sm, stateId);

    juint recvdState = 567;
    juint ret = SBStateMachineProcessInput(sm, 6, &recvdState);

    if (ret != SB_STATE_MACHINE_OK)
    {
        printf("FAILED: testOneStateStateMachine(), when process "
                "valid input error code returned\n\n");
        exit(1);
    }

    if (recvdState != stateId)
    {
        printf("FAILED: testOneStateStateMachine(), when process "
                "valid input current state not set correctly\n\n");
        exit(1);
    }

    if (transitionFunctionOneStateCallStatus.addrCalled != testTransitionFunctionOneState)
    {
        printf("FAILED: testOneStateStateMachine(), when process "
                "valid input transition function not called\n\n");
        exit(1);
    }

    if (transitionFunctionOneStateCallStatus.stateMachineArg != sm)
    {
        printf("FAILED: testOneStateStateMachine(), when process "
                "valid input transition function not called with "
                "correct state machine argument\n\n");
        exit(1);
    }

    if (transitionFunctionOneStateCallStatus.stateMachineArg->context != &context)
    {
        printf("FAILED: testOneStateStateMachine(), when process "
                "valid input transition function not called with "
                "correct state machine context argument\n\n");
        exit(1);
    }

    if (transitionFunctionOneStateCallStatus.tokenArg != testToken)
    {
        printf("FAILED: testOneStateStateMachine(), when process "
                "valid input transition function not called with "
                "correct token argument\n\n");
        exit(1);
    }

    recvdState = 567;
    SBStateMachineGetCurrentState(sm, &recvdState);
    if (recvdState != stateId)
    {
        printf("FAILED: testOneStateStateMachine(), current "
                "state can't be received\n\n");
        exit(1);
    }

    ret = SBStateMachineProcessInput(sm, 6, NULL);
    recvdState = 567;
    SBStateMachineGetCurrentState(sm, &recvdState);
    if (recvdState != stateId)
    {
        printf("FAILED: testOneStateStateMachine(), current "
                "state can't be received after processing "
                "input with NULL third argument\n\n");
        exit(1);
    }

    recvdState = 567;
    ret = SBStateMachineProcessInput(sm, 8, &recvdState);
    if (ret != SB_STATE_MACHINE_TRANSITION_FN_NOT_FOUND)
    {
        printf("FAILED: testOneStateStateMachine(), when processing "
                "unrecognised token incorrect error code returned\n\n");
        exit(1);
    }
    if (recvdState != stateId)
    {
        printf("FAILED: testOneStateStateMachine(), current state "
                "not set when unrecognised token processed\n\n");
        exit(1);
    }

    testTransitionFunctionRetVal = 5;
    recvdState = 567;
    ret = SBStateMachineProcessInput(sm, 6, &recvdState);
    if (ret != SB_STATE_MACHINE_TRANSITION_FN_ATTEMPTS_TRANSITION_TO_UKNOWN_STATE)
    {
        printf("FAILED: testOneStateStateMachine(), when transition function"
                " attempts transition to unknown state incorrect error code "
                "returned\n\n");
        exit(1);
    }

    if (recvdState != stateId)
    {
        printf("FAILED: testOneStateStateMachine(), when transition function"
                " attempts transition to unknown state, state not"
                " returned\n\n");
        exit(1);
    }

    SBStateMachineDestroy(sm);
}

void attemptAddSameStateTwice()
{
    SBStateMachine * sm = createSBStateMachine(NULL);

    transitionFunctionOneStateCallStatus.addrCalled = NULL,
    transitionFunctionOneStateCallStatus.stateMachineArg = NULL,
    transitionFunctionOneStateCallStatus.tokenArg = 0;
    testTransitionFunctionRetVal = 4;

    juint stateId = 4;
    juint testToken = 6;
    SBStateMachineAddState(sm, stateId, 1, testToken, 
            testTransitionFunctionOneState);
    
    juint ret = SBStateMachineAddState(sm, stateId, 1, testToken+1, 
            testTransitionFunction);
    
    if (ret != SB_STATE_MACHINE_STATE_ALREADY_EXISTS)
    {
        printf("FAILED: attemptAddSameStateTwice(), incorrect return code"
                "returned when attempting to add state with ID that machine"
                "already has that state\n\n");
        exit(1);
    }

    SBStateMachineSetCurrentState(sm, stateId);
    SBStateMachineProcessInput(sm, testToken, NULL);

    if (transitionFunctionOneStateCallStatus.addrCalled != testTransitionFunctionOneState)
    {
        printf("FAILED: attemptAddSameStateTwice(), correct transition function"
                " not called after adding same state twice\n\n");
        exit(1);
    }

    SBStateMachineDestroy(sm);
}

void attemptAddSameTokenTransitionFunctionTwice()
{
    allocatorTrackerReset();
    SBStateMachine * sm = createSBStateMachine(NULL);

    transitionFunctionOneStateCallStatus.addrCalled = false,
    transitionFunctionOneStateCallStatus.stateMachineArg = NULL,
    transitionFunctionOneStateCallStatus.tokenArg = 0;
    testTransitionFunctionRetVal = 4;

    juint stateId = 4;
    juint testToken = 6;
    juint ret = SBStateMachineAddState(sm, stateId, 3,
            testToken, testTransitionFunctionOneState,
            testToken+1, testTransitionFunction,
            testToken, testTransitionFunction);

    if (ret != SB_STATE_MACHINE_TWO_TRANSITION_FUNCTIONS_FOR_SAME_TOKEN)
    {
        printf("FAILED: attempattemptAddSameTokenTransitionFunctionTwice(), "
                "adding state with two transition functions for same token "
                "does not result in correct error code\n\n");
        exit(1);
    }

    ret = SBStateMachineSetCurrentState(sm, stateId);

    if (ret != SB_STATE_MACHINE_STATE_NOT_FOUND)
    {
        printf("FAILED: attempattemptAddSameTokenTransitionFunctionTwice(), "
                "after adding state with two transition functions for same"
                " token, state is still added to state machine\n\n");
        exit(1);
    }

    SBStateMachineDestroy(sm);

    if (allocatorTrackerGetBytesAllocated() != allocatorTrackerGetBytesFreed())
    {
        printf("FAILED: attempattemptAddSameTokenTransitionFunctionTwice(), "
                "memory leaked after destroy\n\n");
        exit(1);
    }

}

// transition functions for the subsequent test
juint aTokenDetectedInStateA(SBStateMachine * stateMachine, juint token)
{
    transitionFunctionOneStateCallStatus.addrCalled = aTokenDetectedInStateA;
    transitionFunctionOneStateCallStatus.stateMachineArg = stateMachine;
    transitionFunctionOneStateCallStatus.tokenArg = token;
    return 'B';
}
juint bTokenDetectedInStateA(SBStateMachine * stateMachine, juint token)
{
    transitionFunctionOneStateCallStatus.addrCalled = bTokenDetectedInStateA;
    transitionFunctionOneStateCallStatus.stateMachineArg = stateMachine;
    transitionFunctionOneStateCallStatus.tokenArg = token;
    return 'C';
}

juint aTokenDetectedInStateB(SBStateMachine * stateMachine, juint token)
{
    transitionFunctionOneStateCallStatus.addrCalled = aTokenDetectedInStateB;
    transitionFunctionOneStateCallStatus.stateMachineArg = stateMachine;
    transitionFunctionOneStateCallStatus.tokenArg = token;
    return 'A';
}
juint bTokenDetectedInStateB(SBStateMachine * stateMachine, juint token)
{
    transitionFunctionOneStateCallStatus.addrCalled = bTokenDetectedInStateB;
    transitionFunctionOneStateCallStatus.stateMachineArg = stateMachine;
    transitionFunctionOneStateCallStatus.tokenArg = token;
    return 'C';
}

juint aTokenDetectedInStateC(SBStateMachine * stateMachine, juint token)
{
    transitionFunctionOneStateCallStatus.addrCalled = aTokenDetectedInStateC;
    transitionFunctionOneStateCallStatus.stateMachineArg = stateMachine;
    transitionFunctionOneStateCallStatus.tokenArg = token;
    return 'B';
}
juint bTokenDetectedInStateC(SBStateMachine * stateMachine, juint token)
{
    transitionFunctionOneStateCallStatus.addrCalled = bTokenDetectedInStateC;
    transitionFunctionOneStateCallStatus.stateMachineArg = stateMachine;
    transitionFunctionOneStateCallStatus.tokenArg = token;
    return 'B';
}

// constructs and tests a three state state machine with two inputs:
// A,a->B ; b->C
// B,a->A ; b->C
// C,a->B ; b->B
void testThreeStateStateMachine()
{
    allocatorTrackerReset();
    SBStateMachine * sm = createSBStateMachine(NULL);

    // definition of tokens
    juint aToken = 'a';
    juint bToken = 'b';

    // definition of state ids
    juint AStateId = 'A';
    juint BStateId = 'B';
    juint CStateId = 'C';

    juint ret = SBStateMachineAddState(sm, AStateId, 2,
            aToken, aTokenDetectedInStateA,
            bToken, bTokenDetectedInStateA);
    if (ret != SB_STATE_MACHINE_OK)
    {
        printf("FAILED: testThreeStateStateMachine(), fails to return `SB_STATE_MACHINE_OK` "
                "after adding state A\n\n");
        exit(1);
    }

    ret = SBStateMachineAddState(sm, BStateId, 2,
            aToken, aTokenDetectedInStateB,
            bToken, bTokenDetectedInStateB);
    if (ret != SB_STATE_MACHINE_OK)
    {
        printf("FAILED: testThreeStateStateMachine(), fails to return `SB_STATE_MACHINE_OK` "
                "after adding state B\n\n");
        exit(1);
    }

    ret = SBStateMachineAddState(sm, CStateId, 2,
            aToken, aTokenDetectedInStateC,
            bToken, bTokenDetectedInStateC);
    if (ret != SB_STATE_MACHINE_OK)
    {
        printf("FAILED: testThreeStateStateMachine(), fails to return `SB_STATE_MACHINE_OK` "
                "after adding state C\n\n");
        exit(1);
    }

    ret = SBStateMachineSetCurrentState(sm, AStateId);

    if (ret != SB_STATE_MACHINE_OK)
    {
        printf("FAILED: testThreeStateStateMachine(), when setting current state "
                "incorrect return msg returned\n\n");
        exit(1);
    }

    juint currentState;
    ret = SBStateMachineProcessInput(sm, 'b', &currentState);
    if (currentState != CStateId)
    {
        printf("FAILED: testThreeStateStateMachine(), when processing 'b' "
                "in state 'A' incorrect state reached\n\n");
        exit(1);
    }
    if (transitionFunctionOneStateCallStatus.addrCalled != bTokenDetectedInStateA)
    {
        printf("FAILED: testThreeStateStateMachine(), when processing 'b' "
                "in state 'A' incorrect transition function called\n\n");
        exit(1);
    }

    ret = SBStateMachineProcessInput(sm, 'a', &currentState);
    if (currentState != BStateId)
    {
        printf("FAILED: testThreeStateStateMachine(), when processing 'a' "
                "in state 'C' incorrect state reached\n\n");
        exit(1);
    }
    if (transitionFunctionOneStateCallStatus.addrCalled != aTokenDetectedInStateC)
    {
        printf("FAILED: testThreeStateStateMachine(), when processing 'a' "
                "in state 'C' incorrect transition function called\n\n");
        exit(1);
    }

    ret = SBStateMachineProcessInput(sm, 'b', &currentState);
    if (currentState != CStateId)
    {
        printf("FAILED: testThreeStateStateMachine(), when processing 'b' "
                "in state 'B' incorrect state reached\n\n");
        exit(1);
    }
    if (transitionFunctionOneStateCallStatus.addrCalled != bTokenDetectedInStateB)
    {
        printf("FAILED: testThreeStateStateMachine(), when processing 'b' "
                "in state 'B' incorrect transition function called\n\n");
        exit(1);
    }

    ret = SBStateMachineProcessInput(sm, 'b', &currentState);
    if (currentState != BStateId)
    {
        printf("FAILED: testThreeStateStateMachine(), when processing 'b' "
                "in state 'C' incorrect state reached\n\n");
        exit(1);
    }
    if (transitionFunctionOneStateCallStatus.addrCalled != bTokenDetectedInStateC)
    {
        printf("FAILED: testThreeStateStateMachine(), when processing 'b' "
                "in state 'C' incorrect transition function called\n\n");
        exit(1);
    }

    ret = SBStateMachineProcessInput(sm, 'a', &currentState);
    if (currentState != AStateId)
    {
        printf("FAILED: testThreeStateStateMachine(), when processing 'a' "
                "in state 'B' incorrect state reached\n\n");
        exit(1);
    }
    if (transitionFunctionOneStateCallStatus.addrCalled != aTokenDetectedInStateB)
    {
        printf("FAILED: testThreeStateStateMachine(), when processing 'a' "
                "in state 'B' incorrect transition function called\n\n");
        exit(1);
    }

    SBStateMachineDestroy(sm);

    if (allocatorTrackerGetBytesAllocated() != allocatorTrackerGetBytesFreed())
    {
        printf("FAILED: testThreeStateStateMachine(), when adding one state "
                "memory allocated != memory freed\n\n");
        exit(1);
    }
}

int main()
{
    createSBStateMachineTest();
    addOneState();
    addOneStateInitialMallocFails();
    addOneStateSecondMallocFails();
    addThreeStates();
    setGetState();
    testOneStateStateMachine();
    attemptAddSameStateTwice();
    attemptAddSameTokenTransitionFunctionTwice();
    testThreeStateStateMachine();
}
