#include "global.h"

enum
{
    VIDEO_STATE_IDLE,
    VIDEO_STATE_DRAW,
    VIDEO_STATE_DECOMPRESS
};

struct VideoState
{
    u32 paletteSize;
    u32 frameNum;
    u32 length; 
    u32 fps;
    u32 frameDelay;
    u32 videoState;
    bool32 loop;
    void * videoDataTop;
    void * videoDataPtr;
};

void RLDecompress(void * src, void * dst);

u8 RLDecompressWramBuf[0x80];
void (*RLDecompressWram)(void * src, void * dst);

EWRAM_DATA static u8 sFrameDecompBuffer[20*30*0x40];
static struct VideoState sVideoState;

void PlayVideo(void * video, bool32 loop)
{
    DmaCopy16(3, RLDecompress, RLDecompressWramBuf, 0x80); // moving more data than needed! shouldn't be a problem?
    RLDecompressWram = (void (*)(void * src, void * dst))RLDecompressWramBuf;
    sVideoState.videoDataPtr = video;
    sVideoState.length = READ_INC(sVideoState.videoDataPtr, u32);
    sVideoState.fps = READ_INC(sVideoState.videoDataPtr, u32);
    sVideoState.videoDataTop = sVideoState.videoDataPtr;
    sVideoState.frameDelay = Div(60, sVideoState.fps);
    sVideoState.frameNum = 0;
    sVideoState.loop = loop;
    sVideoState.videoState = VIDEO_STATE_DECOMPRESS;
}

void VideoDecompress(void)
{
    if(sVideoState.videoState == VIDEO_STATE_IDLE)
        return;
    
    if(sVideoState.videoState == VIDEO_STATE_DECOMPRESS)
    {
        sVideoState.paletteSize = READ_INC(sVideoState.videoDataPtr, u32);
        RLDecompressWram(sVideoState.videoDataPtr+sVideoState.paletteSize+0x4, sFrameDecompBuffer);
        sVideoState.videoState = VIDEO_STATE_DRAW;
        if(sVideoState.frameDelay > 1)
            return;
    }
}
void VideoVSync(void)
{
    if(sVideoState.videoState == VIDEO_STATE_IDLE)
        return;

    if(sVideoState.videoState == VIDEO_STATE_DRAW && (gMain.frameCounter % sVideoState.frameDelay) == 0)
    {
        if(sVideoState.frameNum < sVideoState.length-1)
        {
            DmaCopy16(3, sVideoState.videoDataPtr, 0x5000000, sVideoState.paletteSize);
            sVideoState.videoDataPtr += sVideoState.paletteSize;
            DmaCopy16(3, sFrameDecompBuffer, 0x6000000, 30*20*64);
            sVideoState.videoDataPtr += READ_INC(sVideoState.videoDataPtr, u32);
            sVideoState.videoState = VIDEO_STATE_DECOMPRESS;
            sVideoState.frameNum++;
        }
        else
        {
            sVideoState.frameNum = 0;
            sVideoState.videoDataPtr = sVideoState.videoDataTop;
            sVideoState.videoState = sVideoState.loop ? VIDEO_STATE_DECOMPRESS : VIDEO_STATE_IDLE;
        }
    }
}