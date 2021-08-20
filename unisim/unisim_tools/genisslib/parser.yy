%{ /* -*- C++ -*- */
/***************************************************************************
                      parser.ypp  -  syntaxical analyzer
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
#include <vect.hh>
#include <isa.hh>
#include <sourcecode.hh>
#include <action.hh>
#include <comment.hh>
#include <operation.hh>
#include <subdecoder.hh>
#include <variable.hh>
#include <bitfield.hh>
#include <specialization.hh>
#include <scanner.hh>
#include <parsing.hh>
#include <conststr.hh>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>
#include <cstdlib>
  
int yylex();
int yyerror( char const* _err );

%}

/*
 * control characters
 */
%token '*'
%token ':'
%token ','
%token '='
%token '-'
%token '.'
%token '('
%token ')'
%token '['
%token ']'
%token '<'
%token '>'
%token TOK_ENDL
%token TOK_TAB
%token TOK_QUAD_DOT
/*
 * keywords
 */
%token TOK_ACTION
%token TOK_CONST
%token TOK_CONSTRUCTOR
%token TOK_DECL
%token TOK_DECODER
%token TOK_DESTRUCTOR
%token TOK_GROUP
%token TOK_IMPL
%token TOK_INCLUDE
%token TOK_INHERITANCE
%token TOK_NAMESPACE
%token TOK_OP
%token TOK_REWIND
%token TOK_SET
%token TOK_SEXT
%token TOK_SHL
%token TOK_SHR
%token TOK_SPECIALIZE
%token TOK_STATIC
%token TOK_SUBDECODER
%token TOK_TEMPLATE
%token TOK_VAR
/*
 * basic types
 */
%token TOK_SOURCE_CODE
%token TOK_IDENT
%token TOK_INTEGER
%token TOK_STRING
/*
 * complex types
 */
%type<sourcecode> TOK_SOURCE_CODE
%type<persistent_string> TOK_IDENT
%type<uinteger> TOK_INTEGER
%type<volatile_string> TOK_STRING
%type<sourcecode> sourcecode_decl_declaration
%type<sourcecode> sourcecode_impl_declaration
%type<operation> operation_declaration
%type<bitfield_list> bitfield_list
%type<bitfield> bitfield
%type<bitfield_list> bitfield_list_decl
%type<actionproto> action_proto_declaration
%type<sourcecode> sourcecode_declaration
%type<sinteger> action_proto_type
%type<sourcecode> op_condition
%type<param_list> param_list
%type<param> param
%type<inheritance> global_inheritance_declaration
%type<boolean> sext
%type<boolean> constness
%type<uinteger> size_modifier
%type<sinteger> shift
%type<variable> var
%type<variable_list> var_list
%type<sourcecode> returns
%type<variable_list> global_var_list_declaration
%type<group> group_declaration
%type<string_list> operation_list
%type<sourcecode> var_init
%type<param_list> template_declaration
%type<sourcecode> template_scheme
%type<constraint> constraint
%type<constraint_list> constraint_list
%type<specialization> specialization
%type<string_list> namespace_list
%type<uint_list> uinteger_list
%%

input: declaration_list { };

declaration_list: | declaration_list declaration {}
;

namespace_list:
      TOK_IDENT
{
  $$ = new StringVector( $1 );
}
  | namespace_list TOK_QUAD_DOT TOK_IDENT
{
  $$ = &($1->append( $3 ));
}
;

subdecoder_instance:
  TOK_SUBDECODER namespace_list TOK_IDENT template_scheme
{
  StringVector* nmspc_in = $2;
  ConstStr symbol = ConstStr( $3, Scanner::symbols );
  SourceCode* template_scheme = $4;
  
  std::vector<ConstStr> nmspc( nmspc_in->size(), ConstStr() );
  for (intptr_t idx = nmspc_in->size(); (--idx) >= 0;)
    nmspc[idx] = ConstStr( (*nmspc_in)[idx], Scanner::symbols );
  delete nmspc_in;
  
  SDClass const* sdclass = Parsing::isa().sdclass( nmspc );
  if (not sdclass) {
    Parsing::fileloc.err( "error: subdecoder has not been declared" );
    YYABORT;
  }
  
  SDInstance const* sdinstance = Parsing::isa().sdinstance( symbol );
  if (sdinstance) {
    Parsing::fileloc.err( "error: subdecoder instance `%s' redefined", symbol.str() );
    sdinstance->m_fileloc.err( "subdecoder instance `%s' previously defined here", symbol.str() );
    YYABORT;
  }
  
  Parsing::isa().m_sdinstances.append( new SDInstance( symbol, template_scheme, sdclass, Parsing::fileloc ) );
}
;

template_scheme:
{
  $$ = 0;
}
  | '<' TOK_SOURCE_CODE '>'
{
  $$ = $2;
}
;

uinteger_list:
  TOK_INTEGER
{
  $$ = new UIntVector( $1 );
}
  | uinteger_list ',' TOK_INTEGER
{
  $1->push_back( $3 );
  $$ = $1;
}
;

subdecoder_class:
  TOK_SUBDECODER namespace_list '[' uinteger_list ']'
{
  StringVector* nmspc_in = $2;
  UIntVector* insnsizes = $4;
  
  std::vector<ConstStr> nmspc( nmspc_in->size(), ConstStr() );
  for (intptr_t idx = nmspc_in->size(); (--idx) >= 0;)
    nmspc[idx] = ConstStr( (*nmspc_in)[idx], Scanner::symbols );
  delete nmspc_in;
  
  SDClass const* sdclass = Parsing::isa().sdclass( nmspc );
  if (sdclass) {
    Parsing::fileloc.err( "error: subdecoder class redeclared." );
    sdclass->m_fileloc.err( "subdecoder class previously declared here." );
    YYABORT;
  }
  
  Parsing::isa().m_sdclasses.append( new SDClass( nmspc, insnsizes->begin(), insnsizes->end(), Parsing::fileloc ) );
  delete insnsizes;
}
;

global_ident_parameter: TOK_SET TOK_IDENT TOK_IDENT
{
  ConstStr key( $2, Scanner::symbols );
  ConstStr val( $3, Scanner::symbols );
  
  try {
    Parsing::isa().setparam( key, val );
  } catch (Isa::UnknownIdent ui) {
    Parsing::fileloc.err( "error: unknown or illegal ident `%s'.", ui.m_ident.str() );
    YYABORT;
  }
}

global_sourcecode_parameter: TOK_SET TOK_IDENT TOK_SOURCE_CODE
{
  ConstStr key( $2, Scanner::symbols );
  SourceCode* val = $3;
  try {
    Parsing::isa().setparam( key, *val );
    delete val;
  } catch (Isa::UnknownIdent ui) {
    Parsing::fileloc.err( "error: unknown or illegal ident `%s'.", ui.m_ident.str() );
    YYABORT;
  }
}

global_uinteger_parameter: TOK_SET TOK_IDENT TOK_INTEGER
{
  ConstStr key( $2, Scanner::symbols );
  unsigned int val = $3;
  try {
    Parsing::isa().setparam( key, val );
  } catch (Isa::UnknownIdent ui) {
    Parsing::fileloc.err( "error: unknown or illegal ident `%s'.", ui.m_ident.str() );
    YYABORT;
  }
}

template_declaration: TOK_TEMPLATE '<' param_list '>'
{
  $$ = $3;
}
;

declaration:
             TOK_ENDL {}
           | global_ident_parameter TOK_ENDL
           | global_sourcecode_parameter TOK_ENDL
           | global_uinteger_parameter TOK_ENDL
           | subdecoder_class TOK_ENDL
           | subdecoder_instance TOK_ENDL
           | operation_declaration TOK_ENDL
{
  Parsing::isa().add( $1 );
}
           | action_proto_declaration TOK_ENDL
{
  Parsing::isa().m_actionprotos.push_back( $1 );
}
     | action_declaration TOK_ENDL
{
}
           | sourcecode_declaration TOK_ENDL
{
  Parsing::isa().m_decl_srccodes.push_back( $1 );
}
  | sourcecode_decl_declaration TOK_ENDL
{
  Parsing::isa().m_decl_srccodes.push_back( $1 );
}
  | sourcecode_impl_declaration TOK_ENDL
{
  Parsing::isa().m_impl_srccodes.push_back( $1 );
}
     | include {}
     | global_var_list_declaration
{
  Parsing::isa().m_vars.append( *$1 );
  delete $1;
}
     | op_var_list_declaration {}
     | global_inheritance_declaration TOK_ENDL
{
  Parsing::isa().m_inheritances.push_back( $1 );
}
  | group_environment
  | group_declaration
{
  Parsing::isa().m_groups.push_back( $1 );
}
  | namespace_declaration
{
}
  | template_declaration
{
  if( not Parsing::isa().m_tparams.empty() ) {
    Parsing::fileloc.err( "error: template parameters defined more than once" );
    YYABORT;
  }
  Parsing::isa().m_tparams = *$1;
  delete $1;
}
  | specialization TOK_ENDL
{
  Parsing::isa().m_specializations.push_back( $1 );
}
  | user_specialization TOK_ENDL {}
;

namespace_declaration: TOK_NAMESPACE namespace_list
{
  StringVector* nmspc = $2;
  
  for (StringVector::const_iterator ident = nmspc->begin(); ident != nmspc->end(); ++ident) {
    Parsing::isa().m_namespace.push_back( ConstStr( *ident, Scanner::symbols ) );
  }
  
  delete nmspc;
}
;

sourcecode_decl_declaration: TOK_DECL TOK_SOURCE_CODE
{
  $$ = $2;
}
;

sourcecode_impl_declaration: TOK_IMPL TOK_SOURCE_CODE
{
  $$ = $2;
}
;

sourcecode_declaration: TOK_SOURCE_CODE
{
  $$ = $1;
}
;

bitfield_list_decl: '(' bitfield_list ')'
{
  $$ = $2;
}
;

op_condition: TOK_SOURCE_CODE ':'
{
  $$ = $1;
}
  |
{
  $$ = 0;
}
;

operation_declaration : op_condition TOK_OP TOK_IDENT bitfield_list_decl
{
  SourceCode*       op_cond = $1;
  ConstStr          symbol = ConstStr( $3, Scanner::symbols );
  Vector<BitField>* bitfields = $4;
  
  {
    Operation const* prev_op = Parsing::isa().operation( symbol );
    if (prev_op) {
      Parsing::fileloc.err( "error: operation `%s' redefined", symbol.str() );
      prev_op->fileloc.err( "operation `%s' previously defined here", symbol.str() );
      YYABORT;
    }
    
    Group const* prev_grp = Parsing::isa().group( symbol );
    if (prev_grp) {
      Parsing::fileloc.err( "error: operation `%s' redefined", symbol.str() );
      prev_grp->fileloc.err( "group `%s' previously defined here", symbol.str() );
    }
  }

  Operation* operation = new Operation( symbol, *bitfields, Parsing::comments, op_cond, Parsing::fileloc );
  delete bitfields;
  Parsing::comments.clear();

  $$ = operation;
}
;

bitfield_list : bitfield
{
  $$ = new Vector<BitField>( $1 );
}
  | bitfield_list ':' bitfield
{
  $$ = &($1->append( $3 ));
}
;

bitfield: TOK_INTEGER '[' TOK_INTEGER ']'
{
  $$ = new OpcodeBitField( $3, $1 );
}
  | shift sext size_modifier TOK_IDENT '[' TOK_INTEGER ']'
{
  $$ = new OperandBitField( $6, ConstStr( $4, Scanner::symbols ), $1, $3, $2 );
}
  | '?' '[' TOK_INTEGER ']'
{
  $$ = new UnusedBitField( $3 );
}
  | '*' TOK_IDENT '[' TOK_IDENT ']'
{
  ConstStr symbol = ConstStr( $2, Scanner::symbols );
  ConstStr sdinstance_symbol = ConstStr( $4, Scanner::symbols );
  SDInstance const* sdinstance = Parsing::isa().sdinstance( sdinstance_symbol );
  
  if (not sdinstance) {
    Parsing::fileloc.err( "error: subdecoder instance `%s' not declared", sdinstance_symbol.str() );
    YYABORT;
  }
  
  $$ = new SubOpBitField( symbol, sdinstance );
}
  | '>' '<'
{
  $$ = new SeparatorBitField( false );
}
  | '>' TOK_REWIND '<'
{
  $$ = new SeparatorBitField( true );
}
;

shift:
{
  $$ = 0;
}
  | TOK_SHR '<' TOK_INTEGER '>'
{
  $$ = $3;
}
  | TOK_SHL '<' TOK_INTEGER '>'
{
  $$ = -$3;
}
;

sext:
{
  $$ = false;
}
  | TOK_SEXT
{
  $$ = true;
}
;

size_modifier:
{
  $$ = 0;
}
  | '<' TOK_INTEGER '>'
{
  $$ = $2;
}
  | '<' '>'
{
  $$ = 0;
}
;

global_inheritance_declaration:
TOK_INHERITANCE TOK_SOURCE_CODE TOK_SOURCE_CODE '=' TOK_SOURCE_CODE
{
  $$ = new Inheritance( $2, $3, $5 );
}
| TOK_INHERITANCE TOK_SOURCE_CODE TOK_SOURCE_CODE
{
  $$ = new Inheritance( $2, $3, 0 );
}
;

op_var_list_declaration: TOK_IDENT '.' TOK_VAR var_list
{
  ConstStr    target_symbol = ConstStr( $1, Scanner::symbols );
  Vector<Variable>* var_list = $4;
  
  /* Target symbol points to either an operation or a group */
  Operation* operation = Parsing::isa().operation( target_symbol );
  if (operation) {
    /* Target symbol points to an operation */
    operation->variables.append( *var_list );
  } else {
    Group* group = Parsing::isa().group( target_symbol );
    if (group) {
      /* Target symbol points to a group */
      for (Vector<Operation>::iterator gop = group->operations.begin(); gop < group->operations.end(); ++ gop)
        (**gop).variables.append( *var_list );
    } else {
      /* Target symbol doesn't point to anything */
      Parsing::fileloc.err( "error: undefined operation or group `%s'", target_symbol.str() );
      YYABORT;
    }
  }
  delete var_list;
}
;

global_var_list_declaration: TOK_VAR var_list
{
  $$ = $2;
}
;

var_list: var
{
  $$ = new Vector<Variable>( $1 );
}
  | var_list ',' var
{
  $$ = &($1->append( $3 ));
}
;

var_init:
{
  $$ = 0;
}
  | '=' TOK_SOURCE_CODE
{
  $$ = $2;
}
;

var: TOK_IDENT ':' TOK_SOURCE_CODE var_init
{
  ConstStr     symbol = ConstStr( $1, Scanner::symbols );
  SourceCode*  c_type = $3;
  SourceCode*  c_init = $4;
  
  $$ = new Variable( symbol, c_type, c_init );
}
;

action_proto_declaration: action_proto_type TOK_ACTION returns TOK_IDENT '(' param_list ')' constness TOK_SOURCE_CODE
{
  ActionProto::type_t action_proto_type = ActionProto::type_t( $1 );
  SourceCode*         returns = $3;
  ConstStr            symbol = ConstStr( $4, Scanner::symbols );
  Vector<CodePair>*   param_list = $6;
  SourceCode*         default_sourcecode = $9;

  { /* action protype name should be unique */
    ActionProto const*  prev_proto = Parsing::isa().actionproto( symbol );
    if (prev_proto) {
      Parsing::fileloc.err( "error: action prototype `%s' redefined", prev_proto->m_symbol.str() );
      prev_proto->m_fileloc.err( "action prototype `%s' previously defined here", prev_proto->m_symbol.str() );
      YYABORT;
    }
  }
    
  if (returns) {
    switch( action_proto_type ) {
    case ActionProto::Constructor:
      Parsing::fileloc.err( "error: constructor action prototype `%s' must not have a return type (%s)",
                symbol.str(), returns->content.str() );
      YYABORT;
      break;
    case ActionProto::Destructor:
      Parsing::fileloc.err( "error: destructor action prototype `%s' must not have a return type (%s)",
                symbol.str(), returns->content.str() );
      YYABORT;
      break;
    default: break;
    }
  }
  
  if (param_list)
  {
    switch( action_proto_type ) {
    case ActionProto::Constructor:
      Parsing::fileloc.err( "error: constructor action prototype `%s' must not take any arguments", symbol.str() );
      YYABORT;
      break;
    case ActionProto::Static:
      Parsing::fileloc.err( "error: static action prototype `%s' must not take any arguments", symbol.str() );
      YYABORT;
      break;
    case ActionProto::Destructor:
      Parsing::fileloc.err( "error: destructor action prototype `%s' must not take any arguments", symbol.str() );
      YYABORT;
      break;
    default: break;
    }
  }
  
  ActionProto* actionproto =
    new ActionProto( action_proto_type, symbol, returns, *param_list, $8, default_sourcecode, Parsing::comments, Parsing::fileloc );
  Parsing::comments.clear();
  delete param_list;
  $$ = actionproto;
}
;

returns:
{
  $$ = 0;
}
  | TOK_SOURCE_CODE
{
  $$ = $1;
}
;

constness:
{
  $$ = false;
}
| TOK_CONST
{
  $$ = true;
}
;

param: TOK_SOURCE_CODE TOK_SOURCE_CODE
{
  SourceCode *c_type = $1;
  SourceCode *c_symbol = $2;
  $$ = new CodePair( c_type, c_symbol );
}
;

param_list:
{
  $$ = new Vector<CodePair>();
}
  | param
{
  $$ = new Vector<CodePair>( $1 );
}
  | param_list ',' param
{
  $$ = &($1->append( $3 ));
}
;

action_proto_type:
{
  $$ = ActionProto::Common;
}
  | TOK_CONSTRUCTOR
{
  $$ = ActionProto::Constructor;
}
  | TOK_DESTRUCTOR
{
  $$ = ActionProto::Destructor;
}
  | TOK_STATIC
{
  $$ = ActionProto::Static;
}
;

action_declaration: TOK_IDENT '.' TOK_IDENT '=' TOK_SOURCE_CODE
{
  struct : Isa::OOG
  {
    SourceCode* actioncode;
    ActionProto const* actionproto;
    void with( Operation& operation ) override
    {
      if (Action const* prev_action = operation.action( actionproto ))
        {
          Parsing::fileloc.err( "error: action `%s.%s' redefined", operation.symbol.str(), actionproto->m_symbol.str() );
          prev_action->m_fileloc.err( "action `%s.%s' previously defined here", operation.symbol.str(), actionproto->m_symbol.str() );
          exit( -1 );
        }
                        
      operation.add( new Action( actionproto, actioncode, Parsing::comments, Parsing::fileloc ) );
    }
  } oog;
  
  ConstStr    target_symbol = ConstStr( $1, Scanner::symbols );
  ConstStr    action_proto_symbol = ConstStr( $3, Scanner::symbols );
  oog.actioncode = $5;
  /* Action must belong to an action prototype */
  if (not (oog.actionproto = Parsing::isa().actionproto( action_proto_symbol )))
    {
      Parsing::fileloc.err( "error: undefined action prototype `%s'", action_proto_symbol.str() );
      YYABORT;
    }

  if (not Parsing::isa().for_ops(target_symbol, oog))
    {
      /* Target symbol doesn't point to anything */
      Parsing::fileloc.err( "error: undefined operation or group `%s'", target_symbol.str() );
      YYABORT;
    }
  
  Parsing::comments.clear();
}
;

specialization: TOK_SPECIALIZE TOK_IDENT '(' constraint_list ')'
{
  ConstStr            symbol = ConstStr( $2, Scanner::symbols );
  Vector<Constraint>* constraint_list = $4;
  Operation*          operation = Parsing::isa().operation( symbol );
  if (not operation) {
    Parsing::fileloc.err( "error: operation `%s' not defined", symbol.str() );
    YYABORT;
  }
  
  $$ = new Specialization( operation, *constraint_list );
  delete constraint_list;
}
;

user_specialization: TOK_IDENT '.' TOK_SPECIALIZE '(' operation_list ')'
{
  StringVector const* oplist = $5;
  Parsing::isa().m_user_orderings.push_back( Isa::Ordering() );
  Isa::Ordering& order = Parsing::isa().m_user_orderings.back();
  order.fileloc = Parsing::fileloc;
  order.top_op = ConstStr( $1, Scanner::symbols );
  order.under_ops.reserve( oplist->size() );
  for (StringVector::const_iterator itr = oplist->begin(), end = oplist->end(); itr != end; ++itr)
    { order.under_ops.push_back( ConstStr( *itr, Scanner::symbols ) ); }
  delete oplist;
}
;

constraint_list:
  constraint
{
  $$ = new Vector<Constraint>( $1 );
}
  | constraint_list ',' constraint
{
  $$ = &($1->append( $3 ));
}
;

constraint: TOK_IDENT '=' TOK_INTEGER
{
  $$ = new Constraint( ConstStr( $1, Scanner::symbols ), $3 );
}
;

include : TOK_INCLUDE TOK_STRING
{
  if (not Parsing::include( $2) )
    YYABORT;
}
;

group_environment: TOK_GROUP TOK_IDENT TOK_IDENT
{
  ConstStr group_symbol = ConstStr( $2, Scanner::symbols );
  ConstStr command( $3, Scanner::symbols );
  
  try {
    Parsing::isa().group_command( group_symbol, command, Parsing::fileloc );
  } catch (Isa::ParseError pe) {
    YYABORT;
  }
}

group_declaration: TOK_GROUP TOK_IDENT '(' operation_list ')'
{
  ConstStr           group_symbol = ConstStr( $2, Scanner::symbols );
  StringVector*        oplist = $4;
  
  { /* Operations and groups name should not conflict */
    Operation* prev_op = Parsing::isa().operation( group_symbol );
    if (prev_op) {
      Parsing::fileloc.err( "error: group name conflicts with operation `%s'", group_symbol.str() );
      prev_op->fileloc.err( "operation `%s' previously defined here", group_symbol.str() );
      YYABORT;
    }
    
    Group* prev_grp = Parsing::isa().group( group_symbol );
    if (prev_grp) {
      Parsing::fileloc.err( "error: group `%s' redefined", group_symbol.str() );
      prev_grp->fileloc.err( "group `%s' previously defined here", group_symbol.str() );
      YYABORT;
    }
  }
  
  struct : Isa::OOG
  {
    void with( Operation& operation )
    {
      if (opsyms.insert(operation.symbol).second)
        group->operations.append( &operation );
    }
    std::set<ConstStr> opsyms;
    Group* group;
  } oog;
  oog.group = new Group( group_symbol, Parsing::fileloc );
  
  for (StringVector::const_iterator itr = oplist->begin(), end = oplist->end(); itr != end; ++itr)
    {
      ConstStr symbol( *itr, Scanner::symbols );
      
      if (not Parsing::isa().for_ops( symbol, oog ))
        {
          Parsing::fileloc.err( "error: undefined operation or group `%s'", symbol.str() );
          YYABORT;
        }
    }
  
  delete oplist;
  
  $$ = oog.group;
}
;

operation_list:
  TOK_IDENT
{
  $$ = new StringVector( $1 );
}
  | operation_list ',' TOK_IDENT
{
  $$ = &($1->append( $3 ));
}
;

%%

int yyerror( char const* _err ) { Parsing::fileloc.err( "%s! unexpected %s\n", _err, Parsing::tokenname( yychar ).str() ); return 1; }
