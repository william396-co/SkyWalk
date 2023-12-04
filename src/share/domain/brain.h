#pragma once

#include <vector>
#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <utility>

#include "utils/xtime.h"
#include "utils/utils.h"
#include "utils/print.h"

namespace brain {

enum class Status
{
    Invalid,
    Success,
    Failure,
    Running,
};

enum FAILURE_POLICY
{
    FAILED_ON_ONE,
    FAILED_ON_ALL
};
enum SUCCESS_POLICY
{
    SUCCEED_ON_ONE,
    SUCCEED_ON_ALL
};

class Node;
using ChildNodeList = std::vector<Node *>;
using StateList = std::unordered_map<Node *, size_t>;

class Context
{
public:
    Context( bool isdebug = false )
        : m_IsDebug( isdebug ) {}
    ~Context()
    {
        clear();
        m_Debug.clear();
    }

public:
    void clear()
    {
        m_State.clear();
        m_Debug.clear();
    }
    void debug( bool isdebug ) { m_IsDebug = isdebug; }

    std::string print()
    {
        std::string d = m_Debug;
        m_Debug.clear();
        return d;
    }

    size_t get( Node * node ) const
    {
        auto result = m_State.find( node );
        if ( result != m_State.end() ) {
            return result->second;
        }
        return 0;
    }

    void set( Node * node, ssize_t index = -1 )
    {
        auto result = m_State.find( node );
        if ( result != m_State.end() ) {
            if ( index != -1 ) {
                result->second = index;
            } else {
                m_State.erase( result );
            }
        } else if ( index != -1 ) {
            m_State.emplace( node, index );
        }
    }

private:
    friend class Node;
    void print( const std::string & node )
    {
        if ( !m_IsDebug ) {
            return;
        }
        if ( !m_Debug.empty() ) {
            m_Debug += " >> ";
        }
        m_Debug += utils::demangle( node );
    }

    bool m_IsDebug = false;
    std::string m_Debug;
    StateList m_State;
};

class Node
{
public:
    Node() {}
    virtual ~Node() {}
    virtual Status execute( Context * context ) = 0;
    virtual void reset( Status status, Context * context ) { context->set( this ); }

    Status update( Context * context )
    {
#if defined( __DEBUG__ )
        context->print( typeid( *this ).name() );
#endif
        auto status = execute( context );
        if ( status != Status::Running ) {
            reset( status, context );
        }
        return status;
    }
};

class Blackboard
{
public:
    Blackboard() {}
    ~Blackboard() {}
};

class Composite : public Node
{
public:
    Composite()
    {
    }

    virtual ~Composite()
    {
        for ( auto node : m_Child ) {
            delete node;
        }
    }

    virtual void reset( Status status, Context * context )
    {
        Node::reset( status, context );
        for ( auto node : m_Child ) {
            node->reset( status, context );
        }
    }

    bool has() const { return !m_Child.empty(); }
    void add( Node * node ) { m_Child.push_back( node ); }

protected:
    ChildNodeList m_Child;
};

class Decorator : public Node
{
public:
    Decorator()
        : m_Child( nullptr )
    {
    }

    virtual ~Decorator()
    {
        if ( m_Child != nullptr ) {
            delete m_Child;
            m_Child = nullptr;
        }
    }

    virtual void reset( Status status, Context * context )
    {
        Node::reset( status, context );
        if ( m_Child != nullptr ) {
            m_Child->reset( status, context );
        }
    }

    void set( Node * node ) { m_Child = node; }
    bool has() const { return m_Child != nullptr; }

protected:
    Node * m_Child;
};

class Leaf : public Node
{
public:
    Leaf()
        : Leaf( nullptr ) {}
    Leaf( Blackboard * board )
        : m_Blackboard( board ) {}
    virtual ~Leaf() {}

    void blackboard( Blackboard * b ) { m_Blackboard = b; }
    Blackboard * blackboard() const { return m_Blackboard; }

protected:
    Blackboard * m_Blackboard;
};

class Tree
{
public:
    Tree()
        : Tree( nullptr, new Blackboard ) {}

    Tree( Node * node, Blackboard * board )
        : m_Root( node ),
          m_Blackboard( board )
    {
    }
    Tree( Tree const & ) = delete;
    Tree & operator=( Tree const & ) = delete;

    Tree( Tree && other ) noexcept: m_Root { other.m_Root }, m_Blackboard { other.m_Blackboard }
    {
        other.m_Root = nullptr;
        other.m_Blackboard = nullptr;
    }
    Tree & operator=( Tree && other ) noexcept
    {
            delete m_Root;
            delete m_Blackboard;

            m_Root = other.m_Root;
            m_Blackboard = other.m_Blackboard;

            return *this;
    }
    virtual ~Tree()
    {
        delete m_Root;
        delete m_Blackboard;
        println(__FUNCTION__);
    }

public:
    Node * root() const { return m_Root; }
    Blackboard * blackboard() const { return m_Blackboard; }
    Status update( Context * c ) { return m_Root->update( c ); }
    void reset( Node * node, Blackboard * board )
    {
        std::swap( node, m_Root );
        std::swap( board, m_Blackboard );
        delete node;
        delete board;
    }

private:
    Node * m_Root;
    Blackboard * m_Blackboard;
};

template<class Parent> struct DecoratorBuilder;

template<class Parent>
struct CompositeBuilder
{
public:
    CompositeBuilder( Parent * p, Composite * n, Blackboard * b )
        : parent( p ),
          node( n ),
          board( b )
    {
    }

    template<class NodeType, typename... Args>
    CompositeBuilder<Parent> leaf( Args... args )
    {
        auto child = new NodeType( ( args )... );
        assert( child != nullptr );
        child->blackboard( board );
        node->add( child );
        return *this;
    }

    template<class CompositeType, typename... Args>
    CompositeBuilder<CompositeBuilder<Parent>> composite( Args... args )
    {
        auto child = new CompositeType( ( args )... );
        assert( child != nullptr );
        node->add( child );
        return CompositeBuilder<CompositeBuilder<Parent>>( this, (CompositeType *)child, board );
    }

    template<class DecoratorType, typename... Args>
    DecoratorBuilder<CompositeBuilder<Parent>> decorator( Args... args )
    {
        auto child = new DecoratorType( ( args )... );
        assert( child != nullptr );
        node->add( child );
        return DecoratorBuilder<CompositeBuilder<Parent>>( this, (DecoratorType *)child, board );
    }

    Parent & end() { return *parent; }

private:
    Parent * parent = nullptr;
    Composite * node = nullptr;
    Blackboard * board = nullptr;
};

template<class Parent>
struct DecoratorBuilder
{
public:
    DecoratorBuilder( Parent * p, Decorator * n, Blackboard * b )
        : parent( p ),
          node( n ),
          board( b )
    {
    }

    template<class NodeType, typename... Args>
    DecoratorBuilder<Parent> leaf( Args... args )
    {
        auto child = new NodeType( ( args )... );
        assert( child != nullptr );
        child->blackboard( board );
        node->set( child );
        return *this;
    }

    template<class CompositeType, typename... Args>
    CompositeBuilder<DecoratorBuilder<Parent>> composite( Args... args )
    {
        auto child = new CompositeType( ( args )... );
        assert( child != nullptr );
        node->set( child );
        return CompositeBuilder<DecoratorBuilder<Parent>>( this, (CompositeType *)child, board );
    }

    template<class DecoratorType, typename... Args>
    DecoratorBuilder<DecoratorBuilder<Parent>> decorator( Args... args )
    {
        auto child = new DecoratorType( ( args )... );
        assert( child != nullptr );
        node->set( child );
        return DecoratorBuilder<DecoratorBuilder<Parent>>( this, (DecoratorType *)child, board );
    }

    Parent & end() { return *parent; }

private:
    Parent * parent = nullptr;
    Decorator * node = nullptr;
    Blackboard * board = nullptr;
};

struct Builder
{
public:
    template<class NodeType, typename... Args>
    Builder leaf( Args... args )
    {
        root = new NodeType( ( args )... );
        static_cast<NodeType *>( root )->blackboard( board );
        return *this;
    }

    template<class CompositeType, typename... Args>
    CompositeBuilder<Builder> composite( Args... args )
    {
        root = new CompositeType( ( args )... );
        return CompositeBuilder<Builder>( this, static_cast<CompositeType *>( root ), board );
    }

    template<class DecoratorType, typename... Args>
    DecoratorBuilder<Builder> decorator( Args... args )
    {
        root = new DecoratorType( ( args )... );
        return DecoratorBuilder<Builder>( this, static_cast<DecoratorType *>( root ), board );
    }

    Tree * build( Tree * tree = nullptr )
    {
        assert( root != nullptr
            && board != nullptr );
        if ( tree != nullptr ) {
            tree->reset( root, board );
        } else {
            tree = new Tree( root, board );
        }
        return tree;
    }

private:
    Node * root = nullptr;
    Blackboard * board = new Blackboard;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// The Selector composite ticks each child node in order.
// If a child succeeds or runs, the selector returns the same status.
// In the next tick, it will try to run each child in order again.
// If all children fails, only then does the selector fail.
//
// The StatefulSelector composite ticks each child node in order, and remembers what child it prevously tried to tick.
// If a child succeeds or runs, the stateful selector returns the same status.
// In the next tick, it will try to run the next child or start from the beginning again.
// If all children fails, only then does the stateful selector fail.
class Selector : public Composite
{
public:
    Selector( bool with_stateful = false )
        : m_WithStateful( with_stateful )
    {
    }

    virtual Status execute( Context * c )
    {
        assert( has() && "Composite has no children" );

        for ( size_t i = c->get( this ); i < m_Child.size(); ++i ) {
            auto s = m_Child[i]->update( c );
            if ( s != Status::Failure ) {
                c->set( this, i );
                return s;
            }
        }

        return Status::Failure;
    }

    virtual void reset( Status status, Context * c )
    {
        if ( !m_WithStateful
            || status == Status::Failure ) {
            Composite::reset( status, c );
        }
    }

private:
    bool m_WithStateful;
};

// Fallback same with Selector


// The Sequence composite ticks each child node in order.
// If a child fails or runs, the sequence returns the same status.
// In the next tick, it will try to run each child in order again.
// If all children succeeds, only then does the sequence succeed.
//
// The StatefulSequence composite ticks each child node in order, and remembers what child it prevously tried to tick.
// If a child fails or runs, the stateful sequence returns the same status.
// In the next tick, it will try to run the next child or start from the beginning again.
// If all children succeeds, only then does the stateful sequence succeed.
class Sequence : public Composite
{
public:
    Sequence( bool with_stateful = false )
        : m_WithStateful( with_stateful )
    {
    }

    virtual Status execute( Context * c )
    {
        assert( has() && "Composite has no children" );

        for ( size_t i = c->get( this ); i < m_Child.size(); ++i ) {
            auto s = m_Child[i]->update( c );
            if ( s != Status::Success ) {
                c->set( this, i );
                return s;
            }
        }

        return Status::Success;
    }

    virtual void reset( Status status, Context * c )
    {
        if ( !m_WithStateful
            || status == Status::Success ) {
            Composite::reset( status, c );
        }
    }

private:
    bool m_WithStateful;
};

//
class IfElse : public Composite
{
public:
    virtual Status execute( Context * c )
    {
        assert( m_Child.size() == 3 && "IfElse has tree Childs" );

        if ( m_Child[0]->update( c ) == Status::Success ) {
            return m_Child[1]->update( c );
        }

        return m_Child[2]->update( c );
    }
};

// Priority
class Priority : public Composite
{
public:
    virtual Status execute( Context * c )
    {
        Status rs = Status::Failure;
        assert( has() && "Composite has no children" );

        for ( size_t i = 0; i < m_Child.size(); ++i ) {
            auto s = m_Child[i]->update( c );
            if ( i == 0 ) {
                rs = s;
                if ( s == Status::Success ) return s;
            }
        }

        return rs;
    }
};

// Execute behaviors in parallel
// ** There are two policies that control the flow of execution. The first is the policy for failure, and the second is the policy for success.
// For failure, the options are "fail when one child fails" and "fail when all children fail".
// For success, the options are similarly "complete when one child completes", and "complete when all children complete".
class Parallel : public Composite
{
public:
    Parallel( int32_t failc, int32_t succc )
        : m_UsePolicy( false ),
          m_SuccCount( succc ),
          m_FailCount( failc )
    {
    }

    Parallel( FAILURE_POLICY failp = FAILED_ON_ALL, SUCCESS_POLICY succp = SUCCEED_ON_ALL )
        : m_UsePolicy( true ),
          m_SuccCount( 0 ),
          m_FailCount( 0 ),
          m_SuccPolicy( succp ),
          m_FailPolicy( failp )
    {
    }

    virtual Status execute( Context * c )
    {
        assert( has() && "Composite has no children" );

        int32_t succeed_total_count = 0, failed_total_count = 0;
        int32_t failed_min_count = m_FailCount, succeed_min_count = m_SuccCount;

        if ( m_UsePolicy ) {
            if ( m_SuccPolicy != SUCCEED_ON_ALL ) {
                succeed_min_count = 1;
            } else {
                succeed_min_count = m_Child.size();
            }
            if ( m_FailPolicy != FAILED_ON_ALL ) {
                failed_min_count = 1;
            } else {
                failed_min_count = m_Child.size();
            }
        }

        for ( auto node : m_Child ) {
            auto s = node->update( c );

            if ( s == Status::Failure )
                ++failed_total_count;
            else if ( s == Status::Success )
                ++succeed_total_count;
        }

        if ( failed_total_count >= failed_min_count ) return Status::Failure;
        if ( succeed_total_count >= succeed_min_count ) return Status::Success;

        return Status::Running;
    }

protected:
    bool m_UsePolicy;
    int32_t m_SuccCount;
    int32_t m_FailCount;
    SUCCESS_POLICY m_SuccPolicy;
    FAILURE_POLICY m_FailPolicy;
};

// The Runner decorator returns running, regardless of what happends to the child .
class Runner : public Decorator
{
public:
    virtual Status execute( Context * c )
    {
        m_Child->update( c );
        return Status::Running;
    }
};

// The Failer decorator returns failure, regardless of what happens to the child.
class Failer : public Decorator
{
public:
    virtual Status execute( Context * c )
    {
        m_Child->update( c );
        return Status::Failure;
    }
};

// The Succeeder decorator returns success, regardless of what happens to the child.
class Succeeder : public Decorator
{
public:
    virtual Status execute( Context * c )
    {
        m_Child->update( c );
        return Status::Success;
    }
};

// The Inverter decorator inverts the child node's status, i.e. failure becomes success and success becomes failure.
// If the child runs, the Inverter returns the status that it is running too.
class Inverter : public Decorator
{
public:
    virtual Status execute( Context * c )
    {
        auto s = m_Child->update( c );

        if ( s == Status::Success ) {
            return Status::Failure;
        } else if ( s == Status::Failure ) {
            return Status::Success;
        }

        return s;
    }
};

// The Repeater decorator repeats infinitely or to a limit until the child returns success.
class Repeater : public Decorator
{
public:
    Repeater( int32_t limit = 0 )
        : m_Limit( limit )
    {
    }

    virtual Status execute( Context * c )
    {
        int32_t count = c->get( this );

        m_Child->update( c );

        if ( m_Limit > 0
            && ++count == m_Limit ) {
            return Status::Success;
        }

        c->set( this, count );
        return Status::Running;
    }

protected:
    int32_t m_Limit;
};

// The RetryUtilSuccessful decorator
class RetryUntilSuccessful : public Decorator
{
public:
    RetryUntilSuccessful( int32_t attemps = 0 )
        : m_attemps { attemps }
    {
        assert( m_attemps > 0 );
    }
    virtual Status execute( Context * c )
    {
        int num = m_attemps;
        Status ret = Status::Failure;
        do{    
        ret = m_Child->update( c );
            if ( ret == Status::Success ){
                break;
            }
        }while(--num);
        return ret;
    }

protected:
    int32_t m_attemps;
};

// The Duration decorator repeats infinitely or to a limit until the child returns success.
class Duration : public Decorator
{
public:
    Duration( int32_t d )
        : m_Duration( d )
    {
    }

    virtual Status execute( Context * c )
    {
        int64_t expiretime = c->get( this );
        int64_t now = utils::now();

        // child return failure
        auto s = m_Child->update( c );
        if ( s == Status::Failure ) {
            return Status::Success;
        }

        if ( expiretime == 0 ) {
            c->set( this, now + m_Duration );
        } else if ( expiretime < now ) {
            return Status::Success;
        }

        return Status::Running;
    }

protected:
    int32_t m_Duration;
};

// The UntilSuccess decorator repeats until the child returns success and then returns success.
class UntilSuccess : public Decorator
{
public:
    virtual Status execute( Context * c )
    {
        for ( ;; ) {
            auto s = m_Child->update( c );

            if ( s == Status::Success ) {
                return s;
            }
        }

        return Status::Invalid;
    }
};

// The UntilFailure decorator repeats until the child returns fail and then returns success.
class UntilFailure : public Decorator
{
public:
    virtual Status execute( Context * c )
    {
        for ( ;; ) {
            auto s = m_Child->update( c );

            if ( s == Status::Failure ) {
                return Status::Success;
            }
        }

        return Status::Invalid;
    }
};

class Noop : public Leaf
{
public:
    Noop() {}
    virtual ~Noop() {}
    virtual Status execute( Context * c ) { return Status::Success; }
};

} // namespace brain

