AC_DEFUN([UNISIM_CHECK_LIB], [
  if test "x$UNISIM_CHECK_LIB_MAIN" = "x"; then
    UNISIM_CHECK_LIB_MAIN=main
  fi
  unisim_check_lib_save_LIBS="${LIBS}"
  LIBS="-l$1 ${LIBS}"
  AC_MSG_CHECKING([for $2 in -l$1])
  AC_LINK_IFELSE([AC_LANG_SOURCE([[
extern "C" void $2(); extern "C" int $UNISIM_CHECK_LIB_MAIN(int argc, char **argv); int $UNISIM_CHECK_LIB_MAIN(int argc, char **argv) { $2(); return 0; }]])],
      AC_MSG_RESULT([yes]); [$3],
      LIBS="${unisim_check_lib_save_LIBS}"; AC_MSG_RESULT([no]); [$4])
])
