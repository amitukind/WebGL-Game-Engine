/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Reflection/ReflectionDynamic.hpp>

/*
    Reflection Utility
*/

namespace Reflection
{
    template<typename ReflectedType>
    constexpr DecayedStaticTypeInfo<ReflectedType> StaticType()
    {
        return {};
    }

    template<typename ReflectedType>
    constexpr DecayedStaticTypeInfo<ReflectedType> StaticType(const ReflectedType& instance)
    {
        return {};
    }

    inline const DynamicTypeInfo& DynamicType(IdentifierType identifier)
    {
        return Detail::GetRegistry().LookupType(identifier);
    }

    template<typename RegisteredType>
    constexpr const DynamicTypeInfo& DynamicType()
    {
        return RegisteredType::GetTypeStorage().GetTypeInfo();
    }

    template<typename RegisteredType>
    constexpr const DynamicTypeInfo& DynamicType(const RegisteredType& instance)
    {
        return instance.GetTypeInfo();
    }

    template<typename ReflectedType>
    constexpr bool IsReflected()
    {
        return StaticType<ReflectedType>().Reflected;
    }

    template<typename ReflectedType>
    constexpr bool IsReflected(const ReflectedType& type)
    {
        return StaticType<ReflectedType>().Reflected;
    }

    inline bool IsRegistered(IdentifierType identifier)
    {
        return Detail::GetRegistry().LookupType(identifier).IsRegistered();
    }

    template<typename RegisteredType>
    constexpr bool IsRegistered()
    {
        return DynamicType<RegisteredType>().IsRegistered();
    }

    template<typename RegisteredType>
    constexpr bool IsRegistered(const RegisteredType& instance)
    {
        return DynamicType(instance).IsRegistered();
    }

    template<typename ReflectedType>
    IdentifierType GetIdentifier()
    {
        return StaticType<ReflectedType>().Identifier;
    }

    template<typename ReflectedType>
    IdentifierType GetIdentifier(const ReflectedType& type)
    {
        return StaticType<ReflectedType>().Identifier;
    }

    template<typename RegisteredType>
    std::unique_ptr<RegisteredType> Create()
    {
        return std::unique_ptr<RegisteredType>(reinterpret_cast<RegisteredType*>(
            RegisteredType::GetTypeStorage().GetTypeInfo().Instantiate()));
    }

    template<typename RegisteredType>
    std::unique_ptr<RegisteredType> Create(IdentifierType identifier)
    {
        const DynamicTypeInfo& typeInfo = Detail::GetRegistry().LookupType(identifier);
        if(typeInfo.IsType<RegisteredType>())
        {
            return std::unique_ptr<RegisteredType>(
                reinterpret_cast<RegisteredType*>(typeInfo.Instantiate()));
        }

        return nullptr;
    }

    template<typename TargetType, typename SourceType>
    TargetType* Cast(SourceType* instance)
    {
        if(instance->GetTypeInfo().IsType(StaticType<TargetType>().Identifier))
        {
            return reinterpret_cast<TargetType*>(instance);
        }

        return nullptr;
    }

    template<typename TargetType, typename SourceType>
    std::unique_ptr<TargetType> Cast(std::unique_ptr<SourceType>& instance)
    {
        if(instance->GetTypeInfo().IsType(StaticType<TargetType>().Identifier))
        {
            return std::unique_ptr<TargetType>(reinterpret_cast<TargetType*>(instance.release()));
        }

        return nullptr;
    }
}

#define REFLECTION_IDENTIFIER(Type) \
    Reflection::StaticType<Type>().Identifier
