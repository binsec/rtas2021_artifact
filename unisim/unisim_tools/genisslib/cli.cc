/***************************************************************************
                                    cli.cc
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003-2007 CEA and Universite Paris Sud
    authors              : Gilles Mouchard, Yves Lhuillier
    email                : gilles.mouchard@cea.fr, yves.lhuillier@cea.fr
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#include <cli.hh>
#include <cassert>
#include <cstdarg>
#include <strtools.hh>
#include <conststr.hh>
#include <iostream>
#include <cstring>

CLI::CLI()
  : m_displayname( 0 ), m_callname( 0 ), m_opt_tabstop( 0 )
{}

struct Pattern {
  char const* m_begin;
  char const* m_end;

  Pattern( char const* _patterns ) : m_begin( 0 ), m_end( _patterns ) {}

  bool        next() {
    char const* end = m_end;
    while (*end == ',') ++end;
    char const* begin = end;
    while ((*end != '\0') and (*end != ',')) ++end;
    if (end == begin) return false;
    m_begin = begin; m_end = end; return true;
  }

  uintptr_t   length() const { return m_end - m_begin; }
  void        strcpy( char* _buf ) const {
    uintptr_t size = length();
    memcpy( _buf, m_begin, size );
    _buf[size] = '\0';
  }
  bool        operator!=( char const* _string ) const {
    uintptr_t size = length();
    if (strncmp( m_begin, _string, size) != 0 ) return true;
    return _string[size] != '\0';
  }
};

struct InfoArgs : public CLI::Args
{
  CLI&                   m_cli;

  InfoArgs( CLI& _cli ) : m_cli( _cli ) {}
  
  bool match( bool _active, char const* _shortdesc, char const* _longdesc ) { return false; }
  
  bool match( char const* _patterns, char const* _shortdesc, char const* _longdesc ) {
    intptr_t value = 0;
    for( Pattern pattern( _patterns ); pattern.next(); ) { value += 2 + pattern.length(); }
    value += 1 + strlen( _shortdesc );
    if (value > m_cli.m_opt_tabstop) m_cli.m_opt_tabstop = value;
    return false;
  }
};

void
CLI::set( char const* _displayname, char const* _callname )
{
  m_displayname = _displayname;
  m_callname = _callname;
  InfoArgs args( *this );
  parse( args );
}

struct Proto_t : public CLI::Args {
  bool match( char const* _patterns, char const* _shortdesc, char const* _longdesc ) { return false; }
  bool match( bool _active, char const* _shortdesc, char const* _longdesc )
  {
    std::cerr << ' ' << _shortdesc;
    return false;
  }
};

void
CLI::prototype()
{
  std::cerr << "Usage: " << m_callname;
  Proto_t proto;
  parse( proto );
  std::cerr << std::endl;
}

struct Screen
{
  uintptr_t    m_tabstop, m_width;
  std::string  m_buffer;
  uintptr_t    m_lastblank;
  bool         m_lastwasblank;
  
  Screen( uintptr_t _tabstop, uintptr_t _width )
    : m_tabstop( _tabstop ), m_width( _width ), m_lastblank( std::string::npos ), m_lastwasblank( true ) {}
  
  Screen& tab() {
    if( m_buffer.size() >= m_tabstop) return write( '\n' );
    while( m_buffer.size() < m_tabstop ) m_buffer += ' ';
    return *this;
  }
  
  Screen& reset() {
    m_buffer.clear();
    m_lastblank = std::string::npos;
    m_lastwasblank = true;
    return *this;
  }
  
  Screen& flush() {
    for( char const* ptr = m_buffer.c_str(); *ptr; ++ptr )
      if (*ptr > ' ') return write( '\n' );
    return *this;
  }
  
  Screen& write( char ch ) {
    intptr_t index = m_buffer.size();
    m_buffer += ch;
    if (ch == '\n') {
      std::cerr << m_buffer.c_str();
      reset();
      return tab();
    } else if (ch <= ' ') { // considered as blank
      if (not m_lastwasblank) { m_lastblank = index; m_lastwasblank = true; }
    } else {
      m_lastwasblank = false;
    }
    
    if ((m_buffer.size() > m_width) and (m_lastblank > 0) and (m_lastblank != std::string::npos) and (not m_lastwasblank)) {
      char flushbuf[m_buffer.size() + 1];
      memcpy( flushbuf, m_buffer.c_str(), m_lastblank );
      flushbuf[m_lastblank] = '\0';
      std::cerr << flushbuf << std::endl;
      char const* remainder = m_buffer.c_str() + m_lastblank;
      while( *remainder != '\0' and *remainder <= ' ' ) remainder++;
      strcpy( flushbuf, remainder );
      reset();
      tab();
      write( flushbuf );
    }
    return *this;
  }
  
  Screen& write( char const* _str ) { while( *_str ) write( *_str++ ); return *this; }
  Screen& write( char const* _beg, char const* _end ) { while( _beg < _end ) write( *_beg++ ); return *this; }
};
  
struct Opts_t : public CLI::Args {
  intptr_t m_align;
  
  Opts_t( intptr_t _align ) : m_align( _align + 2 ) {}
  
  bool match( bool _active, char const* _shortdesc, char const* _longdesc ) { return false; }
  
  bool match( char const* _patterns, char const* _shortdesc, char const* _longdesc ) {
    Screen screen( m_align, 96 );
    
    char const* sep = "  ";
    for( Pattern pattern( _patterns ); pattern.next(); ) {
      screen.write( sep ).write( pattern.m_begin, pattern.m_end );
      sep = ", ";
    }
    screen.write( " " ).write( _shortdesc ).tab().write( _longdesc );
    screen.flush();
    return false;
  }
};

void
CLI::options() {
  std::cerr << "options:\n";
  Opts_t opts( m_opt_tabstop );
  parse( opts );
}

void
CLI::help() {
  prototype();
  description();
  options();
}

struct ValueArgs : public CLI::Args {
  intptr_t    m_argidx, m_argc;
  char**      m_argvals;
  char const* m_subrem;
  char        m_subarg[3];
  
  std::string m_matched;
  std::string m_tmp;
  
  ValueArgs( intptr_t _argc, char** _argvals )
    : m_argidx( 0 ), m_argc( _argc ), m_argvals( _argvals ), m_subrem( 0 )
  {
    memcpy( m_subarg, "-\0", 3 );
  }
  
  bool match( bool _active, char const* _shortdesc, char const* _longdesc ) {
    if (not _active or m_subarg[1]) return false;
    m_matched = front();
    assert( not m_matched.empty() );
    return true;
  }

  bool match( char const* _patterns, char const* _shortdesc, char const* _longdesc ) {
    char const* arg = front();
    assert( arg );
    for( Pattern pattern( _patterns ); pattern.next(); ) {
      if (pattern != arg) continue;
      m_matched = pop_front();
      return true;
    }
    // Checking for condensed short option form
    if (arg[0] != '-' or arg[1] == '-' or arg[1] == '\0' or arg[2] == '\0') return false;
    m_subarg[1] = arg[1];
    m_subrem = &arg[2];
    // replay
    bool short_match = this->match( _patterns, _shortdesc, _longdesc );
    m_subarg[1] = '\0';
    if (short_match) return true;
    m_subrem = 0;
    return false;
  }
  
  char const* pop_front() {
    if (m_subarg[1]) {
      m_tmp = m_subarg;
      m_subarg[1] = '\0';
      return m_tmp.c_str();
    }
    if (m_subrem) {
      char const* tmp = m_subrem;
      m_subrem = 0;
      m_argidx++;
      return tmp;
    }
    return (m_argidx < m_argc) ? m_argvals[m_argidx++] : 0;
  }
  
  char const* front() const {
    if (m_subarg[1])       return m_subarg;
    if (m_subrem)          return m_subrem;
    if (m_argidx < m_argc) return m_argvals[m_argidx];
    return 0;
  }
  
  bool        next() {
    m_matched.clear();
    assert( m_subarg[1] == '\0' );
    if (m_subrem) {
      m_subarg[1] = *m_subrem++;
      if (*m_subrem == '\0') m_subrem = 0;
      return true;
    }
    return m_argidx < m_argc;
  }
};

void
CLI::process( intptr_t _argc, char** _argv ) {
  for( intptr_t idx = _argc; (--idx) >= 0; ) {
    if (strcmp( _argv[idx], "--help") == 0 or  strcmp( _argv[idx], "-h" ) == 0 ) {
      help();
      throw Exit_t( 0 );
    }
  }

  try {
    for( ValueArgs args( _argc, _argv ); args.next(); ) {
      parse( args );
      if( not args.m_matched.empty() ) continue;
      std::cerr << m_displayname << ": unexpected argument: " << args.front() << "\n";
      throw Exit_t( 1 );
    }
  } catch( ConstStr& _error ) {
    std::cerr << m_displayname << ": " << _error.str();
    help();
    throw Exit_t( 1 );
  }
}

char const* CLI::Args::pop_front() { assert( false ); return 0; }
char const* CLI::Args::front() const { assert( false ); return 0; }

