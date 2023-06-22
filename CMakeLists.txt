cmake_minimum_required(VERSION 3.10)

project(VulkanPBR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
	add_definitions(-DGLFW_EXPOSE_NATIVE_WIN32)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
	add_definitions(-DGLFW_EXPOSE_NATIVE_X11)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	add_definitions(-DVULKAN_PBR_DEBUG)
endif()

#thrid party
add_subdirectory(third_party)

#source
add_subdirectory(source)
