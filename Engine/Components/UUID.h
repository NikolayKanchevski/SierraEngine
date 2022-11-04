//
// Created by Nikolay Kanchevski on 3.11.22.
//

#pragma once

#include <stddef.h>
#include <cstdint>
//#include <xhash>

namespace Sierra::Engine::Components
{

    class UUID
    {
    public:
        /* --- CONSTRUCTORS --- */
        UUID();
        UUID(uint64_t overrideGUID);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint64_t GetUUID() const { return this->uuid; }

        /* --- DESTRUCTOR --- */
        ~UUID();
        UUID(const UUID&) = default;

        operator uint64_t() const { return uuid; }

    private:
//        static std::random_device UUID_randomDevice;
//        static std::mt19937_64 UUID_engine(UUID_randomDevice());
//        static std::uniform_int_distribution<uint64_t> UUID_uniformDistribution;

        uint64_t  uuid;

    };

//    namespace std {
//        template <typename T> struct hash;
//        template<>
//        struct hash<UUID> {
//            size_t operator()(const UUID& uuid)const {
//                return (uint64_t)uuid;
//            }
//        };
//    }


}