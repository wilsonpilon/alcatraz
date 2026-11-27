set throttle off
set output [file normalize [file join [file dirname [info script]] .. out]]
set symbols [open [file join $output alcatraz.noi] r]
set symbols_data [read $symbols]
close $symbols
foreach name {mapper psg fm} {
	if {![regexp "DEF _g_${name}_ok (0x\[0-9A-Fa-f\]+)" $symbols_data -> address]} {
		error "missing symbol: g_${name}_ok"
	}
	set addresses($name) $address
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
	set mapper [debug read memory $::addresses(mapper)]
	set psg [debug read memory $::addresses(psg)]
	set fm [debug read memory $::addresses(fm)]
	set empty_row 1
	set pixels [debug read_block VRAM 0 27136]
	for {set y 123} {$y < 131} {incr y} {
		set row [string range $pixels [expr {$y * 128 + 16}] [expr {$y * 128 + 111}]]
		if {$row ne [string repeat \x11 96]} {set empty_row 0}
	}
	# The BIOS may temporarily map ROM over the program's page during an interrupt.
	if {$mapper > 1 || $psg > 1 || $fm > 1} {
		if {![info exists ::retries]} {set ::retries 0}
		if {[incr ::retries] < 30} {
			after time 0.1 capture
			return
		}
	}
	set name $::env(ALCATRAZ_TEST_NAME)
	set log [open [file join $::output "${name}.log"] w]
	set passed [expr {$screen && $empty_row && $mapper == $::env(ALCATRAZ_TEST_MAPPER)
		&& $psg != 0 && ($fm != 0) == $::env(ALCATRAZ_TEST_FM)}]
	puts $log "screen=$screen mapper=$mapper psg=$psg fm=$fm empty_row=$empty_row"
	puts $log [expr {$passed ? "PASS" : "FAIL"}]
	close $log
	screenshot -raw [file join $::output "${name}.png"]
	exit
}
