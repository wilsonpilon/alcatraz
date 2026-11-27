set throttle off
set output [file normalize [file join [file dirname [info script]] .. out game-test]]
set case $::env(ALCATRAZ_GAME_CASE)
set file [open [file join $output .. alcatraz.noi] r]
set symbols [read $file]
close $file
if {![regexp {DEF _g_Font_MGL_Sample6 (0x[0-9A-Fa-f]+)} $symbols -> address]} {
	error "fonte nao encontrada"
}
set file [open [file join $output .. alcatraz.com] rb]
fconfigure $file -translation binary
seek $file [expr {$address - 0x100}]
set font [read $file 2052]
close $file

proc pixel {image x y} {
	binary scan $image @[expr {$y * 128 + $x / 2}]c pair
	expr {$x % 2 ? ($pair & 15) : (($pair >> 4) & 15)}
}

proc text_matches {image x y text} {
	binary scan $::font @2cu first
	foreach character [split $text ""] {
		scan $character %c code
		if {$code == 32} {
			incr x 6
			continue
		}
		for {set row 0} {$row < 8} {incr row} {
			binary scan $::font @[expr {4 + ($code - $first) * 8 + $row}]c bits
			for {set col 0} {$col < 6} {incr col} {
				if {($bits & (0x80 >> $col)) &&
					[pixel $image [expr {$x + $col}] [expr {$y + $row}]] != 9} {return 0}
			}
		}
		incr x 6
	}
	return 1
}

proc fail {message} {
	set file [open [file join $::output "$::case.log"] w]
	puts $file "FAIL $message"
	close $file
	exit
}

proc press {row mask} {
	keymatrixdown $row $mask
	after time 0.16 [list keymatrixup $row $mask]
}

proc send_next {} {
	if {![llength $::keys]} {
		after time 0.5 verify_step
		return
	}
	set key [lindex $::keys 0]
	set ::keys [lrange $::keys 1 end]
	if {$key eq "ENTER"} {
		press 7 0x80
	} elseif {$key eq "BACK"} {
		press 7 0x20
	} elseif {$key eq "LEFT"} {
		press 8 0x10
	} elseif {$key eq " "} {
		press 8 0x01
	} else {
		set index [string first $key ABCDEFGHIJKLMNOPQRSTUVWXYZ]
		if {$index < 0} {fail "tecla inesperada $key"}
		if {$index < 2} {
			set row 2
			set bit [expr {$index + 6}]
		} else {
			set row [expr {3 + ($index - 2) / 8}]
			set bit [expr {($index - 2) % 8}]
		}
		press $row [expr {1 << $bit}]
	}
	after time 0.34 send_next
}

proc next_step {} {
	if {$::step >= [llength $::steps]} {
		press 7 0x04
		after time 4 {
			if {([debug read {VDP regs} 0] & 14) == 6} {fail "Esc nao retornou ao DOS"}
			set file [open [file join $::output "$::case.log"] w]
			puts $file PASS
			close $file
			exit
		}
		return
	}
	lassign [lindex $::steps $::step] command expected inventory
	set ::expected $expected
	set ::inventory $inventory
	set ::command $command
	incr ::step
	if {$command eq "<LEFT>"} {
		set ::keys {LEFT}
	} elseif {$command eq "<FULL>"} {
		set ::keys [split [string repeat A 31] ""]
	} elseif {$command eq "<ENTER>"} {
		set ::keys {ENTER}
	} elseif {$command eq "<BACKSPACE>"} {
		if {$::case eq "pt"} {
			set ::keys {P E G U X BACK E { } C A M A ENTER}
		} else {
			set ::keys {T A K X BACK E { } B E D ENTER}
		}
	} else {
		set ::keys [split $command ""]
		lappend ::keys ENTER
	}
	send_next
}

proc verify_step {} {
	set image [debug read_block VRAM 0 27136]
	if {![text_matches $image 8 119 $::expected]} {
		fail "comando $::command: resposta esperada $::expected"
	}
	if {$::inventory eq "EMPTY"} {
		set label [expr {$::case eq "pt" ? "VAZIO" : "EMPTY"}]
		if {![text_matches $image 166 45 $label]} {fail "inventario deveria estar vazio"}
	} elseif {$::inventory ne "-"} {
		if {![text_matches $image 182 44 $::inventory]} {fail "inventario: esperado $::inventory"}
	}
	if {$::command eq "OLHE" && ![text_matches $image 68 130 ", CORDA"]} {
		fail "corda deixada na cela nao aparece na descricao"
	}
	if {$::command eq "LOOK" && ![text_matches $image 68 130 ", ROPE"]} {
		fail "rope deixada na cela nao aparece na descricao"
	}
	if {$::command in {"PEGUE LENCOL" "TAKE SHEET"} && [pixel $image 65 65] == 12} {
		fail "lencol carregado ou consumido ainda aparece na cama"
	}
	if {$::command in {"SOLTE LENCOL" "LEAVE SHEET"} && [pixel $image 65 65] != 12} {
		fail "lencol deixado nao aparece na cena"
	}
	next_step
}

proc start_game {} {
	set image [debug read_block VRAM 0 27136]
	set title [expr {$::case eq "pt" ? "ALCATRAZ  /  CELA" : "ALCATRAZ  /  CELL"}]
	set room [expr {$::case eq "pt" ? "VOCE ESTA EM SUA CELA." : "YOU ARE IN YOUR CELL."}]
	set sheet [expr {$::case eq "pt" ? ", LENCOL" : ", SHEET"}]
	set empty [expr {$::case eq "pt" ? "VAZIO" : "EMPTY"}]
	if {![text_matches $image 8 2 $title] || ![text_matches $image 8 119 $room] ||
		![text_matches $image 68 130 $sheet] || ![text_matches $image 166 45 $empty] ||
		[pixel $image 6 18] != 2 || [pixel $image 6 100] != 6 ||
		[pixel $image 65 65] != 12} {
		fail "tela inicial da cela, objetos ou paleta"
	}
	set throttle on
	after time 0.3 {
		screenshot -raw [file join $::output "$::case.png"]
		set throttle off
		set ::step 0
		next_step
	}
}

if {$case eq "pt"} {
	set steps {
		{N {UMA PAREDE BLOQUEIA A PASSAGEM.} EMPTY}
		{S {UMA PAREDE BLOQUEIA A PASSAGEM.} EMPTY}
		{L {UMA PAREDE BLOQUEIA A PASSAGEM.} EMPTY}
		{O {OESTE: GUARDA NO CORREDOR.} EMPTY}
		{<LEFT> {OESTE: GUARDA NO CORREDOR.} EMPTY}
		{<BACKSPACE> {NAO DA PARA PEGAR A CAMA.} EMPTY}
		{{EXAMINE CAMA} {A CAMA E FEITA DE TIJOLOS.} EMPTY}
		{{PEGUE LENCOL} {PEGOU.} LENCOL}
		{{PEGUE LENCOL} {VOCE JA ESTA CARREGANDO.} LENCOL}
		{{SOLTE LENCOL} {DEIXOU NA CELA.} EMPTY}
		{{RASGUE LENCOL} {O LENCOL VIROU TIRAS.} TIRAS}
		{{AMARRE TIRAS} {AS TIRAS VIRARAM UMA CORDA.} CORDA}
		{{SOLTE CORDA} {DEIXOU NA CELA.} EMPTY}
		{OLHE {VOCE ESTA EM SUA CELA.} EMPTY}
		{{PEGUE CORDA} {PEGOU.} CORDA}
		{{SOLTE TIRAS} {VOCE NAO ESTA CARREGANDO.} CORDA}
		{{PEGUE LENCOL} {VOCE NAO VE ISSO AQUI.} CORDA}
		{XYZ {COMANDO AINDA NAO DISPONIVEL.} CORDA}
		{<FULL> {LIMITE: 30 LETRAS OU ESPACOS.} CORDA}
		{<ENTER> {COMANDO AINDA NAO DISPONIVEL.} CORDA}
	}
} else {
	set steps {
		{N {A WALL BLOCKS THE WAY.} EMPTY}
		{S {A WALL BLOCKS THE WAY.} EMPTY}
		{E {A WALL BLOCKS THE WAY.} EMPTY}
		{W {WEST: A GUARD IN THE CORRIDOR.} EMPTY}
		{<LEFT> {WEST: A GUARD IN THE CORRIDOR.} EMPTY}
		{<BACKSPACE> {THE BED CANNOT BE MOVED.} EMPTY}
		{{EXAMINE BED} {THE BED IS MADE OF BRICKS.} EMPTY}
		{{TAKE SHEET} {TAKEN.} SHEET}
		{{TAKE SHEET} {YOU ALREADY HAVE IT.} SHEET}
		{{LEAVE SHEET} {LEFT IN THE CELL.} EMPTY}
		{{TEAR SHEET} {THE SHEET IS NOW STRIPES.} STRIPES}
		{{TIE STRIPES} {THE STRIPES BECAME A ROPE.} ROPE}
		{{LEAVE ROPE} {LEFT IN THE CELL.} EMPTY}
		{LOOK {YOU ARE IN YOUR CELL.} EMPTY}
		{{TAKE ROPE} {TAKEN.} ROPE}
		{{LEAVE STRIPES} {YOU ARE NOT CARRYING IT.} ROPE}
		{{TAKE SHEET} {YOU CANNOT SEE IT HERE.} ROPE}
		{XYZ {COMMAND NOT AVAILABLE YET.} ROPE}
		{<FULL> {LIMIT: 30 LETTERS OR SPACES.} ROPE}
		{<ENTER> {COMMAND NOT AVAILABLE YET.} ROPE}
	}
}

after time 5 {type_via_keybuf "\r"}
after time 30 {
	if {$::case eq "en"} {press 8 0x80}
	after time 2 {press 7 0x80}
}
after time 36 {press 7 0x80}
after time 60 {press 8 0x01}
after time 74 start_game
after time 280 {fail "tempo limite"}
