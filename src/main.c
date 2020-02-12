#include "global.h"
#include "m4a.h"

void (*gIntrTable[14])();

extern u8 gMugiTileSheet[];
extern u8 gMugiTilePalette[];
static void IntrDummy();
static void VBlankIntr();
static void Dma3Intr(void);

static void (*IntrTableFunctionPtrs[])() =
{
    VBlankIntr,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy
};

void ClearRamAndInitInterrupts(void)
{
	u32 i;
	
    RegisterRamReset(RESET_SIO_REGS | RESET_SOUND_REGS | RESET_REGS);
    DmaFill32(3, 0, IWRAM_START, 0x7E00);  // Clear IWRAM
    DmaFill32(3, 0, EWRAM_START, 0x40000); // Clear EWRAM

    RegisterRamReset(RESET_OAM | RESET_VRAM | RESET_PALETTE);

    for (i = 0; i < ARRAY_COUNT(IntrTableFunctionPtrs); i++)
    {
        gIntrTable[i] = IntrTableFunctionPtrs[i];
    }

    REG_WAITCNT = WAITCNT_WS0_N_3 | WAITCNT_WS0_S_1 | WAITCNT_WS1_N_4 | WAITCNT_WS1_S_4 | WAITCNT_WS2_N_4 | WAITCNT_WS2_S_8 | WAITCNT_PHI_OUT_NONE | WAITCNT_PREFETCH_ENABLE;
    REG_IE = INTR_FLAG_VBLANK;
    REG_DISPSTAT = DISPSTAT_VBLANK_INTR;
    REG_IME = TRUE;
}

void IntrDummy(void)
{
}

void InitBG3Tilemap(void)
{
	u32 x, y;
	u16 * charbase31 = (u16 *)0x600F800;
	for(y = 0; y < 20; y++)
	{
		for(x = 0; x < 30; x++)
			*charbase31++ = x + y * 30;
		charbase31+=2;
	}
}

struct Main {
	vu32 frameCounter;
	volatile bool8 drawNextFrame;
};

struct Main gMain;

void VBlankIntr(void)
{
	gMain.frameCounter++;
	gMain.drawNextFrame = TRUE;
}

void AgbMain(void)
{
	u32 offset = 0;
	
	ClearRamAndInitInterrupts();
	gMain.frameCounter = 0;
	gMain.drawNextFrame = TRUE;
	REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(31) | BGCNT_256COLOR; 
	InitBG3Tilemap();
	REG_DISPCNT = (1 << 11) | (1 << 13); 
	REG_WINOUT = 0x8;
	DmaCopy16(3, gMugiTilePalette, 0x5000000, 512); // load palette
    for (;;)
	{
		//SetLCDIORegs();
		// a little too slow because the orignal video was 24 fps.. 
		// would technically have to wait 2.5 frames for it to look alright
		// and that's not possible but we could do frame interleaving but eh
		// this is good enough for now
		if(gMain.drawNextFrame && (gMain.frameCounter % 2) == 0) 
		{
			gMain.drawNextFrame = FALSE;
			DmaSet(3, gMugiTileSheet + offset * 30*20*64, (u16 *)0x6000000, (DMA_ENABLE | DMA_START_VBLANK | DMA_16BIT | DMA_SRC_INC | DMA_DEST_INC) << 16 | ((30*20*64)/(16/8)));
			if(offset < 20) 
				offset++;
			else 
				offset = 0;
		}	
	}
}