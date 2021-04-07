module  Common = Common_piqi

module rec Libcall_piqi:
  sig
    type uint64 = int64
    type lib_ident =
      [
        | `invalid
        | `printf
        | `strcpy
        | `atoi
        | `malloc
        | `free
        | `memcpy
        | `memset
        | `fgetc
        | `read
        | `strchr
        | `exit
        | `strcmp
        | `ctype_b_loc
        | `fscanf
        | `strrchr
        | `strncpy
        | `fstat
        | `realloc
        | `mmap
        | `qsort
        | `bsearch
        | `open_stub
        | `lseek
        | `fread
        | `strncmp
        | `memcmp
        | `fxstat64
        | `getmodulehandle
        | `getprocaddress
        | `getmainargs
        | `gethostname
        | `generic
      ]
    type fscanf_enum =
      [
        | `d
        | `dd
        | `i
        | `o
        | `u
        | `x
        | `xx
        | `f
        | `e
        | `g
        | `ee
        | `a
        | `s
        | `c
        | `sb
        | `p
        | `n
      ]
    type libcall_pol = Libcall_pol.t
    type libcall_t = Libcall_t.t
    type generic_pol = Generic_pol.t
    type ctype_b_loc_pol = Ctype_b_loc_pol.t
    type exit_pol = Exit_pol.t
    type fscanf_pol = Fscanf_pol.t
    type read_pol = Read_pol.t
    type fread_pol = Fread_pol.t
    type strcmp_pol = Strcmp_pol.t
    type strncmp_pol = Strncmp_pol.t
    type memcmp_pol = Memcmp_pol.t
    type strchr_pol = Strchr_pol.t
    type strncpy_pol = Strncpy_pol.t
    type fstat_pol = Fstat_pol.t
    type fxstat64_pol = Fxstat64_pol.t
    type realloc_pol = Realloc_pol.t
    type mmap_pol = Mmap_pol.t
    type qsort_pol = Qsort_pol.t
    type bsearch_pol = Bsearch_pol.t
    type open_pol = Open_pol.t
    type strcpy_pol = Strcpy_pol.t
    type printf_pol = Printf_pol.t
    type atoi_pol = Atoi_pol.t
    type malloc_pol = Malloc_pol.t
    type getmodulehandle_pol = Getmodulehandle_pol.t
    type getprocaddress_pol = Getprocaddress_pol.t
    type getmainargs_pol = Getmainargs_pol.t
    type gethostname_pol = Gethostname_pol.t
    type free_pol = Free_pol.t
    type memcpy_pol = Memcpy_pol.t
    type memset_pol = Memset_pol.t
    type fgetc_pol = Fgetc_pol.t
    type lseek_pol = Lseek_pol.t
    type exit_t = Exit_t.t
    type generic_t = Generic_t.t
    type fscanf_elem_t = Fscanf_elem_t.t
    type fscanf_t = Fscanf_t.t
    type ctype_b_loc_t = Ctype_b_loc_t.t
    type read_t = Read_t.t
    type fread_t = Fread_t.t
    type strchr_t = Strchr_t.t
    type strncpy_t = Strncpy_t.t
    type fstat_t = Fstat_t.t
    type fxstat64_t = Fxstat64_t.t
    type realloc_t = Realloc_t.t
    type mmap_t = Mmap_t.t
    type qsort_t = Qsort_t.t
    type bsearch_t = Bsearch_t.t
    type open_t = Open_t.t
    type strcpy_t = Strcpy_t.t
    type strcmp_t = Strcmp_t.t
    type strncmp_t = Strncmp_t.t
    type memcmp_t = Memcmp_t.t
    type printf_t = Printf_t.t
    type atoi_t = Atoi_t.t
    type malloc_t = Malloc_t.t
    type getmodulehandle_t = Getmodulehandle_t.t
    type getprocaddress_t = Getprocaddress_t.t
    type getmainargs_t = Getmainargs_t.t
    type gethostname_t = Gethostname_t.t
    type free_t = Free_t.t
    type memcpy_t = Memcpy_t.t
    type memset_t = Memset_t.t
    type fgetc_t = Fgetc_t.t
    type lseek_t = Lseek_t.t
  end = Libcall_piqi
and Libcall_pol:
  sig
    type t = {
      mutable name: string;
      mutable ident: Libcall_piqi.lib_ident;
      mutable action: Common.tracing_action;
      mutable restrict_addresses: Libcall_piqi.uint64 list;
      mutable strcpy: Libcall_piqi.strcpy_pol option;
      mutable printf: Libcall_piqi.printf_pol option;
      mutable atoi: Libcall_piqi.atoi_pol option;
      mutable malloc: Libcall_piqi.malloc_pol option;
      mutable getmodulehandle: Libcall_piqi.getmodulehandle_pol option;
      mutable getprocaddress: Libcall_piqi.getprocaddress_pol option;
      mutable getmainargs: Libcall_piqi.getmainargs_pol option;
      mutable gethostname: Libcall_piqi.gethostname_pol option;
      mutable free: Libcall_piqi.free_pol option;
      mutable memcpy: Libcall_piqi.memcpy_pol option;
      mutable memset: Libcall_piqi.memset_pol option;
      mutable fgetc: Libcall_piqi.fgetc_pol option;
      mutable read: Libcall_piqi.read_pol option;
      mutable strchr: Libcall_piqi.strchr_pol option;
      mutable exit: Libcall_piqi.exit_pol option;
      mutable strcmp: Libcall_piqi.strcmp_pol option;
      mutable ctype_b_loc: Libcall_piqi.ctype_b_loc_pol option;
      mutable fscanf: Libcall_piqi.fscanf_pol option;
      mutable strncpy: Libcall_piqi.strncpy_pol option;
      mutable fstat: Libcall_piqi.fstat_pol option;
      mutable realloc: Libcall_piqi.realloc_pol option;
      mutable mmap: Libcall_piqi.mmap_pol option;
      mutable qsort: Libcall_piqi.qsort_pol option;
      mutable bsearch: Libcall_piqi.bsearch_pol option;
      mutable open_stub: Libcall_piqi.open_pol option;
      mutable lseek: Libcall_piqi.lseek_pol option;
      mutable fread: Libcall_piqi.fread_pol option;
      mutable strncmp: Libcall_piqi.strncmp_pol option;
      mutable memcmp: Libcall_piqi.memcmp_pol option;
      mutable fxstat64: Libcall_piqi.fxstat64_pol option;
      mutable generic: Libcall_piqi.generic_pol option;
    }
  end = Libcall_pol
and Libcall_t:
  sig
    type t = {
      mutable func_addr: Libcall_piqi.uint64;
      mutable func_name: string;
      mutable is_traced: bool;
      mutable ident: Libcall_piqi.lib_ident;
      mutable strcpy: Libcall_piqi.strcpy_t option;
      mutable printf: Libcall_piqi.printf_t option;
      mutable atoi: Libcall_piqi.atoi_t option;
      mutable malloc: Libcall_piqi.malloc_t option;
      mutable getmodulehandle: Libcall_piqi.getmodulehandle_t option;
      mutable getprocaddress: Libcall_piqi.getprocaddress_t option;
      mutable getmainargs: Libcall_piqi.getmainargs_t option;
      mutable gethostname: Libcall_piqi.gethostname_t option;
      mutable free: Libcall_piqi.free_t option;
      mutable memcpy: Libcall_piqi.memcpy_t option;
      mutable memset: Libcall_piqi.memset_t option;
      mutable fgetc: Libcall_piqi.fgetc_t option;
      mutable generic: Libcall_piqi.generic_t option;
      mutable read: Libcall_piqi.read_t option;
      mutable strchr: Libcall_piqi.strchr_t option;
      mutable exit: Libcall_piqi.exit_t option;
      mutable strcmp: Libcall_piqi.strcmp_t option;
      mutable ctype_b_loc: Libcall_piqi.ctype_b_loc_t option;
      mutable fscanf: Libcall_piqi.fscanf_t option;
      mutable strncpy: Libcall_piqi.strncpy_t option;
      mutable fstat: Libcall_piqi.fstat_t option;
      mutable realloc: Libcall_piqi.realloc_t option;
      mutable mmap: Libcall_piqi.mmap_t option;
      mutable qsort: Libcall_piqi.qsort_t option;
      mutable bsearch: Libcall_piqi.bsearch_t option;
      mutable open_stub: Libcall_piqi.open_t option;
      mutable lseek: Libcall_piqi.lseek_t option;
      mutable fread: Libcall_piqi.fread_t option;
      mutable strncmp: Libcall_piqi.strncmp_t option;
      mutable memcmp: Libcall_piqi.memcmp_t option;
      mutable fxstat64: Libcall_piqi.fxstat64_t option;
    }
  end = Libcall_t
and Generic_pol:
  sig
    type t = {
      mutable ret: Common.action;
    }
  end = Generic_pol
and Ctype_b_loc_pol:
  sig
    type t = {
      mutable table: Common.memory_pol;
      mutable ret: Common.action;
    }
  end = Ctype_b_loc_pol
and Exit_pol:
  sig
    type t = {
      _dummy: unit;
    }
  end = Exit_pol
and Fscanf_pol:
  sig
    type t = {
      mutable stream: Common.action;
      mutable elems: Common.action;
      mutable ret: Common.action;
    }
  end = Fscanf_pol
and Read_pol:
  sig
    type t = {
      mutable fd: Common.action;
      mutable buf: Common.memory_pol;
      mutable count: Common.action;
      mutable ret: Common.action;
    }
  end = Read_pol
and Fread_pol:
  sig
    type t = {
      mutable ptr: Common.memory_pol;
      mutable size: Common.action;
      mutable nmemb: Common.action;
      mutable stream: Common.action;
      mutable ret: Common.action;
    }
  end = Fread_pol
and Strcmp_pol:
  sig
    type t = {
      mutable src: Common.memory_pol;
      mutable dst: Common.memory_pol;
      mutable ret: Common.action;
    }
  end = Strcmp_pol
and Strncmp_pol:
  sig
    type t = {
      mutable s1: Common.action;
      mutable s2: Common.action;
      mutable n: Common.action;
      mutable ret: Common.action;
    }
  end = Strncmp_pol
and Memcmp_pol:
  sig
    type t = {
      mutable s1: Common.action;
      mutable s2: Common.action;
      mutable n: Common.action;
      mutable ret: Common.action;
    }
  end = Memcmp_pol
and Strchr_pol:
  sig
    type t = {
      mutable sc: Common.memory_pol;
      mutable c: Common.memory_pol;
      mutable ret: Common.action;
    }
  end = Strchr_pol
and Strncpy_pol:
  sig
    type t = {
      mutable dst: Common.action;
      mutable src: Common.action;
      mutable n: Common.action;
      mutable ret: Common.action;
    }
  end = Strncpy_pol
and Fstat_pol:
  sig
    type t = {
      _dummy: unit;
    }
  end = Fstat_pol
and Fxstat64_pol:
  sig
    type t = {
      _dummy: unit;
    }
  end = Fxstat64_pol
and Realloc_pol:
  sig
    type t = {
      mutable ptr: Common.action;
      mutable size: Common.action;
      mutable ret: Common.action;
    }
  end = Realloc_pol
and Mmap_pol:
  sig
    type t = {
      mutable addr: Common.action;
      mutable length: Common.action;
      mutable prot: Common.action;
      mutable flags: Common.action;
      mutable fd: Common.action;
      mutable offset: Common.action;
      mutable ret: Common.memory_pol;
    }
  end = Mmap_pol
and Qsort_pol:
  sig
    type t = {
      mutable base: Common.memory_pol;
      mutable nmemb: Common.action;
      mutable size: Common.action;
      mutable compare: Common.action;
    }
  end = Qsort_pol
and Bsearch_pol:
  sig
    type t = {
      mutable key: Common.memory_pol;
      mutable base: Common.memory_pol;
      mutable nmemb: Common.action;
      mutable size: Common.action;
      mutable compare: Common.action;
      mutable ret: Common.memory_pol;
    }
  end = Bsearch_pol
and Open_pol:
  sig
    type t = {
      mutable pathname: Common.memory_pol;
      mutable flags: Common.action;
      mutable mode: Common.action;
      mutable ret: Common.action;
    }
  end = Open_pol
and Strcpy_pol:
  sig
    type t = {
      mutable src: Common.memory_pol;
      mutable dst: Common.memory_pol;
      mutable ret: Common.action;
    }
  end = Strcpy_pol
and Printf_pol:
  sig
    type t = {
      mutable format: Common.memory_pol;
      mutable ret: Common.action;
    }
  end = Printf_pol
and Atoi_pol:
  sig
    type t = {
      mutable src: Common.memory_pol;
      mutable ret: Common.action;
    }
  end = Atoi_pol
and Malloc_pol:
  sig
    type t = {
      mutable size: Common.action;
      mutable ret: Common.action;
    }
  end = Malloc_pol
and Getmodulehandle_pol:
  sig
    type t = {
      mutable module_name: Common.memory_pol;
      mutable ret: Common.action;
    }
  end = Getmodulehandle_pol
and Getprocaddress_pol:
  sig
    type t = {
      mutable hmodule: Common.action;
      mutable proc_name: Common.memory_pol;
      mutable ret: Common.action;
    }
  end = Getprocaddress_pol
and Getmainargs_pol:
  sig
    type t = {
      mutable argc: Common.memory_pol;
      mutable argv: Common.memory_pol;
      mutable env: Common.memory_pol;
      mutable dowildcard: Common.action;
      mutable startinfo: Common.memory_pol;
      mutable ret: Common.action;
    }
  end = Getmainargs_pol
and Gethostname_pol:
  sig
    type t = {
      mutable name: Common.memory_pol;
      mutable namelen: Common.action;
      mutable ret: Common.action;
    }
  end = Gethostname_pol
and Free_pol:
  sig
    type t = {
      mutable ptr: Common.action;
    }
  end = Free_pol
and Memcpy_pol:
  sig
    type t = {
      mutable dest: Common.memory_pol;
      mutable src: Common.memory_pol;
      mutable size: Common.action;
      mutable ret: Common.action;
    }
  end = Memcpy_pol
and Memset_pol:
  sig
    type t = {
      mutable s: Common.memory_pol;
      mutable c: Common.action;
      mutable size: Common.action;
      mutable ret: Common.action;
    }
  end = Memset_pol
and Fgetc_pol:
  sig
    type t = {
      mutable stream: Common.action;
      mutable ret: Common.action;
    }
  end = Fgetc_pol
and Lseek_pol:
  sig
    type t = {
      mutable fd: Common.action;
      mutable offset: Common.action;
      mutable whence: Common.action;
      mutable ret: Common.action;
    }
  end = Lseek_pol
and Exit_t:
  sig
    type t = {
      _dummy: unit;
    }
  end = Exit_t
and Generic_t:
  sig
    type t = {
      mutable ret: Libcall_piqi.uint64;
    }
  end = Generic_t
and Fscanf_elem_t:
  sig
    type t = {
      mutable type_: Libcall_piqi.fscanf_enum;
      mutable addr: Libcall_piqi.uint64;
      mutable value_dec: Libcall_piqi.uint64 option;
      mutable value_string: string option;
    }
  end = Fscanf_elem_t
and Fscanf_t:
  sig
    type t = {
      mutable stream: Libcall_piqi.uint64;
      mutable elems: Libcall_piqi.fscanf_elem_t list;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Fscanf_t
and Ctype_b_loc_t:
  sig
    type t = {
      mutable table: Common.memory_t;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Ctype_b_loc_t
and Read_t:
  sig
    type t = {
      mutable fd: Libcall_piqi.uint64;
      mutable buf: Common.memory_t;
      mutable count: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Read_t
and Fread_t:
  sig
    type t = {
      mutable ptr: Common.memory_t;
      mutable size: Libcall_piqi.uint64;
      mutable nmemb: Libcall_piqi.uint64;
      mutable stream: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Fread_t
and Strchr_t:
  sig
    type t = {
      mutable s: Libcall_piqi.uint64;
      mutable c: Libcall_piqi.uint64;
      mutable size_max: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Strchr_t
and Strncpy_t:
  sig
    type t = {
      mutable dst: Libcall_piqi.uint64;
      mutable src: Libcall_piqi.uint64;
      mutable n: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Strncpy_t
and Fstat_t:
  sig
    type t = {
      mutable fd: Libcall_piqi.uint64;
      mutable buf: Common.memory_t;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Fstat_t
and Fxstat64_t:
  sig
    type t = {
      mutable vers: Libcall_piqi.uint64;
      mutable fd: Libcall_piqi.uint64;
      mutable buf: Common.memory_t;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Fxstat64_t
and Realloc_t:
  sig
    type t = {
      mutable ptr: Libcall_piqi.uint64;
      mutable size: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Realloc_t
and Mmap_t:
  sig
    type t = {
      mutable addr: Libcall_piqi.uint64;
      mutable length: Libcall_piqi.uint64;
      mutable prot: Libcall_piqi.uint64;
      mutable flags: Libcall_piqi.uint64;
      mutable fd: Libcall_piqi.uint64;
      mutable offset: Libcall_piqi.uint64;
      mutable ret: Common.memory_t;
    }
  end = Mmap_t
and Qsort_t:
  sig
    type t = {
      mutable base: Common.memory_t;
      mutable nmemb: Libcall_piqi.uint64;
      mutable size: Libcall_piqi.uint64;
      mutable compare: Libcall_piqi.uint64;
    }
  end = Qsort_t
and Bsearch_t:
  sig
    type t = {
      mutable key: Common.memory_t;
      mutable base: Common.memory_t;
      mutable nmemb: Libcall_piqi.uint64;
      mutable size: Libcall_piqi.uint64;
      mutable compare: Libcall_piqi.uint64;
      mutable ret: Common.memory_t;
    }
  end = Bsearch_t
and Open_t:
  sig
    type t = {
      mutable pathname: Common.memory_t;
      mutable flags: Libcall_piqi.uint64;
      mutable mode: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Open_t
and Strcpy_t:
  sig
    type t = {
      mutable src: Common.memory_t;
      mutable dst: Common.memory_t;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Strcpy_t
and Strcmp_t:
  sig
    type t = {
      mutable s1: Libcall_piqi.uint64;
      mutable s2: Libcall_piqi.uint64;
      mutable size_max_s1: Libcall_piqi.uint64;
      mutable size_max_s2: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Strcmp_t
and Strncmp_t:
  sig
    type t = {
      mutable s1: Libcall_piqi.uint64;
      mutable s2: Libcall_piqi.uint64;
      mutable n: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Strncmp_t
and Memcmp_t:
  sig
    type t = {
      mutable s1: Libcall_piqi.uint64;
      mutable s2: Libcall_piqi.uint64;
      mutable n: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Memcmp_t
and Printf_t:
  sig
    type t = {
      mutable format: Common.memory_t;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Printf_t
and Atoi_t:
  sig
    type t = {
      mutable src: Common.memory_t;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Atoi_t
and Malloc_t:
  sig
    type t = {
      mutable size: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Malloc_t
and Getmodulehandle_t:
  sig
    type t = {
      mutable module_name: Common.memory_t;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Getmodulehandle_t
and Getprocaddress_t:
  sig
    type t = {
      mutable hmodule: Libcall_piqi.uint64;
      mutable proc_name: Common.memory_t;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Getprocaddress_t
and Getmainargs_t:
  sig
    type t = {
      mutable argc: Common.memory_t;
      mutable argv: Common.memory_t;
      mutable env: Common.memory_t;
      mutable dowildcard: Libcall_piqi.uint64;
      mutable startinfo: Common.memory_t;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Getmainargs_t
and Gethostname_t:
  sig
    type t = {
      mutable name: Common.memory_t;
      mutable namelen: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Gethostname_t
and Free_t:
  sig
    type t = {
      mutable ptr: Libcall_piqi.uint64;
    }
  end = Free_t
and Memcpy_t:
  sig
    type t = {
      mutable dest: Common.memory_t;
      mutable src: Common.memory_t;
      mutable size: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Memcpy_t
and Memset_t:
  sig
    type t = {
      mutable s: Common.memory_t;
      mutable c: Libcall_piqi.uint64;
      mutable size: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Memset_t
and Fgetc_t:
  sig
    type t = {
      mutable stream: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Fgetc_t
and Lseek_t:
  sig
    type t = {
      mutable fd: Libcall_piqi.uint64;
      mutable offset: Libcall_piqi.uint64;
      mutable whence: Libcall_piqi.uint64;
      mutable ret: Libcall_piqi.uint64;
    }
  end = Lseek_t


let rec parse_string x = Piqirun.string_of_block x

and parse_uint64 x = Piqirun.int64_of_varint x
and packed_parse_uint64 x = Piqirun.int64_of_packed_varint x

and parse_bool x = Piqirun.bool_of_varint x
and packed_parse_bool x = Piqirun.bool_of_packed_varint x

and parse_libcall_pol x =
  let x = Piqirun.parse_record x in
  let _name, x = Piqirun.parse_required_field 1 parse_string x in
  let _ident, x = Piqirun.parse_required_field 2 parse_lib_ident x ~default:"\b\255\001" in
  let _action, x = Piqirun.parse_required_field 3 Common.parse_tracing_action x ~default:"\b\000" in
  let _restrict_addresses, x = Piqirun.parse_repeated_field 4 parse_uint64 x in
  let _strcpy, x = Piqirun.parse_optional_field 5 parse_strcpy_pol x in
  let _printf, x = Piqirun.parse_optional_field 6 parse_printf_pol x in
  let _atoi, x = Piqirun.parse_optional_field 7 parse_atoi_pol x in
  let _malloc, x = Piqirun.parse_optional_field 8 parse_malloc_pol x in
  let _getmodulehandle, x = Piqirun.parse_optional_field 9 parse_getmodulehandle_pol x in
  let _getprocaddress, x = Piqirun.parse_optional_field 10 parse_getprocaddress_pol x in
  let _getmainargs, x = Piqirun.parse_optional_field 11 parse_getmainargs_pol x in
  let _gethostname, x = Piqirun.parse_optional_field 12 parse_gethostname_pol x in
  let _free, x = Piqirun.parse_optional_field 13 parse_free_pol x in
  let _memcpy, x = Piqirun.parse_optional_field 14 parse_memcpy_pol x in
  let _memset, x = Piqirun.parse_optional_field 15 parse_memset_pol x in
  let _fgetc, x = Piqirun.parse_optional_field 16 parse_fgetc_pol x in
  let _read, x = Piqirun.parse_optional_field 17 parse_read_pol x in
  let _strchr, x = Piqirun.parse_optional_field 18 parse_strchr_pol x in
  let _exit, x = Piqirun.parse_optional_field 19 parse_exit_pol x in
  let _strcmp, x = Piqirun.parse_optional_field 20 parse_strcmp_pol x in
  let _ctype_b_loc, x = Piqirun.parse_optional_field 22 parse_ctype_b_loc_pol x in
  let _fscanf, x = Piqirun.parse_optional_field 23 parse_fscanf_pol x in
  let _strncpy, x = Piqirun.parse_optional_field 24 parse_strncpy_pol x in
  let _fstat, x = Piqirun.parse_optional_field 25 parse_fstat_pol x in
  let _realloc, x = Piqirun.parse_optional_field 26 parse_realloc_pol x in
  let _mmap, x = Piqirun.parse_optional_field 27 parse_mmap_pol x in
  let _qsort, x = Piqirun.parse_optional_field 28 parse_qsort_pol x in
  let _bsearch, x = Piqirun.parse_optional_field 29 parse_bsearch_pol x in
  let _open_stub, x = Piqirun.parse_optional_field 30 parse_open_pol x in
  let _lseek, x = Piqirun.parse_optional_field 31 parse_lseek_pol x in
  let _fread, x = Piqirun.parse_optional_field 32 parse_fread_pol x in
  let _strncmp, x = Piqirun.parse_optional_field 33 parse_strncmp_pol x in
  let _memcmp, x = Piqirun.parse_optional_field 34 parse_memcmp_pol x in
  let _fxstat64, x = Piqirun.parse_optional_field 35 parse_fxstat64_pol x in
  let _generic, x = Piqirun.parse_optional_field 50 parse_generic_pol x in
  Piqirun.check_unparsed_fields x;
  {
    Libcall_pol.name = _name;
    Libcall_pol.ident = _ident;
    Libcall_pol.action = _action;
    Libcall_pol.restrict_addresses = _restrict_addresses;
    Libcall_pol.strcpy = _strcpy;
    Libcall_pol.printf = _printf;
    Libcall_pol.atoi = _atoi;
    Libcall_pol.malloc = _malloc;
    Libcall_pol.getmodulehandle = _getmodulehandle;
    Libcall_pol.getprocaddress = _getprocaddress;
    Libcall_pol.getmainargs = _getmainargs;
    Libcall_pol.gethostname = _gethostname;
    Libcall_pol.free = _free;
    Libcall_pol.memcpy = _memcpy;
    Libcall_pol.memset = _memset;
    Libcall_pol.fgetc = _fgetc;
    Libcall_pol.read = _read;
    Libcall_pol.strchr = _strchr;
    Libcall_pol.exit = _exit;
    Libcall_pol.strcmp = _strcmp;
    Libcall_pol.ctype_b_loc = _ctype_b_loc;
    Libcall_pol.fscanf = _fscanf;
    Libcall_pol.strncpy = _strncpy;
    Libcall_pol.fstat = _fstat;
    Libcall_pol.realloc = _realloc;
    Libcall_pol.mmap = _mmap;
    Libcall_pol.qsort = _qsort;
    Libcall_pol.bsearch = _bsearch;
    Libcall_pol.open_stub = _open_stub;
    Libcall_pol.lseek = _lseek;
    Libcall_pol.fread = _fread;
    Libcall_pol.strncmp = _strncmp;
    Libcall_pol.memcmp = _memcmp;
    Libcall_pol.fxstat64 = _fxstat64;
    Libcall_pol.generic = _generic;
  }

and parse_libcall_t x =
  let x = Piqirun.parse_record x in
  let _func_addr, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _func_name, x = Piqirun.parse_required_field 2 parse_string x in
  let _is_traced, x = Piqirun.parse_required_field 3 parse_bool x in
  let _ident, x = Piqirun.parse_required_field 4 parse_lib_ident x in
  let _strcpy, x = Piqirun.parse_optional_field 5 parse_strcpy_t x in
  let _printf, x = Piqirun.parse_optional_field 6 parse_printf_t x in
  let _atoi, x = Piqirun.parse_optional_field 7 parse_atoi_t x in
  let _malloc, x = Piqirun.parse_optional_field 8 parse_malloc_t x in
  let _getmodulehandle, x = Piqirun.parse_optional_field 9 parse_getmodulehandle_t x in
  let _getprocaddress, x = Piqirun.parse_optional_field 10 parse_getprocaddress_t x in
  let _getmainargs, x = Piqirun.parse_optional_field 11 parse_getmainargs_t x in
  let _gethostname, x = Piqirun.parse_optional_field 12 parse_gethostname_t x in
  let _free, x = Piqirun.parse_optional_field 13 parse_free_t x in
  let _memcpy, x = Piqirun.parse_optional_field 14 parse_memcpy_t x in
  let _memset, x = Piqirun.parse_optional_field 15 parse_memset_t x in
  let _fgetc, x = Piqirun.parse_optional_field 16 parse_fgetc_t x in
  let _generic, x = Piqirun.parse_optional_field 17 parse_generic_t x in
  let _read, x = Piqirun.parse_optional_field 18 parse_read_t x in
  let _strchr, x = Piqirun.parse_optional_field 19 parse_strchr_t x in
  let _exit, x = Piqirun.parse_optional_field 20 parse_exit_t x in
  let _strcmp, x = Piqirun.parse_optional_field 21 parse_strcmp_t x in
  let _ctype_b_loc, x = Piqirun.parse_optional_field 23 parse_ctype_b_loc_t x in
  let _fscanf, x = Piqirun.parse_optional_field 24 parse_fscanf_t x in
  let _strncpy, x = Piqirun.parse_optional_field 25 parse_strncpy_t x in
  let _fstat, x = Piqirun.parse_optional_field 26 parse_fstat_t x in
  let _realloc, x = Piqirun.parse_optional_field 27 parse_realloc_t x in
  let _mmap, x = Piqirun.parse_optional_field 28 parse_mmap_t x in
  let _qsort, x = Piqirun.parse_optional_field 29 parse_qsort_t x in
  let _bsearch, x = Piqirun.parse_optional_field 30 parse_bsearch_t x in
  let _open_stub, x = Piqirun.parse_optional_field 31 parse_open_t x in
  let _lseek, x = Piqirun.parse_optional_field 32 parse_lseek_t x in
  let _fread, x = Piqirun.parse_optional_field 33 parse_fread_t x in
  let _strncmp, x = Piqirun.parse_optional_field 34 parse_strncmp_t x in
  let _memcmp, x = Piqirun.parse_optional_field 35 parse_memcmp_t x in
  let _fxstat64, x = Piqirun.parse_optional_field 36 parse_fxstat64_t x in
  Piqirun.check_unparsed_fields x;
  {
    Libcall_t.func_addr = _func_addr;
    Libcall_t.func_name = _func_name;
    Libcall_t.is_traced = _is_traced;
    Libcall_t.ident = _ident;
    Libcall_t.strcpy = _strcpy;
    Libcall_t.printf = _printf;
    Libcall_t.atoi = _atoi;
    Libcall_t.malloc = _malloc;
    Libcall_t.getmodulehandle = _getmodulehandle;
    Libcall_t.getprocaddress = _getprocaddress;
    Libcall_t.getmainargs = _getmainargs;
    Libcall_t.gethostname = _gethostname;
    Libcall_t.free = _free;
    Libcall_t.memcpy = _memcpy;
    Libcall_t.memset = _memset;
    Libcall_t.fgetc = _fgetc;
    Libcall_t.generic = _generic;
    Libcall_t.read = _read;
    Libcall_t.strchr = _strchr;
    Libcall_t.exit = _exit;
    Libcall_t.strcmp = _strcmp;
    Libcall_t.ctype_b_loc = _ctype_b_loc;
    Libcall_t.fscanf = _fscanf;
    Libcall_t.strncpy = _strncpy;
    Libcall_t.fstat = _fstat;
    Libcall_t.realloc = _realloc;
    Libcall_t.mmap = _mmap;
    Libcall_t.qsort = _qsort;
    Libcall_t.bsearch = _bsearch;
    Libcall_t.open_stub = _open_stub;
    Libcall_t.lseek = _lseek;
    Libcall_t.fread = _fread;
    Libcall_t.strncmp = _strncmp;
    Libcall_t.memcmp = _memcmp;
    Libcall_t.fxstat64 = _fxstat64;
  }

and parse_generic_pol x =
  let x = Piqirun.parse_record x in
  let _ret, x = Piqirun.parse_required_field 1 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Generic_pol.ret = _ret;
  }

and parse_ctype_b_loc_pol x =
  let x = Piqirun.parse_record x in
  let _table, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _ret, x = Piqirun.parse_required_field 2 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Ctype_b_loc_pol.table = _table;
    Ctype_b_loc_pol.ret = _ret;
  }

and parse_exit_pol x =
  let x = Piqirun.parse_record x in
  Piqirun.check_unparsed_fields x;
  {
    Exit_pol
    ._dummy = ();
  }

and parse_fscanf_pol x =
  let x = Piqirun.parse_record x in
  let _stream, x = Piqirun.parse_required_field 1 Common.parse_action x in
  let _elems, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 3 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Fscanf_pol.stream = _stream;
    Fscanf_pol.elems = _elems;
    Fscanf_pol.ret = _ret;
  }

and parse_read_pol x =
  let x = Piqirun.parse_record x in
  let _fd, x = Piqirun.parse_required_field 1 Common.parse_action x in
  let _buf, x = Piqirun.parse_required_field 2 Common.parse_memory_pol x in
  let _count, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 4 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Read_pol.fd = _fd;
    Read_pol.buf = _buf;
    Read_pol.count = _count;
    Read_pol.ret = _ret;
  }

and parse_fread_pol x =
  let x = Piqirun.parse_record x in
  let _ptr, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _size, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _nmemb, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _stream, x = Piqirun.parse_required_field 4 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 5 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Fread_pol.ptr = _ptr;
    Fread_pol.size = _size;
    Fread_pol.nmemb = _nmemb;
    Fread_pol.stream = _stream;
    Fread_pol.ret = _ret;
  }

and parse_strcmp_pol x =
  let x = Piqirun.parse_record x in
  let _src, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _dst, x = Piqirun.parse_required_field 2 Common.parse_memory_pol x in
  let _ret, x = Piqirun.parse_required_field 3 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Strcmp_pol.src = _src;
    Strcmp_pol.dst = _dst;
    Strcmp_pol.ret = _ret;
  }

and parse_strncmp_pol x =
  let x = Piqirun.parse_record x in
  let _s1, x = Piqirun.parse_required_field 1 Common.parse_action x in
  let _s2, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _n, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 4 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Strncmp_pol.s1 = _s1;
    Strncmp_pol.s2 = _s2;
    Strncmp_pol.n = _n;
    Strncmp_pol.ret = _ret;
  }

and parse_memcmp_pol x =
  let x = Piqirun.parse_record x in
  let _s1, x = Piqirun.parse_required_field 1 Common.parse_action x in
  let _s2, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _n, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 4 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Memcmp_pol.s1 = _s1;
    Memcmp_pol.s2 = _s2;
    Memcmp_pol.n = _n;
    Memcmp_pol.ret = _ret;
  }

and parse_strchr_pol x =
  let x = Piqirun.parse_record x in
  let _sc, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _c, x = Piqirun.parse_required_field 2 Common.parse_memory_pol x in
  let _ret, x = Piqirun.parse_required_field 3 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Strchr_pol.sc = _sc;
    Strchr_pol.c = _c;
    Strchr_pol.ret = _ret;
  }

and parse_strncpy_pol x =
  let x = Piqirun.parse_record x in
  let _dst, x = Piqirun.parse_required_field 1 Common.parse_action x in
  let _src, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _n, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 4 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Strncpy_pol.dst = _dst;
    Strncpy_pol.src = _src;
    Strncpy_pol.n = _n;
    Strncpy_pol.ret = _ret;
  }

and parse_fstat_pol x =
  let x = Piqirun.parse_record x in
  Piqirun.check_unparsed_fields x;
  {
    Fstat_pol
    ._dummy = ();
  }

and parse_fxstat64_pol x =
  let x = Piqirun.parse_record x in
  Piqirun.check_unparsed_fields x;
  {
    Fxstat64_pol
    ._dummy = ();
  }

and parse_realloc_pol x =
  let x = Piqirun.parse_record x in
  let _ptr, x = Piqirun.parse_required_field 1 Common.parse_action x in
  let _size, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 3 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Realloc_pol.ptr = _ptr;
    Realloc_pol.size = _size;
    Realloc_pol.ret = _ret;
  }

and parse_mmap_pol x =
  let x = Piqirun.parse_record x in
  let _addr, x = Piqirun.parse_required_field 1 Common.parse_action x in
  let _length, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _prot, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _flags, x = Piqirun.parse_required_field 4 Common.parse_action x in
  let _fd, x = Piqirun.parse_required_field 5 Common.parse_action x in
  let _offset, x = Piqirun.parse_required_field 6 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 7 Common.parse_memory_pol x in
  Piqirun.check_unparsed_fields x;
  {
    Mmap_pol.addr = _addr;
    Mmap_pol.length = _length;
    Mmap_pol.prot = _prot;
    Mmap_pol.flags = _flags;
    Mmap_pol.fd = _fd;
    Mmap_pol.offset = _offset;
    Mmap_pol.ret = _ret;
  }

and parse_qsort_pol x =
  let x = Piqirun.parse_record x in
  let _base, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _nmemb, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _size, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _compare, x = Piqirun.parse_required_field 4 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Qsort_pol.base = _base;
    Qsort_pol.nmemb = _nmemb;
    Qsort_pol.size = _size;
    Qsort_pol.compare = _compare;
  }

and parse_bsearch_pol x =
  let x = Piqirun.parse_record x in
  let _key, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _base, x = Piqirun.parse_required_field 2 Common.parse_memory_pol x in
  let _nmemb, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _size, x = Piqirun.parse_required_field 4 Common.parse_action x in
  let _compare, x = Piqirun.parse_required_field 5 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 6 Common.parse_memory_pol x in
  Piqirun.check_unparsed_fields x;
  {
    Bsearch_pol.key = _key;
    Bsearch_pol.base = _base;
    Bsearch_pol.nmemb = _nmemb;
    Bsearch_pol.size = _size;
    Bsearch_pol.compare = _compare;
    Bsearch_pol.ret = _ret;
  }

and parse_open_pol x =
  let x = Piqirun.parse_record x in
  let _pathname, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _flags, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _mode, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 4 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Open_pol.pathname = _pathname;
    Open_pol.flags = _flags;
    Open_pol.mode = _mode;
    Open_pol.ret = _ret;
  }

and parse_strcpy_pol x =
  let x = Piqirun.parse_record x in
  let _src, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _dst, x = Piqirun.parse_required_field 2 Common.parse_memory_pol x in
  let _ret, x = Piqirun.parse_required_field 3 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Strcpy_pol.src = _src;
    Strcpy_pol.dst = _dst;
    Strcpy_pol.ret = _ret;
  }

and parse_printf_pol x =
  let x = Piqirun.parse_record x in
  let _format, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _ret, x = Piqirun.parse_required_field 2 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Printf_pol.format = _format;
    Printf_pol.ret = _ret;
  }

and parse_atoi_pol x =
  let x = Piqirun.parse_record x in
  let _src, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _ret, x = Piqirun.parse_required_field 3 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Atoi_pol.src = _src;
    Atoi_pol.ret = _ret;
  }

and parse_malloc_pol x =
  let x = Piqirun.parse_record x in
  let _size, x = Piqirun.parse_required_field 1 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 2 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Malloc_pol.size = _size;
    Malloc_pol.ret = _ret;
  }

and parse_getmodulehandle_pol x =
  let x = Piqirun.parse_record x in
  let _module_name, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _ret, x = Piqirun.parse_required_field 2 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Getmodulehandle_pol.module_name = _module_name;
    Getmodulehandle_pol.ret = _ret;
  }

and parse_getprocaddress_pol x =
  let x = Piqirun.parse_record x in
  let _hmodule, x = Piqirun.parse_required_field 1 Common.parse_action x in
  let _proc_name, x = Piqirun.parse_required_field 2 Common.parse_memory_pol x in
  let _ret, x = Piqirun.parse_required_field 3 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Getprocaddress_pol.hmodule = _hmodule;
    Getprocaddress_pol.proc_name = _proc_name;
    Getprocaddress_pol.ret = _ret;
  }

and parse_getmainargs_pol x =
  let x = Piqirun.parse_record x in
  let _argc, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _argv, x = Piqirun.parse_required_field 2 Common.parse_memory_pol x in
  let _env, x = Piqirun.parse_required_field 3 Common.parse_memory_pol x in
  let _dowildcard, x = Piqirun.parse_required_field 4 Common.parse_action x in
  let _startinfo, x = Piqirun.parse_required_field 5 Common.parse_memory_pol x in
  let _ret, x = Piqirun.parse_required_field 6 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Getmainargs_pol.argc = _argc;
    Getmainargs_pol.argv = _argv;
    Getmainargs_pol.env = _env;
    Getmainargs_pol.dowildcard = _dowildcard;
    Getmainargs_pol.startinfo = _startinfo;
    Getmainargs_pol.ret = _ret;
  }

and parse_gethostname_pol x =
  let x = Piqirun.parse_record x in
  let _name, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _namelen, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 3 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Gethostname_pol.name = _name;
    Gethostname_pol.namelen = _namelen;
    Gethostname_pol.ret = _ret;
  }

and parse_free_pol x =
  let x = Piqirun.parse_record x in
  let _ptr, x = Piqirun.parse_required_field 1 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Free_pol.ptr = _ptr;
  }

and parse_memcpy_pol x =
  let x = Piqirun.parse_record x in
  let _dest, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _src, x = Piqirun.parse_required_field 2 Common.parse_memory_pol x in
  let _size, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 4 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Memcpy_pol.dest = _dest;
    Memcpy_pol.src = _src;
    Memcpy_pol.size = _size;
    Memcpy_pol.ret = _ret;
  }

and parse_memset_pol x =
  let x = Piqirun.parse_record x in
  let _s, x = Piqirun.parse_required_field 1 Common.parse_memory_pol x in
  let _c, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _size, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 4 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Memset_pol.s = _s;
    Memset_pol.c = _c;
    Memset_pol.size = _size;
    Memset_pol.ret = _ret;
  }

and parse_fgetc_pol x =
  let x = Piqirun.parse_record x in
  let _stream, x = Piqirun.parse_required_field 1 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 2 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Fgetc_pol.stream = _stream;
    Fgetc_pol.ret = _ret;
  }

and parse_lseek_pol x =
  let x = Piqirun.parse_record x in
  let _fd, x = Piqirun.parse_required_field 1 Common.parse_action x in
  let _offset, x = Piqirun.parse_required_field 2 Common.parse_action x in
  let _whence, x = Piqirun.parse_required_field 3 Common.parse_action x in
  let _ret, x = Piqirun.parse_required_field 4 Common.parse_action x in
  Piqirun.check_unparsed_fields x;
  {
    Lseek_pol.fd = _fd;
    Lseek_pol.offset = _offset;
    Lseek_pol.whence = _whence;
    Lseek_pol.ret = _ret;
  }

and parse_exit_t x =
  let x = Piqirun.parse_record x in
  Piqirun.check_unparsed_fields x;
  {
    Exit_t
    ._dummy = ();
  }

and parse_generic_t x =
  let x = Piqirun.parse_record x in
  let _ret, x = Piqirun.parse_required_field 1 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Generic_t.ret = _ret;
  }

and parse_fscanf_elem_t x =
  let x = Piqirun.parse_record x in
  let _type_, x = Piqirun.parse_required_field 1 parse_fscanf_enum x in
  let _addr, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _value_dec, x = Piqirun.parse_optional_field 3 parse_uint64 x in
  let _value_string, x = Piqirun.parse_optional_field 4 parse_string x in
  Piqirun.check_unparsed_fields x;
  {
    Fscanf_elem_t.type_ = _type_;
    Fscanf_elem_t.addr = _addr;
    Fscanf_elem_t.value_dec = _value_dec;
    Fscanf_elem_t.value_string = _value_string;
  }

and parse_fscanf_t x =
  let x = Piqirun.parse_record x in
  let _stream, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _elems, x = Piqirun.parse_repeated_field 2 parse_fscanf_elem_t x in
  let _ret, x = Piqirun.parse_required_field 3 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Fscanf_t.stream = _stream;
    Fscanf_t.elems = _elems;
    Fscanf_t.ret = _ret;
  }

and parse_ctype_b_loc_t x =
  let x = Piqirun.parse_record x in
  let _table, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _ret, x = Piqirun.parse_required_field 2 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Ctype_b_loc_t.table = _table;
    Ctype_b_loc_t.ret = _ret;
  }

and parse_read_t x =
  let x = Piqirun.parse_record x in
  let _fd, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _buf, x = Piqirun.parse_required_field 2 Common.parse_memory_t x in
  let _count, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 4 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Read_t.fd = _fd;
    Read_t.buf = _buf;
    Read_t.count = _count;
    Read_t.ret = _ret;
  }

and parse_fread_t x =
  let x = Piqirun.parse_record x in
  let _ptr, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _size, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _nmemb, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _stream, x = Piqirun.parse_required_field 4 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 5 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Fread_t.ptr = _ptr;
    Fread_t.size = _size;
    Fread_t.nmemb = _nmemb;
    Fread_t.stream = _stream;
    Fread_t.ret = _ret;
  }

and parse_strchr_t x =
  let x = Piqirun.parse_record x in
  let _s, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _c, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _size_max, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 4 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Strchr_t.s = _s;
    Strchr_t.c = _c;
    Strchr_t.size_max = _size_max;
    Strchr_t.ret = _ret;
  }

and parse_strncpy_t x =
  let x = Piqirun.parse_record x in
  let _dst, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _src, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _n, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 4 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Strncpy_t.dst = _dst;
    Strncpy_t.src = _src;
    Strncpy_t.n = _n;
    Strncpy_t.ret = _ret;
  }

and parse_fstat_t x =
  let x = Piqirun.parse_record x in
  let _fd, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _buf, x = Piqirun.parse_required_field 2 Common.parse_memory_t x in
  let _ret, x = Piqirun.parse_required_field 3 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Fstat_t.fd = _fd;
    Fstat_t.buf = _buf;
    Fstat_t.ret = _ret;
  }

and parse_fxstat64_t x =
  let x = Piqirun.parse_record x in
  let _vers, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _fd, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _buf, x = Piqirun.parse_required_field 3 Common.parse_memory_t x in
  let _ret, x = Piqirun.parse_required_field 4 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Fxstat64_t.vers = _vers;
    Fxstat64_t.fd = _fd;
    Fxstat64_t.buf = _buf;
    Fxstat64_t.ret = _ret;
  }

and parse_realloc_t x =
  let x = Piqirun.parse_record x in
  let _ptr, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _size, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 3 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Realloc_t.ptr = _ptr;
    Realloc_t.size = _size;
    Realloc_t.ret = _ret;
  }

and parse_mmap_t x =
  let x = Piqirun.parse_record x in
  let _addr, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _length, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _prot, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _flags, x = Piqirun.parse_required_field 4 parse_uint64 x in
  let _fd, x = Piqirun.parse_required_field 5 parse_uint64 x in
  let _offset, x = Piqirun.parse_required_field 6 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 7 Common.parse_memory_t x in
  Piqirun.check_unparsed_fields x;
  {
    Mmap_t.addr = _addr;
    Mmap_t.length = _length;
    Mmap_t.prot = _prot;
    Mmap_t.flags = _flags;
    Mmap_t.fd = _fd;
    Mmap_t.offset = _offset;
    Mmap_t.ret = _ret;
  }

and parse_qsort_t x =
  let x = Piqirun.parse_record x in
  let _base, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _nmemb, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _size, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _compare, x = Piqirun.parse_required_field 4 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Qsort_t.base = _base;
    Qsort_t.nmemb = _nmemb;
    Qsort_t.size = _size;
    Qsort_t.compare = _compare;
  }

and parse_bsearch_t x =
  let x = Piqirun.parse_record x in
  let _key, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _base, x = Piqirun.parse_required_field 2 Common.parse_memory_t x in
  let _nmemb, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _size, x = Piqirun.parse_required_field 4 parse_uint64 x in
  let _compare, x = Piqirun.parse_required_field 5 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 6 Common.parse_memory_t x in
  Piqirun.check_unparsed_fields x;
  {
    Bsearch_t.key = _key;
    Bsearch_t.base = _base;
    Bsearch_t.nmemb = _nmemb;
    Bsearch_t.size = _size;
    Bsearch_t.compare = _compare;
    Bsearch_t.ret = _ret;
  }

and parse_open_t x =
  let x = Piqirun.parse_record x in
  let _pathname, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _flags, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _mode, x = Piqirun.parse_required_field 4 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 5 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Open_t.pathname = _pathname;
    Open_t.flags = _flags;
    Open_t.mode = _mode;
    Open_t.ret = _ret;
  }

and parse_strcpy_t x =
  let x = Piqirun.parse_record x in
  let _src, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _dst, x = Piqirun.parse_required_field 2 Common.parse_memory_t x in
  let _ret, x = Piqirun.parse_required_field 3 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Strcpy_t.src = _src;
    Strcpy_t.dst = _dst;
    Strcpy_t.ret = _ret;
  }

and parse_strcmp_t x =
  let x = Piqirun.parse_record x in
  let _s1, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _s2, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _size_max_s1, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _size_max_s2, x = Piqirun.parse_required_field 4 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 5 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Strcmp_t.s1 = _s1;
    Strcmp_t.s2 = _s2;
    Strcmp_t.size_max_s1 = _size_max_s1;
    Strcmp_t.size_max_s2 = _size_max_s2;
    Strcmp_t.ret = _ret;
  }

and parse_strncmp_t x =
  let x = Piqirun.parse_record x in
  let _s1, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _s2, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _n, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 4 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Strncmp_t.s1 = _s1;
    Strncmp_t.s2 = _s2;
    Strncmp_t.n = _n;
    Strncmp_t.ret = _ret;
  }

and parse_memcmp_t x =
  let x = Piqirun.parse_record x in
  let _s1, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _s2, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _n, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 4 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Memcmp_t.s1 = _s1;
    Memcmp_t.s2 = _s2;
    Memcmp_t.n = _n;
    Memcmp_t.ret = _ret;
  }

and parse_printf_t x =
  let x = Piqirun.parse_record x in
  let _format, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _ret, x = Piqirun.parse_required_field 2 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Printf_t.format = _format;
    Printf_t.ret = _ret;
  }

and parse_atoi_t x =
  let x = Piqirun.parse_record x in
  let _src, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _ret, x = Piqirun.parse_required_field 3 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Atoi_t.src = _src;
    Atoi_t.ret = _ret;
  }

and parse_malloc_t x =
  let x = Piqirun.parse_record x in
  let _size, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 2 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Malloc_t.size = _size;
    Malloc_t.ret = _ret;
  }

and parse_getmodulehandle_t x =
  let x = Piqirun.parse_record x in
  let _module_name, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _ret, x = Piqirun.parse_required_field 2 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Getmodulehandle_t.module_name = _module_name;
    Getmodulehandle_t.ret = _ret;
  }

and parse_getprocaddress_t x =
  let x = Piqirun.parse_record x in
  let _hmodule, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _proc_name, x = Piqirun.parse_required_field 2 Common.parse_memory_t x in
  let _ret, x = Piqirun.parse_required_field 3 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Getprocaddress_t.hmodule = _hmodule;
    Getprocaddress_t.proc_name = _proc_name;
    Getprocaddress_t.ret = _ret;
  }

and parse_getmainargs_t x =
  let x = Piqirun.parse_record x in
  let _argc, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _argv, x = Piqirun.parse_required_field 2 Common.parse_memory_t x in
  let _env, x = Piqirun.parse_required_field 3 Common.parse_memory_t x in
  let _dowildcard, x = Piqirun.parse_required_field 4 parse_uint64 x in
  let _startinfo, x = Piqirun.parse_required_field 5 Common.parse_memory_t x in
  let _ret, x = Piqirun.parse_required_field 6 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Getmainargs_t.argc = _argc;
    Getmainargs_t.argv = _argv;
    Getmainargs_t.env = _env;
    Getmainargs_t.dowildcard = _dowildcard;
    Getmainargs_t.startinfo = _startinfo;
    Getmainargs_t.ret = _ret;
  }

and parse_gethostname_t x =
  let x = Piqirun.parse_record x in
  let _name, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _namelen, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 3 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Gethostname_t.name = _name;
    Gethostname_t.namelen = _namelen;
    Gethostname_t.ret = _ret;
  }

and parse_free_t x =
  let x = Piqirun.parse_record x in
  let _ptr, x = Piqirun.parse_required_field 1 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Free_t.ptr = _ptr;
  }

and parse_memcpy_t x =
  let x = Piqirun.parse_record x in
  let _dest, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _src, x = Piqirun.parse_required_field 2 Common.parse_memory_t x in
  let _size, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 4 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Memcpy_t.dest = _dest;
    Memcpy_t.src = _src;
    Memcpy_t.size = _size;
    Memcpy_t.ret = _ret;
  }

and parse_memset_t x =
  let x = Piqirun.parse_record x in
  let _s, x = Piqirun.parse_required_field 1 Common.parse_memory_t x in
  let _c, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _size, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 4 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Memset_t.s = _s;
    Memset_t.c = _c;
    Memset_t.size = _size;
    Memset_t.ret = _ret;
  }

and parse_fgetc_t x =
  let x = Piqirun.parse_record x in
  let _stream, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 2 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Fgetc_t.stream = _stream;
    Fgetc_t.ret = _ret;
  }

and parse_lseek_t x =
  let x = Piqirun.parse_record x in
  let _fd, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _offset, x = Piqirun.parse_required_field 2 parse_uint64 x in
  let _whence, x = Piqirun.parse_required_field 3 parse_uint64 x in
  let _ret, x = Piqirun.parse_required_field 4 parse_uint64 x in
  Piqirun.check_unparsed_fields x;
  {
    Lseek_t.fd = _fd;
    Lseek_t.offset = _offset;
    Lseek_t.whence = _whence;
    Lseek_t.ret = _ret;
  }

and parse_lib_ident x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `invalid
    | 1l -> `printf
    | 2l -> `strcpy
    | 3l -> `atoi
    | 4l -> `malloc
    | 5l -> `free
    | 6l -> `memcpy
    | 7l -> `memset
    | 8l -> `fgetc
    | 9l -> `read
    | 10l -> `strchr
    | 11l -> `exit
    | 12l -> `strcmp
    | 13l -> `ctype_b_loc
    | 14l -> `fscanf
    | 15l -> `strrchr
    | 16l -> `strncpy
    | 17l -> `fstat
    | 18l -> `realloc
    | 19l -> `mmap
    | 20l -> `qsort
    | 21l -> `bsearch
    | 22l -> `open_stub
    | 23l -> `lseek
    | 24l -> `fread
    | 25l -> `strncmp
    | 26l -> `memcmp
    | 27l -> `fxstat64
    | 100l -> `getmodulehandle
    | 101l -> `getprocaddress
    | 102l -> `getmainargs
    | 103l -> `gethostname
    | 255l -> `generic
    | x -> Piqirun.error_enum_const x
and packed_parse_lib_ident x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `invalid
    | 1l -> `printf
    | 2l -> `strcpy
    | 3l -> `atoi
    | 4l -> `malloc
    | 5l -> `free
    | 6l -> `memcpy
    | 7l -> `memset
    | 8l -> `fgetc
    | 9l -> `read
    | 10l -> `strchr
    | 11l -> `exit
    | 12l -> `strcmp
    | 13l -> `ctype_b_loc
    | 14l -> `fscanf
    | 15l -> `strrchr
    | 16l -> `strncpy
    | 17l -> `fstat
    | 18l -> `realloc
    | 19l -> `mmap
    | 20l -> `qsort
    | 21l -> `bsearch
    | 22l -> `open_stub
    | 23l -> `lseek
    | 24l -> `fread
    | 25l -> `strncmp
    | 26l -> `memcmp
    | 27l -> `fxstat64
    | 100l -> `getmodulehandle
    | 101l -> `getprocaddress
    | 102l -> `getmainargs
    | 103l -> `gethostname
    | 255l -> `generic
    | x -> Piqirun.error_enum_const x

and parse_fscanf_enum x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `d
    | 1l -> `dd
    | 2l -> `i
    | 3l -> `o
    | 4l -> `u
    | 5l -> `x
    | 6l -> `xx
    | 7l -> `f
    | 8l -> `e
    | 9l -> `g
    | 10l -> `ee
    | 11l -> `a
    | 12l -> `s
    | 13l -> `c
    | 14l -> `sb
    | 15l -> `p
    | 16l -> `n
    | x -> Piqirun.error_enum_const x
and packed_parse_fscanf_enum x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `d
    | 1l -> `dd
    | 2l -> `i
    | 3l -> `o
    | 4l -> `u
    | 5l -> `x
    | 6l -> `xx
    | 7l -> `f
    | 8l -> `e
    | 9l -> `g
    | 10l -> `ee
    | 11l -> `a
    | 12l -> `s
    | 13l -> `c
    | 14l -> `sb
    | 15l -> `p
    | 16l -> `n
    | x -> Piqirun.error_enum_const x


let rec gen__string code x = Piqirun.string_to_block code x

and gen__uint64 code x = Piqirun.int64_to_varint code x
and packed_gen__uint64 x = Piqirun.int64_to_packed_varint x

and gen__bool code x = Piqirun.bool_to_varint code x
and packed_gen__bool x = Piqirun.bool_to_packed_varint x

and gen__libcall_pol code x =
  let _name = Piqirun.gen_required_field 1 gen__string x.Libcall_pol.name in
  let _ident = Piqirun.gen_required_field 2 gen__lib_ident x.Libcall_pol.ident in
  let _action = Piqirun.gen_required_field 3 Common.gen__tracing_action x.Libcall_pol.action in
  let _restrict_addresses = Piqirun.gen_repeated_field 4 gen__uint64 x.Libcall_pol.restrict_addresses in
  let _strcpy = Piqirun.gen_optional_field 5 gen__strcpy_pol x.Libcall_pol.strcpy in
  let _printf = Piqirun.gen_optional_field 6 gen__printf_pol x.Libcall_pol.printf in
  let _atoi = Piqirun.gen_optional_field 7 gen__atoi_pol x.Libcall_pol.atoi in
  let _malloc = Piqirun.gen_optional_field 8 gen__malloc_pol x.Libcall_pol.malloc in
  let _getmodulehandle = Piqirun.gen_optional_field 9 gen__getmodulehandle_pol x.Libcall_pol.getmodulehandle in
  let _getprocaddress = Piqirun.gen_optional_field 10 gen__getprocaddress_pol x.Libcall_pol.getprocaddress in
  let _getmainargs = Piqirun.gen_optional_field 11 gen__getmainargs_pol x.Libcall_pol.getmainargs in
  let _gethostname = Piqirun.gen_optional_field 12 gen__gethostname_pol x.Libcall_pol.gethostname in
  let _free = Piqirun.gen_optional_field 13 gen__free_pol x.Libcall_pol.free in
  let _memcpy = Piqirun.gen_optional_field 14 gen__memcpy_pol x.Libcall_pol.memcpy in
  let _memset = Piqirun.gen_optional_field 15 gen__memset_pol x.Libcall_pol.memset in
  let _fgetc = Piqirun.gen_optional_field 16 gen__fgetc_pol x.Libcall_pol.fgetc in
  let _read = Piqirun.gen_optional_field 17 gen__read_pol x.Libcall_pol.read in
  let _strchr = Piqirun.gen_optional_field 18 gen__strchr_pol x.Libcall_pol.strchr in
  let _exit = Piqirun.gen_optional_field 19 gen__exit_pol x.Libcall_pol.exit in
  let _strcmp = Piqirun.gen_optional_field 20 gen__strcmp_pol x.Libcall_pol.strcmp in
  let _ctype_b_loc = Piqirun.gen_optional_field 22 gen__ctype_b_loc_pol x.Libcall_pol.ctype_b_loc in
  let _fscanf = Piqirun.gen_optional_field 23 gen__fscanf_pol x.Libcall_pol.fscanf in
  let _strncpy = Piqirun.gen_optional_field 24 gen__strncpy_pol x.Libcall_pol.strncpy in
  let _fstat = Piqirun.gen_optional_field 25 gen__fstat_pol x.Libcall_pol.fstat in
  let _realloc = Piqirun.gen_optional_field 26 gen__realloc_pol x.Libcall_pol.realloc in
  let _mmap = Piqirun.gen_optional_field 27 gen__mmap_pol x.Libcall_pol.mmap in
  let _qsort = Piqirun.gen_optional_field 28 gen__qsort_pol x.Libcall_pol.qsort in
  let _bsearch = Piqirun.gen_optional_field 29 gen__bsearch_pol x.Libcall_pol.bsearch in
  let _open_stub = Piqirun.gen_optional_field 30 gen__open_pol x.Libcall_pol.open_stub in
  let _lseek = Piqirun.gen_optional_field 31 gen__lseek_pol x.Libcall_pol.lseek in
  let _fread = Piqirun.gen_optional_field 32 gen__fread_pol x.Libcall_pol.fread in
  let _strncmp = Piqirun.gen_optional_field 33 gen__strncmp_pol x.Libcall_pol.strncmp in
  let _memcmp = Piqirun.gen_optional_field 34 gen__memcmp_pol x.Libcall_pol.memcmp in
  let _fxstat64 = Piqirun.gen_optional_field 35 gen__fxstat64_pol x.Libcall_pol.fxstat64 in
  let _generic = Piqirun.gen_optional_field 50 gen__generic_pol x.Libcall_pol.generic in
  Piqirun.gen_record code (_name :: _ident :: _action :: _restrict_addresses :: _strcpy :: _printf :: _atoi :: _malloc :: _getmodulehandle :: _getprocaddress :: _getmainargs :: _gethostname :: _free :: _memcpy :: _memset :: _fgetc :: _read :: _strchr :: _exit :: _strcmp :: _ctype_b_loc :: _fscanf :: _strncpy :: _fstat :: _realloc :: _mmap :: _qsort :: _bsearch :: _open_stub :: _lseek :: _fread :: _strncmp :: _memcmp :: _fxstat64 :: _generic :: [])

and gen__libcall_t code x =
  let _func_addr = Piqirun.gen_required_field 1 gen__uint64 x.Libcall_t.func_addr in
  let _func_name = Piqirun.gen_required_field 2 gen__string x.Libcall_t.func_name in
  let _is_traced = Piqirun.gen_required_field 3 gen__bool x.Libcall_t.is_traced in
  let _ident = Piqirun.gen_required_field 4 gen__lib_ident x.Libcall_t.ident in
  let _strcpy = Piqirun.gen_optional_field 5 gen__strcpy_t x.Libcall_t.strcpy in
  let _printf = Piqirun.gen_optional_field 6 gen__printf_t x.Libcall_t.printf in
  let _atoi = Piqirun.gen_optional_field 7 gen__atoi_t x.Libcall_t.atoi in
  let _malloc = Piqirun.gen_optional_field 8 gen__malloc_t x.Libcall_t.malloc in
  let _getmodulehandle = Piqirun.gen_optional_field 9 gen__getmodulehandle_t x.Libcall_t.getmodulehandle in
  let _getprocaddress = Piqirun.gen_optional_field 10 gen__getprocaddress_t x.Libcall_t.getprocaddress in
  let _getmainargs = Piqirun.gen_optional_field 11 gen__getmainargs_t x.Libcall_t.getmainargs in
  let _gethostname = Piqirun.gen_optional_field 12 gen__gethostname_t x.Libcall_t.gethostname in
  let _free = Piqirun.gen_optional_field 13 gen__free_t x.Libcall_t.free in
  let _memcpy = Piqirun.gen_optional_field 14 gen__memcpy_t x.Libcall_t.memcpy in
  let _memset = Piqirun.gen_optional_field 15 gen__memset_t x.Libcall_t.memset in
  let _fgetc = Piqirun.gen_optional_field 16 gen__fgetc_t x.Libcall_t.fgetc in
  let _generic = Piqirun.gen_optional_field 17 gen__generic_t x.Libcall_t.generic in
  let _read = Piqirun.gen_optional_field 18 gen__read_t x.Libcall_t.read in
  let _strchr = Piqirun.gen_optional_field 19 gen__strchr_t x.Libcall_t.strchr in
  let _exit = Piqirun.gen_optional_field 20 gen__exit_t x.Libcall_t.exit in
  let _strcmp = Piqirun.gen_optional_field 21 gen__strcmp_t x.Libcall_t.strcmp in
  let _ctype_b_loc = Piqirun.gen_optional_field 23 gen__ctype_b_loc_t x.Libcall_t.ctype_b_loc in
  let _fscanf = Piqirun.gen_optional_field 24 gen__fscanf_t x.Libcall_t.fscanf in
  let _strncpy = Piqirun.gen_optional_field 25 gen__strncpy_t x.Libcall_t.strncpy in
  let _fstat = Piqirun.gen_optional_field 26 gen__fstat_t x.Libcall_t.fstat in
  let _realloc = Piqirun.gen_optional_field 27 gen__realloc_t x.Libcall_t.realloc in
  let _mmap = Piqirun.gen_optional_field 28 gen__mmap_t x.Libcall_t.mmap in
  let _qsort = Piqirun.gen_optional_field 29 gen__qsort_t x.Libcall_t.qsort in
  let _bsearch = Piqirun.gen_optional_field 30 gen__bsearch_t x.Libcall_t.bsearch in
  let _open_stub = Piqirun.gen_optional_field 31 gen__open_t x.Libcall_t.open_stub in
  let _lseek = Piqirun.gen_optional_field 32 gen__lseek_t x.Libcall_t.lseek in
  let _fread = Piqirun.gen_optional_field 33 gen__fread_t x.Libcall_t.fread in
  let _strncmp = Piqirun.gen_optional_field 34 gen__strncmp_t x.Libcall_t.strncmp in
  let _memcmp = Piqirun.gen_optional_field 35 gen__memcmp_t x.Libcall_t.memcmp in
  let _fxstat64 = Piqirun.gen_optional_field 36 gen__fxstat64_t x.Libcall_t.fxstat64 in
  Piqirun.gen_record code (_func_addr :: _func_name :: _is_traced :: _ident :: _strcpy :: _printf :: _atoi :: _malloc :: _getmodulehandle :: _getprocaddress :: _getmainargs :: _gethostname :: _free :: _memcpy :: _memset :: _fgetc :: _generic :: _read :: _strchr :: _exit :: _strcmp :: _ctype_b_loc :: _fscanf :: _strncpy :: _fstat :: _realloc :: _mmap :: _qsort :: _bsearch :: _open_stub :: _lseek :: _fread :: _strncmp :: _memcmp :: _fxstat64 :: [])

and gen__generic_pol code x =
  let _ret = Piqirun.gen_required_field 1 Common.gen__action x.Generic_pol.ret in
  Piqirun.gen_record code (_ret :: [])

and gen__ctype_b_loc_pol code x =
  let _table = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Ctype_b_loc_pol.table in
  let _ret = Piqirun.gen_required_field 2 Common.gen__action x.Ctype_b_loc_pol.ret in
  Piqirun.gen_record code (_table :: _ret :: [])

and gen__exit_pol code x =
  Piqirun.gen_record code ([])

and gen__fscanf_pol code x =
  let _stream = Piqirun.gen_required_field 1 Common.gen__action x.Fscanf_pol.stream in
  let _elems = Piqirun.gen_required_field 2 Common.gen__action x.Fscanf_pol.elems in
  let _ret = Piqirun.gen_required_field 3 Common.gen__action x.Fscanf_pol.ret in
  Piqirun.gen_record code (_stream :: _elems :: _ret :: [])

and gen__read_pol code x =
  let _fd = Piqirun.gen_required_field 1 Common.gen__action x.Read_pol.fd in
  let _buf = Piqirun.gen_required_field 2 Common.gen__memory_pol x.Read_pol.buf in
  let _count = Piqirun.gen_required_field 3 Common.gen__action x.Read_pol.count in
  let _ret = Piqirun.gen_required_field 4 Common.gen__action x.Read_pol.ret in
  Piqirun.gen_record code (_fd :: _buf :: _count :: _ret :: [])

and gen__fread_pol code x =
  let _ptr = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Fread_pol.ptr in
  let _size = Piqirun.gen_required_field 2 Common.gen__action x.Fread_pol.size in
  let _nmemb = Piqirun.gen_required_field 3 Common.gen__action x.Fread_pol.nmemb in
  let _stream = Piqirun.gen_required_field 4 Common.gen__action x.Fread_pol.stream in
  let _ret = Piqirun.gen_required_field 5 Common.gen__action x.Fread_pol.ret in
  Piqirun.gen_record code (_ptr :: _size :: _nmemb :: _stream :: _ret :: [])

and gen__strcmp_pol code x =
  let _src = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Strcmp_pol.src in
  let _dst = Piqirun.gen_required_field 2 Common.gen__memory_pol x.Strcmp_pol.dst in
  let _ret = Piqirun.gen_required_field 3 Common.gen__action x.Strcmp_pol.ret in
  Piqirun.gen_record code (_src :: _dst :: _ret :: [])

and gen__strncmp_pol code x =
  let _s1 = Piqirun.gen_required_field 1 Common.gen__action x.Strncmp_pol.s1 in
  let _s2 = Piqirun.gen_required_field 2 Common.gen__action x.Strncmp_pol.s2 in
  let _n = Piqirun.gen_required_field 3 Common.gen__action x.Strncmp_pol.n in
  let _ret = Piqirun.gen_required_field 4 Common.gen__action x.Strncmp_pol.ret in
  Piqirun.gen_record code (_s1 :: _s2 :: _n :: _ret :: [])

and gen__memcmp_pol code x =
  let _s1 = Piqirun.gen_required_field 1 Common.gen__action x.Memcmp_pol.s1 in
  let _s2 = Piqirun.gen_required_field 2 Common.gen__action x.Memcmp_pol.s2 in
  let _n = Piqirun.gen_required_field 3 Common.gen__action x.Memcmp_pol.n in
  let _ret = Piqirun.gen_required_field 4 Common.gen__action x.Memcmp_pol.ret in
  Piqirun.gen_record code (_s1 :: _s2 :: _n :: _ret :: [])

and gen__strchr_pol code x =
  let _sc = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Strchr_pol.sc in
  let _c = Piqirun.gen_required_field 2 Common.gen__memory_pol x.Strchr_pol.c in
  let _ret = Piqirun.gen_required_field 3 Common.gen__action x.Strchr_pol.ret in
  Piqirun.gen_record code (_sc :: _c :: _ret :: [])

and gen__strncpy_pol code x =
  let _dst = Piqirun.gen_required_field 1 Common.gen__action x.Strncpy_pol.dst in
  let _src = Piqirun.gen_required_field 2 Common.gen__action x.Strncpy_pol.src in
  let _n = Piqirun.gen_required_field 3 Common.gen__action x.Strncpy_pol.n in
  let _ret = Piqirun.gen_required_field 4 Common.gen__action x.Strncpy_pol.ret in
  Piqirun.gen_record code (_dst :: _src :: _n :: _ret :: [])

and gen__fstat_pol code x =
  Piqirun.gen_record code ([])

and gen__fxstat64_pol code x =
  Piqirun.gen_record code ([])

and gen__realloc_pol code x =
  let _ptr = Piqirun.gen_required_field 1 Common.gen__action x.Realloc_pol.ptr in
  let _size = Piqirun.gen_required_field 2 Common.gen__action x.Realloc_pol.size in
  let _ret = Piqirun.gen_required_field 3 Common.gen__action x.Realloc_pol.ret in
  Piqirun.gen_record code (_ptr :: _size :: _ret :: [])

and gen__mmap_pol code x =
  let _addr = Piqirun.gen_required_field 1 Common.gen__action x.Mmap_pol.addr in
  let _length = Piqirun.gen_required_field 2 Common.gen__action x.Mmap_pol.length in
  let _prot = Piqirun.gen_required_field 3 Common.gen__action x.Mmap_pol.prot in
  let _flags = Piqirun.gen_required_field 4 Common.gen__action x.Mmap_pol.flags in
  let _fd = Piqirun.gen_required_field 5 Common.gen__action x.Mmap_pol.fd in
  let _offset = Piqirun.gen_required_field 6 Common.gen__action x.Mmap_pol.offset in
  let _ret = Piqirun.gen_required_field 7 Common.gen__memory_pol x.Mmap_pol.ret in
  Piqirun.gen_record code (_addr :: _length :: _prot :: _flags :: _fd :: _offset :: _ret :: [])

and gen__qsort_pol code x =
  let _base = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Qsort_pol.base in
  let _nmemb = Piqirun.gen_required_field 2 Common.gen__action x.Qsort_pol.nmemb in
  let _size = Piqirun.gen_required_field 3 Common.gen__action x.Qsort_pol.size in
  let _compare = Piqirun.gen_required_field 4 Common.gen__action x.Qsort_pol.compare in
  Piqirun.gen_record code (_base :: _nmemb :: _size :: _compare :: [])

and gen__bsearch_pol code x =
  let _key = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Bsearch_pol.key in
  let _base = Piqirun.gen_required_field 2 Common.gen__memory_pol x.Bsearch_pol.base in
  let _nmemb = Piqirun.gen_required_field 3 Common.gen__action x.Bsearch_pol.nmemb in
  let _size = Piqirun.gen_required_field 4 Common.gen__action x.Bsearch_pol.size in
  let _compare = Piqirun.gen_required_field 5 Common.gen__action x.Bsearch_pol.compare in
  let _ret = Piqirun.gen_required_field 6 Common.gen__memory_pol x.Bsearch_pol.ret in
  Piqirun.gen_record code (_key :: _base :: _nmemb :: _size :: _compare :: _ret :: [])

and gen__open_pol code x =
  let _pathname = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Open_pol.pathname in
  let _flags = Piqirun.gen_required_field 2 Common.gen__action x.Open_pol.flags in
  let _mode = Piqirun.gen_required_field 3 Common.gen__action x.Open_pol.mode in
  let _ret = Piqirun.gen_required_field 4 Common.gen__action x.Open_pol.ret in
  Piqirun.gen_record code (_pathname :: _flags :: _mode :: _ret :: [])

and gen__strcpy_pol code x =
  let _src = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Strcpy_pol.src in
  let _dst = Piqirun.gen_required_field 2 Common.gen__memory_pol x.Strcpy_pol.dst in
  let _ret = Piqirun.gen_required_field 3 Common.gen__action x.Strcpy_pol.ret in
  Piqirun.gen_record code (_src :: _dst :: _ret :: [])

and gen__printf_pol code x =
  let _format = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Printf_pol.format in
  let _ret = Piqirun.gen_required_field 2 Common.gen__action x.Printf_pol.ret in
  Piqirun.gen_record code (_format :: _ret :: [])

and gen__atoi_pol code x =
  let _src = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Atoi_pol.src in
  let _ret = Piqirun.gen_required_field 3 Common.gen__action x.Atoi_pol.ret in
  Piqirun.gen_record code (_src :: _ret :: [])

and gen__malloc_pol code x =
  let _size = Piqirun.gen_required_field 1 Common.gen__action x.Malloc_pol.size in
  let _ret = Piqirun.gen_required_field 2 Common.gen__action x.Malloc_pol.ret in
  Piqirun.gen_record code (_size :: _ret :: [])

and gen__getmodulehandle_pol code x =
  let _module_name = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Getmodulehandle_pol.module_name in
  let _ret = Piqirun.gen_required_field 2 Common.gen__action x.Getmodulehandle_pol.ret in
  Piqirun.gen_record code (_module_name :: _ret :: [])

and gen__getprocaddress_pol code x =
  let _hmodule = Piqirun.gen_required_field 1 Common.gen__action x.Getprocaddress_pol.hmodule in
  let _proc_name = Piqirun.gen_required_field 2 Common.gen__memory_pol x.Getprocaddress_pol.proc_name in
  let _ret = Piqirun.gen_required_field 3 Common.gen__action x.Getprocaddress_pol.ret in
  Piqirun.gen_record code (_hmodule :: _proc_name :: _ret :: [])

and gen__getmainargs_pol code x =
  let _argc = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Getmainargs_pol.argc in
  let _argv = Piqirun.gen_required_field 2 Common.gen__memory_pol x.Getmainargs_pol.argv in
  let _env = Piqirun.gen_required_field 3 Common.gen__memory_pol x.Getmainargs_pol.env in
  let _dowildcard = Piqirun.gen_required_field 4 Common.gen__action x.Getmainargs_pol.dowildcard in
  let _startinfo = Piqirun.gen_required_field 5 Common.gen__memory_pol x.Getmainargs_pol.startinfo in
  let _ret = Piqirun.gen_required_field 6 Common.gen__action x.Getmainargs_pol.ret in
  Piqirun.gen_record code (_argc :: _argv :: _env :: _dowildcard :: _startinfo :: _ret :: [])

and gen__gethostname_pol code x =
  let _name = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Gethostname_pol.name in
  let _namelen = Piqirun.gen_required_field 2 Common.gen__action x.Gethostname_pol.namelen in
  let _ret = Piqirun.gen_required_field 3 Common.gen__action x.Gethostname_pol.ret in
  Piqirun.gen_record code (_name :: _namelen :: _ret :: [])

and gen__free_pol code x =
  let _ptr = Piqirun.gen_required_field 1 Common.gen__action x.Free_pol.ptr in
  Piqirun.gen_record code (_ptr :: [])

and gen__memcpy_pol code x =
  let _dest = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Memcpy_pol.dest in
  let _src = Piqirun.gen_required_field 2 Common.gen__memory_pol x.Memcpy_pol.src in
  let _size = Piqirun.gen_required_field 3 Common.gen__action x.Memcpy_pol.size in
  let _ret = Piqirun.gen_required_field 4 Common.gen__action x.Memcpy_pol.ret in
  Piqirun.gen_record code (_dest :: _src :: _size :: _ret :: [])

and gen__memset_pol code x =
  let _s = Piqirun.gen_required_field 1 Common.gen__memory_pol x.Memset_pol.s in
  let _c = Piqirun.gen_required_field 2 Common.gen__action x.Memset_pol.c in
  let _size = Piqirun.gen_required_field 3 Common.gen__action x.Memset_pol.size in
  let _ret = Piqirun.gen_required_field 4 Common.gen__action x.Memset_pol.ret in
  Piqirun.gen_record code (_s :: _c :: _size :: _ret :: [])

and gen__fgetc_pol code x =
  let _stream = Piqirun.gen_required_field 1 Common.gen__action x.Fgetc_pol.stream in
  let _ret = Piqirun.gen_required_field 2 Common.gen__action x.Fgetc_pol.ret in
  Piqirun.gen_record code (_stream :: _ret :: [])

and gen__lseek_pol code x =
  let _fd = Piqirun.gen_required_field 1 Common.gen__action x.Lseek_pol.fd in
  let _offset = Piqirun.gen_required_field 2 Common.gen__action x.Lseek_pol.offset in
  let _whence = Piqirun.gen_required_field 3 Common.gen__action x.Lseek_pol.whence in
  let _ret = Piqirun.gen_required_field 4 Common.gen__action x.Lseek_pol.ret in
  Piqirun.gen_record code (_fd :: _offset :: _whence :: _ret :: [])

and gen__exit_t code x =
  Piqirun.gen_record code ([])

and gen__generic_t code x =
  let _ret = Piqirun.gen_required_field 1 gen__uint64 x.Generic_t.ret in
  Piqirun.gen_record code (_ret :: [])

and gen__fscanf_elem_t code x =
  let _type_ = Piqirun.gen_required_field 1 gen__fscanf_enum x.Fscanf_elem_t.type_ in
  let _addr = Piqirun.gen_required_field 2 gen__uint64 x.Fscanf_elem_t.addr in
  let _value_dec = Piqirun.gen_optional_field 3 gen__uint64 x.Fscanf_elem_t.value_dec in
  let _value_string = Piqirun.gen_optional_field 4 gen__string x.Fscanf_elem_t.value_string in
  Piqirun.gen_record code (_type_ :: _addr :: _value_dec :: _value_string :: [])

and gen__fscanf_t code x =
  let _stream = Piqirun.gen_required_field 1 gen__uint64 x.Fscanf_t.stream in
  let _elems = Piqirun.gen_repeated_field 2 gen__fscanf_elem_t x.Fscanf_t.elems in
  let _ret = Piqirun.gen_required_field 3 gen__uint64 x.Fscanf_t.ret in
  Piqirun.gen_record code (_stream :: _elems :: _ret :: [])

and gen__ctype_b_loc_t code x =
  let _table = Piqirun.gen_required_field 1 Common.gen__memory_t x.Ctype_b_loc_t.table in
  let _ret = Piqirun.gen_required_field 2 gen__uint64 x.Ctype_b_loc_t.ret in
  Piqirun.gen_record code (_table :: _ret :: [])

and gen__read_t code x =
  let _fd = Piqirun.gen_required_field 1 gen__uint64 x.Read_t.fd in
  let _buf = Piqirun.gen_required_field 2 Common.gen__memory_t x.Read_t.buf in
  let _count = Piqirun.gen_required_field 3 gen__uint64 x.Read_t.count in
  let _ret = Piqirun.gen_required_field 4 gen__uint64 x.Read_t.ret in
  Piqirun.gen_record code (_fd :: _buf :: _count :: _ret :: [])

and gen__fread_t code x =
  let _ptr = Piqirun.gen_required_field 1 Common.gen__memory_t x.Fread_t.ptr in
  let _size = Piqirun.gen_required_field 2 gen__uint64 x.Fread_t.size in
  let _nmemb = Piqirun.gen_required_field 3 gen__uint64 x.Fread_t.nmemb in
  let _stream = Piqirun.gen_required_field 4 gen__uint64 x.Fread_t.stream in
  let _ret = Piqirun.gen_required_field 5 gen__uint64 x.Fread_t.ret in
  Piqirun.gen_record code (_ptr :: _size :: _nmemb :: _stream :: _ret :: [])

and gen__strchr_t code x =
  let _s = Piqirun.gen_required_field 1 gen__uint64 x.Strchr_t.s in
  let _c = Piqirun.gen_required_field 2 gen__uint64 x.Strchr_t.c in
  let _size_max = Piqirun.gen_required_field 3 gen__uint64 x.Strchr_t.size_max in
  let _ret = Piqirun.gen_required_field 4 gen__uint64 x.Strchr_t.ret in
  Piqirun.gen_record code (_s :: _c :: _size_max :: _ret :: [])

and gen__strncpy_t code x =
  let _dst = Piqirun.gen_required_field 1 gen__uint64 x.Strncpy_t.dst in
  let _src = Piqirun.gen_required_field 2 gen__uint64 x.Strncpy_t.src in
  let _n = Piqirun.gen_required_field 3 gen__uint64 x.Strncpy_t.n in
  let _ret = Piqirun.gen_required_field 4 gen__uint64 x.Strncpy_t.ret in
  Piqirun.gen_record code (_dst :: _src :: _n :: _ret :: [])

and gen__fstat_t code x =
  let _fd = Piqirun.gen_required_field 1 gen__uint64 x.Fstat_t.fd in
  let _buf = Piqirun.gen_required_field 2 Common.gen__memory_t x.Fstat_t.buf in
  let _ret = Piqirun.gen_required_field 3 gen__uint64 x.Fstat_t.ret in
  Piqirun.gen_record code (_fd :: _buf :: _ret :: [])

and gen__fxstat64_t code x =
  let _vers = Piqirun.gen_required_field 1 gen__uint64 x.Fxstat64_t.vers in
  let _fd = Piqirun.gen_required_field 2 gen__uint64 x.Fxstat64_t.fd in
  let _buf = Piqirun.gen_required_field 3 Common.gen__memory_t x.Fxstat64_t.buf in
  let _ret = Piqirun.gen_required_field 4 gen__uint64 x.Fxstat64_t.ret in
  Piqirun.gen_record code (_vers :: _fd :: _buf :: _ret :: [])

and gen__realloc_t code x =
  let _ptr = Piqirun.gen_required_field 1 gen__uint64 x.Realloc_t.ptr in
  let _size = Piqirun.gen_required_field 2 gen__uint64 x.Realloc_t.size in
  let _ret = Piqirun.gen_required_field 3 gen__uint64 x.Realloc_t.ret in
  Piqirun.gen_record code (_ptr :: _size :: _ret :: [])

and gen__mmap_t code x =
  let _addr = Piqirun.gen_required_field 1 gen__uint64 x.Mmap_t.addr in
  let _length = Piqirun.gen_required_field 2 gen__uint64 x.Mmap_t.length in
  let _prot = Piqirun.gen_required_field 3 gen__uint64 x.Mmap_t.prot in
  let _flags = Piqirun.gen_required_field 4 gen__uint64 x.Mmap_t.flags in
  let _fd = Piqirun.gen_required_field 5 gen__uint64 x.Mmap_t.fd in
  let _offset = Piqirun.gen_required_field 6 gen__uint64 x.Mmap_t.offset in
  let _ret = Piqirun.gen_required_field 7 Common.gen__memory_t x.Mmap_t.ret in
  Piqirun.gen_record code (_addr :: _length :: _prot :: _flags :: _fd :: _offset :: _ret :: [])

and gen__qsort_t code x =
  let _base = Piqirun.gen_required_field 1 Common.gen__memory_t x.Qsort_t.base in
  let _nmemb = Piqirun.gen_required_field 2 gen__uint64 x.Qsort_t.nmemb in
  let _size = Piqirun.gen_required_field 3 gen__uint64 x.Qsort_t.size in
  let _compare = Piqirun.gen_required_field 4 gen__uint64 x.Qsort_t.compare in
  Piqirun.gen_record code (_base :: _nmemb :: _size :: _compare :: [])

and gen__bsearch_t code x =
  let _key = Piqirun.gen_required_field 1 Common.gen__memory_t x.Bsearch_t.key in
  let _base = Piqirun.gen_required_field 2 Common.gen__memory_t x.Bsearch_t.base in
  let _nmemb = Piqirun.gen_required_field 3 gen__uint64 x.Bsearch_t.nmemb in
  let _size = Piqirun.gen_required_field 4 gen__uint64 x.Bsearch_t.size in
  let _compare = Piqirun.gen_required_field 5 gen__uint64 x.Bsearch_t.compare in
  let _ret = Piqirun.gen_required_field 6 Common.gen__memory_t x.Bsearch_t.ret in
  Piqirun.gen_record code (_key :: _base :: _nmemb :: _size :: _compare :: _ret :: [])

and gen__open_t code x =
  let _pathname = Piqirun.gen_required_field 1 Common.gen__memory_t x.Open_t.pathname in
  let _flags = Piqirun.gen_required_field 3 gen__uint64 x.Open_t.flags in
  let _mode = Piqirun.gen_required_field 4 gen__uint64 x.Open_t.mode in
  let _ret = Piqirun.gen_required_field 5 gen__uint64 x.Open_t.ret in
  Piqirun.gen_record code (_pathname :: _flags :: _mode :: _ret :: [])

and gen__strcpy_t code x =
  let _src = Piqirun.gen_required_field 1 Common.gen__memory_t x.Strcpy_t.src in
  let _dst = Piqirun.gen_required_field 2 Common.gen__memory_t x.Strcpy_t.dst in
  let _ret = Piqirun.gen_required_field 3 gen__uint64 x.Strcpy_t.ret in
  Piqirun.gen_record code (_src :: _dst :: _ret :: [])

and gen__strcmp_t code x =
  let _s1 = Piqirun.gen_required_field 1 gen__uint64 x.Strcmp_t.s1 in
  let _s2 = Piqirun.gen_required_field 2 gen__uint64 x.Strcmp_t.s2 in
  let _size_max_s1 = Piqirun.gen_required_field 3 gen__uint64 x.Strcmp_t.size_max_s1 in
  let _size_max_s2 = Piqirun.gen_required_field 4 gen__uint64 x.Strcmp_t.size_max_s2 in
  let _ret = Piqirun.gen_required_field 5 gen__uint64 x.Strcmp_t.ret in
  Piqirun.gen_record code (_s1 :: _s2 :: _size_max_s1 :: _size_max_s2 :: _ret :: [])

and gen__strncmp_t code x =
  let _s1 = Piqirun.gen_required_field 1 gen__uint64 x.Strncmp_t.s1 in
  let _s2 = Piqirun.gen_required_field 2 gen__uint64 x.Strncmp_t.s2 in
  let _n = Piqirun.gen_required_field 3 gen__uint64 x.Strncmp_t.n in
  let _ret = Piqirun.gen_required_field 4 gen__uint64 x.Strncmp_t.ret in
  Piqirun.gen_record code (_s1 :: _s2 :: _n :: _ret :: [])

and gen__memcmp_t code x =
  let _s1 = Piqirun.gen_required_field 1 gen__uint64 x.Memcmp_t.s1 in
  let _s2 = Piqirun.gen_required_field 2 gen__uint64 x.Memcmp_t.s2 in
  let _n = Piqirun.gen_required_field 3 gen__uint64 x.Memcmp_t.n in
  let _ret = Piqirun.gen_required_field 4 gen__uint64 x.Memcmp_t.ret in
  Piqirun.gen_record code (_s1 :: _s2 :: _n :: _ret :: [])

and gen__printf_t code x =
  let _format = Piqirun.gen_required_field 1 Common.gen__memory_t x.Printf_t.format in
  let _ret = Piqirun.gen_required_field 2 gen__uint64 x.Printf_t.ret in
  Piqirun.gen_record code (_format :: _ret :: [])

and gen__atoi_t code x =
  let _src = Piqirun.gen_required_field 1 Common.gen__memory_t x.Atoi_t.src in
  let _ret = Piqirun.gen_required_field 3 gen__uint64 x.Atoi_t.ret in
  Piqirun.gen_record code (_src :: _ret :: [])

and gen__malloc_t code x =
  let _size = Piqirun.gen_required_field 1 gen__uint64 x.Malloc_t.size in
  let _ret = Piqirun.gen_required_field 2 gen__uint64 x.Malloc_t.ret in
  Piqirun.gen_record code (_size :: _ret :: [])

and gen__getmodulehandle_t code x =
  let _module_name = Piqirun.gen_required_field 1 Common.gen__memory_t x.Getmodulehandle_t.module_name in
  let _ret = Piqirun.gen_required_field 2 gen__uint64 x.Getmodulehandle_t.ret in
  Piqirun.gen_record code (_module_name :: _ret :: [])

and gen__getprocaddress_t code x =
  let _hmodule = Piqirun.gen_required_field 1 gen__uint64 x.Getprocaddress_t.hmodule in
  let _proc_name = Piqirun.gen_required_field 2 Common.gen__memory_t x.Getprocaddress_t.proc_name in
  let _ret = Piqirun.gen_required_field 3 gen__uint64 x.Getprocaddress_t.ret in
  Piqirun.gen_record code (_hmodule :: _proc_name :: _ret :: [])

and gen__getmainargs_t code x =
  let _argc = Piqirun.gen_required_field 1 Common.gen__memory_t x.Getmainargs_t.argc in
  let _argv = Piqirun.gen_required_field 2 Common.gen__memory_t x.Getmainargs_t.argv in
  let _env = Piqirun.gen_required_field 3 Common.gen__memory_t x.Getmainargs_t.env in
  let _dowildcard = Piqirun.gen_required_field 4 gen__uint64 x.Getmainargs_t.dowildcard in
  let _startinfo = Piqirun.gen_required_field 5 Common.gen__memory_t x.Getmainargs_t.startinfo in
  let _ret = Piqirun.gen_required_field 6 gen__uint64 x.Getmainargs_t.ret in
  Piqirun.gen_record code (_argc :: _argv :: _env :: _dowildcard :: _startinfo :: _ret :: [])

and gen__gethostname_t code x =
  let _name = Piqirun.gen_required_field 1 Common.gen__memory_t x.Gethostname_t.name in
  let _namelen = Piqirun.gen_required_field 2 gen__uint64 x.Gethostname_t.namelen in
  let _ret = Piqirun.gen_required_field 3 gen__uint64 x.Gethostname_t.ret in
  Piqirun.gen_record code (_name :: _namelen :: _ret :: [])

and gen__free_t code x =
  let _ptr = Piqirun.gen_required_field 1 gen__uint64 x.Free_t.ptr in
  Piqirun.gen_record code (_ptr :: [])

and gen__memcpy_t code x =
  let _dest = Piqirun.gen_required_field 1 Common.gen__memory_t x.Memcpy_t.dest in
  let _src = Piqirun.gen_required_field 2 Common.gen__memory_t x.Memcpy_t.src in
  let _size = Piqirun.gen_required_field 3 gen__uint64 x.Memcpy_t.size in
  let _ret = Piqirun.gen_required_field 4 gen__uint64 x.Memcpy_t.ret in
  Piqirun.gen_record code (_dest :: _src :: _size :: _ret :: [])

and gen__memset_t code x =
  let _s = Piqirun.gen_required_field 1 Common.gen__memory_t x.Memset_t.s in
  let _c = Piqirun.gen_required_field 2 gen__uint64 x.Memset_t.c in
  let _size = Piqirun.gen_required_field 3 gen__uint64 x.Memset_t.size in
  let _ret = Piqirun.gen_required_field 4 gen__uint64 x.Memset_t.ret in
  Piqirun.gen_record code (_s :: _c :: _size :: _ret :: [])

and gen__fgetc_t code x =
  let _stream = Piqirun.gen_required_field 1 gen__uint64 x.Fgetc_t.stream in
  let _ret = Piqirun.gen_required_field 2 gen__uint64 x.Fgetc_t.ret in
  Piqirun.gen_record code (_stream :: _ret :: [])

and gen__lseek_t code x =
  let _fd = Piqirun.gen_required_field 1 gen__uint64 x.Lseek_t.fd in
  let _offset = Piqirun.gen_required_field 2 gen__uint64 x.Lseek_t.offset in
  let _whence = Piqirun.gen_required_field 3 gen__uint64 x.Lseek_t.whence in
  let _ret = Piqirun.gen_required_field 4 gen__uint64 x.Lseek_t.ret in
  Piqirun.gen_record code (_fd :: _offset :: _whence :: _ret :: [])

and gen__lib_ident code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `invalid -> 0l
    | `printf -> 1l
    | `strcpy -> 2l
    | `atoi -> 3l
    | `malloc -> 4l
    | `free -> 5l
    | `memcpy -> 6l
    | `memset -> 7l
    | `fgetc -> 8l
    | `read -> 9l
    | `strchr -> 10l
    | `exit -> 11l
    | `strcmp -> 12l
    | `ctype_b_loc -> 13l
    | `fscanf -> 14l
    | `strrchr -> 15l
    | `strncpy -> 16l
    | `fstat -> 17l
    | `realloc -> 18l
    | `mmap -> 19l
    | `qsort -> 20l
    | `bsearch -> 21l
    | `open_stub -> 22l
    | `lseek -> 23l
    | `fread -> 24l
    | `strncmp -> 25l
    | `memcmp -> 26l
    | `fxstat64 -> 27l
    | `getmodulehandle -> 100l
    | `getprocaddress -> 101l
    | `getmainargs -> 102l
    | `gethostname -> 103l
    | `generic -> 255l
  )
and packed_gen__lib_ident x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `invalid -> 0l
    | `printf -> 1l
    | `strcpy -> 2l
    | `atoi -> 3l
    | `malloc -> 4l
    | `free -> 5l
    | `memcpy -> 6l
    | `memset -> 7l
    | `fgetc -> 8l
    | `read -> 9l
    | `strchr -> 10l
    | `exit -> 11l
    | `strcmp -> 12l
    | `ctype_b_loc -> 13l
    | `fscanf -> 14l
    | `strrchr -> 15l
    | `strncpy -> 16l
    | `fstat -> 17l
    | `realloc -> 18l
    | `mmap -> 19l
    | `qsort -> 20l
    | `bsearch -> 21l
    | `open_stub -> 22l
    | `lseek -> 23l
    | `fread -> 24l
    | `strncmp -> 25l
    | `memcmp -> 26l
    | `fxstat64 -> 27l
    | `getmodulehandle -> 100l
    | `getprocaddress -> 101l
    | `getmainargs -> 102l
    | `gethostname -> 103l
    | `generic -> 255l
  )

and gen__fscanf_enum code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `d -> 0l
    | `dd -> 1l
    | `i -> 2l
    | `o -> 3l
    | `u -> 4l
    | `x -> 5l
    | `xx -> 6l
    | `f -> 7l
    | `e -> 8l
    | `g -> 9l
    | `ee -> 10l
    | `a -> 11l
    | `s -> 12l
    | `c -> 13l
    | `sb -> 14l
    | `p -> 15l
    | `n -> 16l
  )
and packed_gen__fscanf_enum x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `d -> 0l
    | `dd -> 1l
    | `i -> 2l
    | `o -> 3l
    | `u -> 4l
    | `x -> 5l
    | `xx -> 6l
    | `f -> 7l
    | `e -> 8l
    | `g -> 9l
    | `ee -> 10l
    | `a -> 11l
    | `s -> 12l
    | `c -> 13l
    | `sb -> 14l
    | `p -> 15l
    | `n -> 16l
  )


let gen_string x = gen__string (-1) x
let gen_uint64 x = gen__uint64 (-1) x
let gen_bool x = gen__bool (-1) x
let gen_libcall_pol x = gen__libcall_pol (-1) x
let gen_libcall_t x = gen__libcall_t (-1) x
let gen_generic_pol x = gen__generic_pol (-1) x
let gen_ctype_b_loc_pol x = gen__ctype_b_loc_pol (-1) x
let gen_exit_pol x = gen__exit_pol (-1) x
let gen_fscanf_pol x = gen__fscanf_pol (-1) x
let gen_read_pol x = gen__read_pol (-1) x
let gen_fread_pol x = gen__fread_pol (-1) x
let gen_strcmp_pol x = gen__strcmp_pol (-1) x
let gen_strncmp_pol x = gen__strncmp_pol (-1) x
let gen_memcmp_pol x = gen__memcmp_pol (-1) x
let gen_strchr_pol x = gen__strchr_pol (-1) x
let gen_strncpy_pol x = gen__strncpy_pol (-1) x
let gen_fstat_pol x = gen__fstat_pol (-1) x
let gen_fxstat64_pol x = gen__fxstat64_pol (-1) x
let gen_realloc_pol x = gen__realloc_pol (-1) x
let gen_mmap_pol x = gen__mmap_pol (-1) x
let gen_qsort_pol x = gen__qsort_pol (-1) x
let gen_bsearch_pol x = gen__bsearch_pol (-1) x
let gen_open_pol x = gen__open_pol (-1) x
let gen_strcpy_pol x = gen__strcpy_pol (-1) x
let gen_printf_pol x = gen__printf_pol (-1) x
let gen_atoi_pol x = gen__atoi_pol (-1) x
let gen_malloc_pol x = gen__malloc_pol (-1) x
let gen_getmodulehandle_pol x = gen__getmodulehandle_pol (-1) x
let gen_getprocaddress_pol x = gen__getprocaddress_pol (-1) x
let gen_getmainargs_pol x = gen__getmainargs_pol (-1) x
let gen_gethostname_pol x = gen__gethostname_pol (-1) x
let gen_free_pol x = gen__free_pol (-1) x
let gen_memcpy_pol x = gen__memcpy_pol (-1) x
let gen_memset_pol x = gen__memset_pol (-1) x
let gen_fgetc_pol x = gen__fgetc_pol (-1) x
let gen_lseek_pol x = gen__lseek_pol (-1) x
let gen_exit_t x = gen__exit_t (-1) x
let gen_generic_t x = gen__generic_t (-1) x
let gen_fscanf_elem_t x = gen__fscanf_elem_t (-1) x
let gen_fscanf_t x = gen__fscanf_t (-1) x
let gen_ctype_b_loc_t x = gen__ctype_b_loc_t (-1) x
let gen_read_t x = gen__read_t (-1) x
let gen_fread_t x = gen__fread_t (-1) x
let gen_strchr_t x = gen__strchr_t (-1) x
let gen_strncpy_t x = gen__strncpy_t (-1) x
let gen_fstat_t x = gen__fstat_t (-1) x
let gen_fxstat64_t x = gen__fxstat64_t (-1) x
let gen_realloc_t x = gen__realloc_t (-1) x
let gen_mmap_t x = gen__mmap_t (-1) x
let gen_qsort_t x = gen__qsort_t (-1) x
let gen_bsearch_t x = gen__bsearch_t (-1) x
let gen_open_t x = gen__open_t (-1) x
let gen_strcpy_t x = gen__strcpy_t (-1) x
let gen_strcmp_t x = gen__strcmp_t (-1) x
let gen_strncmp_t x = gen__strncmp_t (-1) x
let gen_memcmp_t x = gen__memcmp_t (-1) x
let gen_printf_t x = gen__printf_t (-1) x
let gen_atoi_t x = gen__atoi_t (-1) x
let gen_malloc_t x = gen__malloc_t (-1) x
let gen_getmodulehandle_t x = gen__getmodulehandle_t (-1) x
let gen_getprocaddress_t x = gen__getprocaddress_t (-1) x
let gen_getmainargs_t x = gen__getmainargs_t (-1) x
let gen_gethostname_t x = gen__gethostname_t (-1) x
let gen_free_t x = gen__free_t (-1) x
let gen_memcpy_t x = gen__memcpy_t (-1) x
let gen_memset_t x = gen__memset_t (-1) x
let gen_fgetc_t x = gen__fgetc_t (-1) x
let gen_lseek_t x = gen__lseek_t (-1) x
let gen_lib_ident x = gen__lib_ident (-1) x
let gen_fscanf_enum x = gen__fscanf_enum (-1) x


let rec default_string () = ""
and default_uint64 () = 0L
and default_bool () = false
and default_libcall_pol () =
  {
    Libcall_pol.name = default_string ();
    Libcall_pol.ident = parse_lib_ident (Piqirun.parse_default "\b\255\001");
    Libcall_pol.action = Common.parse_tracing_action (Piqirun.parse_default "\b\000");
    Libcall_pol.restrict_addresses = [];
    Libcall_pol.strcpy = None;
    Libcall_pol.printf = None;
    Libcall_pol.atoi = None;
    Libcall_pol.malloc = None;
    Libcall_pol.getmodulehandle = None;
    Libcall_pol.getprocaddress = None;
    Libcall_pol.getmainargs = None;
    Libcall_pol.gethostname = None;
    Libcall_pol.free = None;
    Libcall_pol.memcpy = None;
    Libcall_pol.memset = None;
    Libcall_pol.fgetc = None;
    Libcall_pol.read = None;
    Libcall_pol.strchr = None;
    Libcall_pol.exit = None;
    Libcall_pol.strcmp = None;
    Libcall_pol.ctype_b_loc = None;
    Libcall_pol.fscanf = None;
    Libcall_pol.strncpy = None;
    Libcall_pol.fstat = None;
    Libcall_pol.realloc = None;
    Libcall_pol.mmap = None;
    Libcall_pol.qsort = None;
    Libcall_pol.bsearch = None;
    Libcall_pol.open_stub = None;
    Libcall_pol.lseek = None;
    Libcall_pol.fread = None;
    Libcall_pol.strncmp = None;
    Libcall_pol.memcmp = None;
    Libcall_pol.fxstat64 = None;
    Libcall_pol.generic = None;
  }
and default_libcall_t () =
  {
    Libcall_t.func_addr = default_uint64 ();
    Libcall_t.func_name = default_string ();
    Libcall_t.is_traced = default_bool ();
    Libcall_t.ident = default_lib_ident ();
    Libcall_t.strcpy = None;
    Libcall_t.printf = None;
    Libcall_t.atoi = None;
    Libcall_t.malloc = None;
    Libcall_t.getmodulehandle = None;
    Libcall_t.getprocaddress = None;
    Libcall_t.getmainargs = None;
    Libcall_t.gethostname = None;
    Libcall_t.free = None;
    Libcall_t.memcpy = None;
    Libcall_t.memset = None;
    Libcall_t.fgetc = None;
    Libcall_t.generic = None;
    Libcall_t.read = None;
    Libcall_t.strchr = None;
    Libcall_t.exit = None;
    Libcall_t.strcmp = None;
    Libcall_t.ctype_b_loc = None;
    Libcall_t.fscanf = None;
    Libcall_t.strncpy = None;
    Libcall_t.fstat = None;
    Libcall_t.realloc = None;
    Libcall_t.mmap = None;
    Libcall_t.qsort = None;
    Libcall_t.bsearch = None;
    Libcall_t.open_stub = None;
    Libcall_t.lseek = None;
    Libcall_t.fread = None;
    Libcall_t.strncmp = None;
    Libcall_t.memcmp = None;
    Libcall_t.fxstat64 = None;
  }
and default_generic_pol () =
  {
    Generic_pol.ret = Common.default_action ();
  }
and default_ctype_b_loc_pol () =
  {
    Ctype_b_loc_pol.table = Common.default_memory_pol ();
    Ctype_b_loc_pol.ret = Common.default_action ();
  }
and default_exit_pol () =
  {
    Exit_pol
    ._dummy = ();
  }
and default_fscanf_pol () =
  {
    Fscanf_pol.stream = Common.default_action ();
    Fscanf_pol.elems = Common.default_action ();
    Fscanf_pol.ret = Common.default_action ();
  }
and default_read_pol () =
  {
    Read_pol.fd = Common.default_action ();
    Read_pol.buf = Common.default_memory_pol ();
    Read_pol.count = Common.default_action ();
    Read_pol.ret = Common.default_action ();
  }
and default_fread_pol () =
  {
    Fread_pol.ptr = Common.default_memory_pol ();
    Fread_pol.size = Common.default_action ();
    Fread_pol.nmemb = Common.default_action ();
    Fread_pol.stream = Common.default_action ();
    Fread_pol.ret = Common.default_action ();
  }
and default_strcmp_pol () =
  {
    Strcmp_pol.src = Common.default_memory_pol ();
    Strcmp_pol.dst = Common.default_memory_pol ();
    Strcmp_pol.ret = Common.default_action ();
  }
and default_strncmp_pol () =
  {
    Strncmp_pol.s1 = Common.default_action ();
    Strncmp_pol.s2 = Common.default_action ();
    Strncmp_pol.n = Common.default_action ();
    Strncmp_pol.ret = Common.default_action ();
  }
and default_memcmp_pol () =
  {
    Memcmp_pol.s1 = Common.default_action ();
    Memcmp_pol.s2 = Common.default_action ();
    Memcmp_pol.n = Common.default_action ();
    Memcmp_pol.ret = Common.default_action ();
  }
and default_strchr_pol () =
  {
    Strchr_pol.sc = Common.default_memory_pol ();
    Strchr_pol.c = Common.default_memory_pol ();
    Strchr_pol.ret = Common.default_action ();
  }
and default_strncpy_pol () =
  {
    Strncpy_pol.dst = Common.default_action ();
    Strncpy_pol.src = Common.default_action ();
    Strncpy_pol.n = Common.default_action ();
    Strncpy_pol.ret = Common.default_action ();
  }
and default_fstat_pol () =
  {
    Fstat_pol
    ._dummy = ();
  }
and default_fxstat64_pol () =
  {
    Fxstat64_pol
    ._dummy = ();
  }
and default_realloc_pol () =
  {
    Realloc_pol.ptr = Common.default_action ();
    Realloc_pol.size = Common.default_action ();
    Realloc_pol.ret = Common.default_action ();
  }
and default_mmap_pol () =
  {
    Mmap_pol.addr = Common.default_action ();
    Mmap_pol.length = Common.default_action ();
    Mmap_pol.prot = Common.default_action ();
    Mmap_pol.flags = Common.default_action ();
    Mmap_pol.fd = Common.default_action ();
    Mmap_pol.offset = Common.default_action ();
    Mmap_pol.ret = Common.default_memory_pol ();
  }
and default_qsort_pol () =
  {
    Qsort_pol.base = Common.default_memory_pol ();
    Qsort_pol.nmemb = Common.default_action ();
    Qsort_pol.size = Common.default_action ();
    Qsort_pol.compare = Common.default_action ();
  }
and default_bsearch_pol () =
  {
    Bsearch_pol.key = Common.default_memory_pol ();
    Bsearch_pol.base = Common.default_memory_pol ();
    Bsearch_pol.nmemb = Common.default_action ();
    Bsearch_pol.size = Common.default_action ();
    Bsearch_pol.compare = Common.default_action ();
    Bsearch_pol.ret = Common.default_memory_pol ();
  }
and default_open_pol () =
  {
    Open_pol.pathname = Common.default_memory_pol ();
    Open_pol.flags = Common.default_action ();
    Open_pol.mode = Common.default_action ();
    Open_pol.ret = Common.default_action ();
  }
and default_strcpy_pol () =
  {
    Strcpy_pol.src = Common.default_memory_pol ();
    Strcpy_pol.dst = Common.default_memory_pol ();
    Strcpy_pol.ret = Common.default_action ();
  }
and default_printf_pol () =
  {
    Printf_pol.format = Common.default_memory_pol ();
    Printf_pol.ret = Common.default_action ();
  }
and default_atoi_pol () =
  {
    Atoi_pol.src = Common.default_memory_pol ();
    Atoi_pol.ret = Common.default_action ();
  }
and default_malloc_pol () =
  {
    Malloc_pol.size = Common.default_action ();
    Malloc_pol.ret = Common.default_action ();
  }
and default_getmodulehandle_pol () =
  {
    Getmodulehandle_pol.module_name = Common.default_memory_pol ();
    Getmodulehandle_pol.ret = Common.default_action ();
  }
and default_getprocaddress_pol () =
  {
    Getprocaddress_pol.hmodule = Common.default_action ();
    Getprocaddress_pol.proc_name = Common.default_memory_pol ();
    Getprocaddress_pol.ret = Common.default_action ();
  }
and default_getmainargs_pol () =
  {
    Getmainargs_pol.argc = Common.default_memory_pol ();
    Getmainargs_pol.argv = Common.default_memory_pol ();
    Getmainargs_pol.env = Common.default_memory_pol ();
    Getmainargs_pol.dowildcard = Common.default_action ();
    Getmainargs_pol.startinfo = Common.default_memory_pol ();
    Getmainargs_pol.ret = Common.default_action ();
  }
and default_gethostname_pol () =
  {
    Gethostname_pol.name = Common.default_memory_pol ();
    Gethostname_pol.namelen = Common.default_action ();
    Gethostname_pol.ret = Common.default_action ();
  }
and default_free_pol () =
  {
    Free_pol.ptr = Common.default_action ();
  }
and default_memcpy_pol () =
  {
    Memcpy_pol.dest = Common.default_memory_pol ();
    Memcpy_pol.src = Common.default_memory_pol ();
    Memcpy_pol.size = Common.default_action ();
    Memcpy_pol.ret = Common.default_action ();
  }
and default_memset_pol () =
  {
    Memset_pol.s = Common.default_memory_pol ();
    Memset_pol.c = Common.default_action ();
    Memset_pol.size = Common.default_action ();
    Memset_pol.ret = Common.default_action ();
  }
and default_fgetc_pol () =
  {
    Fgetc_pol.stream = Common.default_action ();
    Fgetc_pol.ret = Common.default_action ();
  }
and default_lseek_pol () =
  {
    Lseek_pol.fd = Common.default_action ();
    Lseek_pol.offset = Common.default_action ();
    Lseek_pol.whence = Common.default_action ();
    Lseek_pol.ret = Common.default_action ();
  }
and default_exit_t () =
  {
    Exit_t
    ._dummy = ();
  }
and default_generic_t () =
  {
    Generic_t.ret = default_uint64 ();
  }
and default_fscanf_elem_t () =
  {
    Fscanf_elem_t.type_ = default_fscanf_enum ();
    Fscanf_elem_t.addr = default_uint64 ();
    Fscanf_elem_t.value_dec = None;
    Fscanf_elem_t.value_string = None;
  }
and default_fscanf_t () =
  {
    Fscanf_t.stream = default_uint64 ();
    Fscanf_t.elems = [];
    Fscanf_t.ret = default_uint64 ();
  }
and default_ctype_b_loc_t () =
  {
    Ctype_b_loc_t.table = Common.default_memory_t ();
    Ctype_b_loc_t.ret = default_uint64 ();
  }
and default_read_t () =
  {
    Read_t.fd = default_uint64 ();
    Read_t.buf = Common.default_memory_t ();
    Read_t.count = default_uint64 ();
    Read_t.ret = default_uint64 ();
  }
and default_fread_t () =
  {
    Fread_t.ptr = Common.default_memory_t ();
    Fread_t.size = default_uint64 ();
    Fread_t.nmemb = default_uint64 ();
    Fread_t.stream = default_uint64 ();
    Fread_t.ret = default_uint64 ();
  }
and default_strchr_t () =
  {
    Strchr_t.s = default_uint64 ();
    Strchr_t.c = default_uint64 ();
    Strchr_t.size_max = default_uint64 ();
    Strchr_t.ret = default_uint64 ();
  }
and default_strncpy_t () =
  {
    Strncpy_t.dst = default_uint64 ();
    Strncpy_t.src = default_uint64 ();
    Strncpy_t.n = default_uint64 ();
    Strncpy_t.ret = default_uint64 ();
  }
and default_fstat_t () =
  {
    Fstat_t.fd = default_uint64 ();
    Fstat_t.buf = Common.default_memory_t ();
    Fstat_t.ret = default_uint64 ();
  }
and default_fxstat64_t () =
  {
    Fxstat64_t.vers = default_uint64 ();
    Fxstat64_t.fd = default_uint64 ();
    Fxstat64_t.buf = Common.default_memory_t ();
    Fxstat64_t.ret = default_uint64 ();
  }
and default_realloc_t () =
  {
    Realloc_t.ptr = default_uint64 ();
    Realloc_t.size = default_uint64 ();
    Realloc_t.ret = default_uint64 ();
  }
and default_mmap_t () =
  {
    Mmap_t.addr = default_uint64 ();
    Mmap_t.length = default_uint64 ();
    Mmap_t.prot = default_uint64 ();
    Mmap_t.flags = default_uint64 ();
    Mmap_t.fd = default_uint64 ();
    Mmap_t.offset = default_uint64 ();
    Mmap_t.ret = Common.default_memory_t ();
  }
and default_qsort_t () =
  {
    Qsort_t.base = Common.default_memory_t ();
    Qsort_t.nmemb = default_uint64 ();
    Qsort_t.size = default_uint64 ();
    Qsort_t.compare = default_uint64 ();
  }
and default_bsearch_t () =
  {
    Bsearch_t.key = Common.default_memory_t ();
    Bsearch_t.base = Common.default_memory_t ();
    Bsearch_t.nmemb = default_uint64 ();
    Bsearch_t.size = default_uint64 ();
    Bsearch_t.compare = default_uint64 ();
    Bsearch_t.ret = Common.default_memory_t ();
  }
and default_open_t () =
  {
    Open_t.pathname = Common.default_memory_t ();
    Open_t.flags = default_uint64 ();
    Open_t.mode = default_uint64 ();
    Open_t.ret = default_uint64 ();
  }
and default_strcpy_t () =
  {
    Strcpy_t.src = Common.default_memory_t ();
    Strcpy_t.dst = Common.default_memory_t ();
    Strcpy_t.ret = default_uint64 ();
  }
and default_strcmp_t () =
  {
    Strcmp_t.s1 = default_uint64 ();
    Strcmp_t.s2 = default_uint64 ();
    Strcmp_t.size_max_s1 = default_uint64 ();
    Strcmp_t.size_max_s2 = default_uint64 ();
    Strcmp_t.ret = default_uint64 ();
  }
and default_strncmp_t () =
  {
    Strncmp_t.s1 = default_uint64 ();
    Strncmp_t.s2 = default_uint64 ();
    Strncmp_t.n = default_uint64 ();
    Strncmp_t.ret = default_uint64 ();
  }
and default_memcmp_t () =
  {
    Memcmp_t.s1 = default_uint64 ();
    Memcmp_t.s2 = default_uint64 ();
    Memcmp_t.n = default_uint64 ();
    Memcmp_t.ret = default_uint64 ();
  }
and default_printf_t () =
  {
    Printf_t.format = Common.default_memory_t ();
    Printf_t.ret = default_uint64 ();
  }
and default_atoi_t () =
  {
    Atoi_t.src = Common.default_memory_t ();
    Atoi_t.ret = default_uint64 ();
  }
and default_malloc_t () =
  {
    Malloc_t.size = default_uint64 ();
    Malloc_t.ret = default_uint64 ();
  }
and default_getmodulehandle_t () =
  {
    Getmodulehandle_t.module_name = Common.default_memory_t ();
    Getmodulehandle_t.ret = default_uint64 ();
  }
and default_getprocaddress_t () =
  {
    Getprocaddress_t.hmodule = default_uint64 ();
    Getprocaddress_t.proc_name = Common.default_memory_t ();
    Getprocaddress_t.ret = default_uint64 ();
  }
and default_getmainargs_t () =
  {
    Getmainargs_t.argc = Common.default_memory_t ();
    Getmainargs_t.argv = Common.default_memory_t ();
    Getmainargs_t.env = Common.default_memory_t ();
    Getmainargs_t.dowildcard = default_uint64 ();
    Getmainargs_t.startinfo = Common.default_memory_t ();
    Getmainargs_t.ret = default_uint64 ();
  }
and default_gethostname_t () =
  {
    Gethostname_t.name = Common.default_memory_t ();
    Gethostname_t.namelen = default_uint64 ();
    Gethostname_t.ret = default_uint64 ();
  }
and default_free_t () =
  {
    Free_t.ptr = default_uint64 ();
  }
and default_memcpy_t () =
  {
    Memcpy_t.dest = Common.default_memory_t ();
    Memcpy_t.src = Common.default_memory_t ();
    Memcpy_t.size = default_uint64 ();
    Memcpy_t.ret = default_uint64 ();
  }
and default_memset_t () =
  {
    Memset_t.s = Common.default_memory_t ();
    Memset_t.c = default_uint64 ();
    Memset_t.size = default_uint64 ();
    Memset_t.ret = default_uint64 ();
  }
and default_fgetc_t () =
  {
    Fgetc_t.stream = default_uint64 ();
    Fgetc_t.ret = default_uint64 ();
  }
and default_lseek_t () =
  {
    Lseek_t.fd = default_uint64 ();
    Lseek_t.offset = default_uint64 ();
    Lseek_t.whence = default_uint64 ();
    Lseek_t.ret = default_uint64 ();
  }
and default_lib_ident () = `invalid
and default_fscanf_enum () = `d


let piqi = "\226\202\2304\007libcall\226\231\249\238\001\017piqi/libcall.piqi\170\150\212\160\004\011\226\202\2304\006common\162\244\146\155\011\rlibcall_types\218\244\134\182\012\153\022\138\233\142\251\014\146\022\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$U\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005ident\208\215\133\174\005\000\210\171\158\194\006\tlib-ident\192\139\160\247\t\000\138\140\251\240\r\b\218\148\211\024\003\b\255\001\136\158\147\199\014\000\210\203\242$a\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006action\208\215\133\174\005\000\210\171\158\194\006\021common/tracing-action\192\139\160\247\t\000\138\140\251\240\r\007\218\148\211\024\002\b\000\136\158\147\199\014\000\210\203\242$Q\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\018restrict-addresses\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006strcpy\208\215\133\174\005\000\210\171\158\194\006\nstrcpy-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006printf\208\215\133\174\005\000\210\171\158\194\006\nprintf-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004atoi\208\215\133\174\005\000\210\171\158\194\006\batoi-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\016\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006malloc\208\215\133\174\005\000\210\171\158\194\006\nmalloc-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$[\232\146\150q\018\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\015getmodulehandle\208\215\133\174\005\000\210\171\158\194\006\019getmodulehandle-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Y\232\146\150q\020\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\014getprocaddress\208\215\133\174\005\000\210\171\158\194\006\018getprocaddress-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$S\232\146\150q\022\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011getmainargs\208\215\133\174\005\000\210\171\158\194\006\015getmainargs-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$S\232\146\150q\024\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011gethostname\208\215\133\174\005\000\210\171\158\194\006\015gethostname-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\026\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004free\208\215\133\174\005\000\210\171\158\194\006\bfree-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\028\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006memcpy\208\215\133\174\005\000\210\171\158\194\006\nmemcpy-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\030\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006memset\208\215\133\174\005\000\210\171\158\194\006\nmemset-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q \152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005fgetc\208\215\133\174\005\000\210\171\158\194\006\tfgetc-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\"\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004read\208\215\133\174\005\000\210\171\158\194\006\bread-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q$\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006strchr\208\215\133\174\005\000\210\171\158\194\006\nstrchr-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q&\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004exit\208\215\133\174\005\000\210\171\158\194\006\bexit-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q(\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006strcmp\208\215\133\174\005\000\210\171\158\194\006\nstrcmp-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$S\232\146\150q,\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011ctype-b-loc\208\215\133\174\005\000\210\171\158\194\006\015ctype-b-loc-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q.\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006fscanf\208\215\133\174\005\000\210\171\158\194\006\nfscanf-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q0\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007strncpy\208\215\133\174\005\000\210\171\158\194\006\011strncpy-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q2\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005fstat\208\215\133\174\005\000\210\171\158\194\006\tfstat-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q4\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007realloc\208\215\133\174\005\000\210\171\158\194\006\011realloc-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q6\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004mmap\208\215\133\174\005\000\210\171\158\194\006\bmmap-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q8\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005qsort\208\215\133\174\005\000\210\171\158\194\006\tqsort-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q:\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007bsearch\208\215\133\174\005\000\210\171\158\194\006\011bsearch-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q<\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\topen-stub\208\215\133\174\005\000\210\171\158\194\006\bopen-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q>\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005lseek\208\215\133\174\005\000\210\171\158\194\006\tlseek-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q@\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005fread\208\215\133\174\005\000\210\171\158\194\006\tfread-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150qB\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007strncmp\208\215\133\174\005\000\210\171\158\194\006\011strncmp-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150qD\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006memcmp\208\215\133\174\005\000\210\171\158\194\006\nmemcmp-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150qF\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\bfxstat64\208\215\133\174\005\000\210\171\158\194\006\012fxstat64-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150qd\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007generic\208\215\133\174\005\000\210\171\158\194\006\011generic-pol\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\011libcall-pol\218\244\134\182\012\185\021\138\233\142\251\014\178\021\210\203\242$H\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\tfunc-addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\tfunc-name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$S\232\146\150q\006\152\247\223\136\002\000\170\131\252\172\003\007\218\148\211\024\002\b\001\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\tis-traced\208\215\133\174\005\000\210\171\158\194\006\004bool\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005ident\208\215\133\174\005\000\210\171\158\194\006\tlib-ident\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006strcpy\208\215\133\174\005\000\210\171\158\194\006\bstrcpy-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006printf\208\215\133\174\005\000\210\171\158\194\006\bprintf-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004atoi\208\215\133\174\005\000\210\171\158\194\006\006atoi-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\016\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006malloc\208\215\133\174\005\000\210\171\158\194\006\bmalloc-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Y\232\146\150q\018\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\015getmodulehandle\208\215\133\174\005\000\210\171\158\194\006\017getmodulehandle-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$W\232\146\150q\020\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\014getprocaddress\208\215\133\174\005\000\210\171\158\194\006\016getprocaddress-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Q\232\146\150q\022\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011getmainargs\208\215\133\174\005\000\210\171\158\194\006\rgetmainargs-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Q\232\146\150q\024\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011gethostname\208\215\133\174\005\000\210\171\158\194\006\rgethostname-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\026\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004free\208\215\133\174\005\000\210\171\158\194\006\006free-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\028\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006memcpy\208\215\133\174\005\000\210\171\158\194\006\bmemcpy-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\030\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006memset\208\215\133\174\005\000\210\171\158\194\006\bmemset-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q \152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005fgetc\208\215\133\174\005\000\210\171\158\194\006\007fgetc-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\"\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007generic\208\215\133\174\005\000\210\171\158\194\006\tgeneric-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q$\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004read\208\215\133\174\005\000\210\171\158\194\006\006read-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q&\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006strchr\208\215\133\174\005\000\210\171\158\194\006\bstrchr-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q(\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004exit\208\215\133\174\005\000\210\171\158\194\006\006exit-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q*\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006strcmp\208\215\133\174\005\000\210\171\158\194\006\bstrcmp-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Q\232\146\150q.\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\011ctype-b-loc\208\215\133\174\005\000\210\171\158\194\006\rctype-b-loc-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q0\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006fscanf\208\215\133\174\005\000\210\171\158\194\006\bfscanf-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q2\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007strncpy\208\215\133\174\005\000\210\171\158\194\006\tstrncpy-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q4\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005fstat\208\215\133\174\005\000\210\171\158\194\006\007fstat-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q6\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007realloc\208\215\133\174\005\000\210\171\158\194\006\trealloc-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q8\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004mmap\208\215\133\174\005\000\210\171\158\194\006\006mmap-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q:\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005qsort\208\215\133\174\005\000\210\171\158\194\006\007qsort-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q<\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007bsearch\208\215\133\174\005\000\210\171\158\194\006\tbsearch-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q>\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\topen-stub\208\215\133\174\005\000\210\171\158\194\006\006open-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q@\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005lseek\208\215\133\174\005\000\210\171\158\194\006\007lseek-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150qB\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005fread\208\215\133\174\005\000\210\171\158\194\006\007fread-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150qD\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007strncmp\208\215\133\174\005\000\210\171\158\194\006\tstrncmp-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150qF\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006memcmp\208\215\133\174\005\000\210\171\158\194\006\bmemcmp-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150qH\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\bfxstat64\208\215\133\174\005\000\210\171\158\194\006\nfxstat64-t\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tlibcall-t\218\244\134\182\012e\138\233\142\251\014_\210\203\242$I\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\011generic-pol\218\244\134\182\012\190\001\138\233\142\251\014\183\001\210\203\242$O\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005table\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\015ctype-b-loc-pol\218\244\134\182\012\020\138\233\142\251\014\014\218\164\238\191\004\bexit-pol\218\244\134\182\012\134\002\138\233\142\251\014\255\001\210\203\242$L\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006stream\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005elems\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nfscanf-pol\218\244\134\182\012\210\002\138\233\142\251\014\203\002\210\203\242$H\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002fd\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003buf\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005count\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bread-pol\218\244\134\182\012\166\003\138\233\142\251\014\159\003\210\203\242$M\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ptr\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005nmemb\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$L\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006stream\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tfread-pol\218\244\134\182\012\137\002\138\233\142\251\014\130\002\210\203\242$M\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003src\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003dst\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nstrcmp-pol\218\244\134\182\012\204\002\138\233\142\251\014\197\002\210\203\242$H\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002s1\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002s2\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001n\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\011strncmp-pol\218\244\134\182\012\203\002\138\233\142\251\014\196\002\210\203\242$H\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002s1\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002s2\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001n\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nmemcmp-pol\218\244\134\182\012\134\002\138\233\142\251\014\255\001\210\203\242$L\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002sc\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001c\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nstrchr-pol\218\244\134\182\012\206\002\138\233\142\251\014\199\002\210\203\242$I\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003dst\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003src\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001n\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\011strncpy-pol\218\244\134\182\012\021\138\233\142\251\014\015\218\164\238\191\004\tfstat-pol\218\244\134\182\012\024\138\233\142\251\014\018\218\164\238\191\004\012fxstat64-pol\218\244\134\182\012\131\002\138\233\142\251\014\252\001\210\203\242$I\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ptr\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\011realloc-pol\218\244\134\182\012\196\004\138\233\142\251\014\189\004\210\203\242$J\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004addr\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$L\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006length\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004prot\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005flags\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002fd\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$L\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006offset\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bmmap-pol\218\244\134\182\012\218\002\138\233\142\251\014\211\002\210\203\242$N\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004base\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005nmemb\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007compare\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tqsort-pol\218\244\134\182\012\128\004\138\233\142\251\014\249\003\210\203\242$M\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003key\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004base\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005nmemb\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007compare\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\011bsearch-pol\218\244\134\182\012\217\002\138\233\142\251\014\210\002\210\203\242$R\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\bpathname\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005flags\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004mode\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bopen-pol\218\244\134\182\012\137\002\138\233\142\251\014\130\002\210\203\242$M\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003src\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003dst\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nstrcpy-pol\218\244\134\182\012\186\001\138\233\142\251\014\179\001\210\203\242$P\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006format\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nprintf-pol\218\244\134\182\012\181\001\138\233\142\251\014\174\001\210\203\242$M\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003src\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\batoi-pol\218\244\134\182\012\180\001\138\233\142\251\014\173\001\210\203\242$J\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nmalloc-pol\218\244\134\182\012\200\001\138\233\142\251\014\193\001\210\203\242$U\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\011module-name\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\019getmodulehandle-pol\218\244\134\182\012\151\002\138\233\142\251\014\144\002\210\203\242$M\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007hmodule\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$S\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\tproc-name\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\018getprocaddress-pol\218\244\134\182\012\143\004\138\233\142\251\014\136\004\210\203\242$N\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004argc\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004argv\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003env\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$P\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\ndowildcard\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$S\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\tstartinfo\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\015getmainargs-pol\218\244\134\182\012\143\002\138\233\142\251\014\136\002\210\203\242$N\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007namelen\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\015gethostname-pol\218\244\134\182\012b\138\233\142\251\014\\\210\203\242$I\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ptr\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bfree-pol\218\244\134\182\012\217\002\138\233\142\251\014\210\002\210\203\242$N\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004dest\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$M\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003src\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nmemcpy-pol\218\244\134\182\012\208\002\138\233\142\251\014\201\002\210\203\242$K\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001s\208\215\133\174\005\000\210\171\158\194\006\017common/memory-pol\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001c\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nmemset-pol\218\244\134\182\012\181\001\138\233\142\251\014\174\001\210\203\242$L\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006stream\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tfgetc-pol\218\244\134\182\012\211\002\138\233\142\251\014\204\002\210\203\242$H\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002fd\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$L\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006offset\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$L\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006whence\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\rcommon/action\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tlseek-pol\218\244\134\182\012\018\138\233\142\251\014\012\218\164\238\191\004\006exit-t\218\244\134\182\012\\\138\233\142\251\014V\210\203\242$B\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tgeneric-t\218\244\134\182\012\204\002\138\233\142\251\014\197\002\210\203\242$H\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004type\208\215\133\174\005\000\210\171\158\194\006\011fscanf-enum\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\tvalue-dec\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\012value-string\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\rfscanf-elem-t\218\244\134\182\012\246\001\138\233\142\251\014\239\001\210\203\242$E\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006stream\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\005elems\208\215\133\174\005\000\210\171\158\194\006\rfscanf-elem-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bfscanf-t\218\244\134\182\012\179\001\138\233\142\251\014\172\001\210\203\242$M\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005table\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\rctype-b-loc-t\218\244\134\182\012\185\002\138\233\142\251\014\178\002\210\203\242$A\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002fd\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003buf\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005count\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\006read-t\218\244\134\182\012\134\003\138\233\142\251\014\255\002\210\203\242$K\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ptr\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005nmemb\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006stream\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\007fread-t\218\244\134\182\012\178\002\138\233\142\251\014\171\002\210\203\242$@\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001s\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$@\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001c\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$G\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\bsize-max\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bstrchr-t\218\244\134\182\012\176\002\138\233\142\251\014\169\002\210\203\242$B\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003dst\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003src\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$@\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001n\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tstrncpy-t\218\244\134\182\012\241\001\138\233\142\251\014\234\001\210\203\242$A\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002fd\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003buf\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\007fstat-t\218\244\134\182\012\188\002\138\233\142\251\014\181\002\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004vers\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$A\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002fd\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003buf\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\nfxstat64-t\218\244\134\182\012\236\001\138\233\142\251\014\229\001\210\203\242$B\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ptr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\trealloc-t\218\244\134\182\012\150\004\138\233\142\251\014\143\004\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004addr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006length\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004prot\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005flags\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$A\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002fd\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006offset\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\006mmap-t\218\244\134\182\012\193\002\138\233\142\251\014\186\002\210\203\242$L\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004base\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005nmemb\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007compare\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\007qsort-t\218\244\134\182\012\227\003\138\233\142\251\014\220\003\210\203\242$K\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003key\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$L\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004base\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005nmemb\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007compare\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tbsearch-t\218\244\134\182\012\192\002\138\233\142\251\014\185\002\210\203\242$P\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\bpathname\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\005flags\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004mode\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\006open-t\218\244\134\182\012\252\001\138\233\142\251\014\245\001\210\203\242$K\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003src\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003dst\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bstrcpy-t\218\244\134\182\012\134\003\138\233\142\251\014\255\002\210\203\242$A\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002s1\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$A\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002s2\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\011size-max-s1\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$J\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\011size-max-s2\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bstrcmp-t\218\244\134\182\012\174\002\138\233\142\251\014\167\002\210\203\242$A\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002s1\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$A\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002s2\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$@\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001n\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tstrncmp-t\218\244\134\182\012\173\002\138\233\142\251\014\166\002\210\203\242$A\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002s1\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$A\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002s2\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$@\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001n\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bmemcmp-t\218\244\134\182\012\175\001\138\233\142\251\014\168\001\210\203\242$N\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006format\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bprintf-t\218\244\134\182\012\170\001\138\233\142\251\014\163\001\210\203\242$K\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003src\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\006atoi-t\218\244\134\182\012\164\001\138\233\142\251\014\157\001\210\203\242$C\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bmalloc-t\218\244\134\182\012\189\001\138\233\142\251\014\182\001\210\203\242$S\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\011module-name\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\017getmodulehandle-t\218\244\134\182\012\133\002\138\233\142\251\014\254\001\210\203\242$F\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007hmodule\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Q\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\tproc-name\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\016getprocaddress-t\218\244\134\182\012\247\003\138\233\142\251\014\240\003\210\203\242$L\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004argc\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$L\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004argv\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003env\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\ndowildcard\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$Q\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\tstartinfo\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\rgetmainargs-t\218\244\134\182\012\253\001\138\233\142\251\014\246\001\210\203\242$L\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$F\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\007namelen\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\rgethostname-t\218\244\134\182\012Y\138\233\142\251\014S\210\203\242$B\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ptr\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\006free-t\218\244\134\182\012\197\002\138\233\142\251\014\190\002\210\203\242$L\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004dest\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003src\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bmemcpy-t\218\244\134\182\012\183\002\138\233\142\251\014\176\002\210\203\242$I\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001s\208\215\133\174\005\000\210\171\158\194\006\015common/memory-t\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$@\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\001c\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bmemset-t\218\244\134\182\012\165\001\138\233\142\251\014\158\001\210\203\242$E\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006stream\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\007fgetc-t\218\244\134\182\012\181\002\138\233\142\251\014\174\002\210\203\242$A\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002fd\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006offset\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006whence\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\003ret\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\007lseek-t\218\244\134\182\012\245\007\138\176\205\197\001\238\007\218\164\238\191\004\tlib-ident\170\183\218\222\005\024\232\146\150q\000\152\247\223\136\002\000\218\164\238\191\004\007INVALID\170\183\218\222\005\023\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\006PRINTF\170\183\218\222\005\023\232\146\150q\004\152\247\223\136\002\000\218\164\238\191\004\006STRCPY\170\183\218\222\005\021\232\146\150q\006\152\247\223\136\002\000\218\164\238\191\004\004ATOI\170\183\218\222\005\023\232\146\150q\b\152\247\223\136\002\000\218\164\238\191\004\006MALLOC\170\183\218\222\005\021\232\146\150q\n\152\247\223\136\002\000\218\164\238\191\004\004FREE\170\183\218\222\005\023\232\146\150q\012\152\247\223\136\002\000\218\164\238\191\004\006MEMCPY\170\183\218\222\005\023\232\146\150q\014\152\247\223\136\002\000\218\164\238\191\004\006MEMSET\170\183\218\222\005\022\232\146\150q\016\152\247\223\136\002\000\218\164\238\191\004\005FGETC\170\183\218\222\005\021\232\146\150q\018\152\247\223\136\002\000\218\164\238\191\004\004READ\170\183\218\222\005\023\232\146\150q\020\152\247\223\136\002\000\218\164\238\191\004\006STRCHR\170\183\218\222\005\021\232\146\150q\022\152\247\223\136\002\000\218\164\238\191\004\004EXIT\170\183\218\222\005\023\232\146\150q\024\152\247\223\136\002\000\218\164\238\191\004\006STRCMP\170\183\218\222\005\028\232\146\150q\026\152\247\223\136\002\000\218\164\238\191\004\011CTYPE-B-LOC\170\183\218\222\005\023\232\146\150q\028\152\247\223\136\002\000\218\164\238\191\004\006FSCANF\170\183\218\222\005\024\232\146\150q\030\152\247\223\136\002\000\218\164\238\191\004\007STRRCHR\170\183\218\222\005\024\232\146\150q \152\247\223\136\002\000\218\164\238\191\004\007STRNCPY\170\183\218\222\005\022\232\146\150q\"\152\247\223\136\002\000\218\164\238\191\004\005FSTAT\170\183\218\222\005\024\232\146\150q$\152\247\223\136\002\000\218\164\238\191\004\007REALLOC\170\183\218\222\005\021\232\146\150q&\152\247\223\136\002\000\218\164\238\191\004\004MMAP\170\183\218\222\005\022\232\146\150q(\152\247\223\136\002\000\218\164\238\191\004\005QSORT\170\183\218\222\005\024\232\146\150q*\152\247\223\136\002\000\218\164\238\191\004\007BSEARCH\170\183\218\222\005\026\232\146\150q,\152\247\223\136\002\000\218\164\238\191\004\tOPEN-STUB\170\183\218\222\005\022\232\146\150q.\152\247\223\136\002\000\218\164\238\191\004\005LSEEK\170\183\218\222\005\022\232\146\150q0\152\247\223\136\002\000\218\164\238\191\004\005FREAD\170\183\218\222\005\024\232\146\150q2\152\247\223\136\002\000\218\164\238\191\004\007STRNCMP\170\183\218\222\005\023\232\146\150q4\152\247\223\136\002\000\218\164\238\191\004\006MEMCMP\170\183\218\222\005\025\232\146\150q6\152\247\223\136\002\000\218\164\238\191\004\bFXSTAT64\170\183\218\222\005!\232\146\150q\200\001\152\247\223\136\002\000\218\164\238\191\004\015GETMODULEHANDLE\170\183\218\222\005 \232\146\150q\202\001\152\247\223\136\002\000\218\164\238\191\004\014GETPROCADDRESS\170\183\218\222\005\029\232\146\150q\204\001\152\247\223\136\002\000\218\164\238\191\004\011GETMAINARGS\170\183\218\222\005\029\232\146\150q\206\001\152\247\223\136\002\000\218\164\238\191\004\011GETHOSTNAME\170\183\218\222\005\025\232\146\150q\254\003\152\247\223\136\002\000\218\164\238\191\004\007GENERIC\218\244\134\182\012\180\003\138\176\205\197\001\173\003\218\164\238\191\004\011fscanf-enum\170\183\218\222\005\018\232\146\150q\000\152\247\223\136\002\000\218\164\238\191\004\001d\170\183\218\222\005\019\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\002dd\170\183\218\222\005\018\232\146\150q\004\152\247\223\136\002\000\218\164\238\191\004\001i\170\183\218\222\005\018\232\146\150q\006\152\247\223\136\002\000\218\164\238\191\004\001o\170\183\218\222\005\018\232\146\150q\b\152\247\223\136\002\000\218\164\238\191\004\001u\170\183\218\222\005\018\232\146\150q\n\152\247\223\136\002\000\218\164\238\191\004\001x\170\183\218\222\005\019\232\146\150q\012\152\247\223\136\002\000\218\164\238\191\004\002xx\170\183\218\222\005\018\232\146\150q\014\152\247\223\136\002\000\218\164\238\191\004\001f\170\183\218\222\005\018\232\146\150q\016\152\247\223\136\002\000\218\164\238\191\004\001e\170\183\218\222\005\018\232\146\150q\018\152\247\223\136\002\000\218\164\238\191\004\001g\170\183\218\222\005\019\232\146\150q\020\152\247\223\136\002\000\218\164\238\191\004\002ee\170\183\218\222\005\018\232\146\150q\022\152\247\223\136\002\000\218\164\238\191\004\001a\170\183\218\222\005\018\232\146\150q\024\152\247\223\136\002\000\218\164\238\191\004\001s\170\183\218\222\005\018\232\146\150q\026\152\247\223\136\002\000\218\164\238\191\004\001c\170\183\218\222\005\019\232\146\150q\028\152\247\223\136\002\000\218\164\238\191\004\002sb\170\183\218\222\005\018\232\146\150q\030\152\247\223\136\002\000\218\164\238\191\004\001p\170\183\218\222\005\018\232\146\150q \152\247\223\136\002\000\218\164\238\191\004\001n"
include Libcall_piqi
