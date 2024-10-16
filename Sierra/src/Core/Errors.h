//
// Created by Nikolay Kanchevski on 30.09.24.
//
#pragma once

namespace Sierra
{

    class SIERRA_API UnsupportedFeatureError final : public std::logic_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UnsupportedFeatureError(std::string_view message) noexcept;

        /* --- COPY SEMANTICS --- */
        UnsupportedFeatureError(const UnsupportedFeatureError&) = delete;
        UnsupportedFeatureError& operator=(const UnsupportedFeatureError&) = delete;

        /* --- MOVE SEMANTICS --- */
        UnsupportedFeatureError(UnsupportedFeatureError&&) = delete;
        UnsupportedFeatureError& operator=(UnsupportedFeatureError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~UnsupportedFeatureError() noexcept override = default;
    };

    class SIERRA_API UnexpectedTypeError final : public std::runtime_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UnexpectedTypeError(std::string_view message) noexcept;

        /* --- COPY SEMANTICS --- */
        UnexpectedTypeError(const UnexpectedTypeError&) = delete;
        UnexpectedTypeError& operator=(const UnexpectedTypeError&) = delete;

        /* --- MOVE SEMANTICS --- */
        UnexpectedTypeError(UnexpectedTypeError&&) = delete;
        UnexpectedTypeError& operator=(UnexpectedTypeError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~UnexpectedTypeError() noexcept override = default;
    };

    class SIERRA_API InvalidValueError final : public std::runtime_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit InvalidValueError(std::string_view message) noexcept;

        /* --- COPY SEMANTICS --- */
        InvalidValueError(const InvalidValueError&) = delete;
        InvalidValueError& operator=(const InvalidValueError&) = delete;

        /* --- MOVE SEMANTICS --- */
        InvalidValueError(InvalidValueError&&) = delete;
        InvalidValueError& operator=(InvalidValueError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~InvalidValueError() noexcept override = default;
    };

    class SIERRA_API InvalidConfigurationError final : public std::runtime_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit InvalidConfigurationError(std::string_view message) noexcept;

        /* --- COPY SEMANTICS --- */
        InvalidConfigurationError(const InvalidConfigurationError&) = delete;
        InvalidConfigurationError& operator=(const InvalidConfigurationError&) = delete;

        /* --- MOVE SEMANTICS --- */
        InvalidConfigurationError(InvalidConfigurationError&&) = delete;
        InvalidConfigurationError& operator=(InvalidConfigurationError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~InvalidConfigurationError() noexcept override = default;
    };

    class SIERRA_API InvalidOperationError final : public std::runtime_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit InvalidOperationError(std::string_view message) noexcept;

        /* --- COPY SEMANTICS --- */
        InvalidOperationError(const InvalidOperationError&) = delete;
        InvalidOperationError& operator=(const InvalidOperationError&) = delete;

        /* --- MOVE SEMANTICS --- */
        InvalidOperationError(InvalidOperationError&&) = delete;
        InvalidOperationError& operator=(InvalidOperationError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~InvalidOperationError() noexcept override = default;
    };

    class SIERRA_API UnexpectedSizeError final : public std::runtime_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UnexpectedSizeError(const std::string_view message, const size actualSize, const size expectedSize) noexcept;

        /* --- COPY SEMANTICS --- */
        UnexpectedSizeError(const UnexpectedSizeError&) = delete;
        UnexpectedSizeError& operator=(const UnexpectedSizeError&) = delete;

        /* --- MOVE SEMANTICS --- */
        UnexpectedSizeError(UnexpectedSizeError&&) = delete;
        UnexpectedSizeError& operator=(UnexpectedSizeError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~UnexpectedSizeError() noexcept override = default;
    };

    class SIERRA_API ValueOutOfRangeError final : public std::out_of_range
    {
    public:
        /* --- CONSTRUCTORS --- */
        template<typename T> requires (std::is_arithmetic_v<T>)
        explicit ValueOutOfRangeError(const std::string_view message, const T value, const T minInclusive, const T maxInclusive) noexcept
            : std::out_of_range(SR_FORMAT("{0}! Error: Argument value [{1}] is outside valid range [{2}-{3}].", message, value, minInclusive, maxInclusive))
        {

        }

        /* --- COPY SEMANTICS --- */
        ValueOutOfRangeError(const ValueOutOfRangeError&) = delete;
        ValueOutOfRangeError& operator=(const ValueOutOfRangeError&) = delete;

        /* --- MOVE SEMANTICS --- */
        ValueOutOfRangeError(ValueOutOfRangeError&&) = delete;
        ValueOutOfRangeError& operator=(ValueOutOfRangeError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ValueOutOfRangeError() noexcept override = default;
    };

    class SIERRA_API InvalidRangeError final : public std::range_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        template<typename T> requires (std::is_arithmetic_v<T>)
        explicit InvalidRangeError(const std::string_view message, const T offset, const T size, const T minInclusive, const T maxInclusive) noexcept
            : std::range_error(SR_FORMAT("{0}! Error: Argument range [{1}-{2}] is outside valid range [{3}-{4}].", message, offset, offset + size, minInclusive, maxInclusive))
        {

        }

        /* --- COPY SEMANTICS --- */
        InvalidRangeError(const InvalidRangeError&) = delete;
        InvalidRangeError& operator=(const InvalidRangeError&) = delete;

        /* --- MOVE SEMANTICS --- */
        InvalidRangeError(InvalidRangeError&&) = delete;
        InvalidRangeError& operator=(InvalidRangeError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~InvalidRangeError() noexcept override = default;
    };

}