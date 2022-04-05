#include <array>
#include <functional>
#include <iostream>

#include <docopt/docopt.h>
#include <entt/entt.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <spdlog/spdlog.h>

#include <internal_use_only/config.hpp>

namespace coulomb_golf {
namespace {

  constexpr auto USAGE =
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
      fmt::format("{} {}", cmake::project_name, cmake::project_version));
  }

  using Delta = std::chrono::duration<float>;

  struct Sprite
  {
    std::string text{};
    ftxui::Color color{};
  };

  //  template <class T>
  //  struct Vector2d {
  //    float x{};
  //    float y{};
  //
  //    Vector2d(float x_, float y_)
  //      : x{x_}, y{y_}
  //    {}
  //
  //    Vector2d(const Vector2d&) = default;
  //    Vector2d(Vector2d&&) noexcept = default;
  //    Vector2d& operator=(const Vector2d&) = default;
  //    Vector2d& operator=(Vector2d&&) noexcept = default;
  //
  //  protected:
  //    ~Vector2d() = default;
  //  };


  struct Position
  {
    float x{};
    float y{};
  };

  struct Velocity
  {
    float x{};
    float y{};
  };

  struct Charge
  {
    float value;
  };

  void add_entities(entt::registry &registry)
  {
    const auto entity = registry.create();
    registry.emplace<Sprite>(entity, "Hello World!", ftxui::Color::Green);
    registry.emplace<Position>(entity, 10.0F, 10.0F);

    const auto entity2 = registry.create();
    registry.emplace<Sprite>(entity2, "⊝", ftxui::Color::Green);
    registry.emplace<Position>(entity2, 0.0F, 0.0F);
  }

  void test_move_system(entt::registry &registry, const Delta delta)
  {

    for (auto [entity, position] : registry.view<Position>().each()) {
      position.x += 2.0F * delta.count();
      position.y += 2.0F * delta.count();
    }
  }

  void update(entt::registry &registry, const Delta delta) { test_move_system(registry, delta); }

  std::tuple<int, int> world_to_screen(const Position &position)
  {
    return {
      static_cast<int>(position.x * 1.0F),
      static_cast<int>(position.y * 1.0F),
    };
  }

  Position mouse_to_world(int mouse_x, int mouse_y)
  {
    return {
      .x = static_cast<float>(mouse_x) * 2.0F,
      .y = static_cast<float>(mouse_y) * 4.0F,
    };
  }

  void render_loop()
  {
    entt::registry registry;
    Delta delta{};
    auto last_update_time = std::chrono::steady_clock::now();

    add_entities(registry);

    auto canvas_renderer = ftxui::Renderer([&registry, &delta, &last_update_time] {
      // Update the game.
      auto update_time = std::chrono::steady_clock::now();
      delta = update_time - last_update_time;
      last_update_time = update_time;
      update(registry, delta);

      // Render the game's sprites.
      auto c = ftxui::Canvas(200, 200);
      for (auto [entity, sprite, position] : registry.view<const Sprite, const Position>().each()) {
        const auto [screen_x, screen_y] = world_to_screen(position);

        c.DrawText(screen_x, screen_y, sprite.text, sprite.color);
      }

      return canvas(std::move(c));
    });

    // This capture the last mouse position.
    auto canvas_renderer_with_events = CatchEvent(canvas_renderer, [&](ftxui::Event e) {
      if (e.is_mouse() && e.mouse().button == ftxui::Mouse::Left) {
        const auto position = mouse_to_world(e.mouse().x, e.mouse().y);
        const auto entity = registry.create();
        registry.emplace<Position>(entity, position);
        registry.emplace<Sprite>(entity, "⊝", ftxui::Color::Green);
        registry.emplace<Charge>(entity, -1.0F);
      }
      return false;
    });


    auto screen = ftxui::ScreenInteractive::Fullscreen();

    // This thread exists to make sure that the event queue has an event to process at regular intervals.
    std::atomic<bool> refresh_ui_continue = true;
    std::thread refresh_ui([&screen, &refresh_ui_continue] {
      while (refresh_ui_continue) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1.0s / 60.0);// NOLINT magic numbers
        screen.PostEvent(ftxui::Event::Custom);
      }
    });

    // Enter the screen's main loop.
    screen.Loop(canvas_renderer_with_events);
    refresh_ui_continue = false;
    refresh_ui.join();
  }

}// namespace
}// namespace coulomb_golf

int main(int argc, const char **argv)
{
  try {
    coulomb_golf::parse_args(argc, argv);
    coulomb_golf::render_loop();

  } catch (const std::exception &e) {
    fmt::print("Unhandled exception in main: {}", e.what());
  }
}
