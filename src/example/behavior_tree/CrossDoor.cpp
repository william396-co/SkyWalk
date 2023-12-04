#include "CrossDoor.h"
#include "utils/print.h"

constexpr auto attemp_limit = 10;

bool CrossDoor::isDoorClosed()
{
    _door_open ? printlnEx( "Door is Open" ) : printlnEx( "Door is Closed" );

    return !_door_open;
}

bool CrossDoor::passThroughDoor()
{
    !_door_open ? printlnEx( __FUNCTION__, " Failure" ) : printlnEx( __FUNCTION__, " Success" );
    return _door_open;
}

bool CrossDoor::pickLock()
{
    if ( ++_pick_attemps > attemp_limit ) {
        _door_locked = false;
        _door_open = true;
        printlnEx( __FUNCTION__, " Success " );
        return true;
    }
    printlnEx( __FUNCTION__, " : ", _pick_attemps, " times" );
    return false;
}

bool CrossDoor::openDoor()
{
    _door_locked ? printlnEx( __FUNCTION__, " Failure" ) : printlnEx( __FUNCTION__, " Success" );
    return !_door_locked;
}

bool CrossDoor::smashDoor()
{
    _door_locked = false;
    _door_open = true;
    printlnEx( __FUNCTION__, " Success" );
    return true;
}
