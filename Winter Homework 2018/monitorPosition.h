#ifndef monitorPosition_h
#define monitorPosition_h

void appendPosition(double position);
void appendRemotePosition(double position);
double takePosition();
double takeRemotePosition();

void initMonitorPosition();
void closeMonitorPosition();

#endif