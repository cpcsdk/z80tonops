		exx
		rra
		jr nz, .bar1_\@
.bar2_\@
		ld l, a
		BAR0_DISPLAY_LEFT_BAR
		jr .eob_\@
.bar1_\@
		ld l, a
		BAR0_DISPLAY_RIGHT_BAR
.eob_\@
		exx
