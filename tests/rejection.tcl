set throttle off
set output [file normalize [file join [file dirname [info script]] .. out]]
after time 5 {type_via_keybuf "\r"}
after time 35 {
	set ::throttle on
	after time 0.3 {
		screenshot -raw [file join $::output "$::env(ALCATRAZ_TEST_NAME).png"]
		exit
	}
}
