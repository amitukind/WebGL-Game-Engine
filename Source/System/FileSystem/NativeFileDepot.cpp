/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/FileSystem/NativeFileDepot.hpp"
#include "System/FileSystem/NativeFileHandle.hpp"
using namespace System;

namespace
{
    const char* CreateError = "Failed to create native file depot! {}";
}

NativeFileDepot::NativeFileDepot() = default;
NativeFileDepot::~NativeFileDepot() = default;

NativeFileDepot::CreateResult NativeFileDepot::Create(fs::path fileDirectory)
{
    CHECK_ARGUMENT_OR_RETURN(!fileDirectory.empty(),
        Common::Failure(CreateErrors::EmptyDepotDirectoryArgument));

    // Ensure that directory exists.
    fileDirectory = fileDirectory.lexically_normal();
    if(!fs::is_directory(fileDirectory))
    {
        LOG_ERROR(CreateError, "Depot directory argument is not an existing directory.");
        return Common::Failure(CreateErrors::NonExistingDepotDirectory);
    }

    // Create class instance.
    auto instance = std::unique_ptr<NativeFileDepot>(new NativeFileDepot());
    instance->m_fileDirectory = fileDirectory;

    LOG_SUCCESS("Created native file depot for \"{}\" directory.", fileDirectory.generic_string());
    return Common::Success(std::move(instance));
}

FileDepot::OpenFileResult NativeFileDepot::OpenFile(const fs::path& depotPath,
    const fs::path& requestedPath, FileHandle::OpenFlags::Type openFlags)
{
    fs::path resolvedPath = m_fileDirectory / depotPath;
    return NativeFileHandle::Create(resolvedPath, requestedPath, openFlags);
}
