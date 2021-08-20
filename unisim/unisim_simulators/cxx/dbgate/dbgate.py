"""
Python master module for dbgate usage in GDB sessions.
When used as a main program, install or fix link with shared library.
"""

import os

if __name__ == "__main__":
    # Install or fix link with shared library
    import sys
    callname, sharedlib = sys.argv
    moduledir, __init__ = os.path.split(__file__)

    # Checking that the installation is sound
    assert __init__ == '__init__.py'
    assert os.path.exists( os.path.join( moduledir, 'dbgate.gdb' ) )
    
    # Checking the link and fixing if needed
    sharedlib = os.path.realpath( sharedlib )
    link = os.path.join( moduledir, '_dbgate.so' )
    if os.path.realpath( link ) == sharedlib:
        sys.stderr.write( 'Installation is OK.\n' )
        sys.exit(0)
    
    if os.path.islink( link ):
        os.remove( link )
    if os.path.exists( link ):
        sys.stderr.write( "Can't remove %r" % link )
        sys.exit(1)

    sys.stderr.write( 'Linking %r <= %r\n' % (sharedlib, link) )
    os.symlink( sharedlib, link )
    sys.exit(0)
    
    

from ._dbgate import DBGated
import re, gdb

gdbdefs = os.path.abspath(os.path.join(os.path.dirname(__file__),'dbgate.gdb'))

PORT=12345
ROOT="/tmp/dbgate"

SERVER=None

write=None
open=None
close=None

def setup():
    gdb.execute("source %s" % gdbdefs)
    global SERVER, write, open, close 
    SERVER = DBGated(PORT,ROOT)
    write = SERVER.write
    open = SERVER.open
    close = SERVER.close

CALLRE = re.compile('^(\w*) *\((.*)\)$')

def call( callstr ):
    match = CALLRE.match(callstr)
    if not match:
        raise Exception( "malformed %r" % callstr )
    fun, args = match.groups()
    fd = None
    if '@' in args:
        before, path, after = args.split( '@', 2 )
        fd = open(path)
        args = '%s%d%s' % (before,fd,after)

    gdb.execute('break %s' % fun)
    try:
        gdb.execute('call %s(%s)' % (fun,args))
    except gdb.error as err:
        if not err.args[0].startswith('The program being debugged stopped while in a function called from GDB'):
            raise
    gdb.execute('cont')

    if fd is not None:
        close(fd)

