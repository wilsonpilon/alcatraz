set throttle off
set output [file normalize [file join [file dirname [info script]] .. out sc5-test]]
after time 5 {type_via_keybuf "\r"}
after time 35 {
	set ::diagnostic [debug read_block VRAM 0 27136]
	keymatrixdown 7 0x80
	after time 2 {
		set ::held_ok [expr {[debug read_block VRAM 0 27136] eq $::diagnostic}]
		keymatrixup 7 0x80
		after time 30 inspect
	}
}
proc inspect {} {
	set image [debug read_block VRAM 0 27136]
	set case $::env(ALCATRAZ_SC5_CASE)
	set ok [expr {$::held_ok && ([debug read {VDP regs} 0] & 14) == 6 &&
		([debug read {VDP regs} 9] & 128) != 0}]
	if {$case eq "valid"} {
		set file [open [file join $::output ALCATRAZ.SC5] rb]
		fconfigure $file -translation binary
		seek $file 7
		set expected [read $file 27136]
		close $file
		set file [open [file join $::output .. opening_palette.h] r]
		set palette_source [read $file]
		close $file
		set bytes {}
		foreach value [regexp -all -inline {0x[0-9a-f]+} $palette_source] {
			lappend bytes [expr {$value}]
		}
		set expected_palette [binary format c* $bytes]
		set ok [expr {$ok && $image eq $expected &&
			[debug read_block {VDP palette} 0 32] eq $expected_palette}]
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
	keymatrixdown 7 0x04
	after time 0.2 {keymatrixup 7 0x04}
	after time 5 [list check_exit $case]
}
proc check_exit {case} {
	set log [open [file join $::output "${case}.log"] a]
	puts $log [expr {([debug read {VDP regs} 0] & 14) != 6 ? "PASS_EXIT" : "FAIL_EXIT"}]
	close $log
	exit
}
