//
// Created by Nikolay Kanchevski on 9.02.23.
//

#pragma once

/* --- UTILITY --- */
template<typename T>
inline constexpr auto HashType(const T &value)
{
    return std::hash<T>{}(value);
}

/* --- SHORTCUTS --- */
#define ENABLE_IF(...) std::enable_if_t<__VA_ARGS__, bool> = true
#define DELETE_COPY(TYPE) TYPE(const TYPE &) = delete; TYPE &operator=(const TYPE &) = delete

/* --- GETTERS --- */
#define ARRAY_SIZE(ARRAY) ((uSize)(sizeof(ARRAY) / sizeof(*(ARRAY))))
#define GET_HASH(VALUE) HashType(VALUE)
#define GET_UINT_PTR(VALUE) reinterpret_cast<std::uintptr_t>(&VALUE)
#define IS_FLAG_PRESENT(VALUE, FLAG) (((uint) VALUE & (uint) FLAG) != 0)
#define FORMAT_STRING(VALUE, ...) fmt::format(VALUE, __VA_ARGS__)
#define VK_TO_STRING(VALUE, TYPE) vk::to_string((vk::TYPE) VALUE)

/* --- SETTERS --- */
#define MODIFY_CONST(TYPE, NAME, VALUE) \
    TYPE *NAME##Pointer;                \
    NAME##Pointer = (TYPE*)(&NAME);     \
    *NAME##Pointer = VALUE;

/* --- PREPROCESSOR --- */
#define IS_CPP __cplusplus
#define THIS_LINE __LINE__
#define THIS_FILE __FILE__