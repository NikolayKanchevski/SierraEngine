//
// Created by Nikolay Kanchevski on 9.17.24.
//

int main(const int argc, char* argv[])
{
    // Create application
    const std::unique_ptr<Sierra::Application> application = std::unique_ptr<Sierra::Application>(Sierra::CreateApplication(argc, argv));
    if (application == nullptr)
    {
        throw std::runtime_error("Created application returned from Sierra::CreateApplication() must not be a null pointer!");
    }

    while (!application->Update());
}