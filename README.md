# Alcatraz: A Fuga Impossível

Recriação em desenvolvimento do adventure para MSX-BASIC de **Wilson Fazzio
Martins**. O jogo original combina comandos de texto e um labirinto desenhado
com caracteres semigráficos. A nova versão pretende trazer ambientes
ilustrados, inventário gráfico e música, inspirando-se na apresentação MSX2
do projeto [San Quentin](https://github.com/wilsonpilon/sanquentin).

## 1.1 — Robert Stroud: stub inicial e documentação

Pré-release: ainda não há jogo, mapa, parser nem música. O programa mostra
uma tela de diagnóstico em SCREEN 5 e exige MSX 2+, MSX-DOS 2, mapper
principal de pelo menos 2048 KB, PSG e MSX-MUSIC/FM. SCC é opcional e
**ainda não é detectado**; aparece como N/T (não testado). Em sistemas
anteriores a MSX 2+ ou DOS 2, a mensagem é exibida em texto.

O mapper é medido pelo total de segmentos do mapper principal informado pelo
DOS 2, não pela soma dos cartuchos nem pela quantidade livre. A detecção de
FM usa a identificação de ROM MSX-MUSIC; não garante detectar toda placa FM.
Não há certificação em hardware real.

## Compilação

No Windows, instale o [MSXgl](https://github.com/aoineko-fr/MSXgl) em
`MSXgl\`, incluindo as ferramentas SDCC, Node e MSXhex fornecidas pelo
projeto, e execute `build.bat`. A revisão pública testada é
[`1cef53f497796635db4e09feeb26c0da3199f6ab`](https://github.com/aoineko-fr/MSXgl/tree/1cef53f497796635db4e09feeb26c0da3199f6ab).
Mantenha projeto e MSXgl no mesmo volume: o build do MSXgl move arquivos
intermediários entre eles. A saída é `out\alcatraz.com`; execute-a a
partir do MSX-DOS 2. A pasta `out\` e as ferramentas não são versionadas.
Os arquivos de sistema MSX-DOS não são fornecidos neste projeto.

Para executar os quatro testes gráficos de hardware no openMSX, depois do
build, use `pwsh -File tests\run-smoke.ps1 -OpenMSX caminho\openmsx.exe`.
São necessárias as ROMs das máquinas Panasonic FS-A1WX e FS-A1FX, além das
extensões do openMSX. O disco temporário de teste é gerado em `out\`.

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
- Os demais autores das referências conservam seus direitos. A pasta local
  `docs\` contém material de terceiros e não é publicada até que os direitos
  de redistribuição sejam verificados.

O código do projeto usa a licença [GPL-3.0](LICENSE); a biblioteca, suas
fontes e ferramentas têm as licenças indicadas pelo MSXgl.
Veja [OUTLINE.md](OUTLINE.md) para a visão e [resume.txt](resume.txt) para
o registro de decisões, andamento e próximos passos.
