#include "VulkanApplication.hpp"

void VulkanApplication::createInstance()
{
	auto enabledExtensions = window.getRequiredInstanceExtensions();
	std::vector<const char*> enabledLayers{};

#ifdef VULKAN_PBR_DEBUG
	enabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	enabledLayers.push_back("VK_LAYER_KHRONOS_validation");

	vk::DebugUtilsMessengerCreateInfoEXT debugInfo{};
	debugInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
	debugInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
	debugInfo.setPfnUserCallback([](
		VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)->VkBool32
		{
			if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			{
				std::cout << "Info: " << pCallbackData->pMessage << '\n';
			}
			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				std::cout << "Warning: " << pCallbackData->pMessage << '\n';
			}
			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				std::cout << "Error: " << pCallbackData->pMessage << '\n';
			}

			return VK_FALSE;
		});

#endif

	vk::InstanceCreateInfo createInfo{};
	createInfo.setPEnabledExtensionNames(enabledExtensions);
	createInfo.setPEnabledLayerNames(enabledLayers);
#ifdef VULKAN_PBR_DEBUG
	createInfo.setPNext(&debugInfo);
#endif

	instance = vkr::Instance{ context, createInfo };
	debugMesenger = vkr::DebugUtilsMessengerEXT{ instance, debugInfo };

	std::cout << "Info: Success to create instance\n";
}

void VulkanApplication::createSurface()
{
	auto createInfo = window.getVulkanSurfaceCreateInfo();
	surface = vkr::SurfaceKHR{ instance, createInfo };
}

void VulkanApplication::choosePhysicalDevice()
{
	auto enabledPhysicalDevices = instance.enumeratePhysicalDevices();
	if (enabledPhysicalDevices.empty())
	{
		std::cout << "Error: Failed to find physical devices\n";
	}

	for (auto enabledPhysicalDevice : enabledPhysicalDevices)
	{
		auto properties = enabledPhysicalDevice.getProperties();
		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		{
			physicalDevice = enabledPhysicalDevice;
			break;
		}
	}
	if (!(*physicalDevice))
	{
		physicalDevice = enabledPhysicalDevices[0];
	}

	std::cout << "Info: Using physical device " << physicalDevice.getProperties().deviceName << '\n';
}

void VulkanApplication::createDevice()
{
	std::vector<const char*> enabledExtensions;
	std::vector<const char*> enabledLayers;
	vk::PhysicalDeviceFeatures enabledFeatures;
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

	enabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	std::vector<float> queuePriorities = { 1.0, 1.0, 0.5 };
	auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
	for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyProperties.size(); queueFamilyIndex++)
	{
		const auto& property = queueFamilyProperties[queueFamilyIndex];
		if (property.queueCount >= 3 && (property.queueFlags & vk::QueueFlagBits::eGraphics)
			&& (property.queueFlags & vk::QueueFlagBits::eTransfer))
		{
			vk::DeviceQueueCreateInfo queueInfo{};
			queueInfo.setQueueFamilyIndex(queueFamilyIndex);
			queueInfo.setQueuePriorities(queuePriorities);
			queueCreateInfos.push_back(queueInfo);
			break;
		}
	}

	if (queueCreateInfos.empty())
	{
		std::cout << "Error: Failed to find required queues\n";
	}

	vk::DeviceCreateInfo createInfo{};
	createInfo.setPEnabledExtensionNames(enabledExtensions);
	createInfo.setPEnabledLayerNames(enabledLayers);
	createInfo.setPEnabledFeatures(&enabledFeatures);
	createInfo.setQueueCreateInfos(queueCreateInfos);

	device = vkr::Device{ physicalDevice, createInfo };
	std::cout << "Info: Success to create device\n";

	graphicsQueue = vkr::Queue{ device, queueCreateInfos[0].queueFamilyIndex, 0 };
	presentQueue = vkr::Queue{ device, queueCreateInfos[0].queueFamilyIndex, 1 };
	transferQueue = vkr::Queue{ device, queueCreateInfos[0].queueFamilyIndex, 2 };
}

void VulkanApplication::createSwapchain()
{
	auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
	auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
	auto surfacePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);

	vk::SurfaceFormatKHR enabledFormat = surfaceFormats[0];
	for (auto& surfaceFormat : surfaceFormats)
	{
		if (surfaceFormat.format == vk::Format::eB8G8R8A8Srgb && surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			enabledFormat = surfaceFormat;
			break;
		}
	}

	vk::PresentModeKHR enabledPresentMode = surfacePresentModes[0];

	for (auto& presentMode : surfacePresentModes)
	{
		if (presentMode == vk::PresentModeKHR::eMailbox)
		{
			enabledPresentMode = presentMode;
			break;
		}
	}

	vk::SwapchainCreateInfoKHR createInfo{};
	createInfo.setSurface(*surface);

	createInfo.setMinImageCount((surfaceCapabilities.minImageCount == surfaceCapabilities.maxImageCount) ?
		surfaceCapabilities.maxImageCount : (surfaceCapabilities.minImageCount + 1));
	createInfo.setImageExtent(surfaceCapabilities.currentExtent);
	createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	createInfo.setImageArrayLayers(1);
	createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	createInfo.setImageFormat(enabledFormat.format);
	createInfo.setImageColorSpace(enabledFormat.colorSpace);

	createInfo.setPreTransform(surfaceCapabilities.currentTransform);
	createInfo.setClipped(VK_TRUE);
	createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	createInfo.setPresentMode(enabledPresentMode);

	swapchain = vkr::SwapchainKHR{ device, createInfo };

	std::cout << "Info: Success to create swapchain\n";

	swapchainImages = swapchain.getImages();

	vk::ImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
	imageViewCreateInfo.setFormat(enabledFormat.format);
	imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	for (auto& swapchainImage : swapchainImages)
	{
		imageViewCreateInfo.setImage(swapchainImage);
		swapchainImageViews.emplace_back(vkr::ImageView{device, imageViewCreateInfo});
	}

	std::cout << "Info: Success to create image views\n";
}
