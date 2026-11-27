# Alcatraz: A Fuga Impossível

Recriação em desenvolvimento do adventure para MSX-BASIC de **Wilson Fazzio
Martins**. O jogo original combina comandos de texto e um labirinto desenhado
com caracteres semigráficos. A nova versão pretende trazer ambientes
ilustrados, inventário gráfico e música, inspirando-se na apresentação MSX2
do projeto [San Quentin](https://github.com/wilsonpilon/sanquentin).

## 2.8 — Frank Morris: rajada com percussão FM

Pré-release: ainda não há jogo, mapa jogável, parser nem música. Após as verificações
mínimas de DOS e modelo, o programa recria o logotipo Disprosoft em SCREEN 2,
mantém a imagem por cerca de 2,5 a 3 segundos e avança automaticamente para
o diagnóstico em SCREEN 5. Exige MSX 2+, MSX-DOS 2, mapper
principal de pelo menos 2048 KB, PSG e MSX-MUSIC/FM. Em sistemas
anteriores a MSX 2+ ou DOS 2, a mensagem é exibida em texto.

Em uma máquina compatível, pressione **Enter** no diagnóstico para abrir a
tela de título: a imagem aparece primeiro e o logotipo original é desenhado
progressivamente, letra por letra, seguido dos créditos e da
instrução **Tecle [ESPACO] para comecar**. **Espaço** remove créditos e
instrução, mostra dez impactos diagonais de vidro com ruído PSG e camadas
de percussão FM e, após
cerca de dois segundos, avança para uma tela provisória que informa que o
jogo está em desenvolvimento. **Esc** retorna ao DOS, inclusive durante a
animação. A música ainda não foi implementada.
A imagem é carregada de
`ALCATRAZ.SC5` no diretório atual do MSX-DOS. Ausência ou corrupção do
arquivo produz uma mensagem de erro.

O mapper é medido pelo total de segmentos do mapper principal informado pelo
DOS 2, não pela soma dos cartuchos nem pela quantidade livre. A detecção de
FM usa a identificação de ROM MSX-MUSIC; não garante detectar toda placa FM.
Não há certificação em hardware real.

## Compilação

No Windows, instale o [MSXgl](https://github.com/aoineko-fr/MSXgl) em
`MSXgl\`, incluindo as ferramentas SDCC, Node e MSXhex fornecidas pelo
projeto, incluindo MSXimg, e execute `build.bat`. A revisão pública testada é
[`1cef53f497796635db4e09feeb26c0da3199f6ab`](https://github.com/aoineko-fr/MSXgl/tree/1cef53f497796635db4e09feeb26c0da3199f6ab).
Mantenha projeto e MSXgl no mesmo volume: o build do MSXgl move arquivos
intermediários entre eles. A saída é `out\alcatraz.com`; execute-a a
partir do MSX-DOS 2, com `out\ALCATRAZ.SC5` no diretório atual do MSX.
O build converte `images\openscreen.jpg` para SCREEN 5 (256×212), usando 14
cores para a imagem e reservando um tom terracota para o logotipo; a paleta
é incorporada ao programa. O arquivo
SC5 gerado é binário BSAVE de 212 linhas; o carregador exige início na
VRAM 0 e conteúdo completo, e usa a paleta da imagem de abertura compilada
no programa. A pasta `out\` e as ferramentas não são versionadas.
Os arquivos de sistema MSX-DOS não são fornecidos neste projeto.

Para executar os quatro testes gráficos de hardware no openMSX, depois do
build, use `pwsh -File tests\run-smoke.ps1 -OpenMSX caminho\openmsx.exe`.
São necessárias as ROMs das máquinas Panasonic FS-A1WX e FS-A1FX, além das
extensões do openMSX. O disco temporário de teste é gerado em `out\`.
Ele inclui `ALCATRAZ.BAS`, cópia de `docs\alcatraz.bas`, para comparação com
a listagem de referência no MSX-BASIC. O disco inicia a recriação por
`AUTOEXEC.BAT`; para executar o BASIC, inicie o MSX-BASIC com suporte a disco,
insira `smoke.dsk` e execute `RUN "ALCATRAZ.BAS"`. A listagem incluída é a
versão em inglês presente em `docs\alcatraz.bas`.
Para testar imagem antes do desenho, progressão do logotipo, créditos,
impactos, gravações PSG e FM, pausa em 50 e 60 Hz, espaço, saída inclusive
durante as animações, falhas de arquivo e bloqueio em máquina incompatível:
`pwsh -File tests\run-sc5.ps1 -OpenMSX caminho\openmsx.exe`.

Para comparar a abertura com a listagem BASIC local em
`docs\disprosoft.bas`, execute
`pwsh -File tests\run-logo.ps1 -OpenMSX caminho\openmsx.exe`. O teste corrige
apenas na cópia temporária a junção das linhas 40 e 41 e mantém a imagem
original na tela para comparação. Na máquina testada, a tradução apresenta
19 pixels de diferença nos contornos, em 49.152 pixels; paleta e texto
coincidem. As capturas e discos de teste ficam em `out\logo-test\`.

O código está dividido em inicialização (`src\main.c`), diagnóstico
(`src\diagnostic.c`), abertura Disprosoft (`src\disprosoft.c`), título
(`src\title_logo.c`), imagem SC5 (`src\opening.c`) e impactos com som
(`src\opening_shots.c`). A abertura Disprosoft
desenha primitivas em SCREEN 2; os contornos de arcos são uma aproximação
inteira ao BASIC. O logotipo do título usa as coordenadas da listagem
`docs\abertura.asc`, preservando o desenho estilizado; os textos da abertura
usam uma fonte diferente da empregada no diagnóstico.

Cada compilação **bem-sucedida pelo `build.bat`** incrementa Y em X.Y;
uma falha não altera a versão. Um marco completo inicia o próximo X em Y=1:

```bat
build.bat --milestone "Frank Morris" "Descricao do marco"
```

O nome e subtítulo ficam em `version.json`; o binário também mostra X.Y.
Releases e tags são publicados separadamente, após validação; compilar não
faz upload automático. `build.bat` altera `version.json` localmente.

## Créditos e referências

- **Wilson Fazzio Martins**: jogo original, publicado no livro *Jogos de
  Habilidade* (Editora Aleph, 1986).
- **Marcelo / MarMSX**: análise detalhada das regras e implementação original.
- **Guillaume “Aoineko” Blanchard**: biblioteca MSXgl e fonte usada no stub,
  distribuídas sob [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/).
- **ASCII Corporation**: fonte latina de BIOS incluída pelo MSXgl; o arquivo
  informa que seu uso é livre em aplicações MSX.
- **Disprosoft**: identidade visual reproduzida a partir da listagem BASIC
  fornecida para referência. O responsável pelo projeto confirmou possuir as
  permissões necessárias para publicar a reprodução e a listagem; isso não
  implica concessão de direitos a terceiros sobre a marca ou o logotipo.
- A imagem de abertura foi fornecida pelo usuário, que confirmou autorização
  para publicá-la e publicar sua conversão; sua licença específica ainda
  será definida.
- A pasta [docs](docs/README.md) reúne temporariamente material de referência.
  Seus autores conservam seus direitos; suas licenças serão normalizadas
  conforme necessário, em contato com os autores.

O código do projeto usa a licença [GPL-3.0](LICENSE); essa licença não concede
direitos sobre o logotipo. A biblioteca, suas fontes e ferramentas têm as
licenças indicadas pelo MSXgl.
Veja [OUTLINE.md](OUTLINE.md) para a visão e [resume.txt](resume.txt) para
o registro de decisões, andamento e próximos passos.

## Pesquisa e mapa de referência

- [Mapa de salas](MAPA.md): 140 posições do BASIC, saídas direcionais,
  condições e correspondência com o roteiro ilustrado.
- [Referência do original](REFERENCIA_ORIGINAL.md): vocabulário, objetos,
  despacho de ações e fragmentos literais de mensagens.
- [Percurso de referência](PERCURSO.md): caminho de fuga validado na versão
  BASIC em inglês, sem pressupor que o remake já esteja jogável.

Os documentos são gerados por `python tools\mapa_alcatraz.py`. Para conferir
os dados e a geração: `python -m unittest discover -s tests -p test_mapa_alcatraz.py`.
Para reproduzir o percurso no BASIC original em inglês com openMSX:
`python tests\run_original_route.py --openmsx caminho\openmsx.exe`.
