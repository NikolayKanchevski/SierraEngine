//
// Created by Nikolay Kanchevski on 15.11.26.
//

#pragma once

namespace Sierra
{

    [[nodiscard]] SIERRA_API bool IsPathNameValid(std::string_view name) noexcept;

    enum class PathResult : uint8
    {
        Unknown,
        Success,
        NotFound,
        AlreadyExists,
        PermissionDenied,
        ReadOnly,
        Corrupted,
        Locked,
        SizeExceeded
    };

    class SIERRA_API Path final
    {
    public:
        /* --- CONSTRUCTORS --- */
        Path() noexcept = default;
        Path(const char* path);
        Path(const std::string& path);
        Path(std::string_view path);
        Path(const std::filesystem::path& path);
        Path(std::filesystem::path&& path) noexcept;

        /* --- POLLING METHODS --- */
        Path& ChangeFileName(std::string_view fileName);
        Path& ChangeStem(std::string_view stem);
        Path& ChangeExtension(std::string_view extension);
        Path& RemoveExtension();
        void Clear() noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsEmpty() const noexcept { return path.empty(); }
        [[nodiscard]] bool IsAbsolute() const { return path.is_absolute(); }
        [[nodiscard]] bool IsRelative() const { return path.is_relative(); }

        [[nodiscard]] bool HasParentPath() const;
        [[nodiscard]] bool HasFileName() const { return path.has_filename(); }
        [[nodiscard]] bool HasExtension() const { return path.has_extension(); }

        [[nodiscard]] bool IsAncestor(const Path& descendantPath) const noexcept;
        [[nodiscard]] bool IsNeighbor(const Path& neighborPath) const noexcept;

        [[nodiscard]] bool IsHidden() const noexcept;
        [[nodiscard]] bool IsWithinHiddenPath() const noexcept;

        [[nodiscard]] std::string GetString() const;
        [[nodiscard]] std::string GetStem() const;
        [[nodiscard]] std::string GetFileName() const;
        [[nodiscard]] std::string GetExtension() const;

        [[nodiscard]] Path GetParentPath() const;
        [[nodiscard]] Path GetNormalizedPath() const;

        [[nodiscard]] Path GetRelativePath(const Path& basePath) const;
        [[nodiscard]] const std::filesystem::path& GetStandardPath() const noexcept { return path; }

        /* --- ITERATORS --- */
        [[nodiscard]] std::filesystem::path::iterator begin() const { return path.begin(); }
        [[nodiscard]] std::filesystem::path::iterator end() const { return path.end(); }

        /* --- COPY SEMANTICS --- */
        Path(const Path&) = default;
        Path& operator=(const Path&) = default;

        /* --- MOVE SEMANTICS --- */
        Path(Path&&) noexcept = default;
        Path& operator=(Path&&) noexcept = default;

        /* --- OPERATORS --- */
        [[nodiscard]] operator std::string() const { return GetString(); }
        [[nodiscard]] operator std::filesystem::path() const { return path; }

        [[nodiscard]] Path operator/(const Path& other) const;
        Path& operator/=(const Path& other);

        [[nodiscard]] bool operator==(const Path& other) const noexcept { return path == other.path; }
        [[nodiscard]] bool operator!=(const Path& other) const noexcept { return !(*this == other); }

        /* --- DESTRUCTOR --- */
        ~Path() noexcept = default;

    private:
        std::filesystem::path path = { };

    };

}
