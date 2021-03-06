# Copyright 2013 Intel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# - Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include(CheckCCompilerFlag)
include(WaffleCheckThreadLocalStorage)

function(waffle_add_c_flag flag var)
    check_c_compiler_flag("${flag}" ${var})
    if(${var})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
    endif()
endfunction()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -DDEBUG")

# Use '-g1' to produce enough debug info for generating backtraces, but not
# enough for single-stepping.
set(CMAKE_C_FLAGS_RELEASE "-g1 -O2 -DNDEBUG")

waffle_add_c_flag("-Werror=implicit-function-declaration" WERROR_IMPLICIT_FUNCTION_DECLARATION)
waffle_add_c_flag("-Werror=incompatible-pointer-types" WERROR_INCOMPATIBLE_POINTER_TYPES)
waffle_add_c_flag("-Werror=int-conversion" WERROR_INT_CONVERSION)

if(waffle_on_linux)
    # On MacOS, the SSE2 headers trigger this error.
    waffle_add_c_flag("-Werror=missing-prototypes" WERROR_MISSING_PROTOTYPES)
endif()

waffle_check_thread_local_storage()

if(waffle_has_tls)
    add_definitions(-DWAFFLE_HAS_TLS)
endif()

if(waffle_has_tls_model_initial_exec)
    add_definitions(-DWAFFLE_HAS_TLS_MODEL_INITIAL_EXEC)
endif()

if(waffle_on_mac)
    add_definitions(-DWAFFLE_HAS_CGL)
endif()

if(waffle_has_glx)
    add_definitions(-DWAFFLE_HAS_GLX)
endif()

if(waffle_has_wayland)
    add_definitions(-DWAFFLE_HAS_WAYLAND)
endif()

if(waffle_has_x11_egl)
    add_definitions(-DWAFFLE_HAS_X11_EGL)
endif()

if(waffle_has_gbm)
    add_definitions(-DWAFFLE_HAS_GBM)
endif()
