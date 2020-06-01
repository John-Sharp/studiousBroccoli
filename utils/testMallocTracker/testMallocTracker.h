#ifndef TEST_MALLOC_TRACKER_H
#define TEST_MALLOC_TRACKER_H

#include <jTypes.h>
#include <stddef.h>

void allocatorTrackerReset();
jint allocatorTrackerGetBytesAllocated();
jint allocatorTrackerGetBytesFreed();

#endif
