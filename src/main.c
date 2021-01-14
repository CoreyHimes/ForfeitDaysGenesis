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
void drawBackground();
typedef struct
{
    fix16 x;
    fix16 y;
    fix16 x_vel;
    fix16 y_vel;
    int sprite_height;
    int sprite_width;
    Sprite* sprite;
} Entity;

Entity player = {FIX16(100), FIX16(100), FIX16(0), FIX16(0), 32, 16, };
bool player_is_jumping = FALSE;

const int scrollspeeda = 1;
const int scrollspeedb = 2;

int load_title_screen;  //maybe make an enum of states later

TileMap* background;

int main()
{
    load_title_screen = 1;
    JOY_init();
    JOY_setEventHandler( &handleInput );
    VDP_setPlanSize(32,32);
    VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
    VDP_loadTileSet(brick.tileset, 4, CPU);
    //VDP_loadTileSet(roof.tileset, 2, CPU);
    //VDP_loadTileSet(roofedge.tileset, 3, CPU);
    PAL_setPalette(PAL1, brick.palette->data);
    PAL_setPalette(PAL2, character.palette->data);
    PAL_setPalette(PAL3, villain.palette->data);
    
    initGame();
    XGM_setLoopNumber(0);
    XGM_startPlay(&intro);
    int offseta = 0;
    int offsetb = 0;
    while(1)
    {
        if (load_title_screen == 1){
            
            titleScreen();
            SPR_update();
            VDP_waitVSync();
            continue;
        }
        VDP_setHorizontalScroll(BG_B, offsetb -= scrollspeedb);
        VDP_setHorizontalScroll(BG_A, offseta -= scrollspeeda);
        positionPlayer();
        checkCollision();
        SPR_update();
        VDP_waitVSync();
    }
    return (0);
}

void initGame()
{
    VDP_clearTextArea(0,10,40,10);
    player.x = FIX16(144);
    player.y = FIX16(100);
    //badguy_pos_x = 220;
    XGM_setLoopNumber(-1);
    XGM_startPlay(&boss);
    
    drawBackground();
    SPR_init(0,0,0);
    player.sprite =  SPR_addSprite(&character, player.x, player.y, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    //badguy = SPR_addSprite(&villain, badguy_pos_x, badguy_pos_y, TILE_ATTR(PAL3, 0, FALSE, FALSE));
}

void drawBackground()
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i%3 == 0)
            {
                VDP_fillTileMapRectInc(BG_A,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,4),i*4,j*4,4,4);
            }
        }
    }

    for (int i = 0; i < 10; i++)
    {
        for (int j = 4; j < 7; j++)
        {
            VDP_fillTileMapRectInc(BG_B,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,4),i*4,j*4,4,4);
        }
    }

}
void positionPlayer()
{
    player.y = fix16Add(player.y_vel, player.y);
    player.x = fix16Add(player.x_vel, player.x);
    if (player_is_jumping == TRUE)
    {
        player.y_vel = fix16Add(player.y_vel, FIX16(.1));
    }
    if (fix16ToInt(player.y) >= 100 && player_is_jumping == TRUE)
    {
        player_is_jumping = FALSE;
        player.y_vel = FIX16(0);
        player.y = FIX16(100);
    }
    if (player.x_vel > 0) 
    {
        SPR_setHFlip(player.sprite, FALSE);
    }
    else if (player.x_vel < 0)
    {
        SPR_setHFlip(player.sprite, TRUE);
    }
    SPR_setPosition(player.sprite, fix16ToInt(player.x), fix16ToInt(player.y));
}

void checkCollision()
{
    //nothing to collide with
    //if ((player_pos_x < badguy_pos_x) & ((player_pos_x + player_width) > badguy_pos_x))
    //{
    //    player_pos_x = FIX16(0);
    //}
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
            player.x_vel = FIX16(1);
        }
        else if (state & BUTTON_LEFT)
        {
            player.x_vel = FIX16(-1);
        }
        else
        {
            if ((changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT))
            {
                player.x_vel = FIX16(0);
            }
        }
        if (state & BUTTON_C)
        {
            if (player_is_jumping == FALSE)
            {
                player_is_jumping = TRUE;
                player.y_vel = FIX16(-3);
            }
        }
        else
        {
            if (changed & BUTTON_C)
            {
                //player_vel_y = 0;
            }
        }  
    }
}