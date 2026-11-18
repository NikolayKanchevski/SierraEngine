//
// Created by Nikolay Kanchevski on 15.11.26.
//

#include "Path.h"

namespace Sierra
{

    namespace
    {
        constexpr std::string_view FORBIDDEN_CHARACTERS = "/\\:*?\"<>|";

        bool IsHiddenName(const std::filesystem::path::string_type& name) noexcept
        {
            return name.size() > 1 && name[0] == '.' && !(name.size() == 2 && name[1] == '.');
        }
    }

    bool IsPathNameValid(const std::string_view name) noexcept
    {
        return !std::ranges::any_of(FORBIDDEN_CHARACTERS, [name](const char character) -> bool { return name.find(character) != std::string_view::npos; });
    }

    /* --- CONSTRUCTORS --- */

    Path::Path(const char* givenPath)
        : path(givenPath)
    {

    }

    Path::Path(const std::string& givenPath)
        : path(givenPath)
    {

    }

    Path::Path(const std::string_view givenPath)
        : path(givenPath)
    {

    }

    Path::Path(const std::filesystem::path& givenPath)
        : path(givenPath)
    {

    }

    Path::Path(std::filesystem::path&& givenPath) noexcept
        : path(std::move(givenPath))
    {

    }

    void Path::Clear() noexcept
    {
        path.clear();
    }

    /* --- SETTER METHODS --- */

    Path& Path::ChangeFileName(const std::string_view fileName)
    {
        path.replace_filename(fileName);
        return *this;
    }

    Path& Path::ChangeStem(const std::string_view stem)
    {
        std::filesystem::path fileName = stem;

        fileName += path.extension();
        path.replace_filename(fileName);

        return *this;
    }

    Path& Path::ChangeExtension(const std::string_view extension)
    {
        path.replace_extension(extension);
        return *this;
    }

    Path& Path::RemoveExtension()
    {
        return ChangeExtension({ });
    }

    /* --- GETTER METHODS --- */

    bool Path::HasParentPath() const
    {
        return path.has_parent_path() && path != "/";
    }

    bool Path::IsAncestor(const Sierra::Path& descendantPath) const noexcept
    {
        const auto [a, d] = std::ranges::mismatch(path, descendantPath.path);
        return a == path.end() && d != descendantPath.path.end();
    }

    bool Path::IsNeighbor(const Sierra::Path& neighborPath) const noexcept
    {
        return GetParentPath() == neighborPath.GetParentPath();
    }

    bool Path::IsHidden() const noexcept
    {
        return IsHiddenName(path.filename().native());
    }

    bool Path::IsWithinHiddenPath() const noexcept
    {
        return std::ranges::any_of(path.parent_path(), [](const std::filesystem::path& path) -> bool { return IsHiddenName(path.native()); });
    }

    std::string Path::GetString() const
    {
        return path.string();
    }

    std::string Path::GetStem() const
    {
        return path.stem().string();
    }

    std::string Path::GetFileName() const
    {
        return path.filename().string();
    }

    std::string Path::GetExtension() const
    {
        return path.extension().string();
    }

    Path Path::GetParentPath() const
    {
        return path.parent_path();
    }

    Path Path::GetNormalizedPath() const
    {
        return path.lexically_normal();
    }

    Path Path::GetRelativePath(const Path& basePath) const
    {
        return std::filesystem::relative(path, basePath);
    }

    /* --- OPERATORS --- */

    Path Path::operator/(const Path& other) const
    {
        return path / other.path;
    }

    Path& Path::operator/=(const Path& other)
    {
        path /= other.path;
        return *this;
    }

}
