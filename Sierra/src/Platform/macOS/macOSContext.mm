//
// Created by Nikolay Kanchevski on 3.09.23.
//

#include "macOSContext.h"

#include <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

#include "../../Windowing/macOS/CocoaWindow.h"

namespace Sierra
{

    namespace
    {
        NSArray<UTType*>* CreateAllowedContentTypes(const std::span<const std::string_view> allowedFileExtensions)
        {
            NSMutableArray<UTType*>* const contentTypes = [NSMutableArray<UTType*> arrayWithCapacity: allowedFileExtensions.size()];
            for (const std::string_view extension : allowedFileExtensions)
            {
                if (extension.size() <= 1) continue;

                UTType* const contentType = [UTType typeWithFilenameExtension: [NSString stringWithCString: extension.substr(1).data() encoding: NSUTF8StringEncoding]];
                if (contentType != nil) [contentTypes addObject: contentType];
            }

            return [contentTypes count] > 0 ? contentTypes : nil;
        }
    }

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

        [alert setMessageText: [NSString stringWithCString: openInfo.title.data() encoding: NSUTF8StringEncoding]];
        if (!openInfo.message.empty()) [alert setInformativeText: [NSString stringWithCString: openInfo.message.data() encoding: NSUTF8StringEncoding]];

        [alert addButtonWithTitle: [NSString stringWithCString: openInfo.acceptButtonText.data() encoding: NSUTF8StringEncoding]];
        if (!openInfo.declineButtonText.empty()) [alert addButtonWithTitle: [NSString stringWithCString: openInfo.declineButtonText.data() encoding: NSUTF8StringEncoding]];

        switch (openInfo.severity)
        {
            case AlertSeverity::Info:    { [alert setAlertStyle: NSAlertStyleInformational]; break; }
            case AlertSeverity::Warning: { [alert setAlertStyle: NSAlertStyleWarning]; break; }
            case AlertSeverity::Error:   { [alert setAlertStyle: NSAlertStyleCritical]; break; }
        }

        const bool accepted = [alert runModal] == NSAlertFirstButtonReturn;
        [alert release];

        return accepted;
    }

    std::vector<std::filesystem::path> macOSContext::OpenFileSelectDialog(const FileSelectDialogOpenInfo& openInfo) const noexcept
    {
        NSOpenPanel* const panel = [NSOpenPanel openPanel];

        if (openInfo.directoryPath != nullptr)
        {
            [panel setDirectoryURL: [NSURL fileURLWithPath: [NSString stringWithCString: openInfo.directoryPath->string().data() encoding: NSUTF8StringEncoding]]];
        }

        if (!openInfo.message.empty()) [panel setMessage: [NSString stringWithCString: openInfo.message.data() encoding: NSUTF8StringEncoding]];
        if (!openInfo.buttonText.empty()) [panel setPrompt: [NSString stringWithCString: openInfo.buttonText.data() encoding: NSUTF8StringEncoding]];

        [panel setCanChooseFiles: openInfo.allowFiles];
        [panel setCanChooseDirectories: openInfo.allowDirectories];
        [panel setAllowsMultipleSelection: openInfo.allowMultipleSelection];

        if (NSArray<UTType*>* const allowedContentTypes = CreateAllowedContentTypes(openInfo.allowedFileExtensions))
        {
            [panel setAllowedContentTypes: allowedContentTypes];
        }

        std::vector<std::filesystem::path> filePaths = { };
        if ([panel runModal] == NSModalResponseOK)
        {
            filePaths.reserve(panel.URLs.count);
            for (NSURL* const URL in panel.URLs)
            {
                const NSString* const path = [URL.path stringByResolvingSymlinksInPath];
                filePaths.emplace_back(std::string(path.UTF8String));
            }
        }

        return filePaths;
    }

    std::optional<std::filesystem::path> macOSContext::OpenFileSaveDialog(const FileSaveDialogOpenInfo& openInfo) const noexcept
    {
        NSSavePanel* const panel = [NSSavePanel savePanel];

        if (openInfo.directoryPath != nullptr)
        {
            [panel setDirectoryURL: [NSURL fileURLWithPath: [NSString stringWithCString: openInfo.directoryPath->string().data() encoding: NSUTF8StringEncoding]]];
        }

        if (!openInfo.message.empty()) [panel setMessage: [NSString stringWithCString: openInfo.message.data() encoding: NSUTF8StringEncoding]];
        if (!openInfo.buttonText.empty()) [panel setPrompt: [NSString stringWithCString: openInfo.buttonText.data() encoding: NSUTF8StringEncoding]];

        if (!openInfo.fileName.empty()) [panel setNameFieldStringValue: [NSString stringWithCString: openInfo.fileName.data() encoding: NSUTF8StringEncoding]];

        [panel setExtensionHidden: NO];
        [panel setAllowsOtherFileTypes: YES];
        [panel setAccessibilityExpanded: YES];

        if (NSArray<UTType*>* const allowedContentTypes = CreateAllowedContentTypes(openInfo.allowedFileExtensions))
        {
            [panel setAllowedContentTypes: allowedContentTypes];
        }

        if ([panel runModal] == NSModalResponseOK)
        {
            const NSString* const path = [panel.URL.path stringByResolvingSymlinksInPath];
            return std::filesystem::path(std::string(path.UTF8String));
        }

        return std::nullopt;
    }

    /* --- GETTER METHODS --- */

    std::string macOSContext::GetUserName() const noexcept
    {
        const NSString* const userName = NSUserName();
        return userName != nil ? std::string(userName.UTF8String) : "Unknown User";
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
