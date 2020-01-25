// studiousBroccoli

#include "studiousBroccoli.h"
#include <stdarg.h>

typedef struct transitionFnNode {
    juint token;
    SBStateMachineTransitionFunction function;
} transitionFnNode;

#include "listHeaders/transitionFnNodeList.h"
#include "listCode/transitionFnNodeList.inc"

typedef struct state {
    juint stateId;
    transitionFnNodeList * transitionFunctions;
} state;

#include "listHeaders/stateList.h"
#include "listCode/stateList.inc"

typedef struct SBStateMachineInt {
    SBStateMachine stateMachine;

    stateList * states;
    state * currentState;
} SBStateMachineInt;

SBStateMachine * createSBStateMachine(void * context)
{
    SBStateMachineInt * ret = malloc(sizeof(*ret));
    
    if (!ret)
    {
        return NULL;
    }

    ret->stateMachine.context = context;
    ret->states = NULL;
    ret->currentState = NULL;

    return &ret->stateMachine;
}

void cleanUpTfNodes(state * s)
{
    transitionFnNodeList * nodeList;

    while ((nodeList = s->transitionFunctions))
    {
        s->transitionFunctions = s->transitionFunctions->next;
        free(nodeList->val);
        free(nodeList);
    }
}

void SBStateMachineDestroy(SBStateMachine * stateMachineExt)
{
    SBStateMachineInt * stateMachine = (SBStateMachineInt *)stateMachineExt;

    stateList * sList;

    while ((sList = stateMachine->states))
    {
        cleanUpTfNodes(sList->val);
        stateMachine->states = stateMachine->states->next;
        free(sList->val);
        free(sList);
    }

    free(stateMachineExt);

    return;
}

bool stateIdCmp(const state * a, const state * b)
{
    if (a->stateId == b->stateId)
    {
        return true;
    }

    return false;
}


juint SBStateMachineAddState(
        SBStateMachine * stateMachineExt,
        juint stateId,
        juint numberOfTransitionFns, ...)
{
    SBStateMachineInt * stateMachine = (SBStateMachineInt *)stateMachineExt;

    state targetS = {.stateId = stateId, .transitionFunctions = NULL};
    if (stateListSearch(stateMachine->states, &targetS, stateIdCmp))
    {
        return SB_STATE_MACHINE_STATE_ALREADY_EXISTS;
    }

    state * thisState = malloc(sizeof(*thisState));
    if (!thisState)
    {
        return SB_STATE_MACHINE_MALLOC_ERROR;
    }
    thisState->transitionFunctions = NULL;
    thisState->stateId = stateId;

    juint i;
    va_list ap;
    va_start(ap, numberOfTransitionFns);
    for (i = 0 ;i < numberOfTransitionFns; i++)
    {
        transitionFnNode * tfNode = malloc(sizeof(*tfNode));
        if (!tfNode)
        {
            cleanUpTfNodes(thisState);
            free(thisState);
            va_end(ap);
            return SB_STATE_MACHINE_MALLOC_ERROR;
        }
        tfNode->token = va_arg(ap, juint);
        tfNode->function = va_arg(ap, SBStateMachineTransitionFunction);
        thisState->transitionFunctions = transitionFnNodeListAdd(
                thisState->transitionFunctions,
                tfNode);
    }
    va_end(ap);

    transitionFnNodeList * tl, * tm;
    for (tl = thisState->transitionFunctions; tl != NULL; tl = tl->next)
    {
        for (tm = tl->next; tm != NULL; tm = tm->next)
        {
            if (tl->val->token == tm->val->token)
            {
                cleanUpTfNodes(thisState);
                free(thisState);
                return SB_STATE_MACHINE_TWO_TRANSITION_FUNCTIONS_FOR_SAME_TOKEN;
            }
        }
    }

    stateMachine->states = stateListAdd(stateMachine->states, thisState);

    return SB_STATE_MACHINE_OK;
}

juint SBStateMachineSetCurrentState(SBStateMachine * stateMachineExt, juint stateId)
{
    SBStateMachineInt * sm = (SBStateMachineInt *)stateMachineExt;
    state target = {.stateId = stateId, .transitionFunctions = NULL};
    stateList * sl = stateListSearch(sm->states, &target, stateIdCmp);

    if (!sl)
    {
        return SB_STATE_MACHINE_STATE_NOT_FOUND;
    }

    sm->currentState = sl->val;

    return SB_STATE_MACHINE_OK;
}

juint SBStateMachineGetCurrentState(const SBStateMachine * stateMachineExt, juint * currentState)
{
    SBStateMachineInt * sm = (SBStateMachineInt *)stateMachineExt;

    if (!sm->currentState)
    {
        return SB_STATE_MACHINE_NOT_INITIALIZED;
    }

    *currentState = sm->currentState->stateId;
    return SB_STATE_MACHINE_OK;
}

bool tokenCmp(const transitionFnNode * a, const transitionFnNode * b)
{
    if (a->token == b->token)
    {
        return true;
    }

    return false;
}

juint SBStateMachineProcessInput(SBStateMachine * stateMachineExt, juint token, juint * currentState)
{
    SBStateMachineInt * sm = (SBStateMachineInt *)stateMachineExt;

    transitionFnNode target = {.token = token, .function = NULL};
    transitionFnNodeList * tl = transitionFnNodeListSearch(
            sm->currentState->transitionFunctions, &target, tokenCmp);

    if (!tl)
    {
        if (currentState)
        {
            *currentState = sm->currentState->stateId;
        }
        return SB_STATE_MACHINE_TRANSITION_FN_NOT_FOUND;
    }

    juint nextStateId = tl->val->function(stateMachineExt, token);
    juint ret = SB_STATE_MACHINE_OK;
    if (SBStateMachineSetCurrentState(stateMachineExt, nextStateId) \
            == SB_STATE_MACHINE_STATE_NOT_FOUND)
    {
        ret = SB_STATE_MACHINE_TRANSITION_FN_ATTEMPTS_TRANSITION_TO_UKNOWN_STATE;
    }

    if (currentState)
    {
        *currentState = sm->currentState->stateId;
    }

    return ret;
}

