add_executable(VulkanPBR main.cpp "GLFWWindow.cpp" "VulkanApplication.cpp")
target_link_libraries(VulkanPBR 
	PUBLIC Vulkan::Headers
	PUBLIC glfw)