	.section .data
    .include "asm/macros.inc"

	.align 2, 0
	.global sMusicCagayakeGirls
sMusicCagayakeGirls:
	.incbin "sound/shinegirls.pcm"

	.align 2, 0
	.global sVideoKonSeason1Intro
sVideoKonSeason1Intro:
	.incbin "data/video.bin"
