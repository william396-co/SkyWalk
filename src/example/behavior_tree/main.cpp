#include <thread>
#include <chrono>
#include <memory>

#include "AIContext.h"
#include "CrossDoor.h"
#include "utils/print.h"
#include "domain/brain.h"

using namespace brain;

struct IsDoorClosed : public brain::Leaf
{
public:
    virtual ~IsDoorClosed() {}
    virtual brain::Status execute( brain::Context * ctx ) override
    {
        return static_cast<AIContext *>( ctx )->isDoorClosed();
    }
};

struct PassThroughDoor : public brain::Leaf
{
public:
    virtual ~PassThroughDoor() {}
    virtual brain::Status execute( brain::Context * ctx ) override
    {
        return static_cast<AIContext *>( ctx )->passThroughDoor();
    }
};

class OpenDoor : public brain::Leaf
{
public:
    virtual ~OpenDoor() {}
    virtual brain::Status execute( brain::Context * ctx ) override
    {
        return static_cast<AIContext *>( ctx )->openDoor();
    }
};

class PickLock : public brain::Leaf
{
public:
    virtual ~PickLock() {}
    virtual brain::Status execute( brain::Context * ctx ) override
    {
        return static_cast<AIContext *>( ctx )->pickLock();
    }
};

class SmashDoor : public brain::Leaf
{
public:
    virtual ~SmashDoor() {}
    virtual brain::Status execute( brain::Context * ctx ) override
    {
        return static_cast<AIContext *>( ctx )->smashDoor();
    }
};

std::unique_ptr<brain::Tree> make_brain( int num_attemps )
{
    auto bt = 
    brain::Builder()
        .composite<brain::Sequence>()
            .composite<brain::Selector>()
                .decorator<brain::Inverter>()
                    .leaf<IsDoorClosed>()
                .end()
                .composite<brain::Selector>()
                    .leaf<OpenDoor>()
                    .decorator<brain::RetryUntilSuccessful>(num_attemps)
                        .leaf<PickLock>()
                    .end()
                    .leaf<SmashDoor>()
                .end()
            .end()
            .leaf<PassThroughDoor>()
            .end()
        .build( nullptr );
    return std::unique_ptr<brain::Tree>( bt );
}

std::unique_ptr<brain::Tree> make_brain2( int num )
{
    auto bt = brain::Builder()
        .composite<brain::Selector>()
            .leaf<OpenDoor>()
            .decorator<brain::RetryUntilSuccessful>(num)
                .leaf<PickLock>()
            .end()
            .leaf<SmashDoor>()
        .end()
        .build( nullptr );
    return std::unique_ptr<brain::Tree>( bt );
}

void test_bt1()
{
    println( __FUNCTION__ );
    auto bt = make_brain( 7 );
    CrossDoor cd;
    std::unique_ptr<Context> ctx = std::make_unique<AIContext>( &cd );

    while ( true ) {
        bt->update( ctx.get() );
        std::this_thread::sleep_for( std::chrono::milliseconds { 10 } );
        break;
    }
}

void test_bt2()
{
    println( __FUNCTION__ );
    auto  bt = make_brain2( 5 ) ;
    CrossDoor cd;
    std::unique_ptr<Context> ctx = std::make_unique<AIContext>( &cd );

    while ( true ) {
        bt->update( ctx.get() );
        std::this_thread::sleep_for( std::chrono::milliseconds { 10 } );
        break;
    }
}

int main()
{
    std::cout << "open door\n";

 //   test_bt1();
    test_bt2();

    return 0;
}
