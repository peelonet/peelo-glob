/*
 * Copyright (c) 2023, peelo.net
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <cstring>
#include <cwchar>
#include <filesystem>
#include <vector>

#if defined(_WIN32)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#else
#  include <glob.h>
#endif

namespace peelo
{
  template<class T = std::filesystem::path>
  bool
  glob(const std::string& pattern, std::vector<T>& output)
  {
#if defined(_WIN32)
    WIN32_FIND_DATAW find_data;
    const auto parent = std::filesystem::path(pattern).parent_path();
    const std::wstring wpattern(std::begin(pattern), std::end(pattern));
    const auto handle = ::FindFirstFileW(wpattern.c_str(), &find_data);

    if (handle == INVALID_HANDLE_VALUE)
    {
      const auto error = ::GetLastError();

      return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    }
    do
    {
      if (::wcscmp(find_data.cFileName, L".") != 0
          && ::wcscmp(find_data.cFileName, L"..") != 0)
      {
        output.emplace_back(
          parent.empty() ? find_data.cFileName : parent / find_data.cFileName
        );
      }
    }
    while (::FindNextFileW(handle, &find_data) != 0);
    ::FindClose(handle);
#else
    ::glob_t result;
    int return_value;

    std::memset(static_cast<void*>(&result), 0, sizeof(result));
    return_value = ::glob(
      pattern.c_str(),
#if defined(GLOB_TILDE)
      GLOB_TILDE,
#else
      0,
#endif
      nullptr,
      &result
    );
    if (return_value != 0)
    {
      ::globfree(&result);

      return return_value == GLOB_NOMATCH;
    }
    for (std::size_t i = 0; i < result.gl_pathc; ++i)
    {
      output.emplace_back(result.gl_pathv[i]);
    }
    ::globfree(&result);
#endif

    return true;
  }
}
