#include "global.h"
#include "video.h"
#include "sound.h"

void (*gIntrTable[14])(void);
u32 gInterruptVector[0x20] = {0};
struct Main gMain;

// interrupt stuff for moving to ram
extern void _intr(void);
extern u8 interrupt_end;
//

extern u8 sMusicCagayakeGirls[];
extern u8 sVideoKonSeason1Intro[];

static void IntrDummy(void);
static void VBlankIntr(void);

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

void ReadKeys(void)
{
    gMain.joypad.heldKeys = gMain.joypad.heldKeysRaw;
    gMain.joypad.newKeys = gMain.joypad.newKeysRaw;
    gMain.joypad.heldKeysRaw = KEY_NEW();
    gMain.joypad.newKeysRaw = KEY_NEW() & ~gMain.joypad.heldKeys;
    
}

void ClearRamAndInitInterrupts(void)
{
    u32 i;
	
    RegisterRamReset(RESET_SIO_REGS | RESET_SOUND_REGS | RESET_REGS);
    DmaFill32(3, 0, IWRAM_START, 0x7F80);
    DmaFill32(3, 0, EWRAM_START, 0x30000);

    RegisterRamReset(RESET_OAM | RESET_VRAM | RESET_PALETTE);
	DmaCopy32(3, IntrTableFunctionPtrs, gIntrTable, sizeof(gIntrTable));
	DmaCopy32(3, _intr, gInterruptVector, (u32)&interrupt_end - (u32)&_intr);
    REG_WAITCNT = WAITCNT_WS0_N_3 | WAITCNT_WS0_S_1 | WAITCNT_WS1_N_4 | WAITCNT_WS1_S_4 | WAITCNT_WS2_N_4 | WAITCNT_WS2_S_8 | WAITCNT_PHI_OUT_NONE | WAITCNT_PREFETCH_ENABLE;
    INTR_VECTOR = &gInterruptVector;
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

void VBlankIntr(void)
{
	gMain.frameCounter++;
	SoundVSync();
	VideoVSync();
}

void AgbMain(void) // could technically be 30 FPS with my current video setup
{
	ClearRamAndInitInterrupts();
	gMain.frameCounter = 0;
	InitBG3Tilemap();
	REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(31) | BGCNT_256COLOR; 
	REG_DISPCNT = DISPCNT_BG3_ON | DISPCNT_WIN0_ON; 
	REG_WINOUT = WINOUT_WIN01_BG3;
	PlayVideo(sVideoKonSeason1Intro, FALSE);
	InitSound();
	StartPCMStream(sMusicCagayakeGirls, FALSE);
	for (;;)
	{
		if(gMain.joypad.heldKeys == (A_BUTTON|B_BUTTON|START_BUTTON|SELECT_BUTTON))
			break;		
		ReadKeys();
		if(gMain.joypad.newKeys & A_BUTTON)
			StartPCMStream(sMusicCagayakeGirls, FALSE);		
		VideoDecompress();
		VBlankIntrWait();
	}
	DmaFill16(3, 0, 0x5000000, 0x400);
	SoftReset(RESET_ALL);
}
