//
// Created by Nikolay Kanchevski on 19.03.23.
//

#pragma once

namespace Sierra::Engine::Classes
{

    template<typename T = uint32, ENABLE_IF(std::is_same_v<T, uint8> || std::is_same_v<T, uint16> || std::is_same_v<T, uint32> || std::is_same_v<T, uint64>)>
    class IdentifierPool
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline IdentifierPool(const T capacity = std::numeric_limits<T>::max())
            : maxIDs(capacity)
        {

        }

        /* --- SETTER METHODS --- */
        void RemoveID(const T ID)
        {
            totalIDs--;
            freedIDs.push_back(ID);
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsFull() const
        {
            return totalIDs >= maxIDs;
        }

        [[nodiscard]] inline T GetCapacity() const
        {
            return maxIDs;
        }

        [[nodiscard]] inline T GetTotalIDsCount() const
        {
            return totalIDs;
        }
        
        [[nodiscard]] inline T CreateNewID()
        {
            // Check if full
            if (totalIDs >= maxIDs)
            {
                ASSERT_ERROR_FORMATTED("Could not allocate new ID from an ID which already has all [{0}] slots full", maxIDs);
            }

            // Pick new ID
            T newID;
            if (freedIDs.empty())
            {
                newID = totalIDs;
                totalIDs++;
            }
            else
            {
                newID = freedIDs[0];
                freedIDs.erase(freedIDs.begin());
            }

            return newID;
        }

        /* --- DESTRUCTOR --- */
        ~IdentifierPool() = default;
        DELETE_COPY(IdentifierPool);

    private:
        T maxIDs;
        T totalIDs = 0;
        std::vector<T> freedIDs;
    };

}
