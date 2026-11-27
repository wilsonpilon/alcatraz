set throttle off
set output [file normalize [file join [file dirname [info script]] .. out sc5-test]]
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
					set px [expr {$x + $col}]
					binary scan $image @[expr {($y + $row) * 128 + $px / 2}]c pair
					set pixel [expr {$px % 2 ? ($pair & 15) : (($pair >> 4) & 15)}]
					if {$pixel != $white} {return 0}
				}
			}
		}
		incr x 6
	}
	return 1
}
proc read_font {symbols name} {
	if {![regexp "DEF _$name (0x\[0-9A-Fa-f\]+)" $symbols -> address]} {
		error "missing font $name"
	}
	set file [open [file join $::output alcatraz.com] rb]
	fconfigure $file -translation binary
	seek $file [expr {$address - 0x100}]
	set font [read $file 2052]
	close $file
	return $font
}
proc probe_title {} {
	if {!$::probing} {return}
	incr ::tick
	set image [debug read_block VRAM 0 27136]
	if {([debug read {VDP regs} 1] & 0x40) && $image eq $::expected} {
		if {!$::initial_seen} {
			set ::initial_seen 1
			set ::initial_tick $::tick
			screenshot -raw [file join $::output initial.png]
		}
	}
	set left 0
	set right 0
	for {set y 52} {$y < 90} {incr y} {
		binary scan [string range $image [expr {$y * 128 + 8}] [expr {$y * 128 + 52}]] c* bytes
		foreach value $bytes {
			if {($value & 15) == 15} {incr left}
			if {(($value >> 4) & 15) == 15} {incr left}
		}
		binary scan [string range $image [expr {$y * 128 + 100}] [expr {$y * 128 + 120}]] c* bytes
		foreach value $bytes {
			if {($value & 15) == 15} {incr right}
			if {(($value >> 4) & 15) == 15} {incr right}
		}
	}
	if {([debug read {VDP regs} 1] & 0x40) && $left > 100 && $right == 0 && !$::partial_seen} {
		set ::partial_seen 1
		set ::partial_tick $::tick
		screenshot -raw [file join $::output partial.png]
		if {$::env(ALCATRAZ_SC5_CASE) eq "animation-escape"} {
			set ::probing 0
			keymatrixdown 7 0x04
			after time 0.3 {keymatrixup 7 0x04}
			after time 5 check_animation_exit
			return
		}
	}
	if {$::partial_seen && $right > 100 && !$::full_seen} {
		set ::full_seen 1
		set ::full_tick $::tick
	}
	after time 0.05 probe_title
}
proc check_animation_exit {} {
	set log [open [file join $::output animation-escape.log] w]
	puts $log [expr {$::initial_seen && $::partial_seen ? "PASS" : "FAIL"}]
	puts $log [expr {([debug read {VDP regs} 0] & 14) != 6 ? "PASS_EXIT" : "FAIL_EXIT"}]
	close $log
	exit
}
after time 5 {type_via_keybuf "\r"}
after time 35 {
	set ::diagnostic [debug read_block VRAM 0 27136]
	keymatrixdown 7 0x80
	after time 2 {
		set ::held_ok [expr {[debug read_block VRAM 0 27136] eq $::diagnostic}]
		keymatrixup 7 0x80
		set case $::env(ALCATRAZ_SC5_CASE)
		if {$case eq "valid" || $case eq "valid-escape" || $case eq "animation-escape"} {
			set throttle on
			set file [open [file join $::output ALCATRAZ.SC5] rb]
			fconfigure $file -translation binary
			seek $file 7
			set ::expected [read $file 27136]
			close $file
			set ::initial_seen 0
			set ::partial_seen 0
			set ::full_seen 0
			set ::tick 0
			set ::probing 1
			after time 0.05 probe_title
		}
		if {$case eq "valid"} {
			after time 0.1 {keymatrixdown 8 0x01}
		}
		if {$case eq "animation-escape"} {
			after time 30 check_animation_exit
		} else {
			after time 30 inspect
		}
	}
}
proc inspect {} {
	set ::probing 0
	set image [debug read_block VRAM 0 27136]
	set case $::env(ALCATRAZ_SC5_CASE)
	set ok [expr {$::held_ok && ([debug read {VDP regs} 0] & 14) == 6 &&
		([debug read {VDP regs} 9] & 128) != 0}]
	if {$case eq "valid" || $case eq "valid-escape"} {
		set expected $::expected
		set file [open [file join $::output .. opening_palette.h] r]
		set palette_source [read $file]
		close $file
		regexp {#define OPENING_WHITE ([0-9]+)} $palette_source -> white
		set file [open [file join $::output .. alcatraz.noi] r]
		set symbols [read $file]
		close $file
		set font [read_font $symbols g_Font_MGL_Std1]
		set ::game_font [read_font $symbols g_Font_MGL_Sample6]
		set text_ok 1
		foreach {x y text} {
			41 20 {Programa de Wilson F. Martins}
			32 30 {Remake por Wilson "Barney" Pilon}
			77 94 {A FUGA IMPOSSIVEL}
			47 184 {Tecle [ESPACO] para comecar}
		} {
			if {![text_matches $image $font $white $x $y $text]} {set text_ok 0}
		}
		set bytes {}
		foreach value [regexp -all -inline {0x[0-9a-f]+} $palette_source] {
			lappend bytes [expr {$value}]
		}
		set expected_palette [binary format c* $bytes]
		set unchanged 1
		set changes {0 0 0 0}
		for {set y 0} {$y < 212} {incr y} {
			set band -1
			if {$y >= 20 && $y <= 38} {set band 0}
			if {$y >= 52 && $y <= 90} {set band 1}
			if {$y >= 92 && $y <= 104} {set band 2}
			if {$y >= 181 && $y <= 195} {set band 3}
			set start [expr {$y * 128}]
			set before [string range $expected $start [expr {$start + 127}]]
			set after [string range $image $start [expr {$start + 127}]]
			if {$band < 0 && $before ne $after} {set unchanged 0}
			if {$band >= 0 && $before ne $after} {lset changes $band 1}
		}
		set red 0
		binary scan [string range $image [expr {52 * 128}] [expr {91 * 128 - 1}]] c* bytes
		foreach value $bytes {
			if {($value & 15) == 15} {incr red}
			if {(($value >> 4) & 15) == 15} {incr red}
		}
		set ok [expr {$ok && $::initial_seen && $::partial_seen && $::full_seen && $unchanged && $changes eq {1 1 1 1} && $red > 100 && $text_ok &&
			[debug read_block {VDP palette} 0 32] eq $expected_palette}]
		set detail [open [file join $::output "${case}-detail.log"] w]
		puts $detail "initial=$::initial_seen partial=$::partial_seen unchanged=$unchanged bands=$changes accent=$red text=$text_ok"
		if {$::initial_seen && $::partial_seen && $::full_seen} {
			puts $detail "initial_to_partial=[expr {($::partial_tick - $::initial_tick) * 0.05}]s initial_to_last_letter=[expr {($::full_tick - $::initial_tick) * 0.05}]s"
		}
		close $detail
		set ::title $image
	} elseif {$case eq "incompatible"} {
		set ok [expr {$ok && $image eq $::diagnostic}]
	} else {
		set errorfile [file join $::output error.vram]
		if {$case eq "missing"} {
			set file [open $errorfile wb]
			fconfigure $file -translation binary
			puts -nonewline $file $image
			close $file
			set area [string range $image [expr {80 * 128 + 10}] [expr {88 * 128 + 126}]]
			set ok [expr {$ok && $area ne [string repeat \x00 [string length $area]]}]
		} else {
			set file [open $errorfile rb]
			fconfigure $file -translation binary
			set expected [read $file]
			close $file
			set ok [expr {$ok && $image eq $expected}]
		}
	}
	set log [open [file join $::output "${case}.log"] w]
	puts $log [expr {$ok ? "PASS" : "FAIL"}]
	close $log
	set ::throttle on
	after time 0.3 [list snapshot $case]
}
proc snapshot {case} {
	screenshot -raw [file join $::output "${case}.png"]
	if {$case eq "valid"} {
		after time 2 {
			set ::space_held_ok [expr {[debug read_block VRAM 0 27136] eq $::title}]
			keymatrixup 8 0x01
			after time 0.5 {
				keymatrixdown 8 0x01
				after time 0.5 {keymatrixup 8 0x01}
				after time 3 check_start
			}
		}
		return
	}
	exit_with_escape $case
}
proc check_start {} {
	set image [debug read_block VRAM 0 27136]
	set text1 [text_matches $image $::game_font 4 56 90 "JOGO EM DESENVOLVIMENTO"]
	set text2 [text_matches $image $::game_font 4 100 106 "ESC: SAIR"]
	set file [open [file join $::output start-detail.log] w]
	puts $file "held=$::space_held_ok changed=[expr {$image ne $::title}] text1=$text1 text2=$text2"
	close $file
	screenshot -raw [file join $::output start.png]
	set ::start_ok [expr {$::space_held_ok && $image ne $::title &&
		([debug read {VDP regs} 0] & 14) == 6 && $text1 && $text2}]
	exit_with_escape valid
}
proc exit_with_escape {case} {
	keymatrixdown 7 0x04
	after time 0.2 {keymatrixup 7 0x04}
	after time 5 [list check_exit $case]
}
proc check_exit {case} {
	set log [open [file join $::output "${case}.log"] a]
	set ok [expr {([debug read {VDP regs} 0] & 14) != 6}]
	if {$case eq "valid"} {set ok [expr {$ok && $::start_ok}]}
	puts $log [expr {$ok ? "PASS_EXIT" : "FAIL_EXIT"}]
	close $log
	exit
}
