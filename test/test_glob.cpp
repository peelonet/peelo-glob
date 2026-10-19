#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

#include <peelo/glob.hpp>

namespace fs = std::filesystem;

static void
touch(const fs::path& path)
{
  std::ofstream(path) << "x\n";
}

static bool
contains(const std::vector<fs::path>& paths, const fs::path& expected)
{
  const auto expected_generic = expected.generic_string();

  return std::any_of(
    paths.begin(),
    paths.end(),
    [&expected_generic](const fs::path& path)
    {
      return path.generic_string() == expected_generic;
    }
  );
}

int
main()
{
  const auto root = std::filesystem::temp_directory_path()
    / "peelo-glob-test";
  const auto previous = std::filesystem::current_path();

  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root / "subdir");

  touch(root / "Makefile");
  touch(root / "README.md");
  touch(root / "foo.txt");
  touch(root / "bar.txt");
  touch(root / "baz.dat");
  touch(root / "a.cpp");
  touch(root / "ab.cpp");
  touch(root / "subdir" / "nested.txt");

  std::filesystem::current_path(root);

  // No matches should succeed and leave the output empty.
  {
    std::vector<std::filesystem::path> result;

    assert(peelo::glob("no-such-file-xyz", result));
    assert(result.empty());
  }

  // Exact filename match.
  {
    std::vector<std::filesystem::path> result;

    assert(peelo::glob("README.md", result));
    assert(result.size() == 1);
    assert(contains(result, "README.md"));
  }

  // '*' matching a single known file.
  {
    std::vector<std::filesystem::path> result;

    assert(peelo::glob("M*file", result));
    assert(result.size() == 1);
    assert(contains(result, "Makefile"));
  }

  // '*' matching multiple files with the same extension.
  {
    std::vector<std::filesystem::path> result;

    assert(peelo::glob("*.txt", result));
    assert(result.size() == 2);
    assert(contains(result, "foo.txt"));
    assert(contains(result, "bar.txt"));
  }

  // '?' matching exactly one character.
  {
    std::vector<std::filesystem::path> result;

    assert(peelo::glob("?.cpp", result));
    assert(result.size() == 1);
    assert(contains(result, "a.cpp"));
  }

  // Results are appended; the output vector is not cleared.
  {
    std::vector<std::filesystem::path> result;

    assert(peelo::glob("foo.txt", result));
    assert(peelo::glob("bar.txt", result));
    assert(result.size() == 2);
    assert(contains(result, "foo.txt"));
    assert(contains(result, "bar.txt"));
  }

  // '*' matches every entry in the current directory.
  {
    std::vector<std::filesystem::path> result;

    assert(peelo::glob("*", result));
    assert(result.size() == 8);
    assert(contains(result, "Makefile"));
    assert(contains(result, "README.md"));
    assert(contains(result, "foo.txt"));
    assert(contains(result, "bar.txt"));
    assert(contains(result, "baz.dat"));
    assert(contains(result, "a.cpp"));
    assert(contains(result, "ab.cpp"));
    assert(contains(result, "subdir"));
  }

  // Globbing inside a subdirectory keeps the directory prefix.
  {
    std::vector<std::filesystem::path> result;

    assert(peelo::glob("subdir/*.txt", result));
    assert(result.size() == 1);
    assert(contains(result, "subdir/nested.txt"));
  }

  std::filesystem::current_path(previous);
  std::filesystem::remove_all(root);

  return EXIT_SUCCESS;
}
