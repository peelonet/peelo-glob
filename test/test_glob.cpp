#include <cassert>
#include <cstdlib>

#include <peelo/glob.hpp>

int
main(int argc, char** argv)
{
  std::vector<std::filesystem::path> result;

  assert(peelo::glob("no match", result));

  peelo::glob("M*file", result);
  assert(result.size() == 1);

  return EXIT_SUCCESS;
}
