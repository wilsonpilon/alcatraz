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
after time 35 {
	set ::throttle on
	after time 0.3 capture
}
proc capture {} {
	set name $::env(ALCATRAZ_TEST_NAME)
	set log [open [file join $::output "${name}.log"] w]
	set screen [expr {([debug read {VDP regs} 0] & 14) == 6}]
	set mapper [debug read memory $::addresses(mapper)]
	set psg [debug read memory $::addresses(psg)]
	set fm [debug read memory $::addresses(fm)]
	set passed [expr {$screen && $mapper == $::env(ALCATRAZ_TEST_MAPPER)
		&& $psg != 0 && ($fm != 0) == $::env(ALCATRAZ_TEST_FM)}]
	puts $log "screen=$screen mapper=$mapper psg=$psg fm=$fm"
	puts $log [expr {$passed ? "PASS" : "FAIL"}]
	close $log
	screenshot -raw [file join $::output "${name}.png"]
	exit
}
