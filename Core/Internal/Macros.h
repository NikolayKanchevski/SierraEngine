//
// Created by Nikolay Kanchevski on 9.02.23.
//

#pragma once

#define DELETE_COPY(Type) Type(const Type &) = delete; Type &operator=(const Type &) = delete
#define FORWARD_DECLARE_COMPONENT(Type) namespace Sierra::Engine::Components { class Type; }; using Sierra::Engine::Components::Type

#define ENABLE_IF(...) std::enable_if_t<__VA_ARGS__, bool> = true