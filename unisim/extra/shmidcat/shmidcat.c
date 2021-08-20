/*
 * Copyright (c) 2006-2009 Tony Bybell.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/* 
   Gilles Mouchard <gilles.mouchard@cea.fr>: make shmidcat loadable as a dynamic library
   Reda Nouacer <reda.nouacer@cea.fr>
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined _MSC_VER && !defined __MINGW32__
#include <sys/ipc.h>
#include <sys/shm.h>
#endif
#ifdef __MINGW32__
#include <windows.h>
#endif
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

//#include "wave_locale.h"

/* size *must* match in gtkwave */
#define WAVE_PARTIAL_VCD_RING_BUFFER_SIZE (1024*1024)

static char *buf_top, *buf_curr, *buf;
static char *consume_ptr;


unsigned int get_8(char *p)
{
if(p >= (buf + WAVE_PARTIAL_VCD_RING_BUFFER_SIZE))
	{
	p-= WAVE_PARTIAL_VCD_RING_BUFFER_SIZE;
	}

return((unsigned int)((unsigned char)*p));
}

unsigned int get_32(char *p)
{
unsigned int rc;

rc =	(get_8(p++) << 24);
rc |=	(get_8(p++) << 16);
rc |=	(get_8(p++) <<  8);
rc |=	(get_8(p)   <<  0);

return(rc);
}

void put_8(char *p, unsigned int v)
{
if(p >= (buf + WAVE_PARTIAL_VCD_RING_BUFFER_SIZE))
        {
        p -= WAVE_PARTIAL_VCD_RING_BUFFER_SIZE;
        }

*p = (unsigned char)v;
}

void put_32(char *p, unsigned int v)
{
put_8(p++, (v>>24));
put_8(p++, (v>>16));
put_8(p++, (v>>8));
put_8(p,   (v>>0));
}

int consume(void)	/* for testing only...similar code also is on the receiving end in gtkwave */
{
char mybuff[32769];
int rc;

if((rc = *consume_ptr))
	{
	unsigned int len = get_32(consume_ptr+1);
	int i;

	for(i=0;i<len;i++)
		{
		mybuff[i] = get_8(consume_ptr+i+5);
		}
	mybuff[i] = 0;
	printf("%s", mybuff);

	*consume_ptr = 0;
	consume_ptr = consume_ptr+i+5;
	if(consume_ptr >= (buf + WAVE_PARTIAL_VCD_RING_BUFFER_SIZE))
	        {
	        consume_ptr -= WAVE_PARTIAL_VCD_RING_BUFFER_SIZE;
	        }
	}

return(rc);
}

void shmidcat_emit_string(char *s)
{
	int len = strlen(s);
	long l_top, l_curr;
	int consumed;
	int blksiz;

	for(;;)
	{
		while(!*buf_top)
		{
			if((blksiz = get_32(buf_top+1)))
			{
				buf_top += 1 + 4 + blksiz;
				if(buf_top >= (buf + WAVE_PARTIAL_VCD_RING_BUFFER_SIZE))
				{
					buf_top -= WAVE_PARTIAL_VCD_RING_BUFFER_SIZE;
				}
			}
			else
			{
				break;
			}
		}

		l_top = (long)buf_top;
		l_curr = (long)buf_curr;

		if(l_curr >= l_top)
		{
			consumed = l_curr - l_top;
		}
		else
		{
			consumed = (l_curr + WAVE_PARTIAL_VCD_RING_BUFFER_SIZE) - l_top;
		}

		if((consumed + len + 16) > WAVE_PARTIAL_VCD_RING_BUFFER_SIZE) /* just a guardband, it's oversized */
		{
#ifdef __MINGW32__
			Sleep(10);
#else
			struct timeval tv;

	        tv.tv_sec = 0;
	        tv.tv_usec = 1000000 / 100;
	        select(0, NULL, NULL, NULL, &tv);
#endif
			continue;
		}
		else
		{
			char *ss, *sd;
			put_32(buf_curr + 1, len);

			sd = buf_curr + 1 + 4;
			ss = s;
			while(*ss)
			{
				put_8(sd, *ss);
				ss++;
				sd++;
			}

			put_8(sd, 0);	/* next valid */
			put_32(sd+1, 0);	/* next len */
			put_8(buf_curr, 1); /* current valid */

			buf_curr += 1 + 4 + len;

			if(buf_curr >= (buf + WAVE_PARTIAL_VCD_RING_BUFFER_SIZE))
			{
				buf_curr -= WAVE_PARTIAL_VCD_RING_BUFFER_SIZE;
			}

			break;
		}
	}
}


#ifdef __MINGW32__
static char mapName[65];
static HANDLE hMapFile;

static STARTUPINFO si;
static PROCESS_INFORMATION pi;

#else
static struct shmid_ds ds;
static int gtkwave_pid;
#endif

static int shmid;


int shmidcat_init(char *gtk_wave_path)
{

	// parent process
	{

		// Code only executed by parent process

#ifdef __MINGW32__

		shmid = getpid();
		sprintf(mapName, "shmidcat%d", shmid);
		hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, WAVE_PARTIAL_VCD_RING_BUFFER_SIZE, mapName);
		if(hMapFile == NULL) return -1;
		buf_top = buf_curr = buf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, WAVE_PARTIAL_VCD_RING_BUFFER_SIZE);
#else

		shmid = shmget(0, WAVE_PARTIAL_VCD_RING_BUFFER_SIZE, IPC_CREAT | 0600 );

		if(shmid < 0) return -1;

		buf_top = buf_curr = buf = shmat(shmid, NULL, 0);
#endif
		memset(buf, 0, WAVE_PARTIAL_VCD_RING_BUFFER_SIZE);

#if defined(linux) || defined(__linux) || defined(__linux__)
		shmctl(shmid, IPC_RMID, &ds); /* mark for destroy, linux allows queuing up destruction now */
#endif

		consume_ptr = buf;

	}


	char handle_str[80];
	sprintf(handle_str, "%08x", shmid);

#ifdef __MINGW32__

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );


    char cmdline[256];
    memset(cmdline, 0, 256);

    sprintf(cmdline, "%s -I %s", gtk_wave_path, handle_str);

    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        cmdline,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        TRUE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return -1;
    }

#else

	gtkwave_pid = fork();

	if(gtkwave_pid == 0) // child process
	{
		// Code only executed by child process

	int ret = execlp(gtk_wave_path, "-v", "-I", handle_str, (char *)0);

		if(ret < 0) {
			printf("Failed to execute gtk-wave Code= %d\n", ret);
			// Throw exception
			return -1;
		}
	}
	else if(gtkwave_pid < 0) // failed to fork
	{
		printf("Failed to fork\n");
		// Throw exception
		return -1;
	}
#endif

	return shmid;
}

void shmidcat_exit()
{
#ifdef __MINGW32__

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

	UnmapViewOfFile(buf);
	CloseHandle(hMapFile);

#else
	if(gtkwave_pid > 0)
	{
		kill(gtkwave_pid, SIGTERM);
	}

	shmctl(shmid, IPC_RMID, &ds); /* mark for destroy */

#endif

}

/*
 * $Id: shmidcat.c,v 1.9 2011/01/14 16:23:18 gtkwave Exp $
 * $Log: shmidcat.c,v $
 * Revision 1.9  2011/01/14 16:23:18  gtkwave
 * modified to exit on EOF
 *
 * Revision 1.8  2009/12/22 19:57:26  gtkwave
 * warnings fixes on postincrement p++
 *
 * Revision 1.7  2009/04/23 04:57:38  gtkwave
 * ported shmidcat and partial vcd loader function to mingw
 *
 * Revision 1.6  2008/12/16 19:28:20  gtkwave
 * more warnings cleanups
 *
 * Revision 1.5  2008/12/04 16:42:33  gtkwave
 * restart fix for shmidcat
 *
 * Revision 1.4  2008/02/12 23:35:42  gtkwave
 * preparing for 3.1.5 revision bump
 *
 * Revision 1.3  2008/02/12 16:24:05  gtkwave
 * mingw fixes
 *
 * Revision 1.2  2007/08/31 22:50:47  gtkwave
 * update shmidcat for mingw32 compile (disabled)
 *
 * Revision 1.1.1.1  2007/05/30 04:28:18  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:18  gtkwave
 * initial release
 *
 */

