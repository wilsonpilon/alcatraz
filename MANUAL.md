# Alcatraz: A Fuga Impossível — manual

**Manual em construção.** O remake ainda não é um jogo completo: há abertura,
seleção de idioma e diagnóstico, mas não há salas jogáveis, parser, inventário
nem música. As seções sobre a aventura descrevem o BASIC original ou planos
para o remake, não funcionalidades já disponíveis.

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
   aparece a mensagem provisória de desenvolvimento.
5. **Esc** retorna ao DOS nas telas e animações posteriores à seleção.

## Como será a tela de jogo

O projeto prevê uma interface inspirada em San Quentin: na metade superior,
a cena do local à esquerda e o inventário gráfico à direita; na metade inferior,
as mensagens e a entrada de texto. O desenho definitivo e os controles de
movimentação ainda serão implementados e documentados.

## Jogando o original: referência

O original usa comandos textuais em português ou inglês, conforme a listagem.
Na versão portuguesa, exemplos incluem `PEGUE LENCOL`, `EXAMINE CAMA` e
`ABRA GRADES CHAVE`. As palavras são comparadas exatamente; não se deve supor
que esses comandos ou suas traduções já funcionem no remake.

No BASIC original, objetos podem ser carregados e deixados em uma posição para
serem encontrados depois; o limite é de seis objetos carregados. A adaptação
dessas regras, incluindo casos especiais e o vocabulário inglês, ainda precisa
ser implementada e testada.

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
