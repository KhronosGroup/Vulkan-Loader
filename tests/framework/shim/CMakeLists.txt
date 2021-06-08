# ~~~
# Copyright (c) 2021 Valve Corporation
# Copyright (c) 2021 LunarG, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ~~~

if (WIN32)
    add_library(shim-library SHARED shim.h windows_shim.cpp)
    target_link_libraries(shim-library PRIVATE detours cfgmgr32 testing_framework_util)
    target_include_directories(shim-library PUBLIC ${CMAKE_SOURCE_DIR}/loader ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

elseif(UNIX)
    if(APPLE)
        add_library(shim-library SHARED shim.h unix_shim.cpp)
    else()
        add_library(shim-library STATIC shim.h unix_shim.cpp)
    endif()
    target_link_libraries(shim-library PRIVATE testing_framework_util)
    target_include_directories(shim-library PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
endif()