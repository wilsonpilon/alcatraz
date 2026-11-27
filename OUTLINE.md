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
painéis; sua abertura tem trilha FM + PSG. Para Alcatraz, a intenção é
preservar a leitura espacial do labirinto, mas criar cenas, interface e
atmosfera próprias, não copiar artes ou música de terceiros.

## Direção visual

SCREEN 5 é a escolha inicial: 256 pixels de largura, 16 cores escolhidas
entre 512 e comandos gráficos do VDP. Permite cores por pixel e regiões
ilustradas mais flexíveis que SCREEN 2; o custo de memória e transferência
de imagens deve ser medido no hardware de destino. Planejar imagens de salas,
texto/comandos, inventário e uma representação do mapa, com legibilidade
prioritária. A tela atual é apenas um diagnóstico, não o layout final.

Paleta: preto e azuis escuros para profundidade, branco para informação e
vermelho como acento dramático. Desenvolver arte original, coesa e legível
em 256 pixels; avaliar uso de páginas de VRAM e atualizações parciais.
Uma tela de título abre após o diagnóstico: logotipo do BASIC desenhado
sobre a imagem em SCREEN 5, com créditos e instrução de espaço. A imagem
usa 14 cores e o logotipo usa vermelho reservado. O modo suporta até 212
linhas, mas a interface final e a necessidade de
VRAM devem ser validadas antes de serem fixadas.

A abertura Disprosoft precede o diagnóstico, em SCREEN 2, em homenagem à
apresentação da listagem BASIC fornecida. A tradução desenha primitivas em C;
os arcos usam uma aproximação inteira e não são idênticos pixel a pixel.
O modo principal continua sendo SCREEN 5.

## Som e narrativa

Proposta: blues triste do Delta do Mississippi, com alternativa mais pesada
próxima do heavy metal; experimentar uma combinação atmosférica. FM é a base,
PSG complementa, SCC é opcional. Compor material original e estudar silêncio,
repetição e efeitos; não presumir uma época histórica para a narrativa.

## Plataforma e limitações

Alvo solicitado: MSX 2+ ou superior, DOS 2, mapper principal de 2048 KB,
PSG e MSX-MUSIC; SCC opcional. O original é MSX1, mas esta recriação não
promete compatibilidade com MSX1 ou MSX2. A RAM é total, não RAM livre
garantida; a alocação real deverá ser verificada quando houver recursos.
O diagnóstico atual não testa SCC e não garante descobrir FM sem ROM de
identificação. Dispositivos incompatíveis recebem indicação e não iniciam
o jogo; não existe jogo implementado ainda.

## Segmentação proposta

1. **Fundação:** diagnóstico, build reproduzível, versões e testes em emulador.
2. **Pesquisa:** inventariar salas, saídas, objetos, parser e condições;
   separar fatos verificados de hipóteses e registrar procedência.
3. **Motor:** estado, mapa, navegação, inventário, parser, ações e persistência;
   testes focados e comparação com o original.
4. **Apresentação:** identidade visual, interface em painéis, cenas e mapa
   gráfico; protótipo medido quanto a memória e tempo de desenho.
5. **Áudio:** protótipos musicais originais, FM + PSG e SCC opcional.
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
