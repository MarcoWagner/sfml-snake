
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
  sf::Texture m_texture_snake_head;
  sf::Sprite m_shape_snake_head;
  sf::Texture m_texture_snake_body_straight;
  sf::Sprite m_shape_snake_body_straight;
  sf::Texture m_texture_snake_body_bend;
  sf::Sprite m_shape_snake_body_bend;
  sf::Texture m_texture_snake_tail;
  sf::Sprite m_shape_snake_tail;
  sf::Texture m_texture_food;
  sf::Sprite m_shape_food;

  void init_playfield() {
    m_rect_playfield.setSize(sf::Vector2f{tile_size * Game::playfield_width,
                                          tile_size * Game::playfield_height});
    m_rect_playfield.setPosition(playfield_margin, playfield_margin);
    m_rect_playfield.setOutlineThickness(2.0F);
    m_rect_playfield.setOutlineColor(sf::Color::White);
    m_rect_playfield.setFillColor(sf::Color::Transparent);

    Logger::warn(
        m_texture_snake_head.loadFromFile("../resource/snake_head.png"),
        "failed loading image, snake_head");
    m_shape_snake_head.setTexture(m_texture_snake_head);
    m_shape_snake_head.setOrigin(8.0F, 8.0F);

    Logger::warn(m_texture_snake_body_straight.loadFromFile(
                     "../resource/snake_body_straight.png"),
                 "failed loading image, snake_body_straight");
    m_shape_snake_body_straight.setTexture(m_texture_snake_body_straight);
    m_shape_snake_body_straight.setOrigin(8.0F, 8.0F);

    Logger::warn(m_texture_snake_body_bend.loadFromFile(
                     "../resource/snake_body_bend.png"),
                 "failed loading image, snake_body_bend");
    m_shape_snake_body_bend.setTexture(m_texture_snake_body_bend);
    m_shape_snake_body_bend.setOrigin(8.0F, 8.0F);

    Logger::warn(
        m_texture_snake_tail.loadFromFile("../resource/snake_tail.png"),
        "failed loading image, snake_tail");
    m_shape_snake_tail.setTexture(m_texture_snake_tail);
    m_shape_snake_tail.setOrigin(8.0F, 8.0F);

    Logger::warn(m_texture_food.loadFromFile("../resource/food.png"),
                 "failed loading image, food");
    m_shape_food.setTexture(m_texture_food);
  }

  sf::Sprite *get_snake_sprite(const SnakeTile &current_tile,
                               const SnakeTile *prev_tile) {
    sf::Sprite *sprite{};

    bool clockwise = false;

    switch (current_tile.part) {
    case SnakeTile::Part::Body:
      if (prev_tile && prev_tile->facing != current_tile.facing) {
        sprite = &m_shape_snake_body_bend;

        clockwise = (prev_tile->facing == SnakeTile::Direction::Right &&
                     current_tile.facing == SnakeTile::Direction::Down) ||
                    (prev_tile->facing == SnakeTile::Direction::Down &&
                     current_tile.facing == SnakeTile::Direction::Left) ||
                    (prev_tile->facing == SnakeTile::Direction::Left &&
                     current_tile.facing == SnakeTile::Direction::Up) ||
                    (prev_tile->facing == SnakeTile::Direction::Up &&
                     current_tile.facing == SnakeTile::Direction::Right);

      } else {
        sprite = &m_shape_snake_body_straight;
      }
      break;
    case SnakeTile::Part::Head:
      sprite = &m_shape_snake_head;
      break;
    case SnakeTile::Part::Tail:
      sprite = &m_shape_snake_tail;
      break;
    }

    // the "left bend" gets used as "right bend"
    const auto degree = clockwise ? 90.0F : 0.0F;

    switch (current_tile.facing) {
    case SnakeTile::Direction::Right:
      sprite->setRotation(0.0F + degree);
      break;
    case SnakeTile::Direction::Down:
      sprite->setRotation(90.0F + degree);
      break;
    case SnakeTile::Direction::Left:
      sprite->setRotation(180.0F + degree);
      break;
    case SnakeTile::Direction::Up:
      sprite->setRotation(270.0F + degree);
      break;
    }

    return sprite;
  }

  void draw_snake(const Snake &snake) {
    const SnakeTile *prev_tile{};
    for (const auto &tile : snake.tiles) {
      auto sprite = get_snake_sprite(tile, prev_tile);
      sprite->setPosition(
          sf::Vector2f{static_cast<float>(tile_size * tile.position.x +
                                          playfield_margin + tile_size / 2),
                       static_cast<float>(tile_size * tile.position.y +
                                          playfield_margin + tile_size / 2)});
      m_target->draw(*sprite);
      prev_tile = &tile;
    }
  }

  void draw_food(const FoodCollection &food) {
    for (const auto &f : food) {
      m_shape_food.setPosition(sf::Vector2f{
          static_cast<float>(tile_size * f.position.x + playfield_margin),
          static_cast<float>(tile_size * f.position.y + playfield_margin)});
      m_target->draw(m_shape_food);
    }
  }

  void draw_playfield() {
    const auto &game_state = m_game->get_state();

    m_target->draw(m_rect_playfield);
    draw_snake(game_state.player);
    draw_food(game_state.food);
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
