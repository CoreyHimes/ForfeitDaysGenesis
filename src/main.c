/**
 * Hello World Example
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include <genesis.h>
#include <resources.h>
#define TRUE 1
#define FALSE 0
void handleInput(u16 gamePad, u16 changed, u16 state);
void positionPlayer();
void checkCollision();

int player_pos_x = 144;
int player_pos_y = 200;
int player_vel_x = 0;
const int player_width = 8;
const int player_height = 8;
int player_vel_y = 0;

int badguy_pos_x = 220;
const int badguy_pos_y = 200;
int badguy_vel_x = 0;
const int badguy_width = 8;
const int badguy_height = 8;
Sprite* player;
Sprite* badguy;

int main()
{
    JOY_init();
    JOY_setEventHandler( &handleInput );
    VDP_loadTileSet(TEST.tileset,1,DMA);
    PAL_setPalette(PAL1, TEST.palette->data);
    PAL_setPalette(PAL2, character.palette->data);
    PAL_setPalette(PAL3, villain.palette->data);
    
    
    VDP_fillTileMapRect(BG_B,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,1),0,0,40,30);
    SPR_init(0,0,0);
    player = SPR_addSprite(&character, player_pos_x, player_pos_y, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    badguy = SPR_addSprite(&villain, badguy_pos_x, badguy_pos_y, TILE_ATTR(PAL3, 0, FALSE, FALSE));
    XGM_setLoopNumber(0);
    XGM_startPlay(&intro);
    while(1)
    {
        positionPlayer();
        checkCollision();
        SPR_update();
        VDP_waitVSync();
    }
    return (0);
}

void positionPlayer(){
    player_pos_x += player_vel_x;
    player_pos_y += player_vel_y;
    if (player_vel_x > 0) {
        SPR_setHFlip(player, FALSE);

    }
    else if (player_vel_x < 0) {
        SPR_setHFlip(player, TRUE);

    }
    SPR_setPosition(player, player_pos_x, player_pos_y);
}

void checkCollision(){
    if ((player_pos_x < badguy_pos_x) & ((player_pos_x + player_width) > badguy_pos_x))
    {
        player_pos_x = 0;
    }
}

void handleInput(u16 gamePad, u16 changed, u16 state)
{
    if (gamePad == JOY_1)
    {
        if (state & BUTTON_RIGHT)
        {
            player_vel_x = 1;
        }
        else if (state & BUTTON_LEFT)
        {
            player_vel_x = -1;
        }
        else
        {
            if ((changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT))
            {
                player_vel_x = 0;
            }
        }
        if (state & BUTTON_C)
        {
            player_vel_y = -1;
        }
        else
        {
            if (changed & BUTTON_C)
            {
                player_vel_y = 0;
            }
        }
        
        
    }
}