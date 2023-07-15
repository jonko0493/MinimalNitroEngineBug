#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>

void Draw3DScene()
{
    NE_2DViewInit();
    NE_SpriteDrawAll();
    NE_WaitForVBL(NE_UPDATE_ANIMATIONS);
}

int main(int argc, char **argv)
{
    // videoSetModeSub(MODE_5_2D);
    if (!nitroFSInit(NULL))
    {
        iprintf("Nitro FS init failed!");
    }
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);
    NE_Init3D();

    FILE* graphicsFile = fopen("nitro:/graphics/logo_aqi.img", "rb");
    fseek(graphicsFile, 0, SEEK_END);
    int grpLen = ftell(graphicsFile);
    fseek(graphicsFile, 0, SEEK_SET);
    u8 *pixels = (u8*)malloc(grpLen + 1);
    pixels[grpLen] = 0;
    fread(pixels, 1, grpLen, graphicsFile);
    fclose(graphicsFile);

    FILE* paletteFile = fopen("nitro:/graphics/logo_aqi.pal", "rb");
    fseek(paletteFile, 0, SEEK_END);
    int palLen = ftell(paletteFile);
    fseek(paletteFile, 0, SEEK_SET);
    u8 *palette = (u8*)malloc(palLen + 1);
    palette[palLen] = 0;
    fread(palette, 1, palLen, paletteFile);
    fclose(paletteFile);
    
    NE_Material *material = NE_MaterialCreate();
    NE_Palette *pal = NE_PaletteCreate();

    NE_PaletteLoad(pal, (u16 *)palette, 256, NE_PAL256);
    NE_MaterialTexLoad(material, NE_PAL256, 256, 256, NE_TEXGEN_OFF, (void *)pixels);
    NE_MaterialSetPalette(material, pal);

    NE_Sprite *bgTex = NE_SpriteCreate();
    NE_SpriteSetPos(bgTex, 0, 0);
    NE_SpriteSetSize(bgTex, 256, 256);
    NE_SpriteSetPriority(bgTex, 256);
    NE_SpriteSetMaterial(bgTex, material);
    
    while (true)
    {
        NE_Process(Draw3DScene);
    }

    return 0;
}