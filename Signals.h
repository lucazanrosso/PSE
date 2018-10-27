#ifndef Signals_h
#define Signals_h

#include <sys/types.h>
#include "SendingResults.h"

void childSignals(pid_t sendTo, int n, TimeInfo *timeInfo);
void parentSignals(pid_t sendTo, int n, TimeInfo *timeInfo);

#endif
