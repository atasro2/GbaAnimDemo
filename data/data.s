	.section .data
    .include "asm/macros.inc"
	
    .global gMugiTileSheet
gMugiTileSheet:
	.incbin "graphics/mugi_tiles.8bpp"
	
	.global gMugiTilePalette
gMugiTilePalette:
	.incbin "graphics/mugi_tiles.gbapal"
