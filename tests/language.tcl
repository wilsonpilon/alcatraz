set throttle off
set output [file normalize [file join [file dirname [info script]] .. out language-test]]
set case $::env(ALCATRAZ_LANGUAGE_CASE)
set file [open [file join $output .. alcatraz.noi] r]
set symbols [read $file]
close $file

proc read_font {name} {
	if {![regexp "DEF _$name (0x\[0-9A-Fa-f\]+)" $::symbols -> address]} {
		error "missing font $name"
	}
	set file [open [file join $::output .. alcatraz.com] rb]
	fconfigure $file -translation binary
	seek $file [expr {$address - 0x100}]
	set font [read $file 2052]
	close $file
	return $font
}

proc text_matches {image font white x y text} {
	binary scan $font @2cu first
	foreach character [split $text ""] {
		scan $character %c code
		if {$code == 32} {
			incr x 6
			continue
		}
		for {set row 0} {$row < 8} {incr row} {
			binary scan $font @[expr {4 + ($code - $first) * 8 + $row}]c bits
			for {set col 0} {$col < 6} {incr col} {
				if {$bits & (0x80 >> $col)} {
					if {[pixel $image [expr {$x + $col}] [expr {$y + $row}]] != $white} {return 0}
				}
			}
		}
		incr x 6
	}
	return 1
}

proc pixel {image x y} {
	binary scan $image @[expr {$y * 128 + $x / 2}]c pair
	expr {$x % 2 ? ($pair & 15) : (($pair >> 4) & 15)}
}

proc fail {message} {
	set file [open [file join $::output "$::case.log"] w]
	puts $file "FAIL $message"
	close $file
	exit
}

proc leave {} {
	keymatrixdown 7 0x04
	after time 0.3 {keymatrixup 7 0x04}
	after time 4 {
		if {([debug read {VDP regs} 0] & 14) == 6} {fail "Esc nao retornou ao DOS"}
		set file [open [file join $::output "$::case.log"] w]
		puts $file PASS
		close $file
		exit
	}
}

set font [read_font g_Font_MGL_Sample6]
set title_font [read_font g_Font_MGL_Std1]
after time 5 {type_via_keybuf "\r"}
after time 32 {
	set image [debug read_block VRAM 0 27136]
	if {([debug read {VDP regs} 0] & 14) != 6 ||
		![text_matches $image $::font 3 36 127 "PORTUGUES"] ||
		![text_matches $image $::font 3 168 127 "ENGLISH"] ||
		[pixel $image 17 61] != 6 || [pixel $image 139 61] != 8 ||
		[pixel $image 35 77] != 5 || [pixel $image 220 77] != 4} {
		fail "tela inicial ou bandeiras"
	}
	if {$::case eq "pt"} {
		set throttle on
		after time 0.3 {
			screenshot -raw [file join $::output selection.png]
			set throttle off
			proceed_selection
		}
	} else {
		proceed_selection
	}
}

proc proceed_selection {} {
	if {$::case eq "escape"} {
		leave
	} else {
		keymatrixdown 8 0x80
		after time 0.3 {keymatrixup 8 0x80}
		after time 1 check_right
	}
}

proc check_right {} {
	set image [debug read_block VRAM 0 27136]
	if {[pixel $image 17 61] != 8 || [pixel $image 139 61] != 6} {
		fail "selecao English"
	}
	if {$::case eq "pt"} {
		keymatrixdown 8 0x10
		after time 0.3 {keymatrixup 8 0x10}
		after time 1 {
			set image [debug read_block VRAM 0 27136]
			if {[pixel $image 17 61] != 6 || [pixel $image 139 61] != 8} {
				fail "retorno a Portugues"
			}
			confirm
		}
	} else {
		confirm
	}
}

proc confirm {} {
	set ::before [debug read_block VRAM 0 27136]
	keymatrixdown 7 0x80
	after time 1.5 {
		if {[debug read_block VRAM 0 27136] ne $::before} {
			fail "Enter mantido avancou sem soltar"
		}
		keymatrixup 7 0x80
		after time 2 diagnostic
	}
}

proc diagnostic {} {
	set image [debug read_block VRAM 0 27136]
	if {$::case eq "pt"} {
		if {![text_matches $image $::font 4 32 52 "VERIFICACAO DO SISTEMA"] ||
			![text_matches $image $::font 4 32 142 "SISTEMA COMPATIVEL"]} {
			fail "diagnostico portugues"
		}
		leave
		return
	}
	if {![text_matches $image $::font 4 32 52 "SYSTEM CHECK"] ||
		![text_matches $image $::font 4 64 31 "THE IMPOSSIBLE ESCAPE"] ||
		![text_matches $image $::font 4 20 161 "ENTER: OPEN  ESC: EXIT"]} {
		fail "diagnostico ingles"
	}
	if {$::case eq "en-nofm"} {
		if {![text_matches $image $::font 4 32 142 "REQUIREMENTS NOT MET"] ||
			![text_matches $image $::font 3 180 112 "FAIL"]} {
			fail "falha de FM em ingles"
		}
		keymatrixdown 7 0x80
		after time 0.3 {keymatrixup 7 0x80}
		after time 1 {
			if {[debug read_block VRAM 0 27136] ne $::image} {fail "maquina incompativel avancou"}
			leave
		}
		set ::image $image
		return
	}
	if {![text_matches $image $::font 4 32 142 "SYSTEM COMPATIBLE"]} {
		fail "compatibilidade em ingles"
	}
	keymatrixdown 7 0x80
	after time 0.3 {keymatrixup 7 0x80}
	if {$::case eq "en-missing"} {
		after time 3 check_missing
	} else {
		after time 22 check_title
	}
}

proc check_missing {} {
	set image [debug read_block VRAM 0 27136]
	if {![text_matches $image $::font 4 20 80 "ERROR READING ALCATRAZ.SC5"] ||
		![text_matches $image $::font 4 20 96 "ESC: EXIT"]} {
		fail "erro de imagem em ingles"
	}
	leave
}

proc check_title {} {
	set image [debug read_block VRAM 0 27136]
	set file [open [file join $::output .. opening_palette.h] r]
	set palette [read $file]
	close $file
	regexp {#define OPENING_WHITE ([0-9]+)} $palette -> white
	foreach {x y text} {
		44 20 {Program by Wilson F. Martins}
		35 30 {Remake by Wilson "Barney" Pilon}
		65 94 {THE IMPOSSIBLE ESCAPE}
		62 184 {Press [SPACE] to start}
	} {
		if {![text_matches $image $::title_font $white $x $y $text]} {
			fail "titulo em ingles: $text"
		}
	}
	keymatrixdown 8 0x01
	after time 0.3 {keymatrixup 8 0x01}
	after time 10 check_placeholder
}

proc check_placeholder {} {
	set image [debug read_block VRAM 0 27136]
	if {![text_matches $image $::font 4 70 90 "GAME IN DEVELOPMENT"] ||
		![text_matches $image $::font 4 100 106 "ESC: EXIT"]} {
		fail "tela provisoria em ingles"
	}
	leave
}

after time 120 {fail "tempo limite"}
