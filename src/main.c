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
void respawnPlayer();
void animatePlayer();
void backgroundCollision();
void positionCamera();
void renderEntities();
void moveEntities();

Entity player = {FIX32(10), FIX32(80), FIX32(0), FIX32(0), 32, 16, };
Entity badguy = {FIX32(200), FIX32(100), FIX32(0), FIX32(0), 16, 16, };
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
    u16 palette[64];
    memcpy(&palette[0], palette_background.data, 16 * 2);
    memcpy(&palette[16], palette_foreground.data, 16 * 2);
    memcpy(&palette[32], palette_char.data, 16 * 2);

    u16 ind;
    // set all palette to black
    VDP_setPaletteColors(0, (u16*) palette_black, 64);

    PAL_fadeIn(0, (4 * 16) - 1, palette, 20, FALSE);

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
    bga = MAP_create(&bga_map, BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, bgBaseTileIndex[0]));
    bgb = MAP_create(&bgb_map, BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, bgBaseTileIndex[1]));

    initGame();    
    XGM_setLoopNumber(0);
    XGM_startPlay(&intro);
    while(1)
    {
        MAP_scrollTo(bga, camera_x, 100);
        MAP_scrollTo(bgb, camera_x>>4, 100);
        positionPlayer();
        
        moveEntities();
        renderEntities();
        checkCollision(player, badguy);

        positionCamera();
        SPR_update();
        SYS_doVBlankProcess();
    }
    return (0);
}

void initGame()
{
    VDP_clearTextArea(0,10,40,10);
    player.x = FIX32(10);
    player.y = FIX32(140);
    badguy.x = FIX32(200);
    badguy.y = FIX32(156);
    lives = 3;

    XGM_setLoopNumber(-1);
    XGM_startPlay(&boss);
    SPR_init(0,0,0);
    badguy.sprite =  SPR_addSprite(&enemy, badguy.x, badguy.y, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    SPR_setPosition(badguy.sprite, fix32ToInt(badguy.x), fix32ToInt(badguy.y));
    player.sprite =  SPR_addSprite(&character, player.x, player.y, TILE_ATTR(PAL2, 0, FALSE, FALSE));

}

void positionPlayer()
{
    player.x = fix32Add(player.x_vel, player.x);
    player.y = fix32Add(player.y_vel, player.y);
    if (player_is_jumping == TRUE)
    {
        player.y_vel = fix32Add(player.y_vel, FIX32(.1));
    }
    if (fix32ToInt(player.y) >= 140 && player_is_jumping == TRUE)
    {
        player_is_jumping = FALSE;
        player.y_vel = FIX32(0);
        player.y = FIX32(140);
    }
    if (player.x < FIX32(0)) {
        player.x = FIX32(0);
    }
    if (player.x > FIX32(7900)) {
        player.x = FIX32(7900);
    }
    int temp_x = fix32ToInt(player.x) - camera_x;
    SPR_setPosition(player.sprite, temp_x, fix32ToInt(player.y));
    animatePlayer();
}

void renderEntities()
{
    int distance = abs(camera_x - fix32ToInt(badguy.x));
    if (distance > 180){
        return;
    }
    int temp_x = fix32ToInt(badguy.x) - camera_x;
    SPR_setPosition(badguy.sprite, temp_x, fix32ToInt(badguy.y));
}

void moveEntities()
{
    int distance = abs(player.x - badguy.x);
    if (distance > 100000) {
        return;
    }
    if (badguy.x > player.x)
    {
        if (badguy.x_vel > FIX32(-1.5)){
            SPR_setHFlip(badguy.sprite, FALSE);
            badguy.x_vel = fix32Sub(badguy.x_vel, FIX32(.1));
        }
    }
    if (badguy.x < player.x)
    {
        if (badguy.x_vel < FIX32(1.5)){
            SPR_setHFlip(badguy.sprite, TRUE);
            badguy.x_vel = fix32Add(badguy.x_vel, FIX32(.1));
        }
    }
    badguy.x = badguy.x + badguy.x_vel;
}

void positionCamera()
{
    camera_x = fix32ToInt(player.x) - 120;
    if (camera_x > 7900) {
        camera_x = 7900;
    }
    if (camera_x < 0) {
        camera_x = 0;
    }
}

//box collision only for now
void checkCollision(Entity entity1, Entity entity2)
{
    //respawnPlayer();
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
                else 
                {
                    paused = FALSE;
                    XGM_resumePlay();                    
                }
            } 
            else 
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
    }
}