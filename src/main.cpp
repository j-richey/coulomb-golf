#include <array>
#include <functional>
#include <iostream>

#include <docopt/docopt.h>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <spdlog/spdlog.h>

#include <internal_use_only/config.hpp>


static constexpr auto USAGE =
  R"(coulomb-golf

    Usage:
      coulomb-golf
      coulomb-golf (-h | --help)
      coulomb-golf --version

    Options:
      -h --help     Show this screen.
      --version     Show version.
)";

void parse_args(const int argc, const char **argv)
{
  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
    { std::next(argv), std::next(argv, argc) },
    true,
    fmt::format("{} {}", coulomb_golf::cmake::project_name, coulomb_golf::cmake::project_version));
}

int main(int argc, const char **argv)
{
  try {
    parse_args(argc, argv);

  } catch (const std::exception &e) {
    fmt::print("Unhandled exception in main: {}", e.what());
  }
}
