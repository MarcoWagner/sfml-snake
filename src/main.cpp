#include <cstdlib>
#include <exception>
#include <iostream>

#include "game.hpp"
#include "gamedrawer.hpp"

#include <SFML/Graphics.hpp>

class Application {
  sf::RenderWindow m_window;
  std::unique_ptr<Game> m_game;
  std::unique_ptr<GameDrawer> m_gamedrawer;

  sf::Event m_inputbuffer{};

  void inputs() {
    while (m_window.pollEvent(m_inputbuffer)) {
      switch (m_inputbuffer.type) {
      case sf::Event::KeyPressed:
        on_keypressed(m_inputbuffer.key.code);
        break;

      case sf::Event::Closed:
        quit();
        break;

      case sf::Event::Resized:
        on_resize(m_inputbuffer.size);
        break;

      default:
        break;
      }
    }
  }

  void on_resize(const sf::Event::SizeEvent size) {
    // TODO: use sf::View to keep aspectratio
  }

  void on_keypressed(const sf::Keyboard::Key key) {
    switch (key) {
    case sf::Keyboard::Escape:
      on_cancel();
      break;

    case sf::Keyboard::Up:
    case sf::Keyboard::W:
      on_up();
      break;

    case sf::Keyboard::Right:
    case sf::Keyboard::D:
      on_right();
      break;

    case sf::Keyboard::Down:
    case sf::Keyboard::S:
      on_down();
      break;

    case sf::Keyboard::Left:
    case sf::Keyboard::A:
      on_left();
      break;

    case sf::Keyboard::Pause:
    case sf::Keyboard::P:
      on_pause();
      break;

    case sf::Keyboard::Enter:
      on_enter();
      break;

    default:
      break;
    }
  }

  void on_up() { m_game->on_up(); }

  void on_right() { m_game->on_right(); }

  void on_down() { m_game->on_down(); }

  void on_left() { m_game->on_left(); }

  void on_cancel() { quit(); }

  void on_enter() { reset(); }

  void on_pause() { m_game->on_pause(); }

  void quit() { m_window.close(); }

  void tick() { m_game->tick(); }

  void draw() {
    m_window.clear();
    m_gamedrawer->draw();
    m_window.display();
  }

  void reset() {
    m_gamedrawer.reset();
    m_game = std::make_unique<Game>();
    m_gamedrawer = std::make_unique<GameDrawer>(m_game.get(), &m_window);
  }

public:
  Application() : m_window(sf::VideoMode{1280, 720}, "snake") {
    m_window.setFramerateLimit(120); // reduce energy waste
    reset();
  }

  void run() {
    while (m_window.isOpen()) {
      inputs();
      tick();
      draw();
    }
  }
};

int main() try {
  auto app = Application{};
  app.run();
  return EXIT_SUCCESS;
} catch (const std::exception &e) {
  std::cerr << e.what() << "\n";
  return EXIT_FAILURE;
}
