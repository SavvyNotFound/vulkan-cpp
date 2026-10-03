#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Application
{
    public:
        void Run()
        {
            initWindow();
            initVulkan();
            mainLoop();
            cleanUp();
        }
    private:
        void initWindow()
        {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            window = glfwCreateWindow(HEIGHT, WIDTH, NAME.c_str(), nullptr, nullptr);
        }
        
        void initVulkan()
        {
            createInstance();
            setupDebugMessenger();
        }

        void setupDebugMessenger()
        {
            if (!m_EnableValidationLayers) return;

            vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

            vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
            );

            vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
                .messageSeverity = severityFlags,
                .messageType = messageTypeFlags,
                .pfnUserCallback = &debugCallback
            };
            debugMessenger = instance.createDebugUtilsMessengerEXT( debugUtilsMessengerCreateInfoEXT );
        }

        void createInstance()
        {
            constexpr vk::ApplicationInfo appInfo{.pApplicationName    = "Hello Triangle",
                                                  .applicationVersion  = VK_MAKE_VERSION(1, 0, 0),
                                                  .pEngineName         = "No Engine",
                                                  .engineVersion       = VK_MAKE_VERSION(1, 0, 0),
                                                  .apiVersion          = vk::ApiVersion14

            };

            // Get Required layers
            std::vector<const char*> requiredLayers;
            if (m_EnableValidationLayers)
            {
                requiredLayers.assign(m_ValidationLayers.begin(), m_ValidationLayers.end());
            }
            
            // check if required layers are supported
            auto layerProperties = context.enumerateInstanceLayerProperties();
            auto unsupportedLayerIt = std::ranges::find_if(requiredLayers,
                    [&layerProperties](auto const &requiredLayer)
                    {
                        return std::ranges::none_of(layerProperties,
                                [requiredLayer](auto const &layerProperty)
                                {
                                    return strcmp(layerProperty.layerName, requiredLayer) == 0;
                                });
                    });

            if (unsupportedLayerIt != requiredLayers.end())
            {
                throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
            }

            // Get required extensions
            auto requiredExtensions = getRequiredInstanceExtension();

            // Check if required extensions are supported
            auto extensionProperties = context.enumerateInstanceExtensionProperties();
            auto unsupportedPropertyIt = 
                std::ranges::find_if(requiredExtensions,
                            [&extensionProperties](auto const &requiredExtension)
                            {
                                return std::ranges::none_of(extensionProperties,
                                            [requiredExtension](auto const &extensionProperty)
                                            {
                                                return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
                                            }
                                        );
                            }
                        );
            if (unsupportedPropertyIt != requiredExtensions.end())
            {
                throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedPropertyIt));
            }


            vk::InstanceCreateInfo createInfo{
                .pApplicationInfo = &appInfo,
                .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
                .ppEnabledLayerNames = requiredLayers.data(),
                .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
                .ppEnabledExtensionNames = requiredExtensions.data()
            };

            instance = vk::raii::Instance(context, createInfo);

            std::cout << "Available Extensions:\n";
            auto extensions = context.enumerateInstanceExtensionProperties();
            for (const auto& extension : extensions)
                std::cout << "\t" << extension.extensionName << "\n";
        }

        void mainLoop()
        {
            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
            }
        }

        std::vector<const char*> getRequiredInstanceExtension()
        {
            uint32_t glfwExtensionCount = 0;
            auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
            if (m_EnableValidationLayers)
            {
                extensions.push_back(vk::EXTDebugUtilsExtensionName);
            }

            return extensions;
        }

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT        severity,
                                                                vk::DebugUtilsMessageTypeFlagsEXT             type,
                                                                const vk::DebugUtilsMessengerCallbackDataEXT  *pCallbackData,
                                                                void*                                         pUserData
                                                             )
        {
            if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning ||
                    severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
            {
                std::cerr << "Validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
            }

            return vk::False;
        }

        void cleanUp()
        {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    private:
        GLFWwindow* window = nullptr;

        static constexpr uint32_t HEIGHT = 1080;
        static constexpr uint32_t WIDTH = 720;
        static constexpr std::string NAME = "VKEngine";

        const std::vector<char const*> m_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

#ifndef NDEBUG
        static constexpr bool m_EnableValidationLayers = false;
#else
        static constexpr bool m_EnableValidationLayers = true;
#endif


        // Vulkan stuff
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
};

int main()
{
    try
    {
        Application app;
        app.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
