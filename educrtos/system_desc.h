#ifndef __SYSTEM_DESC_H__
#define __SYSTEM_DESC_H__

#ifndef NB_TASKS
#error "NB_TASKS should be defined"
#endif

#include "config.h"

LOW_LEVEL_SYSTEM_DESC(NB_TASKS);
HIGH_LEVEL_SYSTEM_DESC(NB_TASKS);

/* This is for use by system_desc.c: these are the macros used to
   generate most of the data used for the user task. */



#endif
