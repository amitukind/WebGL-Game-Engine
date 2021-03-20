/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Reflection/Precompiled.hpp"
#include "Reflection/ReflectionDynamic.hpp"
#include "Reflection/ReflectionTypes.hpp"
#include "Reflection/ReflectionRegistry.hpp"
using namespace Reflection;

const DynamicTypeInfo DynamicTypeInfo::Invalid;

void DynamicTypeInfo::Register(std::string_view name,
    IdentifierType identifier, DynamicTypeInfo* baseType)
{
    m_registered = true;
    m_name = name;
    m_identifier = identifier;

    if(!IsNullType())
    {
        m_baseType = baseType;
        baseType->AddDerivedType(*this);
    }
    else
    {
        m_baseType = this;
    }
}

void DynamicTypeInfo::AddDerivedType(const DynamicTypeInfo& typeInfo)
{
    auto exiting = std::find_if(m_derivedTypes.begin(), m_derivedTypes.end(),
        [&typeInfo](const DynamicTypeList::value_type& derivedType)
    {
        return std::addressof(derivedType.get()) == std::addressof(typeInfo);
    });

    ASSERT(exiting == m_derivedTypes.end(), "Found existing entry in list of derived types!");
    m_derivedTypes.emplace_back(typeInfo);
}

bool DynamicTypeInfo::IsBaseOf(IdentifierType identifier) const
{
    const DynamicTypeInfo& typeInfo = Reflection::GetRegistry().LookupType(identifier);
    return typeInfo.IsDerivedFrom(m_identifier);
}

bool DynamicTypeInfo::IsDerivedFrom(IdentifierType identifier) const
{
    if(!m_registered)
        return false;

    const DynamicTypeInfo* baseType = m_baseType;
    while(!baseType->IsNullType())
    {
        if(baseType->GetIdentifier() == identifier)
        {
            return true;
        }

        baseType = &baseType->GetBaseType();
    }

    return false;
}
