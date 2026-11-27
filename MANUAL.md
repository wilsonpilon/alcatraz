# Alcatraz: A Fuga Impossível — manual

**Manual em construção.** O remake tem uma primeira cela jogável, com um
conjunto inicial de comandos e inventário. As outras salas, a fuga completa
e a música ainda não foram implementadas.

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

## Tela e comandos da cela

Na metade superior ficam a cena à esquerda e o inventário à direita. Na
metade inferior ficam a descrição ou resposta e a entrada de texto. Digite
um comando e pressione **Enter**; **Backspace** apaga a última letra. A
entrada aceita letras e espaços, até 30 caracteres. **Esc** retorna ao DOS;
não há salvamento.

Use **N, S, L, O** ou as setas para norte, sul, leste e oeste; também são
aceitos os nomes completos. Na cela inicial, norte, sul e leste são paredes.
Oeste indica o corredor com o guarda, mas ainda não permite sair, pois a
próxima sala não foi implementada.

| Português | English | Ação |
|---|---|---|
| `OLHE` | `LOOK` | Descrever a cela e os objetos deixados nela. |
| `INVENTARIO` | `INVENTORY` | Consultar o painel do inventário. |
| `EXAMINE CAMA` | `EXAMINE BED` | Examinar a cama de alvenaria. |
| `PEGUE LENCOL` | `TAKE SHEET` | Pegar o lençol. |
| `SOLTE LENCOL` | `LEAVE SHEET` | Deixar o lençol na cela. |
| `RASGUE LENCOL` ou `CORTE LENCOL` | `TEAR SHEET` ou `CUT SHEET` | Transformar o lençol em tiras. |
| `AMARRE TIRAS` | `TIE STRIPES` | Transformar as tiras em uma corda. |

Também é possível pegar, soltar e examinar `TIRAS`/`STRIPES` e
`CORDA`/`ROPE`. Em inglês, as direções são **N, S, E, W** ou
`NORTH`, `SOUTH`, `EAST`, `WEST`. Os demais comandos e quebra-cabeças
ainda não estão disponíveis.

## Jogando o original: referência

O original usa comandos textuais em português ou inglês, conforme a listagem.
Na versão portuguesa, exemplos incluem `PEGUE LENCOL`, `EXAMINE CAMA` e
`ABRA GRADES CHAVE`. Algumas dessas ações já funcionam na cela do remake,
conforme a tabela acima; abrir as grades ainda não foi implementado.

No BASIC original, objetos podem ser carregados e deixados em uma posição para
serem encontrados depois; o limite original é de seis objetos carregados.
No remake atual, o lençol e suas transformações podem ser deixados e
recuperados na cela; o inventário e as demais regras ainda estão incompletos.

## Mapa e dicas

O [mapa visual](MAPA.png) mostra a grade inicial de 140 posições, inclusive
corredores vazios, e contém **spoilers** sobre itens e perigos. As ligações
podem ser direcionais e mudar durante a partida; a cadeira elétrica é um
destino fatal, não uma sala normalmente alcançável.

Para consultar condições e saídas por sala, veja [MAPA.md](MAPA.md). O
[percurso de referência](PERCURSO.md) revela a solução: foi validado na
listagem BASIC em inglês, não no remake. A [referência do original](REFERENCIA_ORIGINAL.md)
detalha os comandos, objetos e regras pesquisados.

## Créditos

- **Wilson Fazzio Martins**: autor do jogo original.
- **Wilson “Barney” Pilon**: remake em desenvolvimento.
- **Marcelo Silveira / MarMSX**: análise do jogo original utilizada como
  referência.
- **Guillaume “Aoineko” Blanchard**: biblioteca MSXgl.

Consulte os [créditos e informações de licença](README.md#créditos-e-referências)
para detalhes dos materiais e ferramentas.
