## UNISIM_CHECK_SDL
## Checks if the SDL library is installed
#####################################################
AC_DEFUN([UNISIM_CHECK_SDL], [
    # Check if SDL path has been overloaded
    AC_ARG_WITH(sdl,
	AS_HELP_STRING([--with-sdl=<path>], [sdl library to use (will be completed with /include and /lib)]))

    if test -n "$with_sdl"; then
		AC_MSG_NOTICE([using SDL at $with_sdl])
		if test "${host}" = "${build}"; then
			AC_CHECK_PROG(sdl_config_installed, sdl-config, yes, no, $with_sdl/bin)
			if test "$sdl_config_installed" != "yes"; then
					AC_MSG_ERROR([sdl-config not found. Please install SDL development library.])
			fi
			AC_MSG_NOTICE([sdl-config found])
			sdl_cflags="`$with_sdl/bin/sdl-config --cflags`"
			sdl_libs="`$with_sdl/bin/sdl-config --libs`"
			AC_MSG_NOTICE([sdl-config says compiler needs option ${sdl_cflags} ${sdl_libs} to compile and link with SDL])
		else
			# cross-compilation detected
			case "${host}" in
				*mingw32*)
					sdl_cflags="-I${with_sdl}/include/SDL"
					sdl_libs="-L${with_sdl}/lib -lmingw32 -lSDLmain -lSDL -mwindows -lm -luser32 -lgdi32 -lwinmm"
					;;
				*)
					sdl_cflags="-I${with_sdl}/include/SDL"
					sdl_libs="-L${with_sdl}/lib -lSDL"
					;;
			esac
		fi
		CPPFLAGS=${CPPFLAGS}" ${sdl_cflags}"
		LIBS=${LIBS}" ${sdl_libs}"
    else
		AC_CHECK_PROG(sdl_config_installed, sdl-config, yes, no)
		if test "$sdl_config_installed" != "yes"; then
				AC_MSG_ERROR([sdl-config not found. Please install SDL development library.])
		fi
		AC_MSG_NOTICE([sdl-config found])
		sdl_cflags="`sdl-config --cflags`"
		sdl_libs="`sdl-config --libs`"
		AC_MSG_NOTICE([sdl-config says compiler needs option ${sdl_cflags} ${sdl_libs} to compile and link with SDL])
		CPPFLAGS=${CPPFLAGS}" ${sdl_cflags}"
		LIBS=${LIBS}" ${sdl_libs}"
	fi

    # Check for the main SDL header
    AC_CHECK_HEADER(SDL.h, broken_incsdl=no, broken_incsdl=yes)
	
    if test "$broken_incsdl" = "yes"; then
		AC_MSG_ERROR([SDL includes not found (SDL/*.hh). Please use --with-sdl=<path>])
    fi

    # Check for SDL functions
    UNISIM_CHECK_LIB(SDL, SDL_WaitThread,
    UNISIM_CHECK_LIB(SDL, SDL_FreeSurface,
    UNISIM_CHECK_LIB(SDL, SDL_DestroyMutex,
    UNISIM_CHECK_LIB(SDL, SDL_Quit,
    UNISIM_CHECK_LIB(SDL, SDL_Init,
    UNISIM_CHECK_LIB(SDL, SDL_CreateMutex,
    UNISIM_CHECK_LIB(SDL, SDL_CreateThread,
    UNISIM_CHECK_LIB(SDL, SDL_mutexP,
    UNISIM_CHECK_LIB(SDL, SDL_mutexV,
    UNISIM_CHECK_LIB(SDL, SDL_GetKeyName,
    UNISIM_CHECK_LIB(SDL, SDL_Delay,
    UNISIM_CHECK_LIB(SDL, SDL_PollEvent,
    UNISIM_CHECK_LIB(SDL, SDL_SetVideoMode,
    UNISIM_CHECK_LIB(SDL, SDL_WM_SetCaption,
    UNISIM_CHECK_LIB(SDL, SDL_CreateRGBSurface,
    UNISIM_CHECK_LIB(SDL, SDL_LockSurface,
    UNISIM_CHECK_LIB(SDL, SDL_UnlockSurface,
    UNISIM_CHECK_LIB(SDL, SDL_UpperBlit,
    UNISIM_CHECK_LIB(SDL, SDL_UpdateRect,
    UNISIM_CHECK_LIB(SDL, SDL_SaveBMP_RW, broken_sdl=no,
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes),
    broken_sdl=yes)

    if test "$broken_sdl" = "yes"; then
		AC_MSG_NOTICE([SDL not found. No video frame buffer or input devices will be available.])
    else
		AC_DEFINE([HAVE_SDL], [], [Whether SDL is available])
    fi
])
