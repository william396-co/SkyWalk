#include "AIContext.h"
#include <cassert>

#include "CrossDoor.h"

AIContext::AIContext( CrossDoor * cd )
    : m_CrossDoor { cd }
{
    assert( m_CrossDoor != nullptr );
}

AIContext::~AIContext()
{
    m_CrossDoor = nullptr;
}

// SUCCESS if _door_open == true
brain::Status AIContext::isDoorClosed()
{
    return m_CrossDoor->isDoorClosed() ? brain::Status::Success : brain::Status::Failure;
}
// SUCCESS if _door_open == true
brain::Status AIContext::passThroughDoor()
{
    return m_CrossDoor->passThroughDoor() ? brain::Status::Success : brain::Status::Failure;
}
// After 3 attempts, will open a locked door
brain::Status AIContext::pickLock()
{
    return m_CrossDoor->pickLock() ? brain::Status::Success : brain::Status::Failure;
}
// FAILURE if door locked
brain::Status AIContext::openDoor()
{
    return m_CrossDoor->openDoor() ? brain::Status::Success : brain::Status::Failure;
}
// WILL always open a door
brain::Status AIContext::smashDoor()
{
    return m_CrossDoor->smashDoor() ? brain::Status::Success : brain::Status::Failure;
}

