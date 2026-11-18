//
// Created by Nikolay Kanchevski on 3.09.23.
//

#include "macOSContext.h"

#include <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

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
        : windowManager({ }), fileManager({ .fileManager = [NSFileManager defaultManager] })
    {

    }

    /* --- POLLING METHODS --- */

    bool macOSContext::OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const noexcept
    {
        NSAlert* const alert = [[NSAlert alloc] init];

        [alert setMessageText: [NSString stringWithCString: openInfo.title.data() encoding: NSUTF8StringEncoding]];
        if (!openInfo.message.empty()) [alert setInformativeText: [NSString stringWithCString: openInfo.message.data() encoding: NSUTF8StringEncoding]];

        [alert addButtonWithTitle: [NSString stringWithCString: openInfo.acceptButton.text.data() encoding: NSUTF8StringEncoding]];
        if (!openInfo.rejectButton.text.empty()) [alert addButtonWithTitle: [NSString stringWithCString: openInfo.rejectButton.text.data() encoding: NSUTF8StringEncoding]];

        switch (openInfo.severity)
        {
            case AlertSeverity::Info:
            case AlertSeverity::Question:   { [alert setAlertStyle: NSAlertStyleInformational]; break; }
            case AlertSeverity::Warning:    { [alert setAlertStyle: NSAlertStyleWarning]; break; }
            case AlertSeverity::Error:      { [alert setAlertStyle: NSAlertStyleCritical]; break; }
        }

        const bool accepted = [alert runModal] == NSAlertFirstButtonReturn;
        [alert release];

        return accepted;
    }

    std::vector<Path> macOSContext::OpenPathSelectionDialog(const PathSelectionDialogOpenInfo& openInfo) const noexcept
    {
        NSOpenPanel* const panel = [NSOpenPanel openPanel];

        if (openInfo.directoryPath != nullptr)
        {
            [panel setDirectoryURL: [NSURL fileURLWithPath: [NSString stringWithCString: openInfo.directoryPath->GetString().data() encoding: NSUTF8StringEncoding]]];
        }

        if (!openInfo.message.empty()) [panel setMessage: [NSString stringWithCString: openInfo.message.data() encoding: NSUTF8StringEncoding]];
        if (!openInfo.buttonText.empty()) [panel setPrompt: [NSString stringWithCString: openInfo.buttonText.data() encoding: NSUTF8StringEncoding]];

        [panel setCanChooseFiles: openInfo.allowFiles];
        [panel setCanChooseDirectories: openInfo.allowDirectories];
        [panel setAllowsMultipleSelection: openInfo.allowMultiplePaths];

        if (NSArray<UTType*>* const allowedContentTypes = CreateAllowedContentTypes(openInfo.allowedFileExtensions))
        {
            [panel setAllowedContentTypes: allowedContentTypes];
        }

        std::vector<Path> filePaths = { };
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

    std::optional<Path> macOSContext::OpenPathSaveDialog(const PathSaveDialogOpenInfo& openInfo) const noexcept
    {
        NSSavePanel* const panel = [NSSavePanel savePanel];

        if (openInfo.directoryPath != nullptr)
        {
            [panel setDirectoryURL: [NSURL fileURLWithPath: [NSString stringWithCString: openInfo.directoryPath->GetString().data() encoding: NSUTF8StringEncoding]]];
        }

        if (!openInfo.message.empty()) [panel setMessage: [NSString stringWithCString: openInfo.message.data() encoding: NSUTF8StringEncoding]];
        if (!openInfo.buttonText.empty()) [panel setPrompt: [NSString stringWithCString: openInfo.buttonText.data() encoding: NSUTF8StringEncoding]];

        if (!openInfo.pathName.empty()) [panel setNameFieldStringValue: [NSString stringWithCString: openInfo.pathName.data() encoding: NSUTF8StringEncoding]];

        [panel setExtensionHidden: !openInfo.showExtension];
        [panel setAllowsOtherFileTypes: YES];
        [panel setAccessibilityExpanded: YES];

        if (NSArray<UTType*>* const allowedContentTypes = CreateAllowedContentTypes(openInfo.allowedFileExtensions))
        {
            [panel setAllowedContentTypes: allowedContentTypes];
        }

        if ([panel runModal] == NSModalResponseOK)
        {
            const NSString* const path = [panel.URL.path stringByResolvingSymlinksInPath];
            return Path(std::string(path.UTF8String));
        }

        return std::nullopt;
    }

    bool macOSContext::OpenPath(const Path& path) const noexcept
    {
        const std::string pathString = path;
        const bool result = [[NSWorkspace sharedWorkspace] openURL: [NSURL fileURLWithPath: [NSString stringWithCString: pathString.data() encoding: NSUTF8StringEncoding]]];
        return result;
    }

    /* --- GETTER METHODS --- */

    std::string macOSContext::GetUserName() const noexcept
    {
        const NSString* const userName = NSUserName();
        return userName != nil ? std::string(userName.UTF8String) : "Unknown User";
    }

}
