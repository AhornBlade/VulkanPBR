#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>

#include "GLFWWindow.hpp"

namespace vkr = ::vk::raii;

class VulkanApplication
{
public:
	VulkanApplication(std::filesystem::path filepath)
		:filename{ filepath.filename().string() }, window{ 800, 600, filename.c_str() }
	{
		createInstance();
		createSurface();
		choosePhysicalDevice();
		createDevice();
	}



private:
	std::string filename;
	GLFWWindow window;
	vkr::Context context{};
	vkr::Instance instance{nullptr};
	vkr::DebugUtilsMessengerEXT debugMesenger{nullptr};
	vkr::SurfaceKHR surface{nullptr};
	vkr::PhysicalDevice physicalDevice{nullptr};
	vkr::Device device{nullptr};
	vkr::Queue graphicsQueue{nullptr};
	vkr::Queue presentQueue{nullptr};
	vkr::Queue transferQueue{nullptr};

	void createInstance();
	void createSurface();
	void choosePhysicalDevice();

	void createDevice();

};