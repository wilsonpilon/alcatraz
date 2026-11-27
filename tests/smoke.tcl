set throttle off
set output [file normalize [file join [file dirname [info script]] .. out]]
set symbols [open [file join $output alcatraz.noi] r]
set symbols_data [read $symbols]
close $symbols
if {![regexp {DEF _g_Font_MGL_Sample6 (0x[0-9A-Fa-f]+)} $symbols_data -> address]} {
	error "missing diagnostic font"
}
set file [open [file join $output alcatraz.com] rb]
fconfigure $file -translation binary
seek $file [expr {$address - 0x100}]
set font [read $file 2052]
close $file

proc text_matches {image x y color text} {
	binary scan $::font @2cu first
	foreach character [split $text ""] {
		scan $character %c code
		for {set row 0} {$row < 8} {incr row} {
			binary scan $::font @[expr {4 + ($code - $first) * 8 + $row}]c bits
			for {set col 0} {$col < 6} {incr col} {
				if {$bits & (0x80 >> $col)} {
					set px [expr {$x + $col}]
					binary scan $image @[expr {($y + $row) * 128 + $px / 2}]c pair
					set pixel [expr {$px % 2 ? ($pair & 15) : (($pair >> 4) & 15)}]
					if {$pixel != $color} {return 0}
				}
			}
		}
		incr x 6
	}
	return 1
}
after time 5 {type_via_keybuf "\r"}
after time 27 {
	keymatrixdown 7 0x80
	after time 0.3 {keymatrixup 7 0x80}
}
after time 35 {
	set ::throttle on
	after time 0.3 capture
}
proc capture {} {
	set screen [expr {([debug read {VDP regs} 0] & 14) == 6}]
	set empty_row 1
	set pixels [debug read_block VRAM 0 27136]
	set mapper [text_matches $pixels 180 90 4 "OK"]
	set mapper_fail [text_matches $pixels 180 90 3 "FALHA"]
	set psg [text_matches $pixels 180 101 4 "OK"]
	set fm [text_matches $pixels 180 112 4 "OK"]
	set fm_fail [text_matches $pixels 180 112 3 "FALHA"]
	for {set y 123} {$y < 131} {incr y} {
		set row [string range $pixels [expr {$y * 128 + 16}] [expr {$y * 128 + 111}]]
		if {$row ne [string repeat \x11 96]} {set empty_row 0}
	}
	if {!($mapper || $mapper_fail) || !$psg || !($fm || $fm_fail)} {
		if {![info exists ::retries]} {set ::retries 0}
		if {[incr ::retries] < 30} {
			after time 0.1 capture
			return
		}
	}
	set name $::env(ALCATRAZ_TEST_NAME)
	set log [open [file join $::output "${name}.log"] w]
	set passed [expr {$screen && $empty_row && $psg &&
		($::env(ALCATRAZ_TEST_MAPPER) ? $mapper : $mapper_fail) &&
		($::env(ALCATRAZ_TEST_FM) ? $fm : $fm_fail)}]
	puts $log "screen=$screen mapper=$mapper psg=$psg fm=$fm empty_row=$empty_row"
	puts $log [expr {$passed ? "PASS" : "FAIL"}]
	close $log
	if {!$passed} {
		set file [open [file join $::output "${name}-failure.vram"] wb]
		fconfigure $file -translation binary
		puts -nonewline $file $pixels
		close $file
	}
	screenshot -raw [file join $::output "${name}.png"]
	exit
}
