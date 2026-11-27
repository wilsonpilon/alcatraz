# Alcatraz — referência da listagem BASIC

Gerada de `docs/alcatraz.asc`, listagem em português atribuída a Wilson Fazzio Martins. `docs/alcatraz.bas` é a versão em inglês; não presumir que seu vocabulário seja o mesmo.

## Parser e estado

- O original recebe até 30 caracteres, converte minúsculas a maiúsculas e aceita letras e espaços (linhas 920–1000).
- Identifica o verbo pela primeira palavra e o primeiro substantivo pela palavra seguinte; quando há outro, procura-o após o último espaço. As comparações são exatas; palavras intermediárias podem ser ignoradas. A disponibilidade dos objetos é verificada nas linhas 1520–1560, com exceções próprias.
- `SU$` guarda a posição e o nome dos substantivos: `**` é carregado, `..` é contextual, `JJ` é posição fora da grade e dois espaços indicam objeto removido. Soltar um objeto carregado grava a posição atual (linhas 2610–2640). O limite original é seis objetos: a sétima tentativa falha (linhas 2530–2550).
- `PO$` contém, para cada posição, códigos de norte, sul, leste e oeste; `X(0)` a `X(9)` guardam condições. Há 45 combinações exatas e dez despachos genéricos por verbo. As sub-rotinas e sua ordem são parte da regra; não inferir que uma combinação sempre terá sucesso.

## Condições e mecanismos

- `X(0)`: corda colocada no poço; `X(1)`: corda alcançou a chave; `X(2)`: pilha na lanterna; `X(3)`: pilha no gravador; `X(4)`: jornal sob a porta; `X(5)`: fita no gravador; `X(6)`: serra conectada; `X(7)`: lanterna acesa; `X(8)`: serra ligada; `X(9)`: gravador tocando. Ao mudar de posição, o BASIC zera X(6) e X(8) (linha 790).
- Na cela, pedir comida ao guarda desloca-o e deixa a chave fora; jogar e puxar a corda obtém a chave, que abre as grades. O presidiário troca comida por dinheiro; o guarda aceita dinheiro.
- A serra ligada à tomada corta a porta em R035. A outra porta, em R116, abre com jornal colocado sob ela e queimado com fósforos. O cobertor apaga o fogo; água no alarme e espelho na luz desativam as respectivas armadilhas.
- A lanterna precisa de pilha e deve estar acesa e carregada para atravessar a escuridão. O gravador precisa de pilha e fita e deve estar tocando e carregado ao atravessar os cães. O carcereiro aceita baralho; a corda no poço permite a fuga. A pá abre a passagem bloqueada por terra, sem ser necessária ao percurso validado.
- Casos a decidir na adaptação: `PECA` e `DE` têm exceção de visibilidade (linha 1520), e alguns casos especiais não verificam explicitamente se o objeto está carregado antes de transferi-lo (linhas 2330–2360 e 2650–2700). Na linha 2590, `IF J+10` zera X(4) ao pegar um objeto; preservar ou corrigir esse comportamento exige decisão e testes, não uma suposição silenciosa.

## Vocabulário: verbos

`RASGUE`, `AMARRE`, `PUXE`, `COMA`, `COLOQUE`, `LIGUE`, `CORTE`, `TIRE`, `CAVE`, `DISPARE`, `VISTA`, `ENTRE`, `PECA`, `JOGUE`, `QUEIME`, `PEGUE`, `SOLTE`, `DE`, `MATE`, `EXAMINE`, `AMEACE`, `QUEBRE`, `BEBA`, `ABRA`

## Substantivos e posição inicial

| Índice | Nome | Código BASIC | Posição inicial |
|---:|---|---|---|
| 0 | COMIDA | `JJ` | fora do jogo inicialmente (JJ) |
| 1 | TIRAS | `JJ` | fora do jogo inicialmente (JJ) |
| 2 | CORDA | `JJ` | fora do jogo inicialmente (JJ) |
| 3 | CHAVE | `JJ` | fora do jogo inicialmente (JJ) |
| 4 | LENCOL | `DC` | R079 |
| 5 | DINHEIRO | `IG` | R140 |
| 6 | LANTERNA | `AE` | R104 |
| 7 | SERRA | `E?` | R024 |
| 8 | PILHA | `D?` | R023 |
| 9 | FOSFOROS | `EE` | R108 |
| 10 | JORNAL | `AG` | R132 |
| 11 | COBERTOR | `CB` | R064 |
| 12 | AGUA | `<G` | R127 |
| 13 | REVOLVER | `AE` | R104 |
| 14 | FITA | `<B` | R057 |
| 15 | GRAVADOR | `>@` | R031 |
| 16 | PA | `G@` | R040 |
| 17 | OSSO | `>D` | R087 |
| 18 | BARALHO | `D?` | R023 |
| 19 | ROUPA | `FA` | R053 |
| 20 | BEBIDA | `GC` | R082 |
| 21 | ESPELHO | `CF` | R120 |
| 22 | GUARDA | `CC` | R078 |
| 23 | CAMA | `DC` | R079 |
| 24 | GRADES | `..` | objeto contextual (..) |
| 25 | PRESIDIARIO | `CB` | R064 |
| 26 | TOMADA | `B@` | R035 |
| 27 | PORTA | `..` | objeto contextual (..) |
| 28 | FOGO | `?E` | R102 |
| 29 | POCO | `IC` | R084 |
| 30 | CARCEREIRO | `G>` | R012 |
| 31 | TERRA | `IE` | R112 |
| 32 | CAES | `EG` | R136 |
| 33 | ALARME | `AA` | R048 |
| 34 | LUZ | `A>` | R006 |

## Despacho das ações

A tabela traduz os códigos de `AC$` e aponta para a sub-rotina BASIC. Entradas genéricas estão marcadas como tal; devem ser verificadas depois das combinações exatas.

| Ordem | Combinação | Tipo | Linha BASIC |
|---:|---|---|---:|
| 0 | RASGUE + TIRAS | exata | 1620 |
| 1 | RASGUE + CORDA | exata | 1630 |
| 2 | RASGUE + LENCOL | exata | 1630 |
| 3 | RASGUE + DINHEIRO | exata | 1650 |
| 4 | RASGUE + JORNAL | exata | 1660 |
| 5 | RASGUE + COBERTOR | exata | 1660 |
| 6 | RASGUE + BARALHO | exata | 1660 |
| 7 | RASGUE + ROUPA | exata | 1670 |
| 8 | AMARRE + TIRAS | exata | 1680 |
| 9 | AMARRE + CORDA + POCO | exata | 1700 |
| 10 | AMARRE + CORDA + CAMA | exata | 1720 |
| 11 | AMARRE + CORDA + GRADES | exata | 1730 |
| 12 | PUXE + CORDA | exata | 1740 |
| 13 | COMA + COMIDA | exata | 1770 |
| 14 | COMA + OSSO | exata | 1790 |
| 15 | COLOQUE + CORDA + POCO | exata | 1700 |
| 16 | COLOQUE + LENCOL + CAMA | exata | 1800 |
| 17 | COLOQUE + PILHA + LANTERNA | exata | 1820 |
| 18 | COLOQUE + PILHA + GRAVADOR | exata | 1840 |
| 19 | COLOQUE + JORNAL + PORTA | exata | 1860 |
| 20 | COLOQUE + COBERTOR + FOGO | exata | 1890 |
| 21 | COLOQUE + AGUA + FOGO | exata | 1910 |
| 22 | COLOQUE + FITA + GRAVADOR | exata | 1930 |
| 23 | COLOQUE + ESPELHO + LUZ | exata | 1940 |
| 24 | COLOQUE + SERRA + TOMADA | exata | 1970 |
| 25 | COLOQUE + AGUA + ALARME | exata | 1990 |
| 26 | LIGUE + LANTERNA | exata | 2010 |
| 27 | LIGUE + SERRA | exata | 2030 |
| 28 | LIGUE + GRAVADOR | exata | 2060 |
| 29 | CORTE + LENCOL | exata | 1630 |
| 30 | CORTE + CORDA | exata | 1630 |
| 31 | CORTE + TIRAS | exata | 1620 |
| 32 | CORTE + PORTA + SERRA | exata | 2100 |
| 33 | TIRE + SERRA + TOMADA | exata | 2160 |
| 34 | TIRE + PILHA + LANTERNA | exata | 2170 |
| 35 | TIRE + FITA + GRAVADOR | exata | 2180 |
| 36 | TIRE + PILHA + GRAVADOR | exata | 2190 |
| 37 | CAVE + TERRA + PA | exata | 2200 |
| 38 | DISPARE + REVOLVER | exata | 2960 |
| 39 | VISTA + ROUPA | exata | 2230 |
| 40 | ENTRE + POCO | exata | 2240 |
| 41 | BEBA + AGUA | exata | 2260 |
| 42 | BEBA + BEBIDA | exata | 2270 |
| 43 | ABRA + GRADES + CHAVE | exata | 2280 |
| 44 | ABRA + PORTA | exata | 2310 |
| 45 | PECA | genérica por verbo | 2330 |
| 46 | JOGUE | genérica por verbo | 2370 |
| 47 | QUEIME | genérica por verbo | 2450 |
| 48 | PEGUE | genérica por verbo | 2520 |
| 49 | SOLTE | genérica por verbo | 2610 |
| 50 | DE | genérica por verbo | 2650 |
| 51 | MATE | genérica por verbo | 2740 |
| 52 | EXAMINE | genérica por verbo | 2760 |
| 53 | AMEACE | genérica por verbo | 2920 |
| 54 | QUEBRE | genérica por verbo | 2940 |

## Fragmentos literais de mensagens

Trechos entre aspas extraídos de instruções `PRINT` das linhas 1–3095, inclusive abertura e final. Uma linha pode combinar vários trechos com nomes de objetos ou outras expressões; a listagem BASIC é a fonte para montagem, condições e fluxo. Quebras de linha da listagem são convertidas em espaços.

| Linha BASIC | Fragmentos literais, na ordem |
|---:|---|
| 72 | `"APRESENTA"` |
| 520 | `"As grades da cela impedem sua   passagem."` |
| 530 | `"O guarda esta aqui e nao deixa  voce passar."` |
| 540 | `"Aqui ha uma cadeira eletrica.   Voce foi eletrocutado."` |
| 550 | `"Ha uma porta fechada aqui."` |
| 570 | `"Esta muito escuro aqui.         Voce tropecou e quebrou  a perna."` |
| 580 | `"Voce caiu no fogo que restou.   Que modo horrivel de  morrer!"` |
| 590 | `"Voce disparou um alarme!Os guar-das o pegaram! Voce foi  fuzilado"` |
| 600 | `"Voce passou por um feixe de luz que disparou outro alarme!"` |
| 610 | `"Os caes dormem..."` |
| 620 | `"Ha cinco caes aqui. Voce foi    horrivelmente devorado!"` |
| 630 | `"A passagem aqui esta bloqueada  por terra."` |
| 640 | `"Voce deu de cara com o carcerei-ro e ele o denunciou."` |
| 650 | `"Conseguimos sair do predio, mas ainda falta transpor o  patio..."` |
| 820 | `"Aqui tem: "` |
| 840 | `" "` |
| 850 | `"Nada Importante"` |
| 890 | `"Voce tem: "` |
| 1090 | `"O que e' "` + `" ?"` |
| 1200 | `"O que e' "` + `" ?"` |
| 1320 | `"Isto nao e possivel."` |
| 1440 | `"Quer jogar novamente (S/N) ?"` |
| 1560 | `"Nao estou vendo "` |
| 1620 | `"Mais do que ja estao ?"` |
| 1630 | `"Esta' bem. Ficou em tiras."` |
| 1650 | `"Ok, mas e' um desperdicio."` |
| 1660 | `"Ok, nao parecia ser muito util  mesmo..."` |
| 1670 | `"Ok, mas nao sei porque fez isso."` |
| 1680 | `"As "` + `" transformaram-se numa "` + `"!"` |
| 1700 | `"Ok, e ela vai ate' o fundo."` |
| 1720 | `"Nao da',a "` + `" e' de alvenaria."` |
| 1730 | `"Ok..."` |
| 1740 | `"Nada ocorreu."` |
| 1750 | `"A chave veio junto com ela!"` |
| 1770 | `"Nao estava boa... Talvez sem    tempero."` |
| 1790 | `"Voce e' doente? Necessita de um psiquiatra? Nao vou faze-lo."` |
| 1800 | `"Ficou muito arrumada..."` |
| 1820 | `"Ok. Acho que agora vai funcionar"` |
| 1840 | `"Encaixou perfeitamente."` |
| 1860 | `"Nao vejo nenhuma porta."` |
| 1870 | `"Nao ha' espaco para coloca-lo."` |
| 1880 | `"Coube sob ela."` |
| 1890 | `"Ele o abafou!"` |
| 1920 | `"O local ficou cheio de fumaca e voce morreu asfixiado!"` |
| 1930 | `"Ok..."` |
| 1950 | `"Voce conseguiu! desativou o     alarme!"` |
| 1970 | `"Agora ela  esta' pronta para    funcionar."` |
| 2000 | `"Entrou em curto!"` |
| 2010 | `"Nao funciona..."` |
| 2020 | `"Esta' acesa."` |
| 2030 | `"Nao quer ligar..."` |
| 2040 | `"Esta' ligada."` |
| 2060 | `"Acho que esta' quebrado... Nao  funciona de modo  algum."` |
| 2070 | `"Falta alguma coisa para se ouviralgo no gravador ..."` |
| 2090 | `"Toca uma musica suave... Da'    vontade de dormir..."` |
| 2100 | `"Onde esta' a porta?"` |
| 2110 | `"Ela esta' desligada."` |
| 2140 | `"A "` + `" caiu em pedacos."` |
| 2160 | `"Ok..."` |
| 2170 | `"Esta' fora."` |
| 2180 | `"Ok..."` |
| 2190 | `"Ok... Mas o que voce vai fazer  com ela?"` |
| 2210 | `"Um buraco foi aberto."` |
| 2230 | `"Esta' no corpo. Tem um caimento perfeito!"` |
| 2240 | `"Voce foi direto para o fundo.   Morte instantanea..."` |
| 2260 | `"Nao era pura! Voce morreu conta-minado."` |
| 2270 | `"Voce ficou bebado e entregou-se.Voce nao tem inteligencia alguma"` |
| 2280 | `"Abriu!"` |
| 2290 | `"Abriu de novo!"` |
| 2300 | `"Nao estou vendo grades."` |
| 2310 | `"Esta' trancada."` |
| 2320 | `"Nao ha' nenhuma porta por perto."` |
| 2330 | `"A quem?"` |
| 2330 | `"Voce esta' louco? Por que nao   fala com pessoas?"` |
| 2340 | `"Ouviu o que disse mas nada fez."` |
| 2350 | `"Ele satisfez seu pedido e saiu, deixando a chave la' fora..."` |
| 2410 | `"Voce perdeu o objeto."` |
| 2420 | `"Voce nao e' tao forte assim..."` |
| 2430 | `"Nada aconteceu..."` |
| 2440 | `"Encostou num objeto la' fora.   Nao da'  para ver o que e'."` |
| 2440 | `"Nada aconteceu, ainda..."` |
| 2450 | `"Com o que ?!"` |
| 2460 | `"Nao da' para faze-lo."` |
| 2480 | `"Se desfez em cinzas..."` |
| 2510 | `"O "` + `" abriu uma passagem!"` |
| 2520 | `"Nao da'"` |
| 2550 | `"Nao da' para pegar mais nada."` |
| 2560 | `"Voce ja' esta' carregando este  objeto."` |
| 2570 | `"Ok."` |
| 2610 | `"Voce nao carrega "` |
| 2620 | `"Esta' no chao."` |
| 2650 | `"Acho que a  prisao afetou seu QI."` |
| 2660 | `"Os caes devoraram seu braco.    Assim fica impossivel  a fuga."` |
| 2670 | `"Ele aceitou e deu dinheiro em troca a voce!"` |
| 2680 | `"Ele aceitou e deixou voce passar"` |
| 2690 | `"Ele nao era subornavel. Voce foimorto."` |
| 2700 | `"Ele aceitou e foi embora!"` |
| 2710 | `"Mas voce nao carrega esse objeto!"` |
| 2720 | `"Ele aceitou, mas nada aconteceu."` |
| 2740 | `"Seu lugar  nao e' aqui, e' no    manicomio."` |
| 2750 | `"Voce foi morto antes que pudessefazer algo."` |
| 2760 | `"E' eletrica e muito potente."` |
| 2770 | `"E' de 1,5 V."` |
| 2780 | `"A caixa esta' cheia."` |
| 2790 | `"E' bem grosso."` |
| 2800 | `"E' uma fita de musica."` |
| 2810 | `"E' de fitas cassete."` |
| 2820 | `"E' de borracha."` |
| 2830 | `"E' pinga, e das boas!"` |
| 2840 | `"E' feita de tijolos."` |
| 2850 | `"Sao de aco muito resistente."` |
| 2860 | `"E' de 110V."` |
| 2870 | `"Voce se queimou! Nao devia examina-lo!"` |
| 2880 | `"E' muito fundo. Nao da' para verseu final."` |
| 2890 | `"Sao caes policiais."` |
| 2900 | `"Voce foi morto... Ele nao gosta de  ser examinado."` |
| 2910 | `"E' comum."` |
| 2920 | `"So' uma  pessoa com sua mentali- dade diria tal coisa."` |
| 2930 | `"Voce foi morto. Ele nao gosta deameacas."` |
| 2940 | `"Ok... Esta' feito..."` |
| 2950 | `"Nao da'."` |
| 2960 | `"Ok..."` |
| 3010 | `"Voce saiu por uma passagem se-  creta! Voce escapou de  ALCATRAZ!"` |
| 3072 | `"A FUGA IMPOSSIVEL"` |
| 3075 | `"Programa de Wilson F. Martins"` |
| 3080 | `"Tecle SPACO para comecar."` |
