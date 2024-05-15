Q: Is UE_LOG some how configurable to run in shippment mode and save logs to text files?

A: In Unreal Engine, UE_LOG is the macro used for logging messages to the console and log files. By default, logging with UE_LOG in shipping builds is disabled to optimize performance and reduce the size of the executable. However, if logging in shipping mode is necessary, for example, for debugging critical issues that only occur in production, you can configure it to enable logging and direct those logs to files.
Enabling Logging in Shipping Builds

To enable logging in shipping builds, you need to adjust the compilation settings and possibly modify the engine's source code depending on your requirements and the version of Unreal Engine you are using. Here are the general steps:

    Modify Build Configuration:
        Open your project's build configuration file (YourProject.Build.cs).
        Add or modify the bUseLoggingInShipping flag to true. For example:

        csharp

    PublicDefinitions.Add("USE_LOGGING_IN_SHIPPING=1");

    This step is necessary to ensure that the logging calls are not stripped out during the compilation of shipping builds.

Configure Logging Settings:

    In the Unreal Engine configuration files (found in Config folder, typically DefaultEngine.ini), specify the logging settings:

    ini

        [Core.Log]
        LogTemp=Log, Display, File

        This setting tells the engine to log messages from LogTemp (as an example category) to the console (Log), screen (Display), and log files (File).

Directing Logs to Files

By default, Unreal Engine writes logs to files located in the Saved/Logs directory of the project. You can find the logs there after running your application.
Creating Custom Log Files

If you want more control over the log files, such as creating a specific log file for certain types of messages, you will need to modify the engine or add additional logging functionality:

    Implement Custom Logging:
        Use the FOutputDeviceFile class in Unreal to direct logs to a custom file. You can create an instance of this class and use it to log messages in a specific format or location.
        Here's a basic example of how you might set this up in code:

        cpp

        FOutputDeviceFile* MyLogFile = new FOutputDeviceFile(*FPaths::ProjectLogDir() + TEXT("MyCustomLogFile.log"));
        GLog->AddOutputDevice(MyLogFile);
        GLog->Log(TEXT("This is a test log entry in my custom log file."));

Considerations

    Performance Impact: Be aware that enabling logging in shipping builds can impact performance and disk usage, which might be critical depending on the platform and nature of the project.
    Security and Privacy: Ensure that logging does not inadvertently expose sensitive or private data.

Conclusion

Configuring UE_LOG to run in shipping mode and direct output to text files involves enabling logging in shipping through build configurations and possibly extending the logging functionality with custom implementations if more specific logging behavior is needed. Always test these changes thoroughly to understand their impact on the application's performance and functionality.