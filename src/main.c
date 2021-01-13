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
void initGame();
void titleScreen();
int framecount = 0;
fix16 player_pos_x = FIX16(144);
fix16 player_pos_y = FIX16(100);
fix16 player_vel_x = FIX16(0);
const int player_width = 8;
const int player_height = 8;
fix16 player_vel_y = FIX16(0);
bool player_is_jumping = FALSE;

const int scrollspeed = 1;

int load_title_screen;  //maybe make an enum of states later

int badguy_pos_x = 220;
const int badguy_pos_y = 200;
int badguy_vel_x = 0;
const int badguy_width = 8;
const int badguy_height = 8;
Sprite* player;
Sprite* badguy;
TileMap* background;

int main()
{
    load_title_screen = 1;
    JOY_init();
    JOY_setEventHandler( &handleInput );
    VDP_setPlanSize(32,32);
    VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
    VDP_loadTileSet(brick.tileset, 1, DMA);
    PAL_setPalette(PAL1, brick.palette->data);
    PAL_setPalette(PAL2, character.palette->data);
    PAL_setPalette(PAL3, villain.palette->data);
    
    initGame();
    XGM_setLoopNumber(0);
    XGM_startPlay(&intro);
    int offset = 0;
    while(1)
    {
        VDP_setHorizontalScroll(BG_B, offset -= scrollspeed);
        if (load_title_screen == 1){
            
            titleScreen();
            SPR_update();
            VDP_waitVSync();
            continue;
        }
        positionPlayer();
        checkCollision();
        SPR_update();
        framecount += 1;
        VDP_waitVSync();
    }
    return (0);
}

void initGame()
{
    VDP_clearTextArea(0,10,40,10);
    player_pos_x = FIX16(144);
    player_pos_y = FIX16(100);
    badguy_pos_x = 220;
    XGM_setLoopNumber(-1);
    XGM_startPlay(&cave);
    //VDP_setTileMap(BG_B,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,1),0,0,40,30);
    VDP_fillTileMapRect(BG_B,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,1),0,0,40,30);
    //VDP_fillTileMapRectInc(BG_B,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,1),15,13,2,2);
    SPR_init(0,0,0);
    player = SPR_addSprite(&character, player_pos_x, player_pos_y, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    badguy = SPR_addSprite(&villain, badguy_pos_x, badguy_pos_y, TILE_ATTR(PAL3, 0, FALSE, FALSE));
    

}
void positionPlayer()
{
    player_pos_y = fix16Add(player_pos_y, player_vel_y);
    player_pos_x = fix16Add(player_pos_x, player_vel_x);
    if (player_is_jumping == TRUE)
    {
        player_vel_y = fix16Add(player_vel_y, FIX16(.1));
    }
    if (fix16ToInt(player_pos_y) >= 100 && player_is_jumping == TRUE)
    {
        player_is_jumping = FALSE;
        player_vel_y = FIX16(0);
        player_pos_y = FIX16(100);
    }
    if (player_vel_x > 0) 
    {
        SPR_setHFlip(player, FALSE);
    }
    else if (player_vel_x < 0)
    {
        SPR_setHFlip(player, TRUE);
    }
    SPR_setPosition(player, fix16ToInt(player_pos_x), fix16ToInt(player_pos_y));
}

void checkCollision()
{
    if ((player_pos_x < badguy_pos_x) & ((player_pos_x + player_width) > badguy_pos_x))
    {
        player_pos_x = FIX16(0);
    }
}

void titleScreen()
{
    VDP_clearSprites();
    VDP_drawText("Push Start to Start", 8, 12);
}

void handleInput(u16 gamePad, u16 changed, u16 state)
{
    if (load_title_screen == 1) 
    {
        if (gamePad == JOY_1)
        {
            if (state & BUTTON_START)
            {
                load_title_screen = 0;   
                initGame();  
                   
            }    
        }
    }
    if (gamePad == JOY_1)
    {
        if (state & BUTTON_RIGHT)
        {
            player_vel_x = FIX16(1);
        }
        else if (state & BUTTON_LEFT)
        {
            player_vel_x = FIX16(-1);
        }
        else
        {
            if ((changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT))
            {
                player_vel_x = FIX16(0);
            }
        }
        if (state & BUTTON_C)
        {
            if (player_is_jumping == FALSE)
            {
                player_is_jumping = TRUE;
                player_vel_y = FIX16(-3);
            }
        }
        else
        {
            if (changed & BUTTON_C)
            {
                //player_vel_y = 0;
            }
        }

        //if (state & BUTTON_START)
        //{
        //    load_title_screen = TRUE;           
        //}    
    }
}