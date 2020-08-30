/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>

/*
    Build

    Stores information generated and retrieved from the build system.
    Needs to be initialized before build information becomes available.

    Set of information is split between engine and game directories.
    Metadata such as version control information is retrieved from
    respective engine library or game executable project directories.
*/

namespace Build
{
    void Initialize();
    void PrintInfo();

    std::string GetEngineDir();
    std::string GetGameDir();

    std::string GetEngineChangeNumber();
    std::string GetEngineChangeHash();
    std::string GetEngineChangeDate();
    std::string GetEngineBranchName();

    std::string GetGameChangeNumber();
    std::string GetGameChangeHash();
    std::string GetGameChangeDate();
    std::string GetGameBranchName();
}
