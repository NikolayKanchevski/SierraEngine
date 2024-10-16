//
// Created by Nikolay Kanchevski on 4.10.24.
//
#pragma once

namespace Sierra
{

    class SIERRA_API DeviceError : public std::runtime_error
    {
    public:
        /* --- COPY SEMANTICS --- */
        DeviceError(const DeviceError&) = delete;
        DeviceError& operator=(const DeviceError&) = delete;

        /* --- MOVE SEMANTICS --- */
        DeviceError(DeviceError&&) = delete;
        DeviceError& operator=(DeviceError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~DeviceError() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        DeviceError(std::string_view message, std::string_view error) noexcept;

    };

    class SIERRA_API UnknownDeviceError final : public DeviceError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UnknownDeviceError(std::string_view message) noexcept;

        /* --- COPY SEMANTICS --- */
        UnknownDeviceError(const UnknownDeviceError&) = delete;
        UnknownDeviceError& operator=(const UnknownDeviceError&) = delete;

        /* --- MOVE SEMANTICS --- */
        UnknownDeviceError(UnknownDeviceError&&) = delete;
        UnknownDeviceError& operator=(UnknownDeviceError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~UnknownDeviceError() noexcept override = default;
    };

    class SIERRA_API DeviceOutOfMemory : public DeviceError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit DeviceOutOfMemory(std::string_view message) noexcept;

        /* --- COPY SEMANTICS --- */
        DeviceOutOfMemory(const DeviceOutOfMemory&) = delete;
        DeviceOutOfMemory& operator=(const DeviceOutOfMemory&) = delete;

        /* --- MOVE SEMANTICS --- */
        DeviceOutOfMemory(DeviceOutOfMemory&&) = delete;
        DeviceOutOfMemory& operator=(DeviceOutOfMemory&&) = delete;

        /* --- DESTRUCTOR --- */
        ~DeviceOutOfMemory() noexcept override = default;
    };

}