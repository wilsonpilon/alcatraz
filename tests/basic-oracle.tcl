set throttle off
set output [file normalize [file join [file dirname [info script]] .. out logo-test]]
after time 8 {type_via_keybuf "run \"DISPRO.BAS\"\r"}
after time 50 {
	set file [open [file join $::output basic.vram] wb]
	fconfigure $file -translation binary
	puts -nonewline $file [debug read_block VRAM 0 6144]
	puts -nonewline $file [debug read_block VRAM 8192 6144]
	close $file
	set file [open [file join $::output basic.palette] wb]
	fconfigure $file -translation binary
	puts -nonewline $file [debug read_block {VDP palette} 0 32]
	close $file
	set ::throttle on
	after time 0.3 {
		screenshot -raw [file join $::output basic.png]
		exit
	}
}
