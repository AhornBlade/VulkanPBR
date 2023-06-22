#include "GLFWWindow.hpp"

GLFWWindow::GLFWWindow(int width, int height, const char* title)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

GLFWWindow::~GLFWWindow()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

std::vector<const char*> GLFWWindow::getRequiredInstanceExtensions() const noexcept
{
	uint32_t extensionCount;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
	return { extensions, extensions + extensionCount };
}

VK_SURFACE_CREATE_INFO GLFWWindow::getVulkanSurfaceCreateInfo() const noexcept
{
#ifdef VK_USE_PLATFORM_WIN32_KHR
	vk::Win32SurfaceCreateInfoKHR createInfo{};
	createInfo.setHinstance(GetModuleHandle(nullptr));
	createInfo.setHwnd(glfwGetWin32Window(window));
	return createInfo;
#endif
}
