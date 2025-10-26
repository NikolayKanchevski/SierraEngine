//
// Created by Nikolay Kanchevski on 3.09.23.
//

#include "macOSContext.h"

#include "../../Windowing/macOS/CocoaWindow.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    macOSContext::macOSContext()
        : cocoaContext({ .application = [NSApplication sharedApplication] }), fileManager({ .fileManager = [NSFileManager defaultManager] })
    {

    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> macOSContext::CreateWindow(const WindowCreateInfo& createInfo) const
    {
        return std::make_unique<CocoaWindow>(cocoaContext, createInfo);
    }

    bool macOSContext::OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const
    {
        NSAlert* const alert = [[NSAlert alloc] init];

        [alert setMessageText: [NSString stringWithCString: openInfo.title.data() encoding: NSASCIIStringEncoding]];
        if (!openInfo.message.empty()) [alert setInformativeText: [NSString stringWithCString: openInfo.message.data() encoding: NSASCIIStringEncoding]];

        [alert addButtonWithTitle: [NSString stringWithCString: openInfo.acceptButtonText.data() encoding: NSASCIIStringEncoding]];
        if (!openInfo.declineButtonText.empty()) [alert addButtonWithTitle: [NSString stringWithCString: openInfo.declineButtonText.data() encoding: NSASCIIStringEncoding]];

        switch (openInfo.severity)
        {
            case AlertSeverity::Info:    { [alert setAlertStyle: NSAlertStyleInformational]; break; }
            case AlertSeverity::Warning: { [alert setAlertStyle: NSAlertStyleWarning]; break; }
            case AlertSeverity::Error:   { [alert setAlertStyle: NSAlertStyleCritical]; break; }
        }

        return [alert runModal] == NSAlertFirstButtonReturn;
    }

    std::vector<std::filesystem::path> macOSContext::OpenFileSelectDialog(const FileSelectDialogOpenInfo& openInfo) const noexcept
    {
        NSOpenPanel* const panel = [NSOpenPanel openPanel];

        if (openInfo.directoryPath != nullptr)
        {
            const std::string path = openInfo.directoryPath->string();
            [panel setDirectoryURL: [NSURL fileURLWithPath: [NSString stringWithCString: path.c_str() encoding: NSASCIIStringEncoding]]];
        }

        if (!openInfo.message.empty()) [panel setMessage: [NSString stringWithCString: openInfo.message.data() encoding: NSASCIIStringEncoding]];
        if (!openInfo.buttonText.empty()) [panel setPrompt: [NSString stringWithCString: openInfo.buttonText.data() encoding: NSASCIIStringEncoding]];

        [panel setCanChooseFiles: openInfo.allowFiles];
        [panel setCanChooseDirectories: openInfo.allowDirectories];
        [panel setAllowsMultipleSelection: openInfo.allowMultipleSelection];

        NSMutableArray<NSString*>* const allowedFileExtensions = [NSMutableArray<NSString*> arrayWithCapacity: openInfo.allowedFileExtensions.size()];
        if (!openInfo.allowedFileExtensions.empty())
        {
            for (const std::string_view path : openInfo.allowedFileExtensions)
            {
                if (path.empty() || path.size() == 1) continue;

                [allowedFileExtensions addObject: [NSString stringWithCString: path.data() + 1 encoding: NSASCIIStringEncoding]];
            }

            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wdeprecated-declarations"
            [panel setAllowedFileTypes: allowedFileExtensions];
            #pragma clang diagnostic pop
        }

        std::vector<std::filesystem::path> filePaths = { };
        if ([panel runModal] == NSModalResponseOK)
        {
            filePaths.reserve(panel.URLs.count);
            for (NSURL* const URL in panel.URLs)
            {
                const NSString* path = [URL.path stringByResolvingSymlinksInPath];
                filePaths.emplace_back(std::string_view(path.UTF8String, path.length));
            }
        }

        return filePaths;
    }

    std::optional<std::filesystem::path> macOSContext::OpenFileSaveDialog(const FileSaveDialogOpenInfo& openInfo) const noexcept
    {
        NSSavePanel* const panel = [NSSavePanel savePanel];

        if (openInfo.directoryPath != nullptr)
        {
            const std::string path = openInfo.directoryPath->string();
            [panel setDirectoryURL: [NSURL fileURLWithPath: [NSString stringWithCString: path.c_str() encoding: NSASCIIStringEncoding]]];
        }

        if (!openInfo.message.empty()) [panel setMessage: [NSString stringWithCString: openInfo.message.data() encoding: NSASCIIStringEncoding]];
        if (!openInfo.buttonText.empty()) [panel setPrompt: [NSString stringWithCString: openInfo.buttonText.data() encoding: NSASCIIStringEncoding]];

        if (!openInfo.fileName.empty()) [panel setNameFieldStringValue: [NSString stringWithCString: openInfo.fileName.data() encoding: NSASCIIStringEncoding]];

        [panel setExtensionHidden: NO];
        [panel setAllowsOtherFileTypes: YES];
        [panel setAccessibilityExpanded: YES];

        NSMutableArray<NSString*>* const allowedFileExtensions = [NSMutableArray<NSString*> arrayWithCapacity: openInfo.allowedFileExtensions.size()];
        if (!openInfo.allowedFileExtensions.empty())
        {
            for (const std::string_view path : openInfo.allowedFileExtensions)
            {
                if (path.empty() || path.size() == 1) continue;

                [allowedFileExtensions addObject: [NSString stringWithCString: path.data() + 1 encoding: NSASCIIStringEncoding]];
            }

            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wdeprecated-declarations"
                [panel setAllowedFileTypes: allowedFileExtensions];
            #pragma clang diagnostic pop
        }

        if ([panel runModal] == NSModalResponseOK)
        {
            const NSString* path = [panel.URL.path stringByResolvingSymlinksInPath];
            return { std::string_view(path.UTF8String, path.length) };
        }

        return std::nullopt;
    }

    /* --- GETTER METHODS --- */

    std::string_view macOSContext::GetUserName() const noexcept
    {
        return std::string_view(NSUserName().UTF8String, NSUserName().length);
    }

    Screen& macOSContext::GetWindowScreen(const Window& window)
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::Cocoa, UnexpectedTypeError(SR_FORMAT("Cannot get screen of window [{0}], as its windowing backend differs from [WindowingBackendType::Cocoa]", window.GetTitle())));
        const CocoaWindow& cocoaWindow = static_cast<const CocoaWindow&>(window);

        return cocoaContext.GetWindowScreen(cocoaWindow.GetNSWindow());
    }

    void macOSContext::EnumerateScreens(const ScreenEnumerationPredicate& Predicate)
    {
        for (CocoaScreen& screen : cocoaContext.GetScreens())
        {
            Predicate(screen);
        }
    }

    /* --- POLLING METHODS --- */

    void macOSContext::Update()
    {
        cocoaContext.Update();
    }

}