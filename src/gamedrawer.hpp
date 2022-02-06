
#pragma once

#include "fpscounter.hpp"
#include "game.hpp"
#include "logger.hpp"

#include <SFML/Graphics.hpp>

#include <string_view>

class GameDrawer {
  Game *m_game{};
  sf::RenderTarget *m_target{};

  static constexpr auto hud_margin = 5;
  static constexpr auto text_size = 16;
  static constexpr auto text_score = "Score: ";
  static constexpr auto text_fps = "FPS: ";
  sf::Font m_font;
  sf::Text m_text_score;
  sf::Text m_text_fps;
  sf::Text m_text_paused;
  sf::Text m_text_game_over;
  FPSCounter fpscounter;

  void init_hud() {
    Logger::warn(m_font.loadFromFile("../resource/FiraCode-Regular.ttf"),
                 "failed loading font");

    const auto target_size = m_target->getSize();

    m_text_score.setFont(m_font);
    m_text_score.setCharacterSize(text_size);
    m_text_score.setFillColor(sf::Color::White);
    m_text_score.setPosition(hud_margin,
                             target_size.y - text_size - hud_margin);

    m_text_fps.setFont(m_font);
    m_text_fps.setCharacterSize(text_size);
    m_text_fps.setFillColor(sf::Color::White);
    m_text_fps.setString(text_fps);
    m_text_fps.setPosition(target_size.x -
                               text_size * m_text_fps.getString().getSize() -
                               hud_margin,
                           hud_margin);

    m_text_paused.setFont(m_font);
    m_text_paused.setCharacterSize(128);
    m_text_paused.setFillColor(sf::Color::Blue);
    m_text_paused.setString("GAME PAUSED");

    m_text_game_over.setFont(m_font);
    m_text_game_over.setCharacterSize(128);
    m_text_game_over.setFillColor(sf::Color::Red);
    m_text_game_over.setString("GAME OVER");
  }

  void draw_hud() {
    const auto &game_stats = m_game->get_stats();

    m_text_score.setString(text_score + std::to_string(game_stats.score));
    m_text_fps.setString(text_fps + std::to_string(fpscounter.get()));

    m_target->draw(m_text_score);
    m_target->draw(m_text_fps);

    if (game_stats.game_over) {
      m_target->draw(m_text_game_over);
    } else if (game_stats.paused) {
      m_target->draw(m_text_paused);
    }

    fpscounter.tick();
  }

  static constexpr auto playfield_margin = 32;
  static constexpr auto tile_size = 16;
  sf::RectangleShape m_rect_playfield;
  sf::RectangleShape m_shape_playerHead;
  sf::RectangleShape m_shape_playerBody;
  sf::RectangleShape m_shape_playerTail;
  sf::RectangleShape m_shape_food;

  void init_playfield() {
    m_rect_playfield.setSize(sf::Vector2f{tile_size * Game::playfield_width,
                                          tile_size * Game::playfield_height});
    m_rect_playfield.setPosition(playfield_margin, playfield_margin);
    m_rect_playfield.setOutlineThickness(2.0F);
    m_rect_playfield.setOutlineColor(sf::Color::White);
    m_rect_playfield.setFillColor(sf::Color::Transparent);

    m_shape_playerHead.setSize(sf::Vector2f{tile_size, tile_size});
    m_shape_playerHead.setFillColor(sf::Color{0x9b, 0xad, 0xb7});

    m_shape_playerBody.setSize(sf::Vector2f{tile_size, tile_size});
    m_shape_playerBody.setFillColor(sf::Color{0x69, 0x6a, 0x6a});

    m_shape_playerTail.setSize(sf::Vector2f{tile_size, tile_size});
    m_shape_playerTail.setFillColor(sf::Color{0x59, 0x56, 0x52});

    m_shape_food.setSize(sf::Vector2f{tile_size, tile_size});
    m_shape_food.setFillColor(sf::Color{0xd9, 0x57, 0x63});
  }

  void draw_playfield() {
    m_target->draw(m_rect_playfield);

    const auto &game_state = m_game->get_state();

    for (const auto &tile : game_state.player.tiles) {
      sf::Shape *shape{};
      switch (tile.part) {
      case SnakeTile::Part::Body:
        shape = &m_shape_playerBody;
        break;
      case SnakeTile::Part::Head:
        shape = &m_shape_playerHead;
        break;
      case SnakeTile::Part::Tail:
        shape = &m_shape_playerTail;
        break;
      }
      shape->setPosition(sf::Vector2f{
          static_cast<float>(tile_size * tile.position.x + playfield_margin),
          static_cast<float>(tile_size * tile.position.y + playfield_margin)});
      m_target->draw(*shape);
    }

    for (const auto &food : game_state.food) {
      m_shape_food.setPosition(sf::Vector2f{
          static_cast<float>(tile_size * food.position.x + playfield_margin),
          static_cast<float>(tile_size * food.position.y + playfield_margin)});
      m_target->draw(m_shape_food);
    }
  }

public:
  GameDrawer(Game *game, sf::RenderTarget *target)
      : m_game(game), m_target(target) {
    init_hud();
    init_playfield();
  }

  void draw() {
    draw_playfield();
    draw_hud();
  }
};
