/*
This example is the game pong, it shows the following features:
1. Inputs.
2. Text.
3. Drawing shapes.
 */
#define XWRAP_IMPLEMENTATION
#define XWRAP_AUTO_LINK
#include "../xwrap.h"

#include <stdint.h>
#include <stdio.h>

#define ESC 9
#define RIGHT 114
#define LEFT 113
#define PLAYER_SIZE 50 / 2
#define BACKGROUND 0x181818

typedef enum {
    NO_WINNER,
    NPC,
    PLAYER,
    WINNER_LEN,
} Winner;

typedef struct {
    int x, y;
    int vx, vy;
    size_t radius;
    uint32_t color;
} Ball;

typedef struct {
    int x_start;
    int x_end;
    int y_pos;
    int thickness;
    uint32_t color;
} Player;

typedef struct {
    Player npc;
    Player player;
    Winner w;
    Ball ball;

    unsigned int width;
    unsigned int height;
} Game;

void game_update(Game* game)
{
    if (game->ball.x < 0 || game->width < game->ball.x) {
        game->ball.vx = -game->ball.vx;
    }
    if (game->ball.y == game->npc.y_pos) {
        if (game->npc.x_start < game->ball.x && game->ball.x < game->npc.x_end) {
            game->ball.vy = -game->ball.vy;

            game->ball.vx *= 2;
            game->ball.vy *= 2;
        }
    } else if (game->ball.y == game->player.y_pos + game->player.thickness) {
        if (game->player.x_start < game->ball.x && game->ball.x < game->player.x_end) {
            game->ball.vy = -game->ball.vy;

            game->ball.vx *= 2;
            game->ball.vy *= 2;
        }
    } else if (game->ball.y < game->player.y_pos + game->player.thickness) {
        game->w = PLAYER;
    } else if (game->ball.y > game->npc.y_pos) {
        game->w = NPC;
    }

    game->ball.x += game->ball.vx;
    game->ball.y += game->ball.vy;
}

void ball_draw(xw_handle* handle, const Ball ball)
{
    xw_draw_circle(handle, ball.x, ball.y, ball.radius, true, ball.color);
}

void player_draw(xw_handle* handle, const Player player)
{
    xw_draw_rectangle(handle, player.x_start, player.y_pos, player.x_end - player.x_start,
                      player.thickness, true, player.color);
}

void game_draw(xw_handle* handle, const Game game)
{
    ball_draw(handle, game.ball);
    player_draw(handle, game.npc);
    player_draw(handle, game.player);
}

void player_move(Player* player, int width, int distance)
{
    if (player->x_end + distance > width) {
        player->x_start += width - player->x_end + distance;
        player->x_end += width - player->x_end + distance;
        return;
    }
    if (player->x_start + distance < 0) {
        const int width = player->x_end - player->x_start;

        player->x_start = 0;
        player->x_end   = width;
        return;
    }

    player->x_start += distance;
    player->x_end += distance;
}

Game create_game(uint16_t width, uint16_t height, uint16_t player_size)
{
    const int player_thickness = 10;
    return (Game){
        .ball =
            {.x = width / 2, .y = height / 2, .vx = 1, .vy = 1, .radius = 5, .color = 0x00ff0000},
        .npc    = {.x_start   = width / 2 - player_size,
                   .x_end     = width / 2 + player_size,
                   .y_pos     = height - (player_thickness * 2),
                   .thickness = player_thickness,
                   .color     = 0x00ff00},
        .player = {.x_start   = width / 2 - player_size,
                   .x_end     = width / 2 + player_size,
                   .y_pos     = player_thickness,
                   .thickness = player_thickness,
                   .color     = 0x00ff0000},
        .w      = NO_WINNER,
        .width  = width,
        .height = height};
}

int clamp(int x, int min, int max)
{
    if (x < min) {
        return min;
    }
    if (max < x) {
        return max;
    }
    return x;
}

void game_over(xw_handle* handle, const Game game)
{
    xw_draw_background(handle, BACKGROUND);
    char buffer[32] = "GAME OVER";
    if (game.w == NPC) {
        strcat(buffer, ", you lost!");
    } else {
        strcat(buffer, ", you won!");
    }
    // TODO: get width from the text
    xw_draw_text(handle, game.width / 3, game.height / 2, buffer, 0xffffffff);
    xw_draw(handle);
}

int main(int argc, char const* argv[])
{
    const unsigned int width  = 640;
    const unsigned int height = 480;
    xw_handle* handle         = xw_create_window("pong", width, height);

    Game game = create_game(width, height, PLAYER_SIZE);

    for (;;) {
        // TODO: make the movement linear
        while (xw_event_pending(handle)) {
            xw_event event;
            xw_get_next_event(handle, &event);
            switch (event.type) {
                case KeyPress: {
                    switch (event.button.key_code) {
                        case ESC:
                            goto shutdown;
                        case RIGHT:
                            player_move(&game.npc, game.width, 5);
                            break;
                        case LEFT:
                            player_move(&game.npc, game.width, -5);
                            break;
                    }
                }
                case ButtonPress: {
                    if (event.mouse.button == Button1) {
                        printf("Left mouse button clicked at (%d, %d)\n", event.mouse.x,
                               event.mouse.y);
                    } else if (event.mouse.button == Button3) {
                        printf("Right mouse button clicked at (%d, %d)\n", event.mouse.x,
                               event.mouse.y);
                    }
                } break;

                default:
                    break;
            }
        }

        { // NPC movement
            int dist =
                game.ball.x - game.player.x_start - (game.player.x_end - game.player.x_start) / 2;
            dist = clamp(dist, -5, 5);
            player_move(&game.player, game.width, dist);
        }

        xw_draw_background(handle, BACKGROUND);

        game_update(&game);
        if (game.w != NO_WINNER) {
            break;
        }

        game_draw(handle, game);

        xw_draw(handle);
        xw_sleep_ms(33);
    }

    game_over(handle, game);
    xw_wait_for_esc(handle, 0);

shutdown:
    xw_free_window(handle);

    return 0;
}
