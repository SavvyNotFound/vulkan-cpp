#include  <vulkan/vulkan_raii.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

class Application
{
    public:
        void Run()
        {
            initVulkan();
            m_Running = true;
            mainLoop();
            cleanUp();
        }
    private:
        void initVulkan()
        {

        }

        void mainLoop()
        {
            while (m_Running)
            {

            }
        }

        void cleanUp()
        {

        }
    private:
        bool m_Running = false;
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
