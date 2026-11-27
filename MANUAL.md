# Alcatraz: A Fuga Impossível — manual

**Manual em construção.** O remake tem três posições jogáveis, um conjunto
inicial de comandos e inventário. As demais salas, a fuga completa e a música
ainda não foram implementadas.

## A aventura

Você está preso em Alcatraz. Explore a prisão, encontre objetos, descubra como
superar obstáculos e procure uma maneira de escapar. A recriação pretende
transformar o labirinto original em salas interligadas, com cenários e
inventário ilustrados, preservando a exploração e os quebra-cabeças.

## Requisitos

- MSX 2+ ou superior.
- MSX-DOS 2.
- Mapper principal com pelo menos 2048 KB no total.
- PSG e MSX-MUSIC/FM.

A detecção de FM identifica a ROM MSX-MUSIC e pode não reconhecer todas as
placas. Ainda não há certificação em hardware real.

## Como iniciar a versão atual

No MSX-DOS 2, coloque `ALCATRAZ.COM` e `ALCATRAZ.SC5` no diretório atual e
execute:

```text
ALCATRAZ
```

O arquivo SC5 deve ser o distribuído com o executável. O pacote não inclui
arquivos de sistema do MSX-DOS. Para compilar ou usar o disco de testes com
openMSX, consulte as instruções do [README](README.md).

1. Depois das verificações mínimas de DOS e modelo, a abertura Disprosoft
   aparece e avança automaticamente.
2. Na seleção de idioma, use **esquerda/direita** e confirme com **Enter**.
   **Esc** retorna ao DOS. As verificações anteriores à seleção permanecem
   em português.
3. Confira o diagnóstico. Em uma máquina compatível, pressione **Enter**.
4. No título, aguarde o desenho e pressione **Espaço**. Após os efeitos,
   aparece a cela inicial.
5. **Esc** retorna ao DOS nas telas e animações posteriores à seleção.

## Tela e comandos

Na metade superior ficam a cena à esquerda e o inventário à direita. Na
metade inferior ficam a descrição ou resposta e a entrada de texto. Digite
um comando e pressione **Enter**; **Backspace** apaga a última letra. A
entrada aceita letras e espaços, até 30 caracteres. **Esc** retorna ao DOS;
não há salvamento.

Use **N, S, L, O** ou as setas para norte, sul, leste e oeste; também são
aceitos os nomes completos. Da cela R079, oeste leva às grades e ao guarda
em R078. A passagem oeste dali leva a R077 depois que as grades forem
abertas. Em R077, norte e sul levam a salas que ainda não foram implementadas;
o jogo informa isso sem mudar de posição.

| Português | English | Ação |
|---|---|---|
| `OLHE` | `LOOK` | Descrever a cela e os objetos deixados nela. |
| `INVENTARIO` | `INVENTORY` | Consultar o painel do inventário. |
| `EXAMINE CAMA` | `EXAMINE BED` | Examinar a cama de alvenaria. |
| `PEGUE LENCOL` | `TAKE SHEET` | Pegar o lençol. |
| `SOLTE LENCOL` | `LEAVE SHEET` | Deixar o lençol na cela. |
| `RASGUE LENCOL` ou `CORTE LENCOL` | `TEAR SHEET` ou `CUT SHEET` | Transformar o lençol em tiras. |
| `AMARRE TIRAS` | `TIE STRIPES` | Transformar as tiras em uma corda. |
| `PECA COMIDA GUARDA` | `ASK FOOD OFFICER` | Pedir comida ao guarda em R078. |
| `AMARRE CORDA GRADES` | `TIE ROPE BARS` | Amarrar a corda às grades. |
| `JOGUE CORDA` | `THROW ROPE` | Jogar a corda para fora das grades. |
| `PUXE CORDA` | `PULL ROPE` | Puxar a corda. |
| `ABRA GRADES CHAVE` | `OPEN BARS KEYS` | Abrir as grades com a chave disponível. |

Também é possível pegar, soltar e examinar `TIRAS`/`STRIPES` e
`CORDA`/`ROPE`, `COMIDA`/`FOOD` e `CHAVE`/`KEYS`. Objetos deixados
permanecem na sala em que foram soltos. Em inglês, as direções são **N, S, E, W** ou
`NORTH`, `SOUTH`, `EAST`, `WEST`. Os demais comandos e quebra-cabeças
ainda não estão disponíveis.

Como na listagem BASIC, amarrar a corda às grades confirma a ação sem
retirá-la do inventário nem ser obrigatório para jogá-la.

## Jogando o original: referência

O original usa comandos textuais em português ou inglês, conforme a listagem.
Na versão portuguesa, exemplos incluem `PEGUE LENCOL`, `EXAMINE CAMA` e
`ABRA GRADES CHAVE`. Essas ações já funcionam nas posições implementadas,
conforme a tabela acima; os demais quebra-cabeças ainda estão incompletos.

No BASIC original, objetos podem ser carregados e deixados em uma posição para
serem encontrados depois; o limite original é de seis objetos carregados.
No remake atual, objetos podem ser deixados e recuperados nas três posições;
o inventário e as demais regras ainda estão incompletos.

## Mapa e dicas

O [mapa visual no repositório](https://github.com/wilsonpilon/alcatraz/blob/main/MAPA.png) mostra a grade inicial de 140 posições, inclusive
corredores vazios, e contém **spoilers** sobre itens e perigos. As ligações
podem ser direcionais e mudar durante a partida; a cadeira elétrica é um
destino fatal, não uma sala normalmente alcançável.

Para consultar condições e saídas por sala, veja o [mapa de salas](https://github.com/wilsonpilon/alcatraz/blob/main/MAPA.md). A
[solução de referência](https://github.com/wilsonpilon/alcatraz/blob/main/RESOLVE.md) revela o percurso: foi validado na
listagem BASIC em inglês, não no remake. A [referência do original](REFERENCIA_ORIGINAL.md)
detalha os comandos, objetos e regras pesquisados.
O mapa e a solução são temporários e não acompanham o ZIP de release.

## Créditos

- **Wilson Fazzio Martins**: autor do jogo original.
- **Wilson “Barney” Pilon**: remake em desenvolvimento.
- **Marcelo Silveira / MarMSX**: análise do jogo original utilizada como
  referência.
- **Guillaume “Aoineko” Blanchard**: biblioteca MSXgl.

Consulte os [créditos e informações de licença](README.md#créditos-e-referências)
para detalhes dos materiais e ferramentas.
