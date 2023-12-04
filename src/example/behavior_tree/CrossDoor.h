#pragma once

class CrossDoor
{
public:
    CrossDoor() = default;
    ~CrossDoor() = default;

    // SUCCESS if _door_open == true
    bool isDoorClosed();
    // SUCCESS if _door_open == true
    bool passThroughDoor();
    // After 3 attempts, will open a locked door
    bool pickLock();
    // FAILURE if door locked
    bool openDoor();
    // WILL always open a door
    bool smashDoor();

private:
    bool _door_open { false };
    bool _door_locked { true };
    int _pick_attemps { 0 };
};
