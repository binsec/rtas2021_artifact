# GDB helper scripts to attach dbgate methods
break dbgate_write
commands
silent
py dbgate.write( gdb.parse_and_eval("fd"), gdb.inferiors()[0].read_memory( gdb.parse_and_eval("buffer"), gdb.parse_and_eval("size") ).tobytes() )
cont
end

break dbgate_open
commands
silent
py dbgate.open( gdb.parse_and_eval("path") )
cont
end

break dbgate_close
commands
silent
py dbgate.close( gdb.parse_and_eval("fd") )
cont
end
