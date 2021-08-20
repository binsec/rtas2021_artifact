%{ /* -*- C++ -*- */
/***************************************************************************
                      scanner.lex  -  lexical analyzer
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003 CEA and Universite Paris Sud
    author               : Gilles Mouchard
    email                : mouchard@lri.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <parser_defs.hh>
#include <scanner.hh>
#include <parsing.hh>
#include <strtools.hh>
#include <sourcecode.hh>
#include <comment.hh>
#include <iostream>
#include <cerrno>

#ifdef WIN32
#include <windows.h>

#define YY_NEVER_INTERACTIVE 1
#endif

bool                     lex_aborted_scanning = false; ///< true if scanning was aborted, false otherwise
FileLoc                  lex_fileloc_mlt;              ///< Starting line number of multi-line tokens
Opts*                    lex_opts = 0;                 ///< Global option record
int                      lex_bracecount = 0;           ///< Global opened braces count
std::vector<int>         lex_scs;
FileLoc                  Parsing::fileloc;
Isa*                     Parsing::s_isa = 0;
Vector<Comment>          Parsing::comments;
ConstStr::Pool           Scanner::symbols;

struct Inclusion
{
  Inclusion( uint8_t const* _state, intptr_t _size, FileLoc const& _fileloc, Inclusion* _next );
  ~Inclusion();
  
  void restore( uint8_t* _state, intptr_t _size );
  
  uint8_t*               state_backup;
  FileLoc                fileloc;
  Inclusion*             next;

  static Inclusion* stack;
  static void push();
  static bool pop();
};

static void              parse_binary_number( char const* s, int length, unsigned int *value );
static void              lex_sc_enter( int _condition );
static bool              lex_sc_leave();
static std::string&      lex_strbuf();
static int               lex_token( char const* _text );
static ConstStr          lex_charname( char _ch );

/* This is for Flex < 2.5.9 (where yylex_destroy is not defined) */
#if !defined(YY_FLEX_MAJOR_VERSION) || YY_FLEX_MAJOR_VERSION < 2 \
    || (YY_FLEX_MAJOR_VERSION == 2 \
        && (!defined(YY_FLEX_MINOR_VERSION) || YY_FLEX_MINOR_VERSION < 5 \
            || (YY_FLEX_MINOR_VERSION == 5 \
                && (!defined(YY_FLEX_SUBMINOR_VERSION) \
                    || YY_FLEX_SUBMINOR_VERSION < 9))))
# define yylex_destroy() yy_delete_buffer (YY_CURRENT_BUFFER)
#endif

/* This is for column counting. */
#define YY_USER_ACTION Parsing::fileloc.newtoken( yyleng );

%}

identifier [a-zA-Z_][a-zA-Z0-9_]*
binary_number 0b[0-1]+
hexadecimal_number 0x[0-9a-fA-F]+
decimal_number [0-9]+

%x string_context
%x source_code_context
%x c_like_comment_context
%x cpp_like_comment_context
%x char_context

%%

\" { lex_sc_enter( string_context ); }
<string_context>[^\"\n] { lex_strbuf().append( yytext, yyleng ); }
<string_context>\n { lex_strbuf().append( yytext, yyleng ); Parsing::fileloc.newline(); }
<string_context>\\\" { lex_strbuf().append( yytext, yyleng ); }
<string_context>\" { if (lex_sc_leave()) { yylval.volatile_string = lex_strbuf().c_str(); return TOK_STRING; }
}

\{ { lex_sc_enter( source_code_context ); }
<source_code_context>[^/\}\{\"\n\'] {  lex_strbuf().append( yytext, yyleng ); }
<source_code_context>"/" { lex_strbuf().append( yytext, yyleng ); }
<source_code_context>\' { lex_sc_enter( char_context ); lex_strbuf().append( yytext, yyleng ); }
<source_code_context>\{ { lex_bracecount++; lex_strbuf().append( yytext, yyleng ); }
<source_code_context>\" { lex_sc_enter( string_context ); lex_strbuf().append( yytext, yyleng ); }
<source_code_context>\n { lex_strbuf().append( yytext, yyleng ); Parsing::fileloc.newline(); }
<source_code_context>\} {
  if (lex_sc_leave()) {
    yylval.sourcecode = new SourceCode( lex_strbuf().c_str(), lex_fileloc_mlt );
    return TOK_SOURCE_CODE;
  }
}

<char_context>[^\'\n] { lex_strbuf().append( yytext, yyleng ); }
<char_context>\n { lex_strbuf().append( yytext, yyleng ); Parsing::fileloc.newline(); }
<char_context>\\\' { lex_strbuf().append( yytext, yyleng ); }
<char_context>\' { lex_sc_leave(); }

<INITIAL,source_code_context>"/*" { lex_sc_enter( c_like_comment_context ); lex_strbuf().append( yytext, yyleng ); }
<c_like_comment_context>[^*\n] { lex_strbuf().append( yytext, yyleng ); }
<c_like_comment_context>"*"+[^/\n] { lex_strbuf().append( yytext, yyleng ); }
<c_like_comment_context>"*"+\n { lex_strbuf().append( yytext, yyleng ); Parsing::fileloc.newline(); }
<c_like_comment_context>\n { lex_strbuf().append( yytext, yyleng ); Parsing::fileloc.newline(); }
<c_like_comment_context>"*"+"/" {
  if (lex_sc_leave()) {
    lex_strbuf().append( yytext, yyleng );
    Parsing::comments.append( new Comment( lex_strbuf().c_str(), lex_fileloc_mlt ) );
  }
}

<INITIAL,source_code_context>"//" { lex_sc_enter( cpp_like_comment_context ); lex_strbuf().append( yytext, yyleng ); }
<cpp_like_comment_context>[^\n] { lex_strbuf().append( yytext, yyleng ); }
<cpp_like_comment_context>\n {
  if (lex_sc_leave()) {
    Parsing::comments.append( new Comment( lex_strbuf().c_str(), lex_fileloc_mlt ) );
  }
  Parsing::fileloc.newline();
}

{binary_number} { parse_binary_number( yytext, yyleng, &yylval.uinteger ); return TOK_INTEGER; }
{hexadecimal_number} { sscanf( yytext, "%x", &yylval.uinteger ); return TOK_INTEGER; }
{decimal_number} { sscanf(yytext, "%u", &yylval.uinteger); return TOK_INTEGER; }
{identifier} {
  int token = lex_token( yytext );
  if (token == TOK_IDENT)
    yylval.persistent_string = ConstStr( yytext, Scanner::symbols ).str();
  return token;
}
\\\n { Parsing::fileloc.newline(); }
\n { Parsing::fileloc.newline(); return TOK_ENDL; }
";" { return TOK_ENDL; }
"*" { return '*'; }
"." { return '.'; }
"[" { return '['; }
"]" { return ']'; }
"(" { return '('; }
")" { return ')'; }
"<"  { return '<'; }
">"  { return '>'; }
"," { return ','; }
":" { return ':'; }
"=" { return '='; }
"-" { return '-'; }
"?" { return '?'; }
"::" { return TOK_QUAD_DOT; }
" " { }
\t { }
<INITIAL><<EOF>> { if (not Inclusion::pop()) yyterminate(); }
<string_context,char_context,c_like_comment_context,cpp_like_comment_context,source_code_context><<EOF>> {
  Parsing::fileloc.err( "error: unexpected end of file" );
  lex_aborted_scanning = true;
  yyterminate();
  return 0;
}
<INITIAL,string_context,char_context,c_like_comment_context,cpp_like_comment_context,source_code_context>. {
  Parsing::fileloc.err( "error: unexpected %s", lex_charname( yytext[0] ).str() );
  lex_aborted_scanning = true;
  yyterminate();
  return 0;
}

%%

#include <conststr.hh>
#include <cstring>

#include <cassert>
#include <isa.hh>

static bool
lex_open( ConstStr _filename )
{
  fprintf( stderr, "Opening %s\n", _filename.str() );
  yyin = fopen( _filename.str(), "r" );
  
  if (not yyin) {
    Parsing::fileloc.err( "error: can't open file `%s'", _filename.str() );
    return false;
  }
  
  Parsing::isa().m_includes.push_back( _filename );
  Parsing::fileloc.assign( _filename, 1, 1 );
  return true;
}

bool
Scanner::parse( char const* _filename, Opts& _opts, Isa& _isa )
{
  Parsing::s_isa = &_isa;
  lex_opts = &_opts;
  if (not lex_open( ConstStr( _filename) ) )
    return false;
  lex_bracecount = 0;
  lex_scs.clear();
  
#if 0
  // This code is only for testing the lexical analyzer
  int c;
    
  while( (c = yylex()) != 0 ) {
    printf( "%s\n", get_token_name(c) );
  }
#endif
  
  lex_aborted_scanning = false;
  int error = yyparse();
  
  if (yyin) {
    fclose( yyin );
    yyin = 0;
  }
  
  yylex_destroy();
  return (error == 0) && !lex_aborted_scanning;
}

bool
Parsing::include( char const* _filename )
{
  ConstStr filename = lex_opts->locate( _filename );
  
  Inclusion::push();

  if (not lex_open( filename))
    return false;

  yy_switch_to_buffer( yy_create_buffer( yyin, YY_BUF_SIZE ) );
  yylineno = 1;
  return true;
}

void
parse_binary_number( char const* binstr, int length, unsigned int *value )
{
  unsigned int res = 0;
  unsigned int mask = 1;
  for (char const* ptr = binstr + length; (--ptr) >= binstr and *ptr != 'b';) {
    if (*ptr != '0') res |= mask;
    mask <<= 1;
  }
  *value = res;
}

struct Token
{
  char const*                   name;         ///< the name of the token
  int                           token;        ///< the token
} lex_tokens[] =
  {
   {"action", TOK_ACTION},
   {"const", TOK_CONST},
   {"constructor", TOK_CONSTRUCTOR},
   {"decl", TOK_DECL},
   {"decoder", TOK_DECODER},
   {"destructor", TOK_DESTRUCTOR},
   {"group", TOK_GROUP},
   {"impl", TOK_IMPL},
   {"include", TOK_INCLUDE},
   {"inheritance", TOK_INHERITANCE},
   {"namespace", TOK_NAMESPACE},
   {"op", TOK_OP},
   {"rewind", TOK_REWIND},
   {"set", TOK_SET},
   {"sext", TOK_SEXT},
   {"shl", TOK_SHL},
   {"shr", TOK_SHR},
   {"specialize", TOK_SPECIALIZE},
   {"static", TOK_STATIC},
   {"subdecoder", TOK_SUBDECODER},
   {"template", TOK_TEMPLATE},
   {"var", TOK_VAR},
   {0,0}
  };
 
/** Return the name of a token
    @param token the token
    @return the name of the token
*/
ConstStr
Parsing::tokenname( int _token )
{
  /* search into the token table */
  for (int idx = 0; lex_tokens[idx].name; ++ idx)
    if (lex_tokens[idx].token == _token)
      return lex_tokens[idx].name;
  
  /* return a string for the token not in the token table */
  switch( _token ) {
  case TOK_IDENT: return "identifier";
  case TOK_ENDL: return "end of line";
  case TOK_SOURCE_CODE: return "source code";
  case TOK_INTEGER: return "integer";
  case TOK_STRING: return "string";
  case '(': return "`('";
  case ')': return "`)'";
  case '<': return "`<'";
  case '>': return "`>'";
  case ':' : return "`:'";
  case '=' : return "`='";
  case '.': return "`.'";
  case '[': return "`['";
  case ']': return "`]'";
  case '?': return "`?'";
  }

  /* return a string for a character token */
  if (_token < 256)
    return lex_charname( _token );
  
  /* don't know which token it is ! */
  return "unknown token";
}

/** Convert a name to a token
    @param name name of the token
    @return the token
*/
int
lex_token( char const* _text )
{
  for (int idx = 0; lex_tokens[idx].name; ++ idx)
    if (strcmp( lex_tokens[idx].name, _text) == 0 )
      return lex_tokens[idx].token;

  return TOK_IDENT;
}


ConstStr
lex_charname( char _char )
{
  switch( _char ) {
  case '\t': return "tab char";
  case '\n': return "return char";
  case '\r': return "carriage return";
  case '\b': return "back space";
  default: break;
  }
  
  if (_char < 32 or _char >= 126)
    return Str::fmt( "char #%d", _char );

  return Str::fmt( "'%c'", _char );
}

std::string& lex_strbuf() { static std::string s_buffer; return s_buffer; }

Inclusion* Inclusion::stack = 0;

Inclusion::Inclusion( uint8_t const* _state, intptr_t _size, FileLoc const& _fileloc, Inclusion* _next )
  : state_backup( 0 ), fileloc( _fileloc ), next( _next )
{
  state_backup = new uint8_t[_size];
  memcpy( state_backup, _state, _size );
}

Inclusion::~Inclusion()
{
  delete [] state_backup;
}
  
void
Inclusion::restore( uint8_t* _state, intptr_t _size )
{
  memcpy( _state, state_backup, _size );
}

void Inclusion::push()
{
  YY_BUFFER_STATE state = YY_CURRENT_BUFFER;
  
  Inclusion::stack = new Inclusion( (uint8_t const*)&state, sizeof( YY_BUFFER_STATE ), Parsing::fileloc, Inclusion::stack );
}

bool Inclusion::pop()
{
  Inclusion* tail = Inclusion::stack;
  
  if (not tail) return false;
  YY_BUFFER_STATE state;
  tail->restore( (uint8_t*)&state, sizeof( YY_BUFFER_STATE ) );
  yy_delete_buffer( YY_CURRENT_BUFFER );
  if (yyin) fclose( yyin );
  yy_switch_to_buffer( state );
  
  Parsing::fileloc = tail->fileloc;
  
  Inclusion::stack = tail->next;
  tail->next = 0;
  delete tail;
  
  return true;
}

void lex_sc_enter( int _condition )
{
  int sc = YY_START;
  if (sc == INITIAL)
    {
      lex_fileloc_mlt = Parsing::fileloc;
      lex_strbuf().clear();
    }
  if (_condition == source_code_context) {
    if (sc == source_code_context) { lex_bracecount += 1; return; }
    else if (sc == INITIAL) lex_bracecount = 1;
    else assert( false );
  }
  lex_scs.push_back( sc );
  BEGIN( _condition );
}

bool lex_sc_leave()
{
  int oldsc = YY_START;
  int newsc = lex_scs.back();
  
  if (oldsc == source_code_context and ((--lex_bracecount) > 0)) {
    newsc = source_code_context;
  } else {
    lex_scs.pop_back();
    BEGIN( newsc );
  }

  if (newsc == INITIAL) return true;
  lex_strbuf().append( yytext, yyleng );
  return false;
}

int yywrap() { return 1; }

