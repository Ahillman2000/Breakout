#include <string>

#include <Engine/DebugPrinter.h>
#include <Engine/Input.h>
#include <Engine/InputEvents.h>
#include <Engine/Keys.h>
#include <Engine/Sprite.h>

#include "game.h"

/**
 *   @brief   Default Constructor.
 *   @details Consider setting the game's width and height
 *            and even seeding the random number generator.
 */
Breakout::Breakout()
{
  game_name = "BREAKOUT";
}

/**
 *   @brief   Destructor.
 *   @details Remove any non-managed memory and callbacks.
 */
Breakout::~Breakout()
{
  this->inputs->unregisterCallback(static_cast<unsigned int>(key_callback_id));

  this->inputs->unregisterCallback(
    static_cast<unsigned int>(mouse_callback_id));
}

/**
 *   @brief   Initialises the game.
 *   @details The game window is created and all assets required to
 *            run the game are loaded. The keyHandler and clickHandler
 *            callback should also be set in the initialise function.
 *   @return  True if the game initialised correctly.
 */
bool Breakout::init()
{
  setupResolution();
  if (!initAPI())
  {
    return false;
  }

  win = false;

  initGameObjects();

  toggleFPS();

  renderer->setClearColour(ASGE::COLOURS::BLACK);

  // input handling functions
  inputs->use_threads = false;

  key_callback_id =
    inputs->addCallbackFnc(ASGE::E_KEY, &Breakout::keyHandler, this);

  mouse_callback_id =
    inputs->addCallbackFnc(ASGE::E_MOUSE_CLICK, &Breakout::clickHandler, this);

  return true;
}

bool Breakout::initGameObjects()
{
  if (!paddle.initialiseSprite(renderer.get(), "paddleRed"))
  {
    return false;
  }

  if (!ball.initialiseSprite(renderer.get(), "ballBlue"))
  {
    return false;
  }

  for (int i = 0; i < brick_num; i++)
  {
    if (!green_bricks[i].initialiseSprite(renderer.get(),
                                          "element_green_rectangle"))
    {
      return false;
    }
    if (!purple_bricks[i].initialiseSprite(renderer.get(),
                                           "element_purple_rectangle"))
    {
      return false;
    }
    if (!yellow_bricks[i].initialiseSprite(renderer.get(),
                                           "element_yellow_rectangle"))
    {
      return false;
    }
    if (!grey_bricks[i].initialiseSprite(renderer.get(),
                                         "element_grey_rectangle"))
    {
      return false;
    }
    if (!red_bricks[i].initialiseSprite(renderer.get(),
                                        "element_red_rectangle"))
    {
      return false;
    }
  }

  for (int i = 0; i < 4; i++)
  {
    if (!gems[i].initialiseSprite(renderer.get(), "element_blue_polygon"))
    {
      return false;
    }

    gems[i].getSprite()->width(green_bricks[i].getSprite()->height());
    gems[i].getSprite()->height(green_bricks[i].getSprite()->height());
  }

  gems[0].getSprite()->xPos(145);
  gems[0].getSprite()->yPos(30);

  gems[1].getSprite()->xPos(465);
  gems[1].getSprite()->yPos(128);

  gems[2].getSprite()->xPos(720);
  gems[2].getSprite()->yPos(64);

  gems[3].getSprite()->xPos(912);
  gems[3].getSprite()->yPos(0);

  initPaddle();
  initBall();
}

void Breakout::initPaddle()
{
  paddle.getSprite()->xPos((game_width / 2.0) -
                           (paddle.getSprite()->width() / 2.0));
  paddle.getSprite()->yPos(game_height - 50.0);
}

void Breakout::initBall()
{
  ball.getSprite()->xPos(paddle.getSprite()->xPos() +
                         (paddle.getSprite()->width() / 2));
  ball.getSprite()->yPos(game_height - 70.0);

  ball_velocity_x = 300;
  ball_velocity_y = -300;
  ball.setVelocity(Vector2{ 0, 0 });
}

/**
 *   @brief   Sets the game window resolution
 *   @details This function is designed to create the window size, any
 *            aspect ratio scaling factors and safe zones to ensure the
 *            game frames when resolutions are changed in size.
 *   @return  void
 */
void Breakout::setupResolution()
{
  // how will you calculate the game's resolution?
  // will it scale correctly in full screen? what AR will you use?
  // how will the game be framed in native 16:9 resolutions?
  // here are some arbitrary values for you to adjust as you see fit
  // https://www.gamasutra.com/blogs/KenanBolukbasi/20171002/306822/
  // Scaling_and_MultiResolution_in_2D_Games.php

  // 720p is a pretty modest starting point, consider 1080p
  game_width = 1280;
  game_height = 720;
}

/**
 *   @brief   Processes any key inputs
 *   @details This function is added as a callback to handle the game's
 *            keyboard input. For this game, calls to this function
 *            are thread safe, so you may alter the game's state as
 *            you see fit.
 *   @param   data The event data relating to key input.
 *   @see     KeyEvent
 *   @return  void
 */

void Breakout::keyHandler(ASGE::SharedEventData data)
{
  auto key = static_cast<const ASGE::KeyEvent*>(data.get());

  if (key->key == ASGE::KEYS::KEY_ESCAPE)
  {
    signalExit();
  }

  if (in_menu)
  {
    if (key->key == ASGE::KEYS::KEY_LEFT &&
        key->action == ASGE::KEYS::KEY_RELEASED)
    {
      menu_option = 1 - menu_option;
      // ASGE::DebugPrinter{} << menu_option << std::endl;
    }
    else if (key->key == ASGE::KEYS::KEY_RIGHT &&
             key->action == ASGE::KEYS::KEY_RELEASED)
    {
      menu_option = 1 - menu_option;
      // ASGE::DebugPrinter{} << menu_option << std::endl;
    }

    if (key->key == ASGE::KEYS::KEY_ENTER)
    {
      if (menu_option == 1)
      {
        signalExit();
      }
      else
      {
        in_menu = false;
        in_game_screen = true;
      }
    }
  }

  if (in_game_screen)
  {
    if (key->key == ASGE::KEYS::KEY_P)
    {
      in_game_screen = false;
      in_pause_menu = true;
    }

    else if (key->key == ASGE::KEYS::KEY_A)
    {
      if (key->action == ASGE::KEYS::KEY_PRESSED)
      {
        // ASGE::DebugPrinter{} << "A button pressed" << std::endl;
        paddle.setVelocity(Vector2{ -450, 0 });
      }
      else if (key->action == ASGE::KEYS::KEY_RELEASED)
      {
        paddle.setVelocity(Vector2{ 0, 0 });
      }
    }

    else if (key->key == ASGE::KEYS::KEY_D)
    {
      if (key->action == ASGE::KEYS::KEY_PRESSED)
      {
        // ASGE::DebugPrinter{} << "D button pressed" << std::endl;
        paddle.setVelocity(Vector2{ 450, 0 });
      }
      else if (key->action == ASGE::KEYS::KEY_RELEASED)
      {
        paddle.setVelocity(Vector2{ 0, 0 });
      }
    }

    else if (key->key == ASGE::KEYS::KEY_SPACE &&
             key->action == ASGE::KEYS::KEY_PRESSED)
    {
      serve = true;
      // ASGE::DebugPrinter{} << "ball_velocity_y: " << ball_velocity_y <<
      // std::endl;
      ball.setVelocity(Vector2{ ball_velocity_x, ball_velocity_y });
    }
  }

  if (in_pause_menu)
  {
    if (key->key == ASGE::KEYS::KEY_LEFT &&
        key->action == ASGE::KEYS::KEY_RELEASED)
    {
      menu_option = 1 - menu_option;
    }
    else if (key->key == ASGE::KEYS::KEY_RIGHT &&
             key->action == ASGE::KEYS::KEY_RELEASED)
    {
      menu_option = 1 - menu_option;
    }

    if (key->key == ASGE::KEYS::KEY_ENTER)
    {
      if (menu_option == 1)
      {
        signalExit();
      }
      else
      {
        in_pause_menu = false;
        in_game_screen = true;
      }
    }
  }

  if (game_over)
  {
    if (key->key == ASGE::KEYS::KEY_LEFT &&
        key->action == ASGE::KEYS::KEY_RELEASED)
    {
      menu_option = 1 - menu_option;
    }
    else if (key->key == ASGE::KEYS::KEY_RIGHT &&
             key->action == ASGE::KEYS::KEY_RELEASED)
    {
      menu_option = 1 - menu_option;
    }

    if (key->key == ASGE::KEYS::KEY_ENTER &&
        key->action == ASGE::KEYS::KEY_PRESSED)
    {
      if (menu_option == 1)
      {
        signalExit();
      }
      else
      {
        // RESTART GAME NOT WORKING
      }
    }
  }
}

/**
 *   @brief   Processes any click inputs
 *   @details This function is added as a callback to handle the game's
 *            mouse button i
 *
 *
 *            nput. For this game, calls to this function
 *            are thread safe, so you may alter the game's state as you
 *            see fit.
 *   @param   data The event data relating to key input.
 *   @see     ClickEvent
 *   @return  void
 */
void Breakout::clickHandler(ASGE::SharedEventData data)
{
  auto click = static_cast<const ASGE::ClickEvent*>(data.get());

  double x_pos = click->xpos;
  double y_pos = click->ypos;

  ASGE::DebugPrinter{} << "x_pos: " << x_pos << std::endl;
  ASGE::DebugPrinter{} << "y_pos: " << y_pos << std::endl;
}

/**
 *   @brief   Updates the scene
 *   @details Prepares the renderer subsystem before drawing the
 *            current frame. Once the current frame is has finished
 *            the buffers are swapped accordingly and the image shown.
 *   @return  void
 */

void Breakout::collisionDetection()
{
  // BALL AND GAME BOUNDARY COLLISION
  if (ball.getSprite()->xPos() <= 0 ||
      (ball.getSprite()->xPos() + ball.getSprite()->width()) >= game_width)
  {
    // ASGE::DebugPrinter{} << "BOUNCE X" << std::endl;
    ball.setVelocity(Vector2{ ball_velocity_x *= -1, ball_velocity_y });
  }

  if (ball.getSprite()->yPos() <= 0)
  {
    // ASGE::DebugPrinter{} << "BOUNCE Y" << std::endl;
    ball.setVelocity(Vector2{ ball_velocity_x, ball_velocity_y *= -1 });
  }

  if (ball.getSprite()->yPos() + ball.getSprite()->height() >= game_height)
  {
    lives_count -= 1;
    ASGE::DebugPrinter{} << "lives: " << lives_count << std::endl;
    serve = false;
    initBall();
  }

  // PADDLE AND BALL COLLISION
  if (isOverlapping(paddle.getSprite(), ball.getSprite()))
  {
    ball.setVelocity(Vector2{ ball_velocity_x, ball_velocity_y *= -1 });
  }

  // BALL AND BRICKS COLLISION
  for (int i = 0; i < brick_num; i++)
  {
    if (isOverlapping(ball.getSprite(), red_bricks[i].getSprite()) &&
        red_bricks[i].visibility)
    {
      ball.setVelocity(Vector2{ ball_velocity_x, ball_velocity_y *= -1 });
      score++;
      red_bricks[i].visibility = false;
    }
    if (isOverlapping(ball.getSprite(), grey_bricks[i].getSprite()) &&
        grey_bricks[i].visibility)
    {
      ball.setVelocity(Vector2{ ball_velocity_x, ball_velocity_y *= -1 });
      score++;
      grey_bricks[i].visibility = false;
    }
    if (isOverlapping(ball.getSprite(), yellow_bricks[i].getSprite()) &&
        yellow_bricks[i].visibility)
    {
      ball.setVelocity(Vector2{ ball_velocity_x, ball_velocity_y *= -1 });
      score++;
      yellow_bricks[i].visibility = false;
    }
    if (isOverlapping(ball.getSprite(), purple_bricks[i].getSprite()) &&
        purple_bricks[i].visibility)
    {
      ball.setVelocity(Vector2{ ball_velocity_x, ball_velocity_y *= -1 });
      score++;
      purple_bricks[i].visibility = false;
    }
    if (isOverlapping(ball.getSprite(), green_bricks[i].getSprite()) &&
        green_bricks[i].visibility)
    {
      ball.setVelocity(Vector2{ ball_velocity_x, ball_velocity_y *= -1 });
      score++;
      green_bricks[i].visibility = false;
    }
  }

  // GEMS AND PADDLE COLLISION
  for (int i = 0; i < 4; i++)
  {
    if (isOverlapping(gems[i].getSprite(), paddle.getSprite()) &&
        gems[i].visibility)
    {
      ASGE::DebugPrinter{} << "gem collected" << std::endl;
      score += 10;
      gems[i].visibility = false;
    }
  }
}

bool Breakout::isOverlapping(ASGE::Sprite* sprite1, ASGE::Sprite* sprite2)
{
  if ((sprite2->xPos() < sprite1->xPos() + sprite1->width()) &&
      (sprite2->xPos() + sprite2->width() > sprite1->xPos()) &&

      (sprite2->yPos() < sprite1->yPos() + sprite1->height()) &&
      (sprite2->yPos() + sprite2->height() > sprite1->yPos()))
  {
    return true;
  }
}

void Breakout::update(const ASGE::GameTime& game_time)
{
  auto dt_sec = game_time.delta.count() / 1000.0;
  // make sure you use delta time in any movement calculations!

  if (in_game_screen)
  {
    collisionDetection();

    paddle.getSprite()->xPos(paddle.getSprite()->xPos() +
                             (paddle.getVelocity().x * dt_sec));

    if (paddle.getSprite()->xPos() <= 0)
    {
      paddle.getSprite()->xPos(0);
    }
    if (paddle.getSprite()->xPos() + paddle.getSprite()->width() >= game_width)
    {
      paddle.getSprite()->xPos(game_width - paddle.getSprite()->width());
    }

    if (!serve)
    {
      ball.getSprite()->xPos(paddle.getSprite()->xPos() +
                             paddle.getSprite()->width() / 2 -
                             ball.getSprite()->width() / 2);

      ball.getSprite()->yPos(paddle.getSprite()->yPos() -
                             (ball.getSprite()->height() + 1));
    }
    else
    {
      ball.getSprite()->xPos(ball.getSprite()->xPos() +
                             (ball.getVelocity().x * dt_sec));

      ball.getSprite()->yPos(ball.getSprite()->yPos() +
                             (ball.getVelocity().y * dt_sec));
    }

    if (lives_count == 0)
    {
      in_game_screen = false;
      game_over = true;
    }
  }

  int total_brick_count = brick_num * 5;

  for (int i = 0; i < brick_num; i++)
  {
    if (!red_bricks[i].visibility)
    {
      total_brick_count--;
    }
    if (!grey_bricks[i].visibility)
    {
      total_brick_count--;
    }
    if (!yellow_bricks[i].visibility)
    {
      total_brick_count--;
    }
    if (!purple_bricks[i].visibility)
    {
      total_brick_count--;
    }
    if (!green_bricks[i].visibility)
    {
      total_brick_count--;
    }
  }

  if (total_brick_count == 0)
  {
    in_game_screen = false;
    win = true;
  }

  float gem_y_velocity = 200;

  if (!green_bricks[14].visibility)
  {
    gems[3].getSprite()->yPos(gems[3].getSprite()->yPos() +
                              (gem_y_velocity * dt_sec));
  }
  if (!purple_bricks[2].visibility)
  {
    gems[0].getSprite()->yPos(gems[0].getSprite()->yPos() +
                              (gem_y_velocity * dt_sec));
  }
  if (!yellow_bricks[11].visibility)
  {
    gems[2].getSprite()->yPos(gems[2].getSprite()->yPos() +
                              (gem_y_velocity * dt_sec));
  }
  if (!red_bricks[7].visibility)
  {
    gems[1].getSprite()->yPos(gems[1].getSprite()->yPos() +
                              (gem_y_velocity * dt_sec));
  }
}

/**
 *   @brief   Renders the scene
 *   @details Renders all the game objects to the current frame.
 *            Once the current frame is has finished the buffers are
 *            swapped accordingly and the image shown.
 *   @return  void
 */
void Breakout::renderMenuOptions()
{
  renderer->renderText(menu_option == 0 ? ">PLAY" : "PLAY",
                       (game_width * 0.35),
                       (game_height * 0.8),
                       1.0,
                       ASGE::COLOURS::WHITE);

  renderer->renderText(menu_option == 1 ? ">EXIT" : "EXIT",
                       (game_width * 0.55),
                       (game_height * 0.8),
                       1.0,
                       ASGE::COLOURS::WHITE);
}

void Breakout::render(const ASGE::GameTime&)
{
  renderer->setFont(0);

  if (in_menu)
  {
    game_over = false;
    in_game_screen = false;
    win = false;
    in_pause_menu = false;

    renderer->renderText(
      "MAIN MENU", game_width / 2, game_height / 2, 1.0, ASGE::COLOURS::WHITE);

    renderMenuOptions();
  }

  if (in_game_screen)
  {
    game_over = false;
    in_menu = false;
    win = false;
    in_pause_menu = false;

    renderer->renderText("IN GAME, PRESS P TO PAUSE OR Esc TO QUIT",
                         game_width / 2,
                         game_height / 2,
                         1.0,
                         ASGE::COLOURS::WHITE);

    renderer->renderText("LIVES: " + std::to_string(lives_count),
                         10,
                         game_height - 6,
                         1.0,
                         ASGE::COLOURS::WHITE);

    renderer->renderText("SCORE: " + std::to_string(score),
                         game_width - 110,
                         game_height - 6,
                         1.0,
                         ASGE::COLOURS::WHITE);

    renderer->renderSprite(*paddle.getSprite());

    for (int i = 0; i < 4; i++)
    {
      if (gems[i].visibility)
      {
        renderer->renderSprite(*gems[i].getSprite());
      }
    }

    for (int i = 0; i < brick_num; i++)
    {
      if (green_bricks[i].visibility)
      {
        green_bricks[i].getSprite()->xPos(i *
                                          green_bricks->getSprite()->width());
        renderer->renderSprite(*green_bricks[i].getSprite());
      }

      purple_bricks[i].getSprite()->xPos(i *
                                         purple_bricks->getSprite()->width());
      purple_bricks[i].getSprite()->yPos(green_bricks->getSprite()->yPos() +
                                         green_bricks->getSprite()->height());
      if (purple_bricks[i].visibility)
      {
        renderer->renderSprite(*purple_bricks[i].getSprite());
      }

      yellow_bricks[i].getSprite()->xPos(i *
                                         yellow_bricks->getSprite()->width());
      yellow_bricks[i].getSprite()->yPos(purple_bricks->getSprite()->yPos() +
                                         purple_bricks->getSprite()->height());
      if (yellow_bricks[i].visibility)
      {
        renderer->renderSprite(*yellow_bricks[i].getSprite());
      }

      grey_bricks[i].getSprite()->xPos(i * grey_bricks->getSprite()->width());
      grey_bricks[i].getSprite()->yPos(yellow_bricks->getSprite()->yPos() +
                                       yellow_bricks->getSprite()->height());

      if (grey_bricks[i].visibility)
      {
        renderer->renderSprite(*grey_bricks[i].getSprite());
      }

      red_bricks[i].getSprite()->xPos(i * red_bricks->getSprite()->width());
      red_bricks[i].getSprite()->yPos(grey_bricks->getSprite()->yPos() +
                                      grey_bricks->getSprite()->height());

      if (red_bricks[i].visibility)
      {
        renderer->renderSprite(*red_bricks[i].getSprite());
      }
    }

    renderer->renderSprite(*ball.getSprite());
  }

  if (in_pause_menu)
  {
    game_over = false;
    in_game_screen = false;
    win = false;
    in_menu = false;

    renderer->renderText(
      "PAUSE MENU", game_width / 2, game_height / 2, 1.0, ASGE::COLOURS::WHITE);

    renderMenuOptions();
  }

  if (game_over)
  {
    in_menu = false;
    in_game_screen = false;
    win = false;
    in_pause_menu = false;

    renderer->renderText(
      "GAME OVER", game_width / 2, game_height / 2, 1.0, ASGE::COLOURS::WHITE);

    renderMenuOptions();
  }

  if (win)
  {
    game_over = false;
    in_game_screen = false;
    in_menu = false;
    in_pause_menu = false;

    renderer->renderText(
      "YOU WIN", game_width / 2, game_height / 2, 1.0, ASGE::COLOURS::WHITE);

    renderMenuOptions();
  }
}
