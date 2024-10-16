//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

namespace Sierra
{

    class SIERRA_API PathError : public std::runtime_error
    {
    public:
        /* --- COPY SEMANTICS --- */
        PathError(const PathError&) = delete;
        PathError& operator=(const PathError&) = delete;

        /* --- MOVE SEMANTICS --- */
        PathError(PathError&&) = delete;
        PathError& operator=(PathError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~PathError() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        PathError(std::string_view message, std::string_view error) noexcept;

    };

    class SIERRA_API PathInvalidError final : public PathError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit PathInvalidError(std::string_view message, const std::filesystem::path& path) noexcept;

        /* --- COPY SEMANTICS --- */
        PathInvalidError(const PathInvalidError&) = delete;
        PathInvalidError& operator=(const PathInvalidError&) = delete;

        /* --- MOVE SEMANTICS --- */
        PathInvalidError(PathInvalidError&&) = delete;
        PathInvalidError& operator=(PathInvalidError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~PathInvalidError() noexcept override = default;
    };

    class SIERRA_API PathMissingError final : public PathError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit PathMissingError(std::string_view message, const std::filesystem::path& path) noexcept;

        /* --- COPY SEMANTICS --- */
        PathMissingError(const PathMissingError&) = delete;
        PathMissingError& operator=(const PathMissingError&) = delete;

        /* --- MOVE SEMANTICS --- */
        PathMissingError(PathMissingError&&) = delete;
        PathMissingError& operator=(PathMissingError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~PathMissingError() noexcept override = default;
    };

    class SIERRA_API PathAlreadyExistsError final : public PathError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit PathAlreadyExistsError(std::string_view message, const std::filesystem::path& path) noexcept;

        /* --- COPY SEMANTICS --- */
        PathAlreadyExistsError(const PathAlreadyExistsError&) = delete;
        PathAlreadyExistsError& operator=(const PathAlreadyExistsError&) = delete;

        /* --- MOVE SEMANTICS --- */
        PathAlreadyExistsError(PathAlreadyExistsError&&) = delete;
        PathAlreadyExistsError& operator=(PathAlreadyExistsError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~PathAlreadyExistsError() noexcept override = default;
    };
    
}