set(_OpenGL_REQUIRED_VARS OPENGL_gl_LIBRARY)

# Provide OPENGL_USE_<C> variables for each component.
foreach(component ${OpenGL_FIND_COMPONENTS})
  string(TOUPPER ${component} _COMPONENT)
  set(OPENGL_USE_${_COMPONENT} 1)
endforeach()

set(_OpenGL_CACHE_VARS)

if (WIN32)

  if(BORLAND)
    set (OPENGL_gl_LIBRARY import32 CACHE STRING "OpenGL library for win32")
    set (OPENGL_glu_LIBRARY import32 CACHE STRING "GLU library for win32")
  else()
    set (OPENGL_gl_LIBRARY opengl32 CACHE STRING "OpenGL library for win32")
    set (OPENGL_glu_LIBRARY glu32 CACHE STRING "GLU library for win32")
  endif()

  list(APPEND _OpenGL_CACHE_VARS
    OPENGL_gl_LIBRARY
    OPENGL_glu_LIBRARY
    )
elseif (APPLE)
  # The OpenGL.framework provides both gl and glu
  find_library(OPENGL_gl_LIBRARY OpenGL DOC "OpenGL library for OS X")
  find_library(OPENGL_glu_LIBRARY OpenGL DOC
    "GLU library for OS X (usually same as OpenGL library)")
  find_path(OPENGL_INCLUDE_DIR OpenGL/gl.h DOC "Include for OpenGL on OS X")
  list(APPEND _OpenGL_REQUIRED_VARS OPENGL_INCLUDE_DIR)

  list(APPEND _OpenGL_CACHE_VARS
    OPENGL_INCLUDE_DIR
    OPENGL_gl_LIBRARY
    OPENGL_glu_LIBRARY
    )
else()
  if (CMAKE_SYSTEM_NAME MATCHES "HP-UX")
    # Handle HP-UX cases where we only want to find OpenGL in either hpux64
    # or hpux32 depending on if we're doing a 64 bit build.
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(_OPENGL_LIB_PATH
        /opt/graphics/OpenGL/lib/hpux32/)
    else()
      set(_OPENGL_LIB_PATH
        /opt/graphics/OpenGL/lib/hpux64/
        /opt/graphics/OpenGL/lib/pa20_64)
    endif()
  elseif(CMAKE_SYSTEM_NAME STREQUAL Haiku)
    set(_OPENGL_LIB_PATH
      /boot/develop/lib/x86)
    set(_OPENGL_INCLUDE_PATH
      /boot/develop/headers/os/opengl)
  endif()

  # The first line below is to make sure that the proper headers
  # are used on a Linux machine with the NVidia drivers installed.
  # They replace Mesa with NVidia's own library but normally do not
  # install headers and that causes the linking to
  # fail since the compiler finds the Mesa headers but NVidia's library.
  # Make sure the NVIDIA directory comes BEFORE the others.
  #  - Atanas Georgiev <atanas@cs.columbia.edu>
  find_path(OPENGL_INCLUDE_DIR GL/gl.h
    /usr/share/doc/NVIDIA_GLX-1.0/include
    /usr/openwin/share/include
    /opt/graphics/OpenGL/include
    ${_OPENGL_INCLUDE_PATH}
  )
  find_path(OPENGL_GLX_INCLUDE_DIR GL/glx.h ${_OPENGL_INCLUDE_PATH})
  find_path(OPENGL_EGL_INCLUDE_DIR EGL/egl.h ${_OPENGL_INCLUDE_PATH})
  find_path(OPENGL_xmesa_INCLUDE_DIR GL/xmesa.h
    /usr/share/doc/NVIDIA_GLX-1.0/include
    /usr/openwin/share/include
    /opt/graphics/OpenGL/include
  )
  list(APPEND _OpenGL_CACHE_VARS
    OPENGL_INCLUDE_DIR
    OPENGL_GLX_INCLUDE_DIR
    OPENGL_EGL_INCLUDE_DIR
    OPENGL_xmesa_INCLUDE_DIR
    )

  # Search for the GLVND libraries.  We do this regardless of COMPONENTS; we'll
  # take into account the COMPONENTS logic later.
  find_library(OPENGL_opengl_LIBRARY
    NAMES OpenGL
    PATHS ${_OPENGL_LIB_PATH}
  )

  find_library(OPENGL_glx_LIBRARY
    NAMES GLX
    PATHS ${_OPENGL_LIB_PATH}
    PATH_SUFFIXES libglvnd
  )

  find_library(OPENGL_egl_LIBRARY
    NAMES EGL
    PATHS ${_OPENGL_LIB_PATH}
    PATH_SUFFIXES libglvnd
  )

  find_library(OPENGL_glu_LIBRARY
    NAMES GLU MesaGLU
    PATHS ${OPENGL_gl_LIBRARY}
          /opt/graphics/OpenGL/lib
          /usr/openwin/lib
          /usr/shlib
  )

  list(APPEND _OpenGL_CACHE_VARS
    OPENGL_opengl_LIBRARY
    OPENGL_glx_LIBRARY
    OPENGL_egl_LIBRARY
    OPENGL_glu_LIBRARY
    )

  set(_OpenGL_GL_POLICY_WARN 0)
  if(NOT DEFINED OpenGL_GL_PREFERENCE)
    set(OpenGL_GL_PREFERENCE "")
  endif()
  if(NOT OpenGL_GL_PREFERENCE STREQUAL "")
    # A preference has been explicitly specified.
    if(NOT OpenGL_GL_PREFERENCE MATCHES "^(GLVND|LEGACY)$")
      message(FATAL_ERROR
        "OpenGL_GL_PREFERENCE value '${OpenGL_GL_PREFERENCE}' not recognized.  "
        "Allowed values are 'GLVND' and 'LEGACY'."
        )
    endif()
  elseif(OpenGL_FIND_COMPONENTS)
    # No preference was explicitly specified, but the caller did request
    # at least one GLVND component.  Prefer GLVND for legacy GL.
    set(OpenGL_GL_PREFERENCE "GLVND")
  else()
    # No preference was explicitly specified and no GLVND components were
    # requested.  Use a policy to choose the default.
    cmake_policy(GET CMP0072 _OpenGL_GL_POLICY)
    if("x${_OpenGL_GL_POLICY}x" STREQUAL "xNEWx")
      set(OpenGL_GL_PREFERENCE "GLVND")
    else()
      set(OpenGL_GL_PREFERENCE "LEGACY")
      if("x${_OpenGL_GL_POLICY}x" STREQUAL "xx")
        set(_OpenGL_GL_POLICY_WARN 1)
      endif()
    endif()
    unset(_OpenGL_GL_POLICY)
  endif()

  if("x${OpenGL_GL_PREFERENCE}x" STREQUAL "xGLVNDx" AND OPENGL_opengl_LIBRARY AND OPENGL_glx_LIBRARY)
    # We can provide legacy GL using GLVND libraries.
    # Do not use any legacy GL library.
    set(OPENGL_gl_LIBRARY "")
  else()
    # We cannot provide legacy GL using GLVND libraries.
    # Search for the legacy GL library.
    find_library(OPENGL_gl_LIBRARY
      NAMES GL MesaGL
      PATHS /opt/graphics/OpenGL/lib
            /usr/openwin/lib
            /usr/shlib
            ${_OPENGL_LIB_PATH}
      PATH_SUFFIXES libglvnd
      )
    list(APPEND _OpenGL_CACHE_VARS OPENGL_gl_LIBRARY)
  endif()

  if(_OpenGL_GL_POLICY_WARN AND OPENGL_gl_LIBRARY AND OPENGL_opengl_LIBRARY AND OPENGL_glx_LIBRARY)
    cmake_policy(GET_WARNING CMP0072 _cmp0072_warning)
    message(AUTHOR_WARNING
      "${_cmp0072_warning}\n"
      "FindOpenGL found both a legacy GL library:\n"
      "  OPENGL_gl_LIBRARY: ${OPENGL_gl_LIBRARY}\n"
      "and GLVND libraries for OpenGL and GLX:\n"
      "  OPENGL_opengl_LIBRARY: ${OPENGL_opengl_LIBRARY}\n"
      "  OPENGL_glx_LIBRARY: ${OPENGL_glx_LIBRARY}\n"
      "OpenGL_GL_PREFERENCE has not been set to \"GLVND\" or \"LEGACY\", so for "
      "compatibility with CMake 3.10 and below the legacy GL library will be used."
      )
  endif()
  unset(_OpenGL_GL_POLICY_WARN)

  # FPHSA cannot handle "this OR that is required", so we conditionally set what
  # it must look for.  First clear any previous config we might have done:
  set(_OpenGL_REQUIRED_VARS)

  # now we append the libraries as appropriate.  The complicated logic
  # basically comes down to "use libOpenGL when we can, and add in specific
  # context mechanisms when requested, or we need them to preserve the previous
  # default where glx is always available."
  if((NOT OPENGL_USE_EGL AND
      NOT OPENGL_opengl_LIBRARY AND
          OPENGL_glx_LIBRARY AND
      NOT OPENGL_gl_LIBRARY) OR
     (NOT OPENGL_USE_EGL AND
      NOT OPENGL_glx_LIBRARY AND
      NOT OPENGL_gl_LIBRARY) OR
     (NOT OPENGL_USE_EGL AND
          OPENGL_opengl_LIBRARY AND
          OPENGL_glx_LIBRARY) OR
     (    OPENGL_USE_EGL))
    list(APPEND _OpenGL_REQUIRED_VARS OPENGL_opengl_LIBRARY)
  endif()

  # GLVND GLX library.  Preferred when available.
  if((NOT OPENGL_USE_OPENGL AND
      NOT OPENGL_USE_GLX AND
      NOT OPENGL_USE_EGL AND
      NOT OPENGL_glx_LIBRARY AND
      NOT OPENGL_gl_LIBRARY) OR
     (    OPENGL_USE_GLX AND
      NOT OPENGL_USE_EGL AND
      NOT OPENGL_glx_LIBRARY AND
      NOT OPENGL_gl_LIBRARY) OR
     (NOT OPENGL_USE_EGL AND
          OPENGL_opengl_LIBRARY AND
          OPENGL_glx_LIBRARY) OR
     (OPENGL_USE_GLX AND OPENGL_USE_EGL))
    list(APPEND _OpenGL_REQUIRED_VARS OPENGL_glx_LIBRARY)
  endif()

  # GLVND EGL library.
  if(OPENGL_USE_EGL)
    list(APPEND _OpenGL_REQUIRED_VARS OPENGL_egl_LIBRARY)
  endif()

  # Old-style "libGL" library: used as a fallback when GLVND isn't available.
  if((NOT OPENGL_USE_EGL AND
      NOT OPENGL_opengl_LIBRARY AND
          OPENGL_glx_LIBRARY AND
          OPENGL_gl_LIBRARY) OR
     (NOT OPENGL_USE_EGL AND
      NOT OPENGL_glx_LIBRARY AND
          OPENGL_gl_LIBRARY))
    list(APPEND _OpenGL_REQUIRED_VARS OPENGL_gl_LIBRARY)
  endif()

  # We always need the 'gl.h' include dir.
  list(APPEND _OpenGL_REQUIRED_VARS OPENGL_INCLUDE_DIR)

  unset(_OPENGL_INCLUDE_PATH)
  unset(_OPENGL_LIB_PATH)

  find_library(OPENGL_glu_LIBRARY
    NAMES GLU MesaGLU
    PATHS ${OPENGL_gl_LIBRARY}
          /opt/graphics/OpenGL/lib
          /usr/openwin/lib
          /usr/shlib
  )
endif ()

if(OPENGL_xmesa_INCLUDE_DIR)
  set( OPENGL_XMESA_FOUND "YES" )
else()
  set( OPENGL_XMESA_FOUND "NO" )
endif()

if(OPENGL_glu_LIBRARY)
  set( OPENGL_GLU_FOUND "YES" )
else()
  set( OPENGL_GLU_FOUND "NO" )
endif()

# OpenGL_OpenGL_FOUND is a bit unique in that it is okay if /either/ libOpenGL
# or libGL is found.
# Using libGL with libEGL is never okay, though; we handle that case later.
if(NOT OPENGL_opengl_LIBRARY AND NOT OPENGL_gl_LIBRARY)
  set(OpenGL_OpenGL_FOUND FALSE)
else()
  set(OpenGL_OpenGL_FOUND TRUE)
endif()

if(OPENGL_glx_LIBRARY AND OPENGL_GLX_INCLUDE_DIR)
  set(OpenGL_GLX_FOUND TRUE)
else()
  set(OpenGL_GLX_FOUND FALSE)
endif()

if(OPENGL_egl_LIBRARY AND OPENGL_EGL_INCLUDE_DIR)
  set(OpenGL_EGL_FOUND TRUE)
else()
  set(OpenGL_EGL_FOUND FALSE)
endif()

# User-visible names should be plural.
if(OPENGL_EGL_INCLUDE_DIR)
  set(OPENGL_EGL_INCLUDE_DIRS ${OPENGL_EGL_INCLUDE_DIR})
endif()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
if (CMAKE_FIND_PACKAGE_NAME STREQUAL "GLU")
  # FindGLU include()'s this module. It's an old pattern, but rather than
  # trying to suppress this from outside the module (which is then sensitive to
  # the contents, detect the case in this module and suppress it explicitly.
  set(FPHSA_NAME_MISMATCHED 1)
endif ()
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenGL REQUIRED_VARS ${_OpenGL_REQUIRED_VARS}
                                  HANDLE_COMPONENTS)
unset(FPHSA_NAME_MISMATCHED)
unset(_OpenGL_REQUIRED_VARS)

# OpenGL:: targets
if(OPENGL_FOUND)
  # ::OpenGL is a GLVND library, and thus Linux-only: we don't bother checking
  # for a framework version of this library.
  if(OPENGL_opengl_LIBRARY AND NOT TARGET OpenGL::OpenGL)
    if(IS_ABSOLUTE "${OPENGL_opengl_LIBRARY}")
      add_library(OpenGL::OpenGL UNKNOWN IMPORTED)
      set_target_properties(OpenGL::OpenGL PROPERTIES IMPORTED_LOCATION
                            "${OPENGL_opengl_LIBRARY}")
    else()
      add_library(OpenGL::OpenGL INTERFACE IMPORTED)
      set_target_properties(OpenGL::OpenGL PROPERTIES IMPORTED_LIBNAME
                            "${OPENGL_opengl_LIBRARY}")
    endif()
    set_target_properties(OpenGL::OpenGL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                          "${OPENGL_INCLUDE_DIR}")
  endif()

  # ::GLX is a GLVND library, and thus Linux-only: we don't bother checking
  # for a framework version of this library.
  if(OpenGL_GLX_FOUND AND NOT TARGET OpenGL::GLX AND TARGET OpenGL::OpenGL)
    if(IS_ABSOLUTE "${OPENGL_glx_LIBRARY}")
      add_library(OpenGL::GLX UNKNOWN IMPORTED)
      set_target_properties(OpenGL::GLX PROPERTIES IMPORTED_LOCATION
                            "${OPENGL_glx_LIBRARY}")
    else()
      add_library(OpenGL::GLX INTERFACE IMPORTED)
      set_target_properties(OpenGL::GLX PROPERTIES IMPORTED_LIBNAME
                            "${OPENGL_glx_LIBRARY}")
    endif()
    set_target_properties(OpenGL::GLX PROPERTIES INTERFACE_LINK_LIBRARIES
                          OpenGL::OpenGL)
    set_target_properties(OpenGL::GLX PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                          "${OPENGL_GLX_INCLUDE_DIR}")
  endif()

  if(OPENGL_gl_LIBRARY AND NOT TARGET OpenGL::GL)
    # A legacy GL library is available, so use it for the legacy GL target.
    if(IS_ABSOLUTE "${OPENGL_gl_LIBRARY}")
      add_library(OpenGL::GL UNKNOWN IMPORTED)
      if(OPENGL_gl_LIBRARY MATCHES "/([^/]+)\\.framework$")
        set(_gl_fw "${OPENGL_gl_LIBRARY}/${CMAKE_MATCH_1}")
        if(EXISTS "${_gl_fw}.tbd")
          string(APPEND _gl_fw ".tbd")
        endif()
        set_target_properties(OpenGL::GL PROPERTIES
          IMPORTED_LOCATION "${_gl_fw}")
      else()
        set_target_properties(OpenGL::GL PROPERTIES
          IMPORTED_LOCATION "${OPENGL_gl_LIBRARY}")
      endif()
    else()
      add_library(OpenGL::GL INTERFACE IMPORTED)
      set_target_properties(OpenGL::GL PROPERTIES
        IMPORTED_LIBNAME "${OPENGL_gl_LIBRARY}")
    endif()
    set_target_properties(OpenGL::GL PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}")
  elseif(NOT TARGET OpenGL::GL AND TARGET OpenGL::OpenGL AND TARGET OpenGL::GLX)
    # A legacy GL library is not available, but we can provide the legacy GL
    # target using GLVND OpenGL+GLX.
    add_library(OpenGL::GL INTERFACE IMPORTED)
    set_target_properties(OpenGL::GL PROPERTIES INTERFACE_LINK_LIBRARIES
                          OpenGL::OpenGL)
    set_property(TARGET OpenGL::GL APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 OpenGL::GLX)
    set_target_properties(OpenGL::GL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                          "${OPENGL_INCLUDE_DIR}")
  endif()

  # ::EGL is a GLVND library, and thus Linux-only: we don't bother checking
  # for a framework version of this library.
  # Note we test for OpenGL::OpenGL as a target.  When this module is updated to
  # support GLES, we would additionally want to check for the hypothetical GLES
  # target and enable EGL if either ::GLES or ::OpenGL is created.
  if(TARGET OpenGL::OpenGL AND OpenGL_EGL_FOUND AND NOT TARGET OpenGL::EGL)
    if(IS_ABSOLUTE "${OPENGL_egl_LIBRARY}")
      add_library(OpenGL::EGL UNKNOWN IMPORTED)
      set_target_properties(OpenGL::EGL PROPERTIES IMPORTED_LOCATION
                            "${OPENGL_egl_LIBRARY}")
    else()
      add_library(OpenGL::EGL INTERFACE IMPORTED)
      set_target_properties(OpenGL::EGL PROPERTIES IMPORTED_LIBNAME
                            "${OPENGL_egl_LIBRARY}")
    endif()
    set_target_properties(OpenGL::EGL PROPERTIES INTERFACE_LINK_LIBRARIES
                          OpenGL::OpenGL)
    # Note that EGL's include directory is different from OpenGL/GLX's!
    set_target_properties(OpenGL::EGL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                          "${OPENGL_EGL_INCLUDE_DIR}")
  endif()

  if(OPENGL_GLU_FOUND AND NOT TARGET OpenGL::GLU)
    if(IS_ABSOLUTE "${OPENGL_glu_LIBRARY}")
      add_library(OpenGL::GLU UNKNOWN IMPORTED)
      if(OPENGL_glu_LIBRARY MATCHES "/([^/]+)\\.framework$")
        set(_glu_fw "${OPENGL_glu_LIBRARY}/${CMAKE_MATCH_1}")
        if(EXISTS "${_glu_fw}.tbd")
          string(APPEND _glu_fw ".tbd")
        endif()
        set_target_properties(OpenGL::GLU PROPERTIES
          IMPORTED_LOCATION "${_glu_fw}")
      else()
        set_target_properties(OpenGL::GLU PROPERTIES
          IMPORTED_LOCATION "${OPENGL_glu_LIBRARY}")
      endif()
    else()
      add_library(OpenGL::GLU INTERFACE IMPORTED)
      set_target_properties(OpenGL::GLU PROPERTIES
        IMPORTED_LIBNAME "${OPENGL_glu_LIBRARY}")
    endif()
    set_target_properties(OpenGL::GLU PROPERTIES
      INTERFACE_LINK_LIBRARIES OpenGL::GL)
  endif()

  # OPENGL_LIBRARIES mirrors OpenGL::GL's logic ...
  if(OPENGL_gl_LIBRARY)
    set(OPENGL_LIBRARIES ${OPENGL_gl_LIBRARY})
  elseif(TARGET OpenGL::OpenGL AND TARGET OpenGL::GLX)
    set(OPENGL_LIBRARIES ${OPENGL_opengl_LIBRARY} ${OPENGL_glx_LIBRARY})
  else()
    set(OPENGL_LIBRARIES "")
  endif()
  # ... and also includes GLU, if available.
  if(TARGET OpenGL::GLU)
    list(APPEND OPENGL_LIBRARIES ${OPENGL_glu_LIBRARY})
  endif()
endif()

# This deprecated setting is for backward compatibility with CMake1.4
set(OPENGL_LIBRARY ${OPENGL_LIBRARIES})
# This deprecated setting is for backward compatibility with CMake1.4
set(OPENGL_INCLUDE_PATH ${OPENGL_INCLUDE_DIR})

mark_as_advanced(${_OpenGL_CACHE_VARS})
unset(_OpenGL_CACHE_VARS)