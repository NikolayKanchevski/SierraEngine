//
// Created by Nikolay Kanchevski on 9.02.23.
//

#pragma once

/* --- UTILITY --- */
template<typename T>
inline Hash HashType(const T &value)
{
    return std::hash<T>{}(value);
}

template<typename T>
using IsPointer = std::is_pointer<T>;

template<typename T, typename Enable = void>
struct IsSmartPointer : std::false_type { };

template<typename T>
struct IsSmartPointer<T, typename std::enable_if_t<std::is_same<typename std::decay_t<T>, std::unique_ptr<typename std::decay_t<T>::element_type>>::value>> : std::true_type { };

template<typename T>
struct IsSmartPointer<T, typename std::enable_if_t<std::is_same<typename std::decay_t<T>, std::shared_ptr<typename std::decay_t<T>::element_type>>::value>> : std::true_type { };

template<typename T>
struct IsSmartPointer<T, typename std::enable_if_t<std::is_same<typename std::decay_t<T>, std::weak_ptr<typename std::decay_t<T>::element_type>>::value>> : std::true_type { };

/* --- SHORTCUTS --- */
#define PRINT(...) std::cout << __VA_ARGS__ << "\n"
#define ENABLE_IF(...) std::enable_if_t<__VA_ARGS__, bool> = true
#define DELETE_COPY(TYPE) TYPE(const TYPE &) = delete; TYPE &operator=(const TYPE &) = delete

/* --- GETTERS --- */
#define ARRAY_SIZE(ARRAY) ((uSize)(sizeof(ARRAY) / sizeof(*(ARRAY))))
#define GET_UINT_PTR(VALUE) reinterpret_cast<std::uintptr_t>(&VALUE)
#define IS_FLAG_PRESENT(VALUE, FLAG) (((uint) VALUE & (uint) FLAG) != 0)
#define FORMAT_STRING(VALUE, ...) fmt::format(VALUE, __VA_ARGS__)
#define VK_TO_STRING(VALUE, TYPE) vk::to_string((vk::TYPE) VALUE)

/* --- SETTERS --- */
#define MODIFY_CONST(TYPE, NAME, VALUE) \
    TYPE *NAME##Pointer;                \
    NAME##Pointer = (TYPE*)(&NAME);     \
    *NAME##Pointer = VALUE

/* --- GENERATORS --- */
#define DEFINE_ENUM_FLAG_OPERATORS(ENUM_TYPE) \
    inline ENUM_TYPE operator~ (ENUM_TYPE a) { return (ENUM_TYPE)~(int)a; } \
    inline ENUM_TYPE operator| (ENUM_TYPE a, ENUM_TYPE b) { return (ENUM_TYPE)((int)a | (int)b); } \
    inline ENUM_TYPE operator& (ENUM_TYPE a, ENUM_TYPE b) { return (ENUM_TYPE)((int)a & (int)b); } \
    inline ENUM_TYPE operator^ (ENUM_TYPE a, ENUM_TYPE b) { return (ENUM_TYPE)((int)a ^ (int)b); } \
    inline ENUM_TYPE& operator|= (ENUM_TYPE& a, ENUM_TYPE b) { return (ENUM_TYPE&)((int&)a |= (int)b); } \
    inline ENUM_TYPE& operator&= (ENUM_TYPE& a, ENUM_TYPE b) { return (ENUM_TYPE&)((int&)a &= (int)b); } \
    inline ENUM_TYPE& operator^= (ENUM_TYPE& a, ENUM_TYPE b) { return (ENUM_TYPE&)((int&)a ^= (int)b); }

/* --- PREPROCESSOR --- */
#define IS_CPP __cplusplus
#define THIS_LINE __LINE__
#define THIS_FILE __FILE__