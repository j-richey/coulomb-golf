find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(docopt CONFIG)
find_package(EnTT CONFIG)

# Generic test that uses conan libs
add_executable(coulomb-golf main.cpp)
target_link_libraries(
  coulomb-golf
  PRIVATE project_options
          project_warnings
          docopt::docopt
          fmt::fmt
          spdlog::spdlog
          EnTT::EnTT)

target_link_system_libraries(
  coulomb-golf
  PRIVATE
  ftxui::screen
  ftxui::dom
  ftxui::component)

target_include_directories(coulomb-golf PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
