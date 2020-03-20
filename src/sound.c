#include "global.h"
#include "sound.h"

struct Sound
{
	void * pcmFilePtrTop;
	void * pcmFilePtr;
	u8 * audioBuffer; 
	u32 fileSize;
	u32 bufferSize;
	bool32 loop;
	u8 usedBuffer;
	u8 audioBuffer1[704];
	u8 audioBuffer2[704];
};

struct Sound gSound; 

void InitSound(void)
{
    REG_SOUNDCNT_H = SOUND_A_MIX_FULL | SOUND_A_RIGHT_OUTPUT | SOUND_A_LEFT_OUTPUT | SOUND_A_TIMER_0 | SOUND_A_FIFO_RESET;
}

void StartPCMStream(void * pcmFile, bool32 loop)
{
	u32 soundFreq = 0; // Hz
	u32 size = 0;
	LOAD_U32_LE(soundFreq, (u8 *)pcmFile + 4);
	soundFreq /= 1024;
	size = DivRoundNearest(soundFreq * 4389, 262144); // magic!
	gSound.loop = loop;
	gSound.bufferSize = (u16)size;
	gSound.pcmFilePtrTop = pcmFile + 0x10;
	gSound.pcmFilePtr = pcmFile + 0x10;
	gSound.usedBuffer = 0;
	LOAD_U32_LE(gSound.fileSize, (u8 *)pcmFile + 0xC);
    REG_TMCNT_L(0) = 65536 - DivRoundNearest(16777216, soundFreq);
    REG_TMCNT_H(0) = TIMER_ENABLE;
	gSound.audioBuffer = gSound.audioBuffer1;
    REG_DMA1SAD = (u32) gSound.audioBuffer1;
    REG_DMA1DAD = REG_ADDR_FIFO_A;
    REG_DMA1CNT_H = (DMA_ENABLE | DMA_REPEAT | DMA_START_SPECIAL | DMA_32BIT | DMA_SRC_INC | DMA_DEST_FIXED);
	REG_SOUNDCNT_X = SOUND_MASTER_ENABLE;
}

void SoundVSync(void)
{
	u32 size = gSound.bufferSize;
	if(gSound.pcmFilePtr == NULL)
		return;

	if(gSound.pcmFilePtr >= gSound.pcmFilePtrTop+gSound.fileSize)
	{
		if(gSound.loop == TRUE)
			gSound.pcmFilePtr = gSound.pcmFilePtrTop;
		else
		{
			REG_SOUNDCNT_X = 0;
			DmaFill32(2, 0, gSound.audioBuffer1, sizeof(gSound.audioBuffer1)*2);
			gSound.pcmFilePtrTop = gSound.pcmFilePtr = NULL;
			REG_DMA1CNT_H = 0;
			return;
		}
	}
	if(gSound.usedBuffer == 1)
	{
		DmaCopy32(2, gSound.pcmFilePtr, gSound.audioBuffer1, size);
		REG_DMA1CNT_H = 0;
		gSound.audioBuffer = gSound.audioBuffer1;
		REG_DMA1SAD = (u32)(gSound.audioBuffer1);
		REG_DMA1CNT_H = (DMA_ENABLE | DMA_REPEAT | DMA_START_SPECIAL | DMA_32BIT | DMA_SRC_INC | DMA_DEST_FIXED);
	}
	else
	{
		DmaCopy32(2, gSound.pcmFilePtr, gSound.audioBuffer2, size);
		REG_DMA1CNT_H = 0;
		gSound.audioBuffer = gSound.audioBuffer2;
		REG_DMA1SAD = (u32)(gSound.audioBuffer2);
		REG_DMA1CNT_H = (DMA_ENABLE | DMA_REPEAT | DMA_START_SPECIAL | DMA_32BIT | DMA_SRC_INC | DMA_DEST_FIXED);
	}
	gSound.pcmFilePtr += size;
	gSound.usedBuffer ^= 1;
}