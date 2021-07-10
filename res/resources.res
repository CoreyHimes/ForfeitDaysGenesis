
PALETTE palette_background "maps/Level_1_FG.png"
PALETTE palette_foreground "maps/Level_1_BG.png"
PALETTE palette_char "sprites/playermultianimation.png"

TILESET bga_tileset "maps/Level_1_FG.png" BEST ALL
TILESET bgb_tileset "maps/Level_1_BG.png" BEST ALL

MAP bga_map "maps/Level_1_FG.png" bga_tileset BEST 0
MAP bgb_map "maps/Level_1_BG.png" bgb_tileset BEST 0

SPRITE character  "sprites/playermultianimation.png" 2 4 NONE 5
SPRITE enemy  "sprites/enemy.png" 2 2 NONE
XGM    intro  "music/intro.vgm"
XGM    cave  "music/cave.vgm"
XGM    boss  "music/boss.vgm"
WAV    test "sfx/test.wav"