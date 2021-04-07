module Common_ext = Common_piqi_ext
let piqi = Libcall_piqi.piqi


let _ = Piqirun_ext.init_piqi piqi


let _libcall_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/libcall-pol"
let _libcall_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/libcall-t"
let _generic_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/generic-pol"
let _ctype_b_loc_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/ctype-b-loc-pol"
let _exit_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/exit-pol"
let _fscanf_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/fscanf-pol"
let _read_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/read-pol"
let _fread_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/fread-pol"
let _strcmp_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/strcmp-pol"
let _strncmp_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/strncmp-pol"
let _memcmp_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/memcmp-pol"
let _strchr_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/strchr-pol"
let _strncpy_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/strncpy-pol"
let _fstat_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/fstat-pol"
let _fxstat64_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/fxstat64-pol"
let _realloc_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/realloc-pol"
let _mmap_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/mmap-pol"
let _qsort_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/qsort-pol"
let _bsearch_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/bsearch-pol"
let _open_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/open-pol"
let _strcpy_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/strcpy-pol"
let _printf_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/printf-pol"
let _atoi_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/atoi-pol"
let _malloc_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/malloc-pol"
let _getmodulehandle_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/getmodulehandle-pol"
let _getprocaddress_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/getprocaddress-pol"
let _getmainargs_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/getmainargs-pol"
let _gethostname_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/gethostname-pol"
let _free_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/free-pol"
let _memcpy_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/memcpy-pol"
let _memset_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/memset-pol"
let _fgetc_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/fgetc-pol"
let _lseek_pol_piqi_type = Piqirun_ext.find_piqi_type "libcall/lseek-pol"
let _exit_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/exit-t"
let _generic_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/generic-t"
let _fscanf_elem_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/fscanf-elem-t"
let _fscanf_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/fscanf-t"
let _ctype_b_loc_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/ctype-b-loc-t"
let _read_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/read-t"
let _fread_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/fread-t"
let _strchr_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/strchr-t"
let _strncpy_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/strncpy-t"
let _fstat_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/fstat-t"
let _fxstat64_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/fxstat64-t"
let _realloc_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/realloc-t"
let _mmap_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/mmap-t"
let _qsort_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/qsort-t"
let _bsearch_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/bsearch-t"
let _open_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/open-t"
let _strcpy_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/strcpy-t"
let _strcmp_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/strcmp-t"
let _strncmp_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/strncmp-t"
let _memcmp_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/memcmp-t"
let _printf_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/printf-t"
let _atoi_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/atoi-t"
let _malloc_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/malloc-t"
let _getmodulehandle_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/getmodulehandle-t"
let _getprocaddress_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/getprocaddress-t"
let _getmainargs_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/getmainargs-t"
let _gethostname_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/gethostname-t"
let _free_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/free-t"
let _memcpy_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/memcpy-t"
let _memset_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/memset-t"
let _fgetc_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/fgetc-t"
let _lseek_t_piqi_type = Piqirun_ext.find_piqi_type "libcall/lseek-t"
let _lib_ident_piqi_type = Piqirun_ext.find_piqi_type "libcall/lib-ident"
let _fscanf_enum_piqi_type = Piqirun_ext.find_piqi_type "libcall/fscanf-enum"


let parse_libcall_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _libcall_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_libcall_pol buf

let parse_libcall_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _libcall_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_libcall_t buf

let parse_generic_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _generic_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_generic_pol buf

let parse_ctype_b_loc_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _ctype_b_loc_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_ctype_b_loc_pol buf

let parse_exit_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _exit_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_exit_pol buf

let parse_fscanf_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fscanf_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fscanf_pol buf

let parse_read_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _read_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_read_pol buf

let parse_fread_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fread_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fread_pol buf

let parse_strcmp_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _strcmp_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_strcmp_pol buf

let parse_strncmp_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _strncmp_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_strncmp_pol buf

let parse_memcmp_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _memcmp_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_memcmp_pol buf

let parse_strchr_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _strchr_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_strchr_pol buf

let parse_strncpy_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _strncpy_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_strncpy_pol buf

let parse_fstat_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fstat_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fstat_pol buf

let parse_fxstat64_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fxstat64_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fxstat64_pol buf

let parse_realloc_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _realloc_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_realloc_pol buf

let parse_mmap_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _mmap_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_mmap_pol buf

let parse_qsort_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _qsort_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_qsort_pol buf

let parse_bsearch_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _bsearch_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_bsearch_pol buf

let parse_open_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _open_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_open_pol buf

let parse_strcpy_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _strcpy_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_strcpy_pol buf

let parse_printf_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _printf_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_printf_pol buf

let parse_atoi_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _atoi_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_atoi_pol buf

let parse_malloc_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _malloc_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_malloc_pol buf

let parse_getmodulehandle_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _getmodulehandle_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_getmodulehandle_pol buf

let parse_getprocaddress_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _getprocaddress_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_getprocaddress_pol buf

let parse_getmainargs_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _getmainargs_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_getmainargs_pol buf

let parse_gethostname_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _gethostname_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_gethostname_pol buf

let parse_free_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _free_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_free_pol buf

let parse_memcpy_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _memcpy_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_memcpy_pol buf

let parse_memset_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _memset_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_memset_pol buf

let parse_fgetc_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fgetc_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fgetc_pol buf

let parse_lseek_pol ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _lseek_pol_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_lseek_pol buf

let parse_exit_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _exit_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_exit_t buf

let parse_generic_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _generic_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_generic_t buf

let parse_fscanf_elem_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fscanf_elem_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fscanf_elem_t buf

let parse_fscanf_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fscanf_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fscanf_t buf

let parse_ctype_b_loc_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _ctype_b_loc_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_ctype_b_loc_t buf

let parse_read_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _read_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_read_t buf

let parse_fread_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fread_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fread_t buf

let parse_strchr_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _strchr_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_strchr_t buf

let parse_strncpy_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _strncpy_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_strncpy_t buf

let parse_fstat_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fstat_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fstat_t buf

let parse_fxstat64_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fxstat64_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fxstat64_t buf

let parse_realloc_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _realloc_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_realloc_t buf

let parse_mmap_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _mmap_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_mmap_t buf

let parse_qsort_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _qsort_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_qsort_t buf

let parse_bsearch_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _bsearch_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_bsearch_t buf

let parse_open_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _open_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_open_t buf

let parse_strcpy_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _strcpy_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_strcpy_t buf

let parse_strcmp_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _strcmp_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_strcmp_t buf

let parse_strncmp_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _strncmp_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_strncmp_t buf

let parse_memcmp_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _memcmp_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_memcmp_t buf

let parse_printf_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _printf_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_printf_t buf

let parse_atoi_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _atoi_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_atoi_t buf

let parse_malloc_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _malloc_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_malloc_t buf

let parse_getmodulehandle_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _getmodulehandle_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_getmodulehandle_t buf

let parse_getprocaddress_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _getprocaddress_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_getprocaddress_t buf

let parse_getmainargs_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _getmainargs_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_getmainargs_t buf

let parse_gethostname_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _gethostname_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_gethostname_t buf

let parse_free_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _free_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_free_t buf

let parse_memcpy_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _memcpy_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_memcpy_t buf

let parse_memset_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _memset_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_memset_t buf

let parse_fgetc_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fgetc_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fgetc_t buf

let parse_lseek_t ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _lseek_t_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_lseek_t buf

let parse_lib_ident ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _lib_ident_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_lib_ident buf

let parse_fscanf_enum ?opts x (format :Piqirun_ext.input_format) =
  let x_pb = Piqirun_ext.convert _fscanf_enum_piqi_type format `pb x ?opts in
  let buf = Piqirun.init_from_string x_pb in
  Libcall_piqi.parse_fscanf_enum buf


let gen_libcall_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_libcall_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _libcall_pol_piqi_type `pb format x_pb ?opts

let gen_libcall_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_libcall_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _libcall_t_piqi_type `pb format x_pb ?opts

let gen_generic_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_generic_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _generic_pol_piqi_type `pb format x_pb ?opts

let gen_ctype_b_loc_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_ctype_b_loc_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _ctype_b_loc_pol_piqi_type `pb format x_pb ?opts

let gen_exit_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_exit_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _exit_pol_piqi_type `pb format x_pb ?opts

let gen_fscanf_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fscanf_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fscanf_pol_piqi_type `pb format x_pb ?opts

let gen_read_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_read_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _read_pol_piqi_type `pb format x_pb ?opts

let gen_fread_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fread_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fread_pol_piqi_type `pb format x_pb ?opts

let gen_strcmp_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_strcmp_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _strcmp_pol_piqi_type `pb format x_pb ?opts

let gen_strncmp_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_strncmp_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _strncmp_pol_piqi_type `pb format x_pb ?opts

let gen_memcmp_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_memcmp_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _memcmp_pol_piqi_type `pb format x_pb ?opts

let gen_strchr_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_strchr_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _strchr_pol_piqi_type `pb format x_pb ?opts

let gen_strncpy_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_strncpy_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _strncpy_pol_piqi_type `pb format x_pb ?opts

let gen_fstat_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fstat_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fstat_pol_piqi_type `pb format x_pb ?opts

let gen_fxstat64_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fxstat64_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fxstat64_pol_piqi_type `pb format x_pb ?opts

let gen_realloc_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_realloc_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _realloc_pol_piqi_type `pb format x_pb ?opts

let gen_mmap_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_mmap_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _mmap_pol_piqi_type `pb format x_pb ?opts

let gen_qsort_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_qsort_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _qsort_pol_piqi_type `pb format x_pb ?opts

let gen_bsearch_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_bsearch_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _bsearch_pol_piqi_type `pb format x_pb ?opts

let gen_open_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_open_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _open_pol_piqi_type `pb format x_pb ?opts

let gen_strcpy_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_strcpy_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _strcpy_pol_piqi_type `pb format x_pb ?opts

let gen_printf_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_printf_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _printf_pol_piqi_type `pb format x_pb ?opts

let gen_atoi_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_atoi_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _atoi_pol_piqi_type `pb format x_pb ?opts

let gen_malloc_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_malloc_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _malloc_pol_piqi_type `pb format x_pb ?opts

let gen_getmodulehandle_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_getmodulehandle_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _getmodulehandle_pol_piqi_type `pb format x_pb ?opts

let gen_getprocaddress_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_getprocaddress_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _getprocaddress_pol_piqi_type `pb format x_pb ?opts

let gen_getmainargs_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_getmainargs_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _getmainargs_pol_piqi_type `pb format x_pb ?opts

let gen_gethostname_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_gethostname_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _gethostname_pol_piqi_type `pb format x_pb ?opts

let gen_free_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_free_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _free_pol_piqi_type `pb format x_pb ?opts

let gen_memcpy_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_memcpy_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _memcpy_pol_piqi_type `pb format x_pb ?opts

let gen_memset_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_memset_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _memset_pol_piqi_type `pb format x_pb ?opts

let gen_fgetc_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fgetc_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fgetc_pol_piqi_type `pb format x_pb ?opts

let gen_lseek_pol ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_lseek_pol x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _lseek_pol_piqi_type `pb format x_pb ?opts

let gen_exit_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_exit_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _exit_t_piqi_type `pb format x_pb ?opts

let gen_generic_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_generic_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _generic_t_piqi_type `pb format x_pb ?opts

let gen_fscanf_elem_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fscanf_elem_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fscanf_elem_t_piqi_type `pb format x_pb ?opts

let gen_fscanf_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fscanf_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fscanf_t_piqi_type `pb format x_pb ?opts

let gen_ctype_b_loc_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_ctype_b_loc_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _ctype_b_loc_t_piqi_type `pb format x_pb ?opts

let gen_read_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_read_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _read_t_piqi_type `pb format x_pb ?opts

let gen_fread_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fread_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fread_t_piqi_type `pb format x_pb ?opts

let gen_strchr_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_strchr_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _strchr_t_piqi_type `pb format x_pb ?opts

let gen_strncpy_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_strncpy_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _strncpy_t_piqi_type `pb format x_pb ?opts

let gen_fstat_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fstat_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fstat_t_piqi_type `pb format x_pb ?opts

let gen_fxstat64_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fxstat64_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fxstat64_t_piqi_type `pb format x_pb ?opts

let gen_realloc_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_realloc_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _realloc_t_piqi_type `pb format x_pb ?opts

let gen_mmap_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_mmap_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _mmap_t_piqi_type `pb format x_pb ?opts

let gen_qsort_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_qsort_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _qsort_t_piqi_type `pb format x_pb ?opts

let gen_bsearch_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_bsearch_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _bsearch_t_piqi_type `pb format x_pb ?opts

let gen_open_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_open_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _open_t_piqi_type `pb format x_pb ?opts

let gen_strcpy_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_strcpy_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _strcpy_t_piqi_type `pb format x_pb ?opts

let gen_strcmp_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_strcmp_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _strcmp_t_piqi_type `pb format x_pb ?opts

let gen_strncmp_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_strncmp_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _strncmp_t_piqi_type `pb format x_pb ?opts

let gen_memcmp_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_memcmp_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _memcmp_t_piqi_type `pb format x_pb ?opts

let gen_printf_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_printf_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _printf_t_piqi_type `pb format x_pb ?opts

let gen_atoi_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_atoi_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _atoi_t_piqi_type `pb format x_pb ?opts

let gen_malloc_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_malloc_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _malloc_t_piqi_type `pb format x_pb ?opts

let gen_getmodulehandle_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_getmodulehandle_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _getmodulehandle_t_piqi_type `pb format x_pb ?opts

let gen_getprocaddress_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_getprocaddress_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _getprocaddress_t_piqi_type `pb format x_pb ?opts

let gen_getmainargs_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_getmainargs_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _getmainargs_t_piqi_type `pb format x_pb ?opts

let gen_gethostname_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_gethostname_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _gethostname_t_piqi_type `pb format x_pb ?opts

let gen_free_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_free_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _free_t_piqi_type `pb format x_pb ?opts

let gen_memcpy_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_memcpy_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _memcpy_t_piqi_type `pb format x_pb ?opts

let gen_memset_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_memset_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _memset_t_piqi_type `pb format x_pb ?opts

let gen_fgetc_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fgetc_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fgetc_t_piqi_type `pb format x_pb ?opts

let gen_lseek_t ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_lseek_t x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _lseek_t_piqi_type `pb format x_pb ?opts

let gen_lib_ident ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_lib_ident x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _lib_ident_piqi_type `pb format x_pb ?opts

let gen_fscanf_enum ?opts x (format :Piqirun_ext.output_format) =
  let buf = Libcall_piqi.gen_fscanf_enum x in
  let x_pb = Piqirun.to_string buf in
  Piqirun_ext.convert _fscanf_enum_piqi_type `pb format x_pb ?opts


let print_libcall_pol ?opts x =
  Stdlib.print_endline (gen_libcall_pol x `piq ?opts)
let prerr_libcall_pol ?opts x =
  Stdlib.prerr_endline (gen_libcall_pol x `piq ?opts)

let print_libcall_t ?opts x =
  Stdlib.print_endline (gen_libcall_t x `piq ?opts)
let prerr_libcall_t ?opts x =
  Stdlib.prerr_endline (gen_libcall_t x `piq ?opts)

let print_generic_pol ?opts x =
  Stdlib.print_endline (gen_generic_pol x `piq ?opts)
let prerr_generic_pol ?opts x =
  Stdlib.prerr_endline (gen_generic_pol x `piq ?opts)

let print_ctype_b_loc_pol ?opts x =
  Stdlib.print_endline (gen_ctype_b_loc_pol x `piq ?opts)
let prerr_ctype_b_loc_pol ?opts x =
  Stdlib.prerr_endline (gen_ctype_b_loc_pol x `piq ?opts)

let print_exit_pol ?opts x =
  Stdlib.print_endline (gen_exit_pol x `piq ?opts)
let prerr_exit_pol ?opts x =
  Stdlib.prerr_endline (gen_exit_pol x `piq ?opts)

let print_fscanf_pol ?opts x =
  Stdlib.print_endline (gen_fscanf_pol x `piq ?opts)
let prerr_fscanf_pol ?opts x =
  Stdlib.prerr_endline (gen_fscanf_pol x `piq ?opts)

let print_read_pol ?opts x =
  Stdlib.print_endline (gen_read_pol x `piq ?opts)
let prerr_read_pol ?opts x =
  Stdlib.prerr_endline (gen_read_pol x `piq ?opts)

let print_fread_pol ?opts x =
  Stdlib.print_endline (gen_fread_pol x `piq ?opts)
let prerr_fread_pol ?opts x =
  Stdlib.prerr_endline (gen_fread_pol x `piq ?opts)

let print_strcmp_pol ?opts x =
  Stdlib.print_endline (gen_strcmp_pol x `piq ?opts)
let prerr_strcmp_pol ?opts x =
  Stdlib.prerr_endline (gen_strcmp_pol x `piq ?opts)

let print_strncmp_pol ?opts x =
  Stdlib.print_endline (gen_strncmp_pol x `piq ?opts)
let prerr_strncmp_pol ?opts x =
  Stdlib.prerr_endline (gen_strncmp_pol x `piq ?opts)

let print_memcmp_pol ?opts x =
  Stdlib.print_endline (gen_memcmp_pol x `piq ?opts)
let prerr_memcmp_pol ?opts x =
  Stdlib.prerr_endline (gen_memcmp_pol x `piq ?opts)

let print_strchr_pol ?opts x =
  Stdlib.print_endline (gen_strchr_pol x `piq ?opts)
let prerr_strchr_pol ?opts x =
  Stdlib.prerr_endline (gen_strchr_pol x `piq ?opts)

let print_strncpy_pol ?opts x =
  Stdlib.print_endline (gen_strncpy_pol x `piq ?opts)
let prerr_strncpy_pol ?opts x =
  Stdlib.prerr_endline (gen_strncpy_pol x `piq ?opts)

let print_fstat_pol ?opts x =
  Stdlib.print_endline (gen_fstat_pol x `piq ?opts)
let prerr_fstat_pol ?opts x =
  Stdlib.prerr_endline (gen_fstat_pol x `piq ?opts)

let print_fxstat64_pol ?opts x =
  Stdlib.print_endline (gen_fxstat64_pol x `piq ?opts)
let prerr_fxstat64_pol ?opts x =
  Stdlib.prerr_endline (gen_fxstat64_pol x `piq ?opts)

let print_realloc_pol ?opts x =
  Stdlib.print_endline (gen_realloc_pol x `piq ?opts)
let prerr_realloc_pol ?opts x =
  Stdlib.prerr_endline (gen_realloc_pol x `piq ?opts)

let print_mmap_pol ?opts x =
  Stdlib.print_endline (gen_mmap_pol x `piq ?opts)
let prerr_mmap_pol ?opts x =
  Stdlib.prerr_endline (gen_mmap_pol x `piq ?opts)

let print_qsort_pol ?opts x =
  Stdlib.print_endline (gen_qsort_pol x `piq ?opts)
let prerr_qsort_pol ?opts x =
  Stdlib.prerr_endline (gen_qsort_pol x `piq ?opts)

let print_bsearch_pol ?opts x =
  Stdlib.print_endline (gen_bsearch_pol x `piq ?opts)
let prerr_bsearch_pol ?opts x =
  Stdlib.prerr_endline (gen_bsearch_pol x `piq ?opts)

let print_open_pol ?opts x =
  Stdlib.print_endline (gen_open_pol x `piq ?opts)
let prerr_open_pol ?opts x =
  Stdlib.prerr_endline (gen_open_pol x `piq ?opts)

let print_strcpy_pol ?opts x =
  Stdlib.print_endline (gen_strcpy_pol x `piq ?opts)
let prerr_strcpy_pol ?opts x =
  Stdlib.prerr_endline (gen_strcpy_pol x `piq ?opts)

let print_printf_pol ?opts x =
  Stdlib.print_endline (gen_printf_pol x `piq ?opts)
let prerr_printf_pol ?opts x =
  Stdlib.prerr_endline (gen_printf_pol x `piq ?opts)

let print_atoi_pol ?opts x =
  Stdlib.print_endline (gen_atoi_pol x `piq ?opts)
let prerr_atoi_pol ?opts x =
  Stdlib.prerr_endline (gen_atoi_pol x `piq ?opts)

let print_malloc_pol ?opts x =
  Stdlib.print_endline (gen_malloc_pol x `piq ?opts)
let prerr_malloc_pol ?opts x =
  Stdlib.prerr_endline (gen_malloc_pol x `piq ?opts)

let print_getmodulehandle_pol ?opts x =
  Stdlib.print_endline (gen_getmodulehandle_pol x `piq ?opts)
let prerr_getmodulehandle_pol ?opts x =
  Stdlib.prerr_endline (gen_getmodulehandle_pol x `piq ?opts)

let print_getprocaddress_pol ?opts x =
  Stdlib.print_endline (gen_getprocaddress_pol x `piq ?opts)
let prerr_getprocaddress_pol ?opts x =
  Stdlib.prerr_endline (gen_getprocaddress_pol x `piq ?opts)

let print_getmainargs_pol ?opts x =
  Stdlib.print_endline (gen_getmainargs_pol x `piq ?opts)
let prerr_getmainargs_pol ?opts x =
  Stdlib.prerr_endline (gen_getmainargs_pol x `piq ?opts)

let print_gethostname_pol ?opts x =
  Stdlib.print_endline (gen_gethostname_pol x `piq ?opts)
let prerr_gethostname_pol ?opts x =
  Stdlib.prerr_endline (gen_gethostname_pol x `piq ?opts)

let print_free_pol ?opts x =
  Stdlib.print_endline (gen_free_pol x `piq ?opts)
let prerr_free_pol ?opts x =
  Stdlib.prerr_endline (gen_free_pol x `piq ?opts)

let print_memcpy_pol ?opts x =
  Stdlib.print_endline (gen_memcpy_pol x `piq ?opts)
let prerr_memcpy_pol ?opts x =
  Stdlib.prerr_endline (gen_memcpy_pol x `piq ?opts)

let print_memset_pol ?opts x =
  Stdlib.print_endline (gen_memset_pol x `piq ?opts)
let prerr_memset_pol ?opts x =
  Stdlib.prerr_endline (gen_memset_pol x `piq ?opts)

let print_fgetc_pol ?opts x =
  Stdlib.print_endline (gen_fgetc_pol x `piq ?opts)
let prerr_fgetc_pol ?opts x =
  Stdlib.prerr_endline (gen_fgetc_pol x `piq ?opts)

let print_lseek_pol ?opts x =
  Stdlib.print_endline (gen_lseek_pol x `piq ?opts)
let prerr_lseek_pol ?opts x =
  Stdlib.prerr_endline (gen_lseek_pol x `piq ?opts)

let print_exit_t ?opts x =
  Stdlib.print_endline (gen_exit_t x `piq ?opts)
let prerr_exit_t ?opts x =
  Stdlib.prerr_endline (gen_exit_t x `piq ?opts)

let print_generic_t ?opts x =
  Stdlib.print_endline (gen_generic_t x `piq ?opts)
let prerr_generic_t ?opts x =
  Stdlib.prerr_endline (gen_generic_t x `piq ?opts)

let print_fscanf_elem_t ?opts x =
  Stdlib.print_endline (gen_fscanf_elem_t x `piq ?opts)
let prerr_fscanf_elem_t ?opts x =
  Stdlib.prerr_endline (gen_fscanf_elem_t x `piq ?opts)

let print_fscanf_t ?opts x =
  Stdlib.print_endline (gen_fscanf_t x `piq ?opts)
let prerr_fscanf_t ?opts x =
  Stdlib.prerr_endline (gen_fscanf_t x `piq ?opts)

let print_ctype_b_loc_t ?opts x =
  Stdlib.print_endline (gen_ctype_b_loc_t x `piq ?opts)
let prerr_ctype_b_loc_t ?opts x =
  Stdlib.prerr_endline (gen_ctype_b_loc_t x `piq ?opts)

let print_read_t ?opts x =
  Stdlib.print_endline (gen_read_t x `piq ?opts)
let prerr_read_t ?opts x =
  Stdlib.prerr_endline (gen_read_t x `piq ?opts)

let print_fread_t ?opts x =
  Stdlib.print_endline (gen_fread_t x `piq ?opts)
let prerr_fread_t ?opts x =
  Stdlib.prerr_endline (gen_fread_t x `piq ?opts)

let print_strchr_t ?opts x =
  Stdlib.print_endline (gen_strchr_t x `piq ?opts)
let prerr_strchr_t ?opts x =
  Stdlib.prerr_endline (gen_strchr_t x `piq ?opts)

let print_strncpy_t ?opts x =
  Stdlib.print_endline (gen_strncpy_t x `piq ?opts)
let prerr_strncpy_t ?opts x =
  Stdlib.prerr_endline (gen_strncpy_t x `piq ?opts)

let print_fstat_t ?opts x =
  Stdlib.print_endline (gen_fstat_t x `piq ?opts)
let prerr_fstat_t ?opts x =
  Stdlib.prerr_endline (gen_fstat_t x `piq ?opts)

let print_fxstat64_t ?opts x =
  Stdlib.print_endline (gen_fxstat64_t x `piq ?opts)
let prerr_fxstat64_t ?opts x =
  Stdlib.prerr_endline (gen_fxstat64_t x `piq ?opts)

let print_realloc_t ?opts x =
  Stdlib.print_endline (gen_realloc_t x `piq ?opts)
let prerr_realloc_t ?opts x =
  Stdlib.prerr_endline (gen_realloc_t x `piq ?opts)

let print_mmap_t ?opts x =
  Stdlib.print_endline (gen_mmap_t x `piq ?opts)
let prerr_mmap_t ?opts x =
  Stdlib.prerr_endline (gen_mmap_t x `piq ?opts)

let print_qsort_t ?opts x =
  Stdlib.print_endline (gen_qsort_t x `piq ?opts)
let prerr_qsort_t ?opts x =
  Stdlib.prerr_endline (gen_qsort_t x `piq ?opts)

let print_bsearch_t ?opts x =
  Stdlib.print_endline (gen_bsearch_t x `piq ?opts)
let prerr_bsearch_t ?opts x =
  Stdlib.prerr_endline (gen_bsearch_t x `piq ?opts)

let print_open_t ?opts x =
  Stdlib.print_endline (gen_open_t x `piq ?opts)
let prerr_open_t ?opts x =
  Stdlib.prerr_endline (gen_open_t x `piq ?opts)

let print_strcpy_t ?opts x =
  Stdlib.print_endline (gen_strcpy_t x `piq ?opts)
let prerr_strcpy_t ?opts x =
  Stdlib.prerr_endline (gen_strcpy_t x `piq ?opts)

let print_strcmp_t ?opts x =
  Stdlib.print_endline (gen_strcmp_t x `piq ?opts)
let prerr_strcmp_t ?opts x =
  Stdlib.prerr_endline (gen_strcmp_t x `piq ?opts)

let print_strncmp_t ?opts x =
  Stdlib.print_endline (gen_strncmp_t x `piq ?opts)
let prerr_strncmp_t ?opts x =
  Stdlib.prerr_endline (gen_strncmp_t x `piq ?opts)

let print_memcmp_t ?opts x =
  Stdlib.print_endline (gen_memcmp_t x `piq ?opts)
let prerr_memcmp_t ?opts x =
  Stdlib.prerr_endline (gen_memcmp_t x `piq ?opts)

let print_printf_t ?opts x =
  Stdlib.print_endline (gen_printf_t x `piq ?opts)
let prerr_printf_t ?opts x =
  Stdlib.prerr_endline (gen_printf_t x `piq ?opts)

let print_atoi_t ?opts x =
  Stdlib.print_endline (gen_atoi_t x `piq ?opts)
let prerr_atoi_t ?opts x =
  Stdlib.prerr_endline (gen_atoi_t x `piq ?opts)

let print_malloc_t ?opts x =
  Stdlib.print_endline (gen_malloc_t x `piq ?opts)
let prerr_malloc_t ?opts x =
  Stdlib.prerr_endline (gen_malloc_t x `piq ?opts)

let print_getmodulehandle_t ?opts x =
  Stdlib.print_endline (gen_getmodulehandle_t x `piq ?opts)
let prerr_getmodulehandle_t ?opts x =
  Stdlib.prerr_endline (gen_getmodulehandle_t x `piq ?opts)

let print_getprocaddress_t ?opts x =
  Stdlib.print_endline (gen_getprocaddress_t x `piq ?opts)
let prerr_getprocaddress_t ?opts x =
  Stdlib.prerr_endline (gen_getprocaddress_t x `piq ?opts)

let print_getmainargs_t ?opts x =
  Stdlib.print_endline (gen_getmainargs_t x `piq ?opts)
let prerr_getmainargs_t ?opts x =
  Stdlib.prerr_endline (gen_getmainargs_t x `piq ?opts)

let print_gethostname_t ?opts x =
  Stdlib.print_endline (gen_gethostname_t x `piq ?opts)
let prerr_gethostname_t ?opts x =
  Stdlib.prerr_endline (gen_gethostname_t x `piq ?opts)

let print_free_t ?opts x =
  Stdlib.print_endline (gen_free_t x `piq ?opts)
let prerr_free_t ?opts x =
  Stdlib.prerr_endline (gen_free_t x `piq ?opts)

let print_memcpy_t ?opts x =
  Stdlib.print_endline (gen_memcpy_t x `piq ?opts)
let prerr_memcpy_t ?opts x =
  Stdlib.prerr_endline (gen_memcpy_t x `piq ?opts)

let print_memset_t ?opts x =
  Stdlib.print_endline (gen_memset_t x `piq ?opts)
let prerr_memset_t ?opts x =
  Stdlib.prerr_endline (gen_memset_t x `piq ?opts)

let print_fgetc_t ?opts x =
  Stdlib.print_endline (gen_fgetc_t x `piq ?opts)
let prerr_fgetc_t ?opts x =
  Stdlib.prerr_endline (gen_fgetc_t x `piq ?opts)

let print_lseek_t ?opts x =
  Stdlib.print_endline (gen_lseek_t x `piq ?opts)
let prerr_lseek_t ?opts x =
  Stdlib.prerr_endline (gen_lseek_t x `piq ?opts)

let print_lib_ident ?opts x =
  Stdlib.print_endline (gen_lib_ident x `piq ?opts)
let prerr_lib_ident ?opts x =
  Stdlib.prerr_endline (gen_lib_ident x `piq ?opts)

let print_fscanf_enum ?opts x =
  Stdlib.print_endline (gen_fscanf_enum x `piq ?opts)
let prerr_fscanf_enum ?opts x =
  Stdlib.prerr_endline (gen_fscanf_enum x `piq ?opts)


