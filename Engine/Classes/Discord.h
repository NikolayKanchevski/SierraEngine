//
// Created by Nikolay Kanchevski on 11.07.23.
//

#pragma once

#include "../../Core/Rendering/Vulkan/Abstractions/Texture.h"

namespace Sierra::Engine::Discord
{

    // Forward declare
    void Start(uint64 applicationID, const Callback &OnUserLoadedCallback);

    /* --- TYPES --- */
    class User
    {
    public:
        /* --- CONSTRUCTORS --- */
        User() = default;
        User(const discord::User &user);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsLoaded() const { return GetId() != 0; }
        [[nodiscard]] inline bool IsIconTextureLoaded() const { return iconTexture != nullptr; }
        [[nodiscard]] inline int64 GetId() const { return discordUser.GetId(); }
        [[nodiscard]] inline const char* GetUsername() const { return discordUser.GetUsername(); }
        [[nodiscard]] inline const char* GetDiscriminator() const { return discordUser.GetDiscriminator(); }
        [[nodiscard]] inline SharedPtr<Rendering::Texture>& GetIconTexture() { return iconTexture; }
        [[nodiscard]] inline discord::User& GetDiscordUser() { return discordUser; }

        /* --- OPERATORS --- */
        User& operator==(const User &other);
        friend void Discord::Start(uint64 applicationID, const Callback &OnUserLoadedCallback);

    private:
        discord::User discordUser{};
        SharedPtr<Rendering::Texture> iconTexture;

    };


    using ActivityType = discord::ActivityType;
    class Activity
    {
    public:
        /* --- CONSTRUCTORS --- */
        Activity() = default;
        Activity(const discord::Activity &activity);

        /* --- SETTER METHODS --- */
        Activity& SetType(ActivityType type);
        Activity& SetName(char const* name);
        Activity& SetState(char const* state);
        Activity& SetDetails(char const* details);
        Activity& SetLargeImage(char const* text);
        Activity& SetLargeImageHoverText(char const* text);
        Activity& SetSmallImage(char const* text);
        Activity& SetSmallImageHoverText(char const* text);
        void Clear(const Callback &OnActivityClearedCallback = [] { });

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline ActivityType GetType() const { return discordActivity.GetType(); }
        [[nodiscard]] inline char const* GetName() const { return discordActivity.GetName(); }
        [[nodiscard]] inline char const* GetState() const { return discordActivity.GetState(); }
        [[nodiscard]] inline char const* GetDetails() const { return discordActivity.GetDetails(); }
        [[nodiscard]] inline char const* GetLargeImage() const { return discordActivity.GetAssets().GetLargeImage(); }
        [[nodiscard]] inline char const* GetLargeImageHoverText() const { return discordActivity.GetAssets().GetLargeText(); }
        [[nodiscard]] inline char const* GetSmallImage() const { return discordActivity.GetAssets().GetSmallImage(); }
        [[nodiscard]] inline char const* GetSmallImageHoverText() const { return discordActivity.GetAssets().GetSmallText(); }
        [[nodiscard]] inline discord::Activity& GetDiscordActivity() { return discordActivity; }

        /* --- OPERATORS --- */
        Activity& operator==(const Activity &other);

    private:
        discord::Activity discordActivity;

    };

    /* --- POLLING METHODS --- */
    void Start(uint64 applicationID, const Callback &OnUserLoadedCallback = []{ });
    void Update();

    /* --- GETTER METHODS --- */
    [[nodiscard]] bool IsInitialized();
    [[nodiscard]] User& GetUser();
    [[nodiscard]] Activity& GetActivity();

    /* --- DESTRUCTOR --- */
    void Destroy();
}