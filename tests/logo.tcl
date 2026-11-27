set throttle off
set output [file normalize [file join [file dirname [info script]] .. out logo-test]]
after time 5 {type_via_keybuf "\r"}
proc find_logo {} {
	set mode [debug read {VDP regs} 0]
	set display [debug read {VDP regs} 1]
	if {($mode & 14) == 2 && ($display & 64) &&
		[debug read VRAM 0x092a] != 0} {
		set file [open [file join $::output logo.vram] wb]
		fconfigure $file -translation binary
		puts -nonewline $file [debug read_block VRAM 0 6144]
		puts -nonewline $file [debug read_block VRAM 8192 6144]
		close $file
		set file [open [file join $::output logo.palette] wb]
		fconfigure $file -translation binary
		puts -nonewline $file [debug read_block {VDP palette} 0 32]
		close $file
		set ::throttle on
		after time 0.3 {
			screenshot -raw [file join $::output logo.png]
			set ::throttle off
			after time 1 check_pause
		}
	} else {
		after time 0.02 find_logo
	}
}
proc check_pause {} {
	set ::held [expr {([debug read {VDP regs} 0] & 14) == 2 &&
		([debug read {VDP regs} 1] & 64) != 0}]
	after time 7 check_selection
}
proc check_selection {} {
	set file [open [file join $::output logo.log] w]
	puts $file [expr {$::held && ([debug read {VDP regs} 0] & 14) == 6 ? "PASS" : "FAIL"}]
	close $file
	exit
}
after time 6 find_logo
after time 60 {exit}
