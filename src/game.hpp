#pragma once

#include "intervalrunner.hpp"

#include <random>
#include <vector>

struct GameStats {
  int score{};
  bool game_over{};
  bool paused{};
};

struct Position {
  int x{};
  int y{};

  bool operator==(const Position &rhv) const {
    return x == rhv.x && y == rhv.y;
  }
};

struct SnakeTile {
  enum class Part { Head, Body, Tail };
  enum class Direction { Up, Right, Down, Left };

  Position position{0, 0};
  Part part{Part::Head};
  Direction facing{Direction::Right};
};

using SnakeTileCollection = std::vector<SnakeTile>;

struct Snake {
  int length{1}; // start length
  SnakeTileCollection tiles{};
};

struct Food {
  Position position{};
};
using FoodCollection = std::vector<Food>;

struct GameState {
  Snake player;
  FoodCollection food;
};

class Game : public IntervalRunner {
  GameStats m_stats{};
  GameState m_state{};

  std::random_device m_rd;
  std::uniform_int_distribution<int> m_food_dist{
      0, playfield_width *playfield_height - 1};

  void advance_head(Snake &snake) {
    auto head = snake.tiles.back();
    switch (head.facing) {
    case SnakeTile::Direction::Down:
      head.position.y += 1;
      if (head.position.y >= playfield_height)
        head.position.y = 0;
      break;
    case SnakeTile::Direction::Up:
      head.position.y -= 1;
      if (head.position.y < 0)
        head.position.y = playfield_height - 1;
      break;
    case SnakeTile::Direction::Right:
      head.position.x += 1;
      if (head.position.x >= playfield_width)
        head.position.x = 0;
      break;
    case SnakeTile::Direction::Left:
      head.position.x -= 1;
      if (head.position.x < 0)
        head.position.x = playfield_width - 1;
      break;
    }
    snake.tiles.push_back(std::move(head));
  }

  void advance_snake(Snake &snake) {
    advance_head(snake);

    // remove exces
    while (snake.tiles.size() > snake.length) {
      snake.tiles.erase(begin(snake.tiles));
    }

    // update parts
    if (snake.tiles.size() > 1) {
      snake.tiles.front().part = SnakeTile::Part::Tail;
      auto it_head = prev(end(snake.tiles));
      for (auto it = next(begin(snake.tiles)); it != it_head; ++it) {
        it->part = SnakeTile::Part::Body;
      }
    }
  }

  FoodCollection::const_iterator
  find_food_collision(const Position &what) const {
    return std::find_if(
        begin(m_state.food), end(m_state.food),
        [&what](const auto &food) { return food.position == what; });
  }

  void collect_food(Snake &snake, FoodCollection::const_iterator it) {
    ++snake.length;
    ++m_stats.score;
    m_state.food.erase(it);
  }

  bool has_snake_collision(const Position &what) const {
    return std::any_of(
        begin(m_state.player.tiles), end(m_state.player.tiles),
        [&what](const auto &tile) { return tile.position == what; });
  }

  bool has_deadly_collision(const Snake &snake) const {
    const auto &head = snake.tiles.back().position;
    auto body_end = prev(end(snake.tiles));
    return std::any_of(begin(snake.tiles), body_end, [&head](const auto &tile) {
      return tile.position == head;
    });
  }

  void insert_food() {
    const auto index = m_food_dist(m_rd);

    for (auto index_upwards = index;
         index_upwards < playfield_width * playfield_height; ++index_upwards) {
      const auto t = Position{index_upwards % playfield_width,
                              index_upwards / playfield_width};
      if (!has_snake_collision(t) &&
          find_food_collision(t) == end(m_state.food)) {
        m_state.food.push_back(Food{t});
        return;
      }
    }

    for (auto index_downwards = index; index_downwards >= 0;
         --index_downwards) {
      const auto t = Position{index_downwards % playfield_width,
                              index_downwards / playfield_width};
      if (!has_snake_collision(t) &&
          find_food_collision(t) == end(m_state.food)) {
        m_state.food.push_back(Food{t});
        return;
      }
    }

    // snake has reached max length
    if (m_state.food.empty()) {
      m_stats.game_over = true;
    }
  }

public:
  static constexpr auto playfield_width = 75;
  static constexpr auto playfield_height = 40;
  static constexpr auto initial_food_count = 20;

  Game() : IntervalRunner(FPS{12}) {
    auto snake = SnakeTile{};
    snake.position = Position{playfield_width / 2, playfield_height / 2};
    m_state.player.tiles.push_back(std::move(snake));
    for (auto i = 0; i < initial_food_count; ++i) {
      insert_food();
    }
  }

  bool isGameOver() const { return m_stats.game_over; }

  const GameStats &get_stats() const { return m_stats; }

  const GameState &get_state() const { return m_state; }

  void run_frame() override {
    if (m_stats.paused || m_stats.game_over) {
      return;
    }

    advance_snake(m_state.player);

    if (const auto it =
            find_food_collision(m_state.player.tiles.back().position);
        it != end(m_state.food)) {
      collect_food(m_state.player, it);
      insert_food();
    }

    if (has_deadly_collision(m_state.player)) {
      m_stats.game_over = true;
    }
  }

  void on_up() {
    if (m_state.player.tiles.back().facing != SnakeTile::Direction::Down) {
      m_state.player.tiles.back().facing = SnakeTile::Direction::Up;
    }
  }

  void on_right() {
    if (m_state.player.tiles.back().facing != SnakeTile::Direction::Left) {
      m_state.player.tiles.back().facing = SnakeTile::Direction::Right;
    }
  }

  void on_down() {
    if (m_state.player.tiles.back().facing != SnakeTile::Direction::Up) {
      m_state.player.tiles.back().facing = SnakeTile::Direction::Down;
    }
  }

  void on_left() {
    if (m_state.player.tiles.back().facing != SnakeTile::Direction::Right) {
      m_state.player.tiles.back().facing = SnakeTile::Direction::Left;
    }
  }

  void on_pause() { m_stats.paused = !m_stats.paused; }
};
