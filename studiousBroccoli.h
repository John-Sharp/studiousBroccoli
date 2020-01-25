#ifndef STUDIOUS_BROCOLLI_H
#define STUDIOUS_BROCOLLI_H

#include <jTypes.h>

enum
{
    SB_STATE_MACHINE_OK,
    SB_STATE_MACHINE_MALLOC_ERROR,
    SB_STATE_MACHINE_STATE_NOT_FOUND,
    SB_STATE_MACHINE_NOT_INITIALIZED,
    SB_STATE_MACHINE_TRANSITION_FN_NOT_FOUND,
    SB_STATE_MACHINE_TRANSITION_FN_ATTEMPTS_TRANSITION_TO_UKNOWN_STATE,
    SB_STATE_MACHINE_STATE_ALREADY_EXISTS,
    SB_STATE_MACHINE_TWO_TRANSITION_FUNCTIONS_FOR_SAME_TOKEN
};

typedef struct SBStateMachine {
    void * context;
} SBStateMachine;

typedef juint (*SBStateMachineTransitionFunction)(SBStateMachine * stateMachine, juint token);

SBStateMachine * createSBStateMachine(void * context);
void SBStateMachineDestroy(SBStateMachine * stateMachine);

juint SBStateMachineAddState(
        SBStateMachine * stateMachine,
        juint stateId,
        juint numberOfTransitionFns, ...);

juint SBStateMachineProcessInput(SBStateMachine * stateMachine, juint token, juint * currentState);
juint SBStateMachineGetCurrentState(const SBStateMachine * stateMachine, juint * currentStateId);
juint SBStateMachineSetCurrentState(SBStateMachine * stateMachine, juint stateId);

#endif
