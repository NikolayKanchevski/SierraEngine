//
// Created by Nikolay Kanchevski on 11.07.23.
//

#include "Discord.h"

namespace Sierra::Engine
{

    /* --- PROPERTIES --- */

    discord::Core* core = nullptr;
    Discord::User currentUser{};
    Discord::Activity currentActivity{};

    /* --- TYPES | USER --- */

    Discord::User::User(const discord::User &user)
        : discordUser(user)
    {

    }

    Discord::User &Discord::User::operator==(const Discord::User &other)
    {
        discordUser = other.discordUser;
        return *this;
    }

    /* --- TYPES | ACTIVITY --- */

    Discord::Activity::Activity(const discord::Activity &activity)
        : discordActivity(activity)
    {

    }

    Discord::Activity& Discord::Activity::SetType(const ActivityType type)
    {
        discordActivity.SetType(type);
        return *this;
    }

    Discord::Activity& Discord::Activity::SetName(const char* name)
    {
        discordActivity.SetName(name);
        return *this;
    }

    Discord::Activity& Discord::Activity::SetState(const char* state)
    {
        discordActivity.SetState(state);
        return *this;
    }

    Discord::Activity& Discord::Activity::SetDetails(const char* details)
    {
        discordActivity.SetDetails(details);
        return *this;
    }

    Discord::Activity& Discord::Activity::SetLargeImage(const char* text)
    {
        discordActivity.GetAssets().SetLargeImage(text);
        return *this;
    }

    Discord::Activity& Discord::Activity::SetLargeImageHoverText(const char* text)
    {
        discordActivity.GetAssets().SetLargeText(text);
        return *this;
    }

    Discord::Activity& Discord::Activity::SetSmallImage(const char* text)
    {
        discordActivity.GetAssets().SetSmallImage(text);
        return *this;
    }

    Discord::Activity& Discord::Activity::SetSmallImageHoverText(const char* text)
    {
        discordActivity.GetAssets().SetSmallText(text);
        return *this;
    }

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "readability-convert-member-functions-to-static"
    void Discord::Activity::Clear(const std::function<void()> &OnActivityClearedCallback)
    {
        core->ActivityManager().ClearActivity([](const discord::Result result){
            DC_VALIDATE(result, FORMAT_STRING("Clearing Discord activity failed with error code: {0}", static_cast<uint>(result)));
        });

        OnActivityClearedCallback();
    }
    #pragma clang diagnostic pop

    Discord::Activity &Discord::Activity::operator==(const Discord::Activity &other)
    {
        discordActivity = other.discordActivity;
        return *this;
    }

    /* --- POLLING METHODS --- */

    void Discord::Start(const uint64 applicationID, const Callback &OnUserLoadedCallback)
    {
        // Create Discord instance
        if (discord::Result result = discord::Core::Create(applicationID, DiscordCreateFlags_NoRequireDiscord, &core); result != discord::Result::Ok)
        {
            ASSERT_WARNING_FORMATTED("Could not create Discord instance! Error code: {0}", static_cast<uint>(result));
            return;
        }

        #if DEBUG
            // Set debug callback
            core->SetLogHook(discord::LogLevel::Warn, [&](discord::LogLevel log, const char* message) { ASSERT_WARNING_FORMATTED("Discord SDK failed: {0}", message); });
            core->SetLogHook(discord::LogLevel::Error, [&](discord::LogLevel log, const char* message) { ASSERT_ERROR_FORMATTED("Discord SDK failed: {0}", message); });
        #endif

        // Load local user and trigger callback
        core->UserManager().OnCurrentUserUpdate.Connect([OnUserLoadedCallback]
        {
            discord::User currentDiscordUser{};
            core->UserManager().GetCurrentUser(&currentDiscordUser);
            currentUser = Discord::User(currentDiscordUser);
            OnUserLoadedCallback();
        });

        // Create activity
        discord::Activity currentDiscordActivity{};
        currentDiscordActivity.SetApplicationId(applicationID);
        currentDiscordActivity.GetTimestamps().SetStart(time(nullptr));
        currentActivity = Discord::Activity(currentDiscordActivity);

        Update();
    }

    void Discord::Update()
    {
        if (!IsInitialized()) return;

        core->ActivityManager().UpdateActivity(currentActivity.GetDiscordActivity(), [](const discord::Result result){
            DC_VALIDATE(result, "Could not update Discord activity");
        });

        DC_VALIDATE(core->RunCallbacks(), "Running Discord callbacks failed");
    }

    /* --- GETTER METHODS --- */

    bool Discord::IsInitialized()
    {
        #if !defined(SR_DISCORD_APPLICATION_KEY) || SR_DISCORD_APPLICATION_KEY == 0
            return false;
        #endif
        return core != nullptr;
    }

    Discord::Activity& Discord::GetActivity()
    {
        ASSERT_WARNING_IF(!IsInitialized(), "Discord instance is not initialized and Discord::GetActivity() returns garbage data");
        return currentActivity;
    }

    Discord::User& Discord::GetUser()
    {
        ASSERT_WARNING_IF(!IsInitialized(), "Discord instance is not initialized and Discord::GetUser() returns garbage data");
        return currentUser;
    }

    /* --- DESTRUCTOR --- */

    void Discord::Destroy()
    {
        if (!IsInitialized()) return;
        currentActivity.Clear();
    }
}