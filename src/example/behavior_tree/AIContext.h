#pragma once

#include "domain/brain.h"

class CrossDoor;
class AIContext : public brain::Context
{
public:
    explicit AIContext( CrossDoor * cd );
    ~AIContext();

    // SUCCESS if _door_open == true
    brain::Status isDoorClosed();
    // SUCCESS if _door_open == true
    brain::Status passThroughDoor();
    // After 3 attempts, will open a locked door
    brain::Status pickLock();
    // FAILURE if door locked
    brain::Status openDoor();
    // WILL always open a door
    brain::Status smashDoor();

private:
    CrossDoor * m_CrossDoor;
};
