# Alcatraz — visão de trabalho

Documento vivo; decisões de design abaixo são propostas, não funcionalidades
já implementadas.

## Premissa e impressões

O original de Wilson Fazzio Martins é uma aventura para MSX1 em BASIC:
exploração de um labirinto, interação por verbos e objetos e desafios de fuga.
O mapa semigráfico é parte de sua identidade. A análise de Marcelo / MarMSX
é uma referência importante para reconstruir regras e dependências, sem
confundir a documentação com o produto novo.

Em San Quentin, a edição MSX2 combina ilustrações, texto e inventário em
painéis; sua abertura tem trilha FM + PSG. Alcatraz será adaptado para
salas interconectadas, em vez da visão superior do labirinto original.
As regras e os quebra-cabeças serão estudados no BASIC de Alcatraz; o mapa
de salas será definido a partir do material que o usuário ainda enviará.

## Direção visual

SCREEN 5 é a escolha inicial: 256 pixels de largura, 16 cores escolhidas
entre 512 e comandos gráficos do VDP. Permite cores por pixel e regiões
ilustradas mais flexíveis que SCREEN 2; o custo de memória e transferência
de imagens deve ser medido no hardware de destino. A metade superior terá a
ilustração do local à esquerda e inventário desenhado à direita; a metade
inferior terá a saída de texto e a entrada de comandos. A tela atual é
apenas um diagnóstico, não o layout final.

Paleta: preto e azuis escuros para profundidade, branco para informação e
vermelho como acento dramático. Desenvolver arte original, coesa e legível
em 256 pixels; avaliar uso de páginas de VRAM e atualizações parciais.
Uma tela de título abre após o diagnóstico: logotipo do BASIC desenhado
sobre a imagem em SCREEN 5, desenhado progressivamente letra por letra, com
créditos e instrução de espaço. A imagem usa 14 cores e o logotipo usa um
terracota reservado, escolhido em harmonia com a fotografia. O modo suporta até 212
linhas, mas a interface final e a necessidade de
VRAM devem ser validadas antes de serem fixadas.

A abertura Disprosoft precede o diagnóstico, em SCREEN 2, em homenagem à
apresentação da listagem BASIC fornecida. A tradução desenha primitivas em C;
os arcos usam uma aproximação inteira e não são idênticos pixel a pixel.
O modo principal continua sendo SCREEN 5.

## Som e narrativa

Proposta: blues triste do Delta do Mississippi, com alternativa mais pesada
próxima do heavy metal; experimentar uma combinação atmosférica. FM é a base,
PSG complementa. Compor material original e estudar silêncio,
repetição e efeitos; não presumir uma época histórica para a narrativa.

## Plataforma e limitações

Alvo solicitado: MSX 2+ ou superior, DOS 2, mapper principal de 2048 KB,
PSG e MSX-MUSIC. O original é MSX1, mas esta recriação não
promete compatibilidade com MSX1 ou MSX2. A RAM é total, não RAM livre
garantida; a alocação real deverá ser verificada quando houver recursos.
O diagnóstico atual não garante descobrir FM sem ROM de
identificação. Dispositivos incompatíveis recebem indicação e não iniciam
o jogo; não existe jogo implementado ainda.

## Segmentação proposta

1. **Fundação:** diagnóstico, build reproduzível, versões e testes em emulador.
2. **Pesquisa:** inventariar salas, saídas, objetos, parser e condições;
   separar fatos verificados de hipóteses e registrar procedência; aguardar
   o mapa do usuário antes de definir a conversão para salas.
3. **Motor:** estado, mapa, navegação, inventário, parser, ações e persistência;
   testes focados e comparação com o original.
4. **Apresentação:** identidade visual, interface em painéis e cenas;
   protótipo medido quanto a memória e tempo de desenho.
5. **Áudio:** protótipos musicais originais, FM + PSG.
6. **Integração:** percursos, regressões, acessibilidade da leitura, testes em
   diferentes configurações e hardware real; empacotamento e distribuição.

## Conversas e decisões

### 2026-11-27

O projeto parte do BASIC e de documentos locais, mas pretende uma recriação
gráfica no espírito da edição MSX2 de San Quentin. Solicitados MSX 2+, mapper
de 2048 KB, DOS 2, PSG, FM obrigatório e SCC opcional; paleta vermelha, azul,
branca e preta; possível blues triste ou heavy metal. Versões seguem X.Y,
com Y por compilação e nomes de presos de Alcatraz por marco. Conta Copilot
preferida confirmada: `wilsonpilon`.

San Quentin foi encontrado em `E:\sanquentin` e examinado: SCREEN 5,
três painéis e FM + PSG são boas referências. Propõe-se SCREEN 5 para este
projeto. Inicialmente, os documentos de referência ficaram locais.

Na conversa seguinte, foi informada a autorização da Aleph para uso dos
livros e confirmada a permissão para publicar os materiais de terceiros.
As referências foram incluídas temporariamente em [docs](docs/README.md),
sem alteração; suas licenças serão normalizadas conforme necessário, em
contato com os autores.

Na revisão seguinte, foi solicitada a correção do carregamento da imagem
SC5 e confirmada a autorização para publicar a imagem fornecida e sua
conversão. Enter avança do diagnóstico compatível; Esc sai para o DOS.

Na conversa seguinte, foi solicitada a reprodução da abertura Disprosoft
em SCREEN 2, antes do jogo, e a separação profissional por responsabilidades.
A listagem local contém uma junção das linhas 40 e 41, interpretada no teste
como duas instruções distintas, sem modificar o original. Comparação no
openMSX: mesma paleta e texto, 19 pixels de diferença nos contornos. O código
foi separado em inicialização, abertura Disprosoft, diagnóstico e imagem SC5.
O responsável confirmou ter as permissões necessárias para publicar a
reprodução da abertura e a listagem BASIC de referência.

Na conversa seguinte, foi solicitada a tela de título com o logotipo
estilizado original sobre a imagem SC5, crédito ao autor e ao remake, e
espera pela tecla espaço. A listagem indicada `docs\intro.bas` estava vazia;
as coordenadas equivalentes foram encontradas em `docs\abertura.asc`.
O espaço abre uma tela provisória, pois o jogo ainda não existe. Animação
de tiros e música de introdução ficam para instruções futuras.

Na conversa seguinte, foi solicitada a exibição da fotografia antes do
desenho progressivo, uma cor mais suave e, se possível, letras e fontes
melhores. O logotipo mantém as formas originais reconhecíveis, agora em
terracota com sombra escura; os textos usam outra fonte MSXgl. Esc também
interrompe o desenho. A animação de tiros e a música continuam futuras.

Na conversa seguinte, o crédito do remake foi alterado para Wilson "Barney"
Pilon e centralizado na tela.

Na conversa seguinte, o usuário pediu uma rajada diagonal após Espaço, com
impactos de vidro e sons, seguida de uma pausa de cerca de dois segundos.
Os créditos e a instrução desaparecem antes dos tiros; o fundo original é
restaurado. Os efeitos usam PSG e FM. O usuário optou por adiar o SCC,
pois a detecção automática disponível pode alterar outros cartuchos.

Na conversa seguinte, o usuário aprovou os gráficos, pediu tiros mais
encorpados e decidiu eliminar o suporte a SCC e sua linha no diagnóstico.
Os disparos combinam ruído PSG com corpo e percussão de bumbo e caixa FM.

Na conversa seguinte, o usuário definiu a adaptação do labirinto para salas
interconectadas, com parser textual e interface no estilo de San Quentin.
Na metade superior ficam cena à esquerda e inventário desenhado à direita;
na inferior, mensagens e comando. O usuário enviará o mapa completo.

### Estudo comparativo: San Quentin e Alcatraz

- Em `E:\sanquentin\src\sq_game_ui_msx2.c`, SCREEN 5 usa 256 × 212
  pixels: a cena ocupa aproximadamente x=5..125 e y=20..102; o inventário
  ocupa o painel direito. A faixa de mensagens tem nove linhas de até 40
  caracteres, e a entrada fica separada na base. F1–F4 enviam as direções.
- A cena de San Quentin é composta por primitivas gráficas e pode mudar
  conforme o estado; o inventário apresenta ícones de 16 × 16 e nomes,
  com até três itens visíveis. A interface redesenha os painéis quando
  localização, condições ou objetos mudam, e rola o texto usando o VDP.
  Esses números são referência, não limites já decididos para Alcatraz.
- Em San Quentin, `sq_input.c`, `sq_parser.c`, `sq_engine.c` e
  `sq_game.c` separam entrada, interpretação, regras e ciclo do jogo.
  O parser de San Quentin aceita prefixos; isso não define automaticamente
  a linguagem do remake de Alcatraz.
- Em `docs\alcatraz.asc`, o BASIC guarda o labirinto em `PO$`, com quatro
  códigos direcionais por posição. A movimentação consulta esses códigos,
  que também podem ser alterados por ações. Objetos são associados a
  coordenadas ou ao inventário em `SU$`; comandos podem envolver verbo e
  até dois objetos, e flags controlam condições como luz e equipamentos.
  Preservar essas dependências exige estudar as ações e os pontos de
  passagem antes de agrupar posições em salas.
- Próximo passo de pesquisa: confrontar o mapa a receber com o BASIC e
  documentar, para cada sala e passagem, objetos, ações, bloqueios,
  mudanças de estado e consequências. Não presumir quantidade de salas,
  limite de inventário ou conexões definitivas antes dessa análise.
