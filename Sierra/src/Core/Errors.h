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
        UnsupportedFeatureError(const UnsupportedFeatureError&) noexcept = default;
        UnsupportedFeatureError& operator=(const UnsupportedFeatureError&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        UnsupportedFeatureError(UnsupportedFeatureError&&) noexcept = default;
        UnsupportedFeatureError& operator=(UnsupportedFeatureError&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~UnsupportedFeatureError() noexcept override = default;
    };

    class SIERRA_API UnexpectedTypeError final : public std::runtime_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UnexpectedTypeError(std::string_view message) noexcept;

        /* --- COPY SEMANTICS --- */
        UnexpectedTypeError(const UnexpectedTypeError&) noexcept = default;
        UnexpectedTypeError& operator=(const UnexpectedTypeError&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        UnexpectedTypeError(UnexpectedTypeError&&) noexcept = default;
        UnexpectedTypeError& operator=(UnexpectedTypeError&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~UnexpectedTypeError() noexcept override = default;
    };

    class SIERRA_API InvalidValueError final : public std::runtime_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit InvalidValueError(std::string_view message) noexcept;

        /* --- COPY SEMANTICS --- */
        InvalidValueError(const InvalidValueError&) noexcept = default;
        InvalidValueError& operator=(const InvalidValueError&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        InvalidValueError(InvalidValueError&&) noexcept = default;
        InvalidValueError& operator=(InvalidValueError&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~InvalidValueError() noexcept override = default;
    };

    class SIERRA_API InvalidConfigurationError final : public std::runtime_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit InvalidConfigurationError(std::string_view message) noexcept;

        /* --- COPY SEMANTICS --- */
        InvalidConfigurationError(const InvalidConfigurationError&) noexcept = default;
        InvalidConfigurationError& operator=(const InvalidConfigurationError&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        InvalidConfigurationError(InvalidConfigurationError&&) noexcept = default;
        InvalidConfigurationError& operator=(InvalidConfigurationError&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~InvalidConfigurationError() noexcept override = default;
    };

    class SIERRA_API InvalidOperationError final : public std::runtime_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit InvalidOperationError(std::string_view message) noexcept;

        /* --- COPY SEMANTICS --- */
        InvalidOperationError(const InvalidOperationError&) noexcept = default;
        InvalidOperationError& operator=(const InvalidOperationError&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        InvalidOperationError(InvalidOperationError&&) noexcept = default;
        InvalidOperationError& operator=(InvalidOperationError&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~InvalidOperationError() noexcept override = default;
    };

    class SIERRA_API UnexpectedSizeError final : public std::runtime_error
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UnexpectedSizeError(const std::string_view message, const size actualSize, const size expectedSize) noexcept;

        /* --- COPY SEMANTICS --- */
        UnexpectedSizeError(const UnexpectedSizeError&) noexcept = default;
        UnexpectedSizeError& operator=(const UnexpectedSizeError&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        UnexpectedSizeError(UnexpectedSizeError&&) noexcept = default;
        UnexpectedSizeError& operator=(UnexpectedSizeError&&) noexcept = default;

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
        ValueOutOfRangeError(const ValueOutOfRangeError&) noexcept = default;
        ValueOutOfRangeError& operator=(const ValueOutOfRangeError&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        ValueOutOfRangeError(ValueOutOfRangeError&&) noexcept = default;
        ValueOutOfRangeError& operator=(ValueOutOfRangeError&&) noexcept = default;

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
        InvalidRangeError(const InvalidRangeError&) noexcept = default;
        InvalidRangeError& operator=(const InvalidRangeError&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        InvalidRangeError(InvalidRangeError&&) noexcept = default;
        InvalidRangeError& operator=(InvalidRangeError&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~InvalidRangeError() noexcept override = default;
    };

}