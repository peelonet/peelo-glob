# peelo-glob

![Build](https://github.com/peelonet/peelo-glob/workflows/Build/badge.svg)

[C++17] header only wrapper to [glob()] function.

Does not work on Windows.

## Example

```C++
#include <peelo/glob.hpp>
#include <iostream>

int
main(int argc, char** argv)
{
  std::vector<std::filesystem::path> results;

  // The functions returns `true` if the globbing was successful, `false`
  // otherwise.
  if (peelo::glob("*.cpp", results))
  {
    // The vector given as argument now contains `std::filesystem::path`
    // objects that are matches of the glob.
    for (const auto& path : results)
    {
      std::cout << path << std::endl;
    }
  }

  return 0;
}
```

[C++17]: https://en.cppreference.com/w/cpp/17
[glob()]: https://pubs.opengroup.org/onlinepubs/009695399/basedefs/glob.h.html
