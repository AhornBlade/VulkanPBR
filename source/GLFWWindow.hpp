#pragma once

#include <vector>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan_raii.hpp>

#ifdef VK_USE_PLATFORM_WIN32_KHR
#define VK_SURFACE_CREATE_INFO vk::Win32SurfaceCreateInfoKHR
#elif(defined(VK_USE_PLATFORM_XCB_KHR))

#endif

class GLFWWindow
{
public:
	GLFWWindow() = default;
	GLFWWindow(int width, int height, const char* title);
	~GLFWWindow();

	GLFWWindow(const GLFWWindow&) = delete;
	GLFWWindow& operator=(const GLFWWindow&) = delete;
	GLFWWindow(GLFWWindow&&) noexcept = delete;
	GLFWWindow& operator=(GLFWWindow&&) noexcept = delete;

	std::vector<const char*> getRequiredInstanceExtensions() const noexcept;
	VK_SURFACE_CREATE_INFO getVulkanSurfaceCreateInfo() const noexcept;

private:
	GLFWwindow* window;
};
