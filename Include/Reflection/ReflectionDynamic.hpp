/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Reflection/ReflectionDetail.hpp"

/*
    Reflection Dynamic

    Run-time reflection interface.
*/

namespace Reflection
{
    struct NullType;

    template<typename ReflectedType>
    struct StaticTypeInfo;

    template<typename ReflectedType>
    constexpr StaticTypeInfo<ReflectedType> StaticType();

    template<typename ReflectedType>
    IdentifierType GetIdentifier();

    class DynamicTypeInfo final
    {
    public:
        friend class Registry;
        static const DynamicTypeInfo Invalid;

        using DynamicTypeList = std::vector<std::reference_wrapper<const DynamicTypeInfo>>;
        
    public:
        DynamicTypeInfo() = default;
        ~DynamicTypeInfo() = default;

        bool IsRegistered() const
        {
            return m_registered;
        }

        bool IsNullType() const 
        {
            return m_registered && m_identifier == Reflection::GetIdentifier<NullType>();
        }

        bool HasBaseType() const
        {
            return m_registered && !m_baseType->IsNullType();
        }

        const std::string_view& GetName() const
        {
            return m_name;
        }

        IdentifierType GetIdentifier() const
        {
            return m_identifier;
        }

        const DynamicTypeInfo& GetBaseType() const
        {
            return *m_baseType;
        }

        const DynamicTypeList& GetDerivedTypes() const
        {
            return m_derivedTypes;
        }

        bool IsType(IdentifierType identifier) const
        {
            return m_registered && m_identifier == identifier;
        }

        template<typename OtherType>
        bool IsType() const
        {
            return IsType(StaticType<OtherType>().Identifier);
        }

        bool IsBaseOf(IdentifierType identifier) const;
        bool IsDerivedFrom(IdentifierType identifier) const;

        template<typename OtherType>
        bool IsBaseOf() const
        {
            return IsBaseOf(StaticType<OtherType>().Identifier);
        }

        template<typename OtherType>
        bool IsDerivedFrom() const
        {
            return IsDerivedFrom(StaticType<OtherType>().Identifier);
        }

    private:
        void Register(std::string_view name, IdentifierType identifier, DynamicTypeInfo* baseType);
        void AddDerivedType(const DynamicTypeInfo& typeInfo);

        bool m_registered = false;
        std::string_view m_name = "<UnregisteredType>";
        IdentifierType m_identifier = InvalidIdentifier;
        const DynamicTypeInfo* m_baseType = &Invalid;
        DynamicTypeList m_derivedTypes;
    };

    inline const DynamicTypeInfo& DynamicType(IdentifierType identifier)
    {
        return Detail::GetRegistry().LookupType(identifier);
    }

    template<typename RegisteredType>
    constexpr const DynamicTypeInfo& DynamicType()
    {
        return Detail::GetRegistry().LookupType(StaticType<RegisteredType>().Identifier);
    }

    template<typename RegisteredType>
    constexpr const DynamicTypeInfo& DynamicType(const RegisteredType& instance)
    {
        return Detail::GetRegistry().LookupType(StaticType<RegisteredType>().Identifier);
    }

    inline bool IsRegistered(IdentifierType identifier)
    {
        return Detail::GetRegistry().LookupType(identifier).IsRegistered();
    }

    template<typename RegisteredType>
    constexpr bool IsRegistered()
    {
        return Detail::GetRegistry().LookupType(
            StaticType<RegisteredType>().Identifier).IsRegistered();
    }

    template<typename RegisteredType>
    constexpr bool IsRegistered(const RegisteredType& instance)
    {
        return Detail::GetRegistry().LookupType(
            StaticType<RegisteredType>().Identifier).IsRegistered();
    }

    struct DynamicTypeStorage
    {
    public:
        const DynamicTypeInfo& GetTypeInfo() const
        {
            return DynamicType;
        }

    private:
        friend class Registry;
        DynamicTypeInfo DynamicType;
    };
}
