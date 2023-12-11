
#ifndef __SRC_EXTRA_PROCESSOR_H__
#define __SRC_EXTRA_PROCESSOR_H__

#include "lua/luaenv.h"
#include "base/message.h"
#include "protocol/types.h"
#include "message/protocol.h"

#include "pb2lua.h"

template <typename Ch>
class AppProcessor
{
public:
    AppProcessor() = default;
    AppProcessor( ILuaEnv * env, const std::string & method )
        : m_LuaEnv( env ),
          m_LuaMethod( method )
    {}

    virtual ~AppProcessor() = default;

    virtual bool ishook( MessageID id ) const { return false; }
    virtual void deal( SSMessage * cmd ) {}
    virtual int32_t process( Ch ch, MessageID cmd, GameMessage * msg ) { return 0; }

public:
    // 调用lua函数
    int32_t invoke( Ch ch, MessageID cmd, GameMessage * msg ) {
        if ( m_LuaEnv != nullptr
            && !m_LuaMethod.empty() ) {
            return m_LuaEnv->state()[m_LuaMethod]( ch, cmd, msg );
        }
        return 0;
    }

protected:
    ILuaEnv * m_LuaEnv = nullptr;
    std::string m_LuaMethod;
};

#endif
