/*************************************************************************************************
 Copyright 2021 Jamar Phillip

Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*************************************************************************************************/

/*For using cmake*/
//#define __PEN_CMAKE__

/*For compiling projects on Linux*/
//#define __PEN_LINUX__

/*I suggest using Linux if using Cmake, and using Visual Studio if using Windows*/
#ifdef __PEN_CMAKE__
#ifndef __PEN_LINUX__
#define __PEN_LINUX__
#endif
#endif

#ifndef __PEN_LINUX__
#ifdef __APPLE__
#define __PEN_LINUX__
#endif
#endif

#ifndef __PEN_CMAKE__
#define ROOT_DIR ""
#endif

/*For mobile graphics rendering*/
//#define __PEN_MOBILE__

#ifdef __PEN_MOBILE__
#include <GLES2/gl2.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#define __PEN_ES__
#endif
