/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>
#include <filesystem>
#include "Common/Debug.hpp"

/*
    Utility
*/

namespace Common
{
    template<typename Type>
    constexpr Type Clamp(const Type& value, const Type& lower, const Type& upper)
    {
        return std::max(lower, std::min(value, upper));
    }

    template<typename Type, std::size_t Size>
    constexpr std::size_t StaticArraySize(const Type(&)[Size])
    {
        return Size;
    }

    template<typename Target, typename Source>
    constexpr Target NumericalCast(const Source& value)
    {
        /*
            Casts numerical types with assertion guarantee for data loss.
            Checks in debug if conversion will lead to any loss of data.
            This is useful when dealing with libraries which do not fully
            convert from 32bit to 64bit types on their own (e.g. OpenGL).
            Loss check is performed by converting to target type and then
            back to source type, after which results are compared.
        */

        ASSERT(static_cast<Source>(static_cast<Target>(value)) == value,
            "Numerical conversion failed due to data loss!");

        return static_cast<Target>(value);
    }

    template<typename Type>
    void ClearContainer(Type& container)
    {
        Type temp;
        container.swap(temp);
    }

    std::string GetTextFileContent(const std::filesystem::path path);
    std::vector<char> GetBinaryFileContent(const std::filesystem::path path);

    std::string StringLowerCase(const std::string text);
    std::vector<std::string> StringTokenize(const std::string text, char character = ' ');
    std::string StringTrimLeft(const std::string text, const char* characters = " ");
    std::string StringTrimRight(const std::string text, const char* characters = " ");
    std::string StringTrim(const std::string text, const char* characters = " ");

    uint32_t StringHash(const std::string string);
    uint32_t CalculateCRC32(uint32_t crc, const uint8_t* data, std::size_t size);

    template<typename Type>
    bool ReorderWithIndices(std::vector<Type>& elements, const std::vector<std::size_t>& order)
    {
        /*
            Reorders vector using an array of indices.
            This is useful in case we have two collections that need to be sorted in
            same way based on information from both. Sort can be performed on array of
            indices that then can be used to quickly rearrange elements in two collections.
            Result will not make sense if order indices are duplicated!
        */

        if(elements.size() != order.size())
            return false;

        for(std::size_t i : order)
        {
            if(i >= elements.size())
                return false;
        }

        if(elements.size() == 0)
            return true;

        std::vector<std::size_t> indices(order.size());
        std::iota(indices.begin(), indices.end(), 0);

        for(std::size_t i = 0; i < elements.size(); ++i)
        {
            std::size_t desiredPlacement = order[i];

            if(i != elements.size() - 1)
            {
                for(std::size_t j = i; j < elements.size(); ++j)
                {
                    if(desiredPlacement == indices[j])
                    {
                        std::swap(elements[i], elements[j]);
                        std::swap(indices[i], indices[j]);
                        break;
                    }
                }
            }

            ASSERT(indices[i] == order[i],
                "Elements will not be ordered correctly due to detected duplication of indices");
        }

        return true;
    }
}
