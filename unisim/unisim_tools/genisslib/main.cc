/***************************************************************************
                                   main.cc
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

/** @file GenISSLib.c
    @brief generates the instruction set simulator library
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <main.hh>
#include <cli.hh>
#include <isa.hh>
#include <generator.hh>
#include <scanner.hh>
#include <strtools.hh>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <unistd.h>

#if defined(GIL_XPROC) && defined(HAVE_DLFCN_H)
#include <dlfcn.h>
#define GIL_MAIN base_main
#else
#define GIL_MAIN main
#endif // defined(GIL_XPROC) && defined(HAVE_DLFCN_H)

#define DEFAULT_OUTPUT "iss"

struct GIL : public CLI, public Opts
{
  void description() { std::cerr << "Generator of instruction set simulators." << std::endl; }
  
  void version()
  {
    std::cerr << "       " << m_displayname << " v" GENISSLIB_VERSION "\n"
         << "built date : " __DATE__ "\n"
         << "copyright  : " COPYRIGHT "\n"
         << "authors    : " AUTHORS "\n"
         << "emails     : " EMAILS << std::endl;
  }
  
  GIL()
    : CLI(), Opts(DEFAULT_OUTPUT), inputname( 0 ), lookupdirs()
  {}

  virtual char const* appname() const override { return GENISSLIB; }
  virtual char const* appversion() const override { return GENISSLIB_VERSION; }
  virtual ConstStr locate( char const* _name ) const override;
  bool add_lookupdir( char const* _dir );
  
  bool setminwordsize( char const* _arg )
  {
    if (not _arg) return false;
    minwordsize = strtoul( _arg, 0, 0 );
    return true;
  }
  
  bool setverbosity( char const* _arg )
  {
    if (not _arg) return false;
    verbosity = strtoul( _arg, 0, 0 );
    return true;
  }
  
  bool on_off( bool GIL::*_member, char const* _arg )
  {
    if (not _arg) return false;
    this->*_member = (strcmp( _arg, "on" ) == 0);
    return true;
  }
  
  void parse( CLI::Args& _args )
  {
    if (_args.match( "-I", "<directory>", "Adds the directory <directory> "
                     "to the search paths for include directives." ))
      {
        if( this->add_lookupdir( _args.pop_front() ) ) return;
        std::cerr << GENISSLIB ": '-I' must be followed by a directory.\n";
        help();
        throw CLI::Exit_t( 1 );
      }
    
    if (_args.match( "-o,--output", "<output>", "Sets the prefix "
                     "of the generated source files to <output>; "
                     "(default is <output>=\"" DEFAULT_OUTPUT "\")." ))
      {
        if( (outputprefix = _args.pop_front()) ) return;
        std::cerr << GENISSLIB ": '-o' must be followed by an output name.\n";
        help();
        throw CLI::Exit_t( 1 );
      }
    
    if (_args.match( "-w,--min-word-size", "<size>", "Uses <size> as the "
                     "minimum bit size for holding an operand bit field." ))
      {
        if( this->setminwordsize( _args.pop_front() ) ) return;
        std::cerr << GENISSLIB ": '-w' must be followed by a size.\n";
        help();
        throw CLI::Exit_t( 1 );
      }
    
    if (_args.match( "--specialization", "on/off", "Toggles specialized "
                     "operation generation (default: on)." ))
      {
        if( this->on_off( &GIL::specialization, _args.pop_front() ) ) return;
        std::cerr << GENISSLIB ": '--specialization' must be followed by 'on' or 'off'.\n";
        help();
        throw CLI::Exit_t( 1 );
      }

    if (_args.match( "--source-lines", "on/off", "Toggles the output of "
                     "source line references in generated files (default: on)." ))
      {
        if (this->on_off( &GIL::sourcelines, _args.pop_front() )) return;
        std::cerr << GENISSLIB ": '--source-lines' must be followed by 'on' or 'off'.\n";
        help();
        throw CLI::Exit_t( 1 );
      }

    if (_args.match( "--comments", "on/off", "Toggles the output of comments in generated files (default: on)." ))
      {
        if (this->on_off( &GIL::comments, _args.pop_front() )) return;
        std::cerr << GENISSLIB ": '--comments' must be followed by 'on' or 'off'.\n";
        help();
        throw CLI::Exit_t( 1 );
      }

    if (_args.match( "--private-members", "on/off", "Toggles the output of "
                     "private class specifier to protect generated class members (default: on)." ))
      {
        if (this->on_off( &GIL::privatemembers, _args.pop_front() )) return;
        std::cerr << GENISSLIB ": '--source-lines' must be followed by 'on' or 'off'.\n";
        help();
        throw CLI::Exit_t( 1 );
      }

    if (_args.match( "-v,--version", "", "Displays " GENISSLIB " version and exits." ))
      {
        version();
        throw CLI::Exit_t( 0 );
      }
    
    if (_args.match( "-M", "<filename>", "Output a rule file (<filename>) suitable for make "
                     "describing the dependencies of the main source file." ))
      {
        if( (depfilename = _args.pop_front()) ) return;
        std::cerr << GENISSLIB ": '-M' must be followed by a file name.\n";
        help();
        throw CLI::Exit_t( 1 );
      }

    if (_args.match( "-E,--expand", "<filename>", "Expands the preprocessed input file to "
                     "<filename>." ))
      {
        if( (expandname = _args.pop_front()) ) return;
        std::cerr << GENISSLIB ": '-E' must be followed by a file name.\n";
        help();
        throw CLI::Exit_t( 1 );
      }
    
    if (_args.match( "--verbosity", "<verb_level>", "Sets verbosity level to <verb_level>." ))
      {
        if (this->setverbosity( _args.pop_front() )) return;
        std::cerr << GENISSLIB ": '--verbosity' must be followed by a size.\n";
        help();
        throw CLI::Exit_t( 1 );
    }
    
    if (_args.match( not inputname, "<inputfile>", "The input file to process" ))
      {
        inputname = _args.pop_front();
        return;
      }
  }

  char const*      inputname;
  std::vector<ConstStr>   lookupdirs;
};

int
GIL_MAIN (int argc, char** argv, char** envp)
{
  GIL gil;
  gil.set( GENISSLIB, argv[0] );
  try {
    gil.process( argc-1, argv+1 );
    
    if (not gil.inputname) {
      std::cerr << GENISSLIB ": no input file.\n";
      gil.help();
      throw CLI::Exit_t( 1 );
    }
    
    Isa isa;
    if (not Scanner::parse( gil.inputname, gil, isa ) )
      throw CLI::Exit_t( 1 );
  
    if (gil.expandname) {
      std::ofstream expandfile( gil.expandname );
      if (not expandfile.good()) {
        std::cerr << GENISSLIB ": can't open output file `" << gil.expandname << "'" << std::endl;
        throw CLI::Exit_t( 1 );
      }
      
      isa.expand( expandfile );
    }
    
    if (gil.depfilename) {
      std::ofstream depfile( gil.depfilename );
      if (not depfile.good()) {
        std::cerr << GENISSLIB ": can't open output file `" << gil.depfilename << "'" << std::endl;
        throw CLI::Exit_t( 1 );
      }
      
      isa.deps( depfile, gil.outputprefix );
    }
    
    if (not isa.sanity_checks()) throw CLI::Exit_t( 1 );

    // Specialization
    if (gil.specialization)
      isa.specialize();
    
    struct Gen {
      Gen( Generator* _gen ) : gen(_gen) {} Generator* gen;
      ~Gen() { delete gen; }
      Generator* operator -> () { return gen; }
    } generator( isa.generator( isa, gil ) );
    
    // generator->init( isa, gil.verbosity );
    
    try {
      // Back-end specific preprocess
      generator->finalize();
      // ISA statistics
      generator->isastats();
      // ISS Generation
      generator->iss();
    } catch (Generator::Error) {
      std::cerr << GENISSLIB ": compilation aborted.\n";
      throw CLI::Exit_t( 1 );
    }
  } catch( CLI::Exit_t& _code ) {
    return _code.m_value;
  }
  
  return 0;
}

bool
GIL::add_lookupdir( char const* _dir )
{
  if (not _dir)
    return false;
#ifdef WIN32
  if ((((_dir[0] >= 'a' && _dir[0] <= 'z') || (_dir[0] >= 'A' && _dir[0] <= 'Z')) && (_dir[1] == ':') && ((_dir[2] == '\\') || (_dir[2] == '/'))) || (*_dir == '/'))
  {
     // convert '\' into '/' to have a UNIX friendly path as gcc doesn't like '\' in filenames in #line directives
     int len = strlen(_dir);
     char cv_dir[len + 1];
     const char *pch;
     char *cv_pch;
     for(pch = _dir, cv_pch = cv_dir; *pch; pch++, cv_pch++)
     {
        if(*pch == '\\') *cv_pch = '/'; else *cv_pch = *pch;
     }
     cv_dir[len] = 0;
     lookupdirs.push_back( cv_dir );
  }
#else
  if (*_dir == '/')
  {
    lookupdirs.push_back( _dir );
    return true;
  }
#endif
  
  std::string buffer;
  for (intptr_t capacity = 128; true; capacity *= 2) {
    char storage[capacity];
    if (not getcwd( storage, capacity )) {
      if (errno != ERANGE)
        return false;
      continue; 
    }
#ifdef WIN32
    assert((((storage[0] >= 'a' && storage[0] <= 'z') || (storage[0] >= 'A' && storage[0] <= 'Z')) && (storage[1] == ':') && ((storage[2] == '\\') || (storage[2] == '/'))) || (*storage == '/'));
    // convert '\' into '/' to have a UNIX friendly path as gcc doesn't like '\' in filenames in #line directives
    {
      char *pch;
      for(pch = storage; *pch; pch++)
      {
         if(*pch == '\\') *pch = '/';
      }
    }
    int len = strlen(_dir);
    char cv_dir[len + 1];
    {
      const char *pch;
      char *cv_pch;
      for(pch = _dir, cv_pch = cv_dir; *pch; pch++, cv_pch++)
      {
         if(*pch == '\\') *cv_pch = '/'; else *cv_pch = *pch;
      }
      cv_dir[len] = 0;
    }
    buffer = buffer + storage + "/" + cv_dir;
#else
    assert( storage[0] == '/' ); // a directory path does not start with '/' on a windows host !
    buffer = buffer + storage + "/" + _dir;
#endif
    lookupdirs.push_back( buffer.c_str() );
    break;
  }
  return true;
}

ConstStr
GIL::locate( char const* _name ) const
{
  for (auto const& dir : lookupdirs)
    {
      std::string buffer = std::string() + dir.str() + "/" + _name;
      if (access( buffer.c_str(), R_OK ) != 0) continue;
      return buffer.c_str();
    }
  return _name;
}

#if defined(GIL_XPROC) && defined(HAVE_DLFCN_H)

bool
log_dl_failure( std::ostream& _sink ) {
  char const* err = dlerror();
  if (err) _sink << GENISSLIB ": " << err << std::endl;
  return err;
}

typedef int (*gxp_abi_t) ();
typedef int (*gxp_main_t) (int argc, char** argv, char** envp);

int
main (int argc, char** argv, char** envp)
{
  if (argc < 2 or (strncmp( argv[1], "xproc=", 6 ) != 0))
    return base_main (argc, argv, envp);
  
  char const* xproc_path = argv[1] + 6;
  
  // Open the xproc module.
  void* handle = dlopen( xproc_path, RTLD_NOW );
  if (log_dl_failure (std::cerr)) return 1;
  
  // Check that xproc module and genisslib abi match.
  gxp_abi_t gxp_abi = (gxp_abi_t)dlsym (handle, "gxp_abi");
  if (log_dl_failure (std::cerr)) return 1;
  if (gxp_abi() != 1) {
    std::cerr << GENISSLIB ": process extension '" << xproc_path
         << "' doesn't match current version of genisslib.\n";
    return 1;
  }
  
  // Launch xproc module main procedure
  gxp_main_t gxp_main = (gxp_main_t)dlsym (handle, "gxp_main");
  if (log_dl_failure (std::cerr)) return 1;
  argv[1] = argv[0];
  return gxp_main (argc-1, argv+1, envp);
}

#endif // defined(GIL_XPROC) && defined(HAVE_DLFCN_H)
