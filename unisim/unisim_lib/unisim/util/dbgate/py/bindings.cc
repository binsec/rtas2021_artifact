/*
 *  Copyright (c) 2019-2020,
 *  Commissariat a l'Energie Atomique (CEA),
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#include <unisim/util/dbgate/dbgate.hh>
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stddef.h>

#if PY_VERSION_HEX >= 0x03000000
#if PY_VERSION_HEX >= 0x03050000
typedef PyAsyncMethods* cmpfunc;
#else
typedef void* cmpfunc;
#endif
#endif

typedef struct {
  PyObject_HEAD
  unisim::util::dbgate::DBGated *obj;
} PyUnisimUtilDbgateDBGated;


static PyMethodDef dbgate_functions[] = { {NULL, NULL, 0, NULL} };

static int
PyUnisimUtilDbgateDBGated__tp_init(PyUnisimUtilDbgateDBGated* self, PyObject* args)
{
  int port;
  char const* workdir = 0;

  if (!PyArg_ParseTuple(args, "i|s", &port, &workdir))
    return -1;
  
  self->obj = new unisim::util::dbgate::DBGated(port, workdir);
  return 0;
}

static void
PyUnisimUtilDbgateDBGated__tp_dealloc(PyUnisimUtilDbgateDBGated* self)
{
  unisim::util::dbgate::DBGated* tmp = self->obj;
  self->obj = NULL;
  delete tmp;
  Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject*
PyUnisimUtilDbgateDBGated_open(PyUnisimUtilDbgateDBGated* self, PyObject* args)
{
  char const* name;

  if (!PyArg_ParseTuple(args, "s", &name))
    return 0;
  
  return Py_BuildValue("i", self->obj->open(name));
}

PyObject*
PyUnisimUtilDbgateDBGated_close(PyUnisimUtilDbgateDBGated* self, PyObject* args)
{
  int fd;

  if (!PyArg_ParseTuple(args, "i", &fd))
    return 0;

  self->obj->close(fd);

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* 
PyUnisimUtilDbgateDBGated_write(PyUnisimUtilDbgateDBGated* self, PyObject* args)
{
  int fd;
  char const *buffer;
  Py_ssize_t size;

  if (not PyArg_ParseTuple( args, "is#", &fd, &buffer, &size ))
    return 0;
    
  self->obj->write(fd, buffer, size);
    
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef PyUnisimUtilDbgateDBGated_methods[] =
  {
   {"open", (PyCFunction)PyUnisimUtilDbgateDBGated_open, METH_VARARGS, "open(name)\nopen a debug file and return its descriptor" },
   {"close", (PyCFunction)PyUnisimUtilDbgateDBGated_close, METH_VARARGS, "close(fd)\nclose a debug file identified by its descriptor" },
   {"write", (PyCFunction)PyUnisimUtilDbgateDBGated_write, METH_VARARGS, "write(fd, buf, size)\nwrite to a file identified by its descriptor" },
   {NULL, NULL, 0, NULL}
  };

PyTypeObject PyUnisimUtilDbgateDBGated_Type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   "dbgate.DBGated",                  /* tp_name */
   sizeof(PyUnisimUtilDbgateDBGated), /* tp_basicsize */
   0,                                 /* tp_itemsize */
   /* methods */
   (destructor)PyUnisimUtilDbgateDBGated__tp_dealloc,        /* tp_dealloc */
   (printfunc)0,                      /* tp_print */
   (getattrfunc)NULL,       /* tp_getattr */
   (setattrfunc)NULL,       /* tp_setattr */
   0,                       /* tp_compare */
   (reprfunc)NULL,             /* tp_repr */
   (PyNumberMethods*)NULL,     /* tp_as_number */
   (PySequenceMethods*)NULL, /* tp_as_sequence */
   (PyMappingMethods*)NULL,   /* tp_as_mapping */
   (hashfunc)NULL,             /* tp_hash */
   (ternaryfunc)NULL,          /* tp_call */
   (reprfunc)NULL,              /* tp_str */
   (getattrofunc)NULL,     /* tp_getattro */
   (setattrofunc)NULL,     /* tp_setattro */
   (PyBufferProcs*)NULL,  /* tp_as_buffer */
   Py_TPFLAGS_DEFAULT,    /* tp_flags */
   "DBGated(port[, workdir]) => Create a DBGate server@port with cwd=workdir (defaults to a tmpdir).\n", /* Documentation string */
   (traverseproc)NULL,     /* tp_traverse */
   (inquiry)NULL,             /* tp_clear */
   (richcmpfunc)NULL,   /* tp_richcompare */
   0,             /* tp_weaklistoffset */
   (getiterfunc)NULL,          /* tp_iter */
   (iternextfunc)NULL,     /* tp_iternext */
   (struct PyMethodDef*)PyUnisimUtilDbgateDBGated_methods, /* tp_methods */
   (struct PyMemberDef*)0,              /* tp_members */
   0,                     /* tp_getset */
   NULL,                              /* tp_base */
   NULL,                              /* tp_dict */
   (descrgetfunc)NULL,    /* tp_descr_get */
   (descrsetfunc)NULL,    /* tp_descr_set */
   0,                 /* tp_dictoffset */
   (initproc)PyUnisimUtilDbgateDBGated__tp_init,             /* tp_init */
   (allocfunc)PyType_GenericAlloc,           /* tp_alloc */
   (newfunc)PyType_GenericNew,               /* tp_new */
   (freefunc)0,             /* tp_free */
   (inquiry)NULL,             /* tp_is_gc */
   NULL,                              /* tp_bases */
   NULL,                              /* tp_mro */
   NULL,                              /* tp_cache */
   NULL,                              /* tp_subclasses */
   NULL,                              /* tp_weaklist */
   (destructor) NULL                  /* tp_del */
  };

#if PY_VERSION_HEX >= 0x03000000
#define MOD_ERROR NULL
#define MOD_INIT(name) PyObject* PyInit_##name(void)
#define MOD_RETURN(val) val
#else
#define MOD_ERROR
#define MOD_INIT(name) void init##name(void)
#define MOD_RETURN(val)
#endif

#if defined(__cplusplus)
extern "C"
#endif
#if defined(__GNUC__) && __GNUC__ >= 4
__attribute__ ((visibility("default")))
#endif


MOD_INIT(_dbgate)
{
  PyObject* m;
  
#if PY_VERSION_HEX >= 0x03000000
  static struct PyModuleDef dbgate_moduledef = { PyModuleDef_HEAD_INIT, "dbgate", NULL, -1, dbgate_functions, };
  m = PyModule_Create(&dbgate_moduledef);
#else
  m = Py_InitModule3((char *) "dbgate", dbgate_functions, NULL);
#endif
  
  if (m == NULL)
    return MOD_ERROR;
  
  /* Register the 'unisim::util::dbgate::DBGated' class */
  if (PyType_Ready(&PyUnisimUtilDbgateDBGated_Type))
    return MOD_ERROR;
    
  PyModule_AddObject( m, "DBGated", (PyObject*)&PyUnisimUtilDbgateDBGated_Type );
  return MOD_RETURN(m);
}
