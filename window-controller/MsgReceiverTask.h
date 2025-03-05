#ifndef __MSG_RECEIVER_TASK__
#define __MSG_RECEIVER_TASK__

#include "Task.h"
#include "MsgService.h"

class MsgReceiverTask : public Task {
    MsgServiceClass& msgService;
    bool& manualMode;
    int& windowPosition;
    float& currentTemperature;

public:
    MsgReceiverTask(MsgServiceClass& service, bool& mode, int& pos, float& temp);
    void init(int period) override;
    void tick() override;
};

#endif
