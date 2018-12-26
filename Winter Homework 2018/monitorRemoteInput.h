#ifndef monitorRemoteInput_h
#define monitorRemoteInput_h

void appendRemoteInput(double change);
double takeRemoteInput();
void forceSignalingRemoteInput();
void resetRemoteInput();

void initMonitorRemoteInput();
void closeMonitorRemoteInput();

#endif