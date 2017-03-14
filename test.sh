echo "
		ld a, (bc)
		inc c
		inc c
		add (hl)
		dec e
" | ./z802nops

