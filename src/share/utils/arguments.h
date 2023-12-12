
#ifndef __SRC_UTILS_ARGUMENTS_H__
#define __SRC_UTILS_ARGUMENTS_H__

#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>

namespace utils {

class Arguments
{
public :
    Arguments( int argc, const char ** argv ) {
        std::string key;
        for ( int i = 0; i < argc; ++i ) {
            if ( strncmp( argv[i], "--", 2 ) == 0 ) {
                key = split( false, argv[i] );
            } else if ( strncmp( argv[i], "-", 1 ) == 0 ) {
                key = split( true, argv[i] );
            } else if ( !key.empty() ){
                m_Arguments[key] = argv[i];
                key.clear();
            } else {
                m_Tokens.emplace_back( argv[i] );
            }
        }
    }

    ~Arguments() {
        m_Tokens.clear();
        m_Arguments.clear();
    }

    bool has( const std::string & key ) const {
        if ( m_Arguments.end()
            != m_Arguments.find( key ) ) {
            return true;
        }
        return m_Tokens.end()
            != std::find( m_Tokens.begin(), m_Tokens.end(), key );
    }

    std::string command() const {
        auto pos = m_Tokens[0].find_last_of( "/" );
        if ( pos == std::string::npos ) {
            return m_Tokens[0];
        }
        return m_Tokens[0].substr( pos+1 );
    }

    std::string operator[] ( size_t pos ) const {
        if ( pos < m_Tokens.size() ) {
            return m_Tokens[ pos ];
        }
        return "";
    }

    std::string operator[] ( const std::string & key ) const {
        auto pos = m_Arguments.find( key );
        if ( pos != m_Arguments.end() ) {
            return pos->second;
        }
        return "";
    }

private :
    std::string split( bool single, const std::string & value ) {
        std::string key;
        auto pos = value.find_last_of( "=" );
        if ( pos != std::string::npos ) {
            m_Arguments.emplace( value.substr(0, pos), value.substr(pos+1) );
        } else if ( single ) {
            if ( value.size() == 2 ) {
                key = value;
                m_Arguments.emplace( key, "" );
            } else if ( value.size() > 2 ) {
                m_Arguments.emplace( value.substr(0, 2), value.substr(2) );
            }
        } else {
            key = value;
            m_Arguments.emplace( key, "" );
        }
        return key;
    }

private :
    std::vector<std::string> m_Tokens;
    std::map<std::string, std::string> m_Arguments;
};

}

#endif
