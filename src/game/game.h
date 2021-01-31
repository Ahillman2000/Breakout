#pragma once
#include "Vector2.h"
#include <Engine/OGLGame.h>
#include <string>

#include "GameObject.h"

/**
 *  An OpenGL Game based on ASGE.
 */
class Breakout : public ASGE::OGLGame
{
 public:
  Breakout();
  ~Breakout() final;
  bool init() override;

  enum
  {
    brick_num = 20
  };

  GameObject paddle;
  GameObject ball;

  GameObject green_bricks[brick_num];
  GameObject purple_bricks[brick_num];
  GameObject yellow_bricks[brick_num];
  GameObject grey_bricks[brick_num];
  GameObject red_bricks[brick_num];

  GameObject gems[4];

 private:
  void keyHandler(ASGE::SharedEventData data);

  void clickHandler(ASGE::SharedEventData data);

  void setupResolution();

  bool initGameObjects();

  void initPaddle();

  void initBall();

  void collisionDetection();

  bool isOverlapping(ASGE::Sprite*, ASGE::Sprite*);

  void update(const ASGE::GameTime&) override;

  void renderMenuOptions();

  void render(const ASGE::GameTime&) override;

  int key_callback_id = -1;   /**< Key Input Callback ID. */
  int mouse_callback_id = -1; /**< Mouse Input Callback ID. */

  int menu_option = 0;

  bool in_menu = true;
  bool in_game_screen = false;
  bool in_pause_menu = false;
  bool game_over = false;
  bool win = false;

  float ball_velocity_x = 300;
  float ball_velocity_y = -300;

  bool serve = false;
  int lives_count = 3;
  int score = 0;
};