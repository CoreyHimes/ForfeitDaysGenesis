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
int camera_x = 0;
s16 camera_y;
void handleInput(u16 gamePad, u16 changed, u16 state);
void positionPlayer();
void checkCollision(Entity entity1, Entity entity2);
void initGame();
void titleScreen();
void drawBackground();
void respawnPlayer();
void animatePlayer();
void updateBackground();
void backgroundCollision();
void calculateCamera();

Entity player = {FIX32(100), FIX32(100), FIX32(0), FIX32(0), 32, 16, };
Entity badguy = {FIX32(200), FIX32(100), FIX32(-2), FIX32(0), 16, 16, };
bool player_is_jumping = FALSE;

int load_title_screen;  //maybe make an enum of states later
int paused;

Map *bgb;
Map *bga;
// 42 * 32 = complete tilemap update; * 2 as we have 2 full plans to update potentially
// used for alternate map update mode
u16 tilemapBuf[42 * 32 * 2];
u16 bufOffset;
// BG start tile index
u16 bgBaseTileIndex[2];

int main()
{
    u16 ind;
    load_title_screen = TRUE;
    paused = FALSE;
    JOY_init();
    JOY_setEventHandler( &handleInput );
        // load background tilesets in VRAM
    ind = TILE_USERINDEX;
    bgBaseTileIndex[0] = ind;
    VDP_loadTileSet(&bga_tileset, ind, DMA);
    ind += bga_tileset.numTile;
    bgBaseTileIndex[1] = ind;
    VDP_loadTileSet(&bgb_tileset, ind, DMA);
    ind += bgb_tileset.numTile;

    // initialize variables
    bufOffset = 0;
    
    // init backgrounds
    bga = MAP_create(&bga_map, BG_A, TILE_ATTR_FULL(0, FALSE, FALSE, FALSE, bgBaseTileIndex[0]));
    bgb = MAP_create(&bgb_map, BG_B, TILE_ATTR_FULL(0, FALSE, FALSE, FALSE, bgBaseTileIndex[1]));

    initGame();    
    XGM_setLoopNumber(0);
    XGM_startPlay(&intro);
    int offseta = 0;
    int offsetb = 0;
    int x = 0;
    int y = 800;
    while(1)
    {
        MAP_scrollTo(bga, x, y);
        // scrolling is slower on BGB
        MAP_scrollTo(bgb, x<<3, y<<5);
        positionPlayer();
        SPR_update();
        // sync frame and do vblank process
        SYS_doVBlankProcess();
        // reset tilemap buffer position after update
        bufOffset = 0;
        x++;
    }
    return (0);
}

void calculateCamera()
{
    camera_x = player.x_vel;

     VDP_setHorizontalScroll(BG_A, fix32ToInt(camera_x));  //Tieing these to player velocity is wrong, eventually tie this to a "camera"
    //VDP_setHorizontalScroll(BG_B, offseta -= fix32ToInt(player.x_vel) >> 3);
}
void initGame()
{
    VDP_clearTextArea(0,10,40,10);
    player.x = FIX32(144);
    player.y = FIX32(80);
    badguy.x = FIX32(200);
    badguy.y = FIX32(100);
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
    int tile = 0;
    int x_cam_high = camera_x + 64;
    for (int x = camera_x; x < x_cam_high; x++)
    {
        for (int y = 0; y < 15; y++)
        {
        }
    }

}

void updateBackground()
{
    int tile = 0;
    for (int y = 0; y < 15; y++){
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
    animatePlayer();
}

void backgroundCollision()
{
    //get indexes of level array that player is hovering
    int leftPlayer = fix32ToInt(player.x) >> 3;
    int rightPlayer = (fix32ToInt(player.x) + player.width) >> 3;
    int playerTop = fix32ToInt(player.y) >> 3;
    int playerBottom = (fix32ToInt(player.y) + player.height) >> 3;
    //char str[80];

    //sprintf(str, "left = %i right = %i", playerTop, playerBottom);

    //VDP_drawText(str, 0, 0);
    for (int x = leftPlayer; x <= rightPlayer; x++){
        for (int y = playerTop; y <= playerBottom; y++){
        }
    }   
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