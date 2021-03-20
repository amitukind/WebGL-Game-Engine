/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Reflection Utility
*/

namespace Reflection
{
    template<typename ReflectedType>
    constexpr StaticTypeInfo<ReflectedType> StaticType()
    {
        return {};
    }

    template<typename ReflectedType>
    constexpr StaticTypeInfo<ReflectedType> StaticType(const ReflectedType& instance)
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
        return StaticTypeInfo<ReflectedType>::Reflected;
    }

    template<typename ReflectedType>
    constexpr bool IsReflected(const ReflectedType& type)
    {
        return StaticTypeInfo<ReflectedType>::Reflected;
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
        return StaticTypeInfo<ReflectedType>::Identifier;
    }

    template<typename ReflectedType>
    IdentifierType GetIdentifier(const ReflectedType& type)
    {
        return StaticTypeInfo<ReflectedType>::Identifier;
    }
}

#define REFLECTION_IDENTIFIER(Type) \
    Reflection::StaticType<Type>().Identifier
