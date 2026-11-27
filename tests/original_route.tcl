set throttle off
set outdir $::env(ALCATRAZ_ROUTE_WORK)
source [file join $outdir events.tcl]
set log [open [file join $outdir route.log] w]
set index 0

proc line {} {
    expr {[debug read memory 0xf41c] + 256 * [debug read memory 0xf41d]}
}

proc fail {message} {
    puts $::log "FAIL $message line=[line]"
    close $::log
    exit
}

proc next_event {} {
    if {$::index >= [llength $::events]} {
        after time 5 check_victory
        return
    }
    set event [lindex $::events $::index]
    incr ::index
    puts $::log "$::index: $event"
    flush $::log
    if {[lindex $event 0] eq "cmd"} {
        type_via_keybuf "[lindex $event 1]\r"
        after time 5 next_event
    } else {
        set mask [dict get {N 0x20 S 0x40 L 0x80 O 0x10} [lindex $event 1]]
        set x [lindex $event 2]
        set y [lindex $event 3]
        set address [expr {6144 + $x * 2 + $y * 64 + 3}]
        keymatrixdown 8 $mask
        after time 0.0005 [list wait_move $address $mask 0]
    }
}

proc wait_move {address mask attempt} {
    if {[debug read VRAM $address] == 14} {
        keymatrixup 8 $mask
        after time 0.05 next_event
    } elseif {$attempt > 10000} {
        keymatrixup 8 $mask
        fail "movimento falhou endereço=$address evento=$::index"
    } else {
        after time 0.0005 [list wait_move $address $mask [expr {$attempt + 1}]]
    }
}

proc check_victory {} {
    if {[line] != 3020} {fail "vitória não detectada"}
    puts $::log "PASS vitória BASIC em 3020"
    close $::log
    exit
}

after time 5 {type_via_keybuf "run \"ALCATRAZ.BAS\"\r"}
after time 50 {
    keymatrixdown 8 0x01
    after time 0.3 {keymatrixup 8 0x01}
}
after time 90 next_event
