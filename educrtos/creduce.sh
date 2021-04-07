config_file=/home/olivier/simple-os/binsec.ini
exe_full=system_10tasks.exe
scheduling=rr
shape_arg=

cp /home/olivier/simple-os/{kernel_10tasks.ld,system_desc_10tasks.o,copy64.o} ./
gcc -m32 -nostdlib -ffreestanding -Wl,-Tkernel_10tasks.ld -o system_10tasks.exe -ffreestanding -O1 -Wall -Wextra -std=gnu11 -fno-pic -foptimize-sibling-calls  -fno-asynchronous-unwind-tables  -fno-stack-protector -g 			           -DROUND_ROBIN_SCHEDULING singlefile.i system_desc_10tasks.o copy64.o -lgcc
timeout 10s /tmp/binsec -config /home/olivier/simple-os/binsec.ini system_10tasks.exe -codex-x86-types rr -codex-nb-tasks 10 -codex-no-dyn-threads -codex-debug-level 0 |& grep -F '"domains/constraint_domain2.ml", line 834'
exit_code=$?
if [ "$?" == 124 ]; then exit 1; else exit $exit_code; fi
