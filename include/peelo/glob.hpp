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
#include <filesystem>
#include <vector>

#include <glob.h>

namespace peelo
{
  template<class T = std::filesystem::path>
  bool
  glob(const std::string& pattern, std::vector<T>& output)
  {
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

    return true;
  }
}
