/*
 * Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "PyUtils.h"
#include "PyCUDA.h"



const uint32_t pyUtilsMaxFunctions = 128;
      uint32_t pyUtilsNumFunctions = 0;

static PyMethodDef pyUtilsFunctions[pyUtilsMaxFunctions];


// add functions
void PyUtils_AddFunctions( PyMethodDef* functions )
{
	uint32_t count = 0;
		
	if( !functions )
		return;
	
	while(true)
	{
		if( !functions[count].ml_name || !functions[count].ml_meth )
			break;
		
		if( pyUtilsNumFunctions >= pyUtilsMaxFunctions - 1 )
		{
			printf("jetson.utils -- exceeded max number of functions to register (%u)\n", pyUtilsMaxFunctions);
			return;
		}
		
		memcpy(pyUtilsFunctions + pyUtilsNumFunctions, functions + count, sizeof(PyMethodDef));
		
		pyUtilsNumFunctions++;
		count++;
	}
}


// register functions
bool PyUtils_RegisterFunctions()
{
	printf("jetson.utils -- registering module functions...\n");
	
	// zero the master list of functions, so it end with NULL sentinel
	memset(pyUtilsFunctions, 0, sizeof(PyMethodDef) * pyUtilsMaxFunctions);
	
	// add functions to the master list
	PyUtils_AddFunctions(PyCUDA_RegisterFunctions());
	
	printf("jetson.utils -- done registering module functions\n");
	return true;
}


// register object types
bool PyUtils_RegisterTypes( PyObject* module )
{
	printf("jetson.utils -- registering module types...\n");
	
	if( !PyCUDA_RegisterTypes(module) )
		printf("jetson.utils -- failed to register CUDA types\n");

	printf("jetson.utils -- done registering module types\n");
	return true;
}

#ifdef PYTHON_3
static struct PyModuleDef pyUtilsModuleDef = {
        PyModuleDef_HEAD_INIT,
        "jetson_utils_python",
        NULL,
        -1,
        pyUtilsFunctions
};

PyMODINIT_FUNC
PyInit_jetson_utils_python(void)
{
	printf("jetson.utils -- initializing Python %i.%i bindings...\n", PY_MAJOR_VERSION, PY_MINOR_VERSION);
	
	// register functions
	if( !PyUtils_RegisterFunctions() )
		printf("jetson.utils -- failed to register module functions\n");
	
	// create the module
	PyObject* module = PyModule_Create(&pyUtilsModuleDef);
	
	if( !module )
	{
		printf("jetson.utils -- PyModule_Create() failed\n");
		return NULL;
	}
	
	// register types
	if( !PyUtils_RegisterTypes(module) )
		printf("jetson.utils -- failed to register module types\n");
	
	printf("jetson.utils -- done Python %i.%i binding initialization\n", PY_MAJOR_VERSION, PY_MINOR_VERSION);
	return module;
}

#else
PyMODINIT_FUNC
initjetson_utils_python(void)
{
	printf("jetson.utils -- initializing Python %i.%i bindings...\n", PY_MAJOR_VERSION, PY_MINOR_VERSION);
	
	// register functions
	if( !PyUtils_RegisterFunctions() )
		printf("jetson.utils -- failed to register module functions\n");
	
	// create the module
	PyObject* module = Py_InitModule("jetson_utils_python", pyUtilsFunctions);
	
	if( !module )
	{
		printf("jetson.utils -- Py_InitModule() failed\n");
		return;
	}
	
	// register types
	if( !PyUtils_RegisterTypes(module) )
		printf("jetson.utils -- failed to register module types\n");
	
	printf("jetson.utils -- done Python %i.%i binding initialization\n", PY_MAJOR_VERSION, PY_MINOR_VERSION);
}
#endif


