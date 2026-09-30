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
        }

        void createInstance()
        {
            constexpr vk::ApplicationInfo appInfo{.pApplicationName    = "Hello Triangle",
                                                  .applicationVersion  = VK_MAKE_VERSION(1, 0, 0),
                                                  .pEngineName         = "No Engine",
                                                  .engineVersion       = VK_MAKE_VERSION(1, 0, 0),
                                                  .apiVersion          = vk::ApiVersion14

            };

            // Get required instance extension from glfw
            uint32_t glfwExtensionCount = 0;
            auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            // Check if required glfw extensions are supported by vulkan impl
            auto extensionProperties = context.enumerateInstanceExtensionProperties();
            for (uint32_t i = 0; i < glfwExtensionCount; i++)
            {
                if (std::ranges::none_of(extensionProperties,
                            [glfwExtension = glfwExtensions[i]](auto const& extensionProperty)
                            {
                                return strcmp(extensionProperty.extensionName, glfwExtension) == 0;
                            }
                            ))
                {
                    throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
                }
            }

            vk::InstanceCreateInfo createInfo{
                .pApplicationInfo = &appInfo,
                .enabledExtensionCount = glfwExtensionCount,
                .ppEnabledExtensionNames = glfwExtensions
            };

            instance = vk::raii::Instance(context, createInfo);
        }

        void mainLoop()
        {
            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
            }
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

        // Vulkan stuff
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;
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
