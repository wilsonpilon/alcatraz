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

proc save_screen {name image} {
	foreach {suffix data} [list vram $image palette [debug read_block {VDP palette} 0 32]] {
		set file [open [file join $::output "$name.$suffix"] wb]
		fconfigure $file -translation binary
		puts -nonewline $file $data
		close $file
	}
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
	} elseif {$key eq "RIGHT"} {
		press 8 0x80
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
	} elseif {$command eq "<RIGHT>"} {
		set ::keys {RIGHT}
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
		set y 44
		foreach item [split $::inventory "+"] {
			if {![text_matches $image 182 $y $item]} {fail "inventario: esperado $item em $y"}
			incr y 18
		}
	}
	if {$::command eq "OLHE" && $::expected eq "VOCE ESTA EM SUA CELA." &&
		![text_matches $image 68 130 ", CORDA"]} {
		fail "corda deixada na cela nao aparece na descricao"
	}
	if {$::command eq "LOOK" && $::expected eq "YOU ARE IN YOUR CELL." &&
		![text_matches $image 62 130 ", ROPE"]} {
		fail "rope deixada na cela nao aparece na descricao"
	}
	if {$::command eq "OLHE" && $::expected eq "VOCE ESTA NO CORREDOR." &&
		![text_matches $image 8 130 "AQUI: COMIDA"]} {
		fail "comida deixada no corredor nao aparece na descricao"
	}
	if {$::command eq "LOOK" && $::expected eq "YOU ARE IN THE CORRIDOR." &&
		![text_matches $image 8 130 "HERE: FOOD"]} {
		fail "food deixada no corredor nao aparece na descricao"
	}
	if {$::command in {"PEGUE LENCOL" "TAKE SHEET"} && [pixel $image 65 65] == 12} {
		fail "lencol carregado ou consumido ainda aparece na cama"
	}
	if {$::command in {"SOLTE LENCOL" "LEAVE SHEET"} && [pixel $image 65 65] != 12} {
		fail "lencol deixado nao aparece na cena"
	}
	if {$::command in {"PECA COMIDA GUARDA" "ASK FOOD OFFICER"} &&
		[pixel $image 110 42] == 10} {
		fail "guarda continua na cena depois de sair"
	}
	if {$::command in {"ABRA GRADES CHAVE" "OPEN BARS KEYS"} &&
		$::expected in {"AS GRADES ABRIRAM!" "THE BARS ARE OPEN!"} &&
		[pixel $image 16 40] == 8} {
		fail "as grades continuam fechadas na cena"
	}
	if {$::command in {"SOLTE COMIDA" "LEAVE FOOD"} && [pixel $image 48 90] != 11} {
		fail "comida deixada nao aparece na cena"
	}
	if {$::case eq "pt" && !$::guard_saved && $::expected eq "VOCE ESTA JUNTO AS GRADES."} {
		if {[pixel $image 110 42] != 10 || [pixel $image 16 40] != 8} {
			fail "guarda ou grades fechadas ausentes na cena"
		}
		set ::guard_saved 1
		save_screen guard $image
	}
	if {$::case eq "pt" && !$::corridor_saved && $::expected eq "VOCE ESTA NO CORREDOR."} {
		set ::corridor_saved 1
		save_screen corridor $image
	}
	next_step
}

proc start_game {} {
	set image [debug read_block VRAM 0 27136]
	set title [expr {$::case eq "pt" ? "ALCATRAZ  /  CELA" : "ALCATRAZ  /  CELL"}]
	set room [expr {$::case eq "pt" ? "VOCE ESTA EM SUA CELA." : "YOU ARE IN YOUR CELL."}]
	set sheet [expr {$::case eq "pt" ? ", LENCOL" : ", SHEET"}]
	set empty [expr {$::case eq "pt" ? "VAZIO" : "EMPTY"}]
	set sheet_x [expr {$::case eq "pt" ? 68 : 62}]
	if {![text_matches $image 8 2 $title] || ![text_matches $image 8 119 $room] ||
		![text_matches $image $sheet_x 130 $sheet] || ![text_matches $image 166 45 $empty] ||
		[pixel $image 6 18] != 2 || [pixel $image 6 100] != 6 ||
		[pixel $image 65 65] != 12} {
		fail "tela inicial da cela, objetos ou paleta"
	}
	set throttle on
	after time 0.3 {
		screenshot -raw [file join $::output "$::case.png"]
		set throttle off
		set ::step 0
		set ::guard_saved 0
		set ::corridor_saved 0
		next_step
	}
}

if {$case eq "pt"} {
	set steps {
		{N {UMA PAREDE BLOQUEIA A PASSAGEM.} EMPTY}
		{S {UMA PAREDE BLOQUEIA A PASSAGEM.} EMPTY}
		{L {UMA PAREDE BLOQUEIA A PASSAGEM.} EMPTY}
		{O {VOCE ESTA JUNTO AS GRADES.} EMPTY}
		{N {UMA PAREDE BLOQUEIA A PASSAGEM.} EMPTY}
		{S {UMA PAREDE BLOQUEIA A PASSAGEM.} EMPTY}
		{O {AS GRADES IMPEDEM A PASSAGEM.} EMPTY}
		{<LEFT> {AS GRADES IMPEDEM A PASSAGEM.} EMPTY}
		{L {VOCE ESTA EM SUA CELA.} EMPTY}
		{<BACKSPACE> {NAO DA PARA PEGAR A CAMA.} EMPTY}
		{{EXAMINE CAMA} {A CAMA E FEITA DE TIJOLOS.} EMPTY}
		{{PEGUE LENCOL} {PEGOU.} LENCOL}
		{{PEGUE LENCOL} {VOCE JA ESTA CARREGANDO.} LENCOL}
		{{SOLTE LENCOL} {DEIXOU AQUI.} EMPTY}
		{{RASGUE LENCOL} {O LENCOL VIROU TIRAS.} TIRAS}
		{{AMARRE TIRAS} {AS TIRAS VIRARAM UMA CORDA.} CORDA}
		{{SOLTE CORDA} {DEIXOU AQUI.} EMPTY}
		{OLHE {VOCE ESTA EM SUA CELA.} EMPTY}
		{{PEGUE CORDA} {PEGOU.} CORDA}
		{{SOLTE TIRAS} {VOCE NAO ESTA CARREGANDO.} CORDA}
		{{PEGUE LENCOL} {VOCE NAO VE ISSO AQUI.} CORDA}
		{XYZ {COMANDO AINDA NAO DISPONIVEL.} CORDA}
		{<FULL> {LIMITE: 30 LETRAS OU ESPACOS.} CORDA}
		{<ENTER> {COMANDO AINDA NAO DISPONIVEL.} CORDA}
		{O {VOCE ESTA JUNTO AS GRADES.} CORDA}
		{{JOGUE CORDA} {NADA ACONTECEU.} CORDA}
		{{PUXE CORDA} {NADA ACONTECEU.} CORDA}
		{{ABRA GRADES CHAVE} {VOCE NAO VE ISSO AQUI.} CORDA}
		{{PECA COMIDA GUARDA} {O GUARDA TROUXE COMIDA} CORDA}
		{{PECA COMIDA GUARDA} {O GUARDA NAO ESTA AQUI.} CORDA}
		{{PEGUE COMIDA} {PEGOU.} CORDA+COMIDA}
		{{AMARRE CORDA GRADES} {CORDA AMARRADA NAS GRADES.} CORDA+COMIDA}
		{{JOGUE CORDA} {A CORDA TOCOU EM ALGO.} CORDA+COMIDA}
		{{PUXE CORDA} {A CHAVE VEIO COM A CORDA!} CORDA+COMIDA+CHAVE}
		{{PUXE CORDA} {NADA ACONTECEU.} CORDA+COMIDA+CHAVE}
		{{SOLTE CHAVE} {DEIXOU AQUI.} CORDA+COMIDA}
		{L {VOCE ESTA EM SUA CELA.} CORDA+COMIDA}
		{{PEGUE CHAVE} {VOCE NAO VE ISSO AQUI.} CORDA+COMIDA}
		{O {VOCE ESTA JUNTO AS GRADES.} CORDA+COMIDA}
		{{PEGUE CHAVE} {PEGOU.} CORDA+COMIDA+CHAVE}
		{{ABRA GRADES CHAVE} {AS GRADES ABRIRAM!} CORDA+COMIDA+CHAVE}
		{<LEFT> {VOCE ESTA NO CORREDOR.} CORDA+COMIDA+CHAVE}
		{N {ESSA SALA AINDA NAO ESTA PRONTA.} CORDA+COMIDA+CHAVE}
		{S {ESSA SALA AINDA NAO ESTA PRONTA.} CORDA+COMIDA+CHAVE}
		{O {UMA PAREDE BLOQUEIA A PASSAGEM.} CORDA+COMIDA+CHAVE}
		{{SOLTE COMIDA} {DEIXOU AQUI.} CORDA+CHAVE}
		{L {VOCE ESTA JUNTO AS GRADES.} CORDA+CHAVE}
		{{PEGUE COMIDA} {VOCE NAO VE ISSO AQUI.} CORDA+CHAVE}
		{O {VOCE ESTA NO CORREDOR.} CORDA+CHAVE}
		{OLHE {VOCE ESTA NO CORREDOR.} CORDA+CHAVE}
		{{PEGUE COMIDA} {PEGOU.} CORDA+COMIDA+CHAVE}
		{<RIGHT> {VOCE ESTA JUNTO AS GRADES.} CORDA+COMIDA+CHAVE}
		{L {VOCE ESTA EM SUA CELA.} CORDA+COMIDA+CHAVE}
	}
} else {
	set steps {
		{N {A WALL BLOCKS THE WAY.} EMPTY}
		{S {A WALL BLOCKS THE WAY.} EMPTY}
		{E {A WALL BLOCKS THE WAY.} EMPTY}
		{W {YOU ARE BY THE BARS.} EMPTY}
		{N {A WALL BLOCKS THE WAY.} EMPTY}
		{S {A WALL BLOCKS THE WAY.} EMPTY}
		{W {THE BARS BLOCK YOUR WAY.} EMPTY}
		{<LEFT> {THE BARS BLOCK YOUR WAY.} EMPTY}
		{E {YOU ARE IN YOUR CELL.} EMPTY}
		{<BACKSPACE> {THE BED CANNOT BE MOVED.} EMPTY}
		{{EXAMINE BED} {THE BED IS MADE OF BRICKS.} EMPTY}
		{{TAKE SHEET} {TAKEN.} SHEET}
		{{TAKE SHEET} {YOU ALREADY HAVE IT.} SHEET}
		{{LEAVE SHEET} {LEFT HERE.} EMPTY}
		{{TEAR SHEET} {THE SHEET IS NOW STRIPES.} STRIPES}
		{{TIE STRIPES} {THE STRIPES BECAME A ROPE.} ROPE}
		{{LEAVE ROPE} {LEFT HERE.} EMPTY}
		{LOOK {YOU ARE IN YOUR CELL.} EMPTY}
		{{TAKE ROPE} {TAKEN.} ROPE}
		{{LEAVE STRIPES} {YOU ARE NOT CARRYING IT.} ROPE}
		{{TAKE SHEET} {YOU CANNOT SEE IT HERE.} ROPE}
		{XYZ {COMMAND NOT AVAILABLE YET.} ROPE}
		{<FULL> {LIMIT: 30 LETTERS OR SPACES.} ROPE}
		{<ENTER> {COMMAND NOT AVAILABLE YET.} ROPE}
		{W {YOU ARE BY THE BARS.} ROPE}
		{{THROW ROPE} {NOTHING HAPPENED.} ROPE}
		{{PULL ROPE} {NOTHING HAPPENED.} ROPE}
		{{OPEN BARS KEYS} {YOU CANNOT SEE IT HERE.} ROPE}
		{{ASK FOOD OFFICER} {THE OFFICER BROUGHT FOOD} ROPE}
		{{ASK FOOD OFFICER} {THE OFFICER IS NOT HERE.} ROPE}
		{{TAKE FOOD} {TAKEN.} ROPE+FOOD}
		{{TIE ROPE BARS} {THE ROPE IS TIED TO THE BARS.} ROPE+FOOD}
		{{THROW ROPE} {THE ROPE TOUCHED SOMETHING.} ROPE+FOOD}
		{{PULL ROPE} {THE KEYS CAME WITH THE ROPE!} ROPE+FOOD+KEYS}
		{{PULL ROPE} {NOTHING HAPPENED.} ROPE+FOOD+KEYS}
		{{LEAVE KEYS} {LEFT HERE.} ROPE+FOOD}
		{E {YOU ARE IN YOUR CELL.} ROPE+FOOD}
		{{TAKE KEYS} {YOU CANNOT SEE IT HERE.} ROPE+FOOD}
		{W {YOU ARE BY THE BARS.} ROPE+FOOD}
		{{TAKE KEYS} {TAKEN.} ROPE+FOOD+KEYS}
		{{OPEN BARS KEYS} {THE BARS ARE OPEN!} ROPE+FOOD+KEYS}
		{<LEFT> {YOU ARE IN THE CORRIDOR.} ROPE+FOOD+KEYS}
		{N {THIS ROOM IS NOT READY YET.} ROPE+FOOD+KEYS}
		{S {THIS ROOM IS NOT READY YET.} ROPE+FOOD+KEYS}
		{W {A WALL BLOCKS THE WAY.} ROPE+FOOD+KEYS}
		{{LEAVE FOOD} {LEFT HERE.} ROPE+KEYS}
		{E {YOU ARE BY THE BARS.} ROPE+KEYS}
		{{TAKE FOOD} {YOU CANNOT SEE IT HERE.} ROPE+KEYS}
		{W {YOU ARE IN THE CORRIDOR.} ROPE+KEYS}
		{LOOK {YOU ARE IN THE CORRIDOR.} ROPE+KEYS}
		{{TAKE FOOD} {TAKEN.} ROPE+FOOD+KEYS}
		{<RIGHT> {YOU ARE BY THE BARS.} ROPE+FOOD+KEYS}
		{E {YOU ARE IN YOUR CELL.} ROPE+FOOD+KEYS}
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
after time 600 {fail "tempo limite"}
