/**
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include <genesis.h>
#include <resources.h>
#define TRUE 1
#define FALSE 0

#define ANIMATION_STAND 0
#define ANIMATION_RUN 1

#define SFX_TEST 64
typedef struct
{
    fix32 x;
    fix32 y;
    fix32 x_vel;
    fix32 y_vel;
    int height;
    int width;
    Sprite* sprite;
} Entity;



int lives;
u16 tilemapBuffer[42*32];

s16 camera_x;
s16 camera_y;
void handleInput(u16 gamePad, u16 changed, u16 state);
void positionPlayer();
void checkCollision(Entity entity1, Entity entity2);
void initGame();
void titleScreen();
void drawBackground();
void respawnPlayer();
void animatePlayer();

Entity player = {FIX32(100), FIX32(100), FIX32(0), FIX32(0), 32, 16, };
Entity badguy = {FIX32(200), FIX32(100), FIX32(-2), FIX32(0), 16, 16, };
bool player_is_jumping = FALSE;

int load_title_screen;  //maybe make an enum of states later
int paused;


int main()
{
    load_title_screen = TRUE;
    paused = FALSE;
    JOY_init();
    JOY_setEventHandler( &handleInput );
    VDP_setPlanSize(32,32);
    VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
    VDP_loadTileSet(brick.tileset, 4, CPU);
    PAL_setPalette(PAL1, brick.palette->data);
    PAL_setPalette(PAL2, character.palette->data);
    PAL_setPalette(PAL3, enemy.palette->data);
    

    initGame();    
    XGM_setLoopNumber(0);
    XGM_startPlay(&intro);
    int offseta = 0;
    int offsetb = 0;
    while(1)
    {
        if (load_title_screen == TRUE){
            
            titleScreen();
            SPR_update();
            VDP_waitVSync();
            continue;
        }
        if (paused == TRUE)
        {
            continue; //don't change anything if paused
        }
        VDP_setHorizontalScroll(BG_B, offsetb -= fix32ToInt(player.x_vel));
        VDP_setHorizontalScroll(BG_A, offseta -= fix32ToInt(player.x_vel)/2);
        if (offseta <= -256) offseta = 0;
        if (offsetb <= -256) offsetb = 0;
        checkCollision(player, badguy);
        positionPlayer();      
        animatePlayer();  
        SPR_setPosition(badguy.sprite, fix32ToInt(badguy.x) , fix32ToInt(badguy.y));
        SPR_update();
        VDP_waitVSync();
    }
    return (0);
}

void initGame()
{
    VDP_clearTextArea(0,10,40,10);
    player.x = FIX32(144);
    player.y = FIX32(player.height);
    badguy.x = FIX32(200);
    badguy.y = FIX32(70);
    lives = 3;

    XGM_setLoopNumber(-1);
    XGM_startPlay(&boss);
    
    drawBackground();
    SPR_init(0,0,0);
    badguy.sprite =  SPR_addSprite(&enemy, badguy.x, badguy.y, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    player.sprite =  SPR_addSprite(&character, player.x, player.y, TILE_ATTR(PAL2, 0, FALSE, FALSE));
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
    player.y = fix32Add(player.y_vel, player.y);
    player.x = fix32Add(player.x_vel, player.x);
    if (player_is_jumping == TRUE)
    {
        player.y_vel = fix32Add(player.y_vel, FIX32(.1));
    }
    if (fix32ToInt(player.y) >= 100 && player_is_jumping == TRUE)
    {
        player_is_jumping = FALSE;
        player.y_vel = FIX32(0);
        player.y = FIX32(100);
    }
    SPR_setPosition(player.sprite, fix32ToInt(player.x) , fix32ToInt(player.y));
}

//box collision only for now
void checkCollision(Entity entity1, Entity entity2)
{
    if (((entity1.x < entity2.x) & ((entity1.width + fix32ToInt(entity1.x)) > fix32ToInt(entity2.x))) |
        ((entity2.x < entity1.x) & ((entity2.width + fix32ToInt(entity2.x)) > fix32ToInt(entity1.x))))
    {
        if (((entity1.y < entity2.y) & ((entity1.height + fix32ToInt(entity1.y)) > fix32ToInt(entity2.y))) |
         ((entity2.y < entity1.y) & ((entity2.height + fix32ToInt(entity2.y)) > fix32ToInt(entity1.y))))
        {
            respawnPlayer();
            
        }
    }
}

void animatePlayer()
{
    if (player.x_vel != 0)
    {
        SPR_setAnim(player.sprite, ANIMATION_RUN);
    }
    else
    {
        SPR_setAnim(player.sprite, ANIMATION_STAND);
    }
    if (player.x_vel > 0) 
    {
        SPR_setHFlip(player.sprite, FALSE);
    }
    else if (player.x_vel < 0)
    {
        SPR_setHFlip(player.sprite, TRUE);
    }
}

void respawnPlayer()
{
    if (lives == 1) 
    {
        load_title_screen = TRUE;
        XGM_setLoopNumber(0);
         XGM_startPlay(&intro);
        return;
    }
    player.x = FIX32(10);
    lives--;
}

void titleScreen()
{
    VDP_clearSprites();
    VDP_drawText("Push Start to Start", 8, 12);
}

void handleInput(u16 gamePad, u16 changed, u16 state)
{
    if (gamePad == JOY_1)
    {
        if (state & BUTTON_START)
        {
            if (load_title_screen == FALSE)
            {
                if (paused == FALSE)
                {
                    paused = TRUE;
                    XGM_pausePlay();

                }
                else {
                    paused = FALSE;
                    XGM_resumePlay();
                    
                }

            } else 
            {
                load_title_screen = FALSE;   
                initGame();                   
            }  

        }
        
        if (paused == TRUE){
            return; //return early if paused, no need to check anything besides start
        }
        if (state & BUTTON_RIGHT)
        {
            player.x_vel = FIX32(2);
        }
        else if (state & BUTTON_LEFT)
        {
            player.x_vel = FIX32(-2);
        }
        else
        {
            if ((changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT))
            {
                player.x_vel = FIX32(0);
            }
        }
        if (state & BUTTON_C)
        {
            if (player_is_jumping == FALSE)
            {
                player_is_jumping = TRUE;
                player.y_vel = FIX32(-3);
                //XGM_startPlayPCM(SFX_TEST,1,SOUND_PCM_CH2);
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