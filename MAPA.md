# Alcatraz — mapa de salas inicial

Gerado de `docs/alcatraz.asc` por `tools/mapa_alcatraz.py`. É um mapa de referência, não um jogo implementado.

Cada posição da grade BASIC corresponde a uma sala, inclusive corredores vazios. R001 é (PC=0, PL=2), R140 é (13,11) e o início é R079 (8,7). Norte diminui PL; leste aumenta PC. As conexões abaixo são direcionais e representam o estado inicial, antes de qualquer ação.

No diagrama `-` e `|` indicam passagem livre nos dois sentidos; `?` indica passagem condicional, fatal ou assimétrica. A tabela é a autoridade para o sentido e a condição. Um destino fatal é mostrado para localizar a tentativa, mas a morte ocorre antes da entrada. R093 (cadeira elétrica) não é uma sala normalmente alcançável.

```text
          0      1      2      3      4      5      6      7      8      9     10     11     12     13   PC
PL=02  R001 - R002   R003 - R004 - R005 ? R006 - R007 - R008 - R009 - R010 - R011   R012 ? R013 - R014
        |      |      |                    |                                  |      |             |
PL=03  R015   R016   R017 - R018 - R019   R020   R021 - R022   R023   R024   R025   R026 - R027   R028
        |      |                    |      |      ?      |      |      |      |             |      |
PL=04  R029   R030   R031 - R032 - R033   R034   R035   R036 - R037   R038   R039   R040   R041   R042
        |      |                           ?      |                    |      |      |      |      |
PL=05  R043   R044   R045 - R046 - R047   R048   R049 - R050 - R051 - R052   R053   R054   R055   R056
        |      |      |      |      |      |      |                    |      |      |      |      |
PL=06  R057   R058 - R059   R060   R061   R062   R063   R064 - R065 ? R066   R067   R068 - R069   R070
                      |      |      |      |      |                    ?      |             |      |
PL=07  R071 - R072   R073   R074   R075   R076   R077 ? R078 - R079   R080   R081   R082   R083   R084
        |      |      |      |      |      |      |                    ?      |      |      |
PL=08  R085   R086   R087   R088   R089 - R090   R091 - R092 ? R093   R094   R095   R096 - R097 - R098
        |      |             ?                           ?             ?      |                    |
PL=09  R099   R100   R101 - R102 - R103   R104 - R105 - R106 - R107   R108   R109   R110 - R111 - R112
        |      |      |      ?      |                           |             |      |             ?
PL=10  R113   R114   R115   R116   R117 - R118 - R119 - R120   R121   R122 - R123   R124   R125 - R126
        |      |      |      |                                  |      |             |      |
PL=11  R127   R128 - R129   R130 - R131 - R132 - R133 - R134 - R135   R136 ? R137 ? R138   R139 - R140
```

## Legenda dos códigos

- **0** — livre.
- **1** — parede.
- **2** — grades.
- **3** — guarda.
- **4** — cadeira elétrica: morte.
- **5** — porta fechada.
- **6** — escuridão: exige lanterna acesa em mãos.
- **7** — fogo: morte.
- **8** — alarme: morte.
- **9** — feixe de luz: morte.
- **A** — cães: exige gravador ligado em mãos.
- **B** — terra.
- **C** — carcereiro: morte.
- **D** — saída do prédio para o pátio.

Passagens condicionais podem deixar de impedir o avanço por ações ou mudanças de estado. A lista não transforma paredes em saídas: consulte as regras do BASIC e os efeitos sobre `PO$` em cada posição. O código D atravessa após exibir uma mensagem; não é a vitória final.

## Salas e saídas iniciais

Os poucos nomes são descritivos, inferidos dos mapas e dos objetos, não nomes de salas fornecidos pelo BASIC; as demais posições ficam sem nome, inclusive os corredores vazios. A coluna de objetos é a posição inicial, não o inventário atual. Grades e portas são contextuais no BASIC e aparecem pelas condições de passagem.

| Sala | Nome descritivo | PC,PL | Norte | Sul | Leste | Oeste | Objetos ou personagens iniciais |
|---|---|---|---|---|---|---|---|
| R001 | — | 0,2 | — | R015 | R002 | — | — |
| R002 | — | 1,2 | — | R016 | — | R001 | — |
| R003 | — | 2,2 | — | R017 | R004 | — | — |
| R004 | — | 3,2 | — | — | R005 | R003 | — |
| R005 | — | 4,2 | — | — | R006 | R004 | — |
| R006 | — | 5,2 | — | R020 | R007 | R005 [9] | LUZ |
| R007 | — | 6,2 | — | — | R008 | R006 | — |
| R008 | — | 7,2 | — | — | R009 | R007 | — |
| R009 | — | 8,2 | — | — | R010 | R008 | — |
| R010 | — | 9,2 | — | — | R011 | R009 | — |
| R011 | — | 10,2 | — | R025 | — | R010 | — |
| R012 | — | 11,2 | — | R026 | R013 [C] | — | CARCEREIRO |
| R013 | — | 12,2 | — | — | R014 | R012 | — |
| R014 | — | 13,2 | — | R028 | — | R013 | — |
| R015 | — | 0,3 | R001 | R029 | — | — | — |
| R016 | — | 1,3 | R002 | R030 | — | — | — |
| R017 | — | 2,3 | R003 | — | R018 | — | — |
| R018 | — | 3,3 | — | — | R019 | R017 | — |
| R019 | — | 4,3 | — | R033 | — | R018 | — |
| R020 | — | 5,3 | R006 | R034 | — | — | — |
| R021 | — | 6,3 | — | R035 | R022 | — | — |
| R022 | — | 7,3 | — | R036 | — | R021 | — |
| R023 | — | 8,3 | — | R037 | — | — | PILHA, BARALHO |
| R024 | — | 9,3 | — | R038 | — | — | SERRA |
| R025 | — | 10,3 | R011 | R039 | — | — | — |
| R026 | — | 11,3 | R012 | — | R027 | — | — |
| R027 | — | 12,3 | — | R041 | — | R026 | — |
| R028 | — | 13,3 | R014 | R042 | — | — | — |
| R029 | — | 0,4 | R015 | R043 | — | — | — |
| R030 | — | 1,4 | R016 | R044 | — | — | — |
| R031 | — | 2,4 | — | — | R032 | — | GRAVADOR |
| R032 | — | 3,4 | — | — | R033 | R031 | — |
| R033 | — | 4,4 | R019 | — | — | R032 | — |
| R034 | — | 5,4 | R020 | R048 | — | — | — |
| R035 | — | 6,4 | R021 [5] | R049 | — | — | TOMADA |
| R036 | — | 7,4 | R022 | — | R037 | — | — |
| R037 | — | 8,4 | R023 | — | — | R036 | — |
| R038 | — | 9,4 | R024 | R052 | — | — | — |
| R039 | — | 10,4 | R025 | R053 | — | — | — |
| R040 | — | 11,4 | — | R054 | — | — | PA |
| R041 | — | 12,4 | R027 | R055 | — | — | — |
| R042 | — | 13,4 | R028 | R056 | — | — | — |
| R043 | — | 0,5 | R029 | R057 | — | — | — |
| R044 | — | 1,5 | R030 | R058 | — | — | — |
| R045 | — | 2,5 | — | R059 | R046 | — | — |
| R046 | — | 3,5 | — | R060 | R047 | R045 | — |
| R047 | — | 4,5 | — | R061 | — | R046 | — |
| R048 | — | 5,5 | R034 [8] | R062 | — | — | ALARME |
| R049 | — | 6,5 | R035 | R063 | R050 | — | — |
| R050 | — | 7,5 | — | — | R051 | R049 | — |
| R051 | — | 8,5 | — | — | R052 | R050 | — |
| R052 | — | 9,5 | R038 | R066 | — | R051 | — |
| R053 | — | 10,5 | R039 | R067 | — | — | ROUPA |
| R054 | — | 11,5 | R040 | R068 | — | — | — |
| R055 | — | 12,5 | R041 | R069 | — | — | — |
| R056 | — | 13,5 | R042 | R070 | — | — | — |
| R057 | — | 0,6 | R043 | — | — | — | FITA |
| R058 | — | 1,6 | R044 | — | R059 | — | — |
| R059 | — | 2,6 | R045 | R073 | — | R058 | — |
| R060 | — | 3,6 | R046 | R074 | — | — | — |
| R061 | — | 4,6 | R047 | R075 | — | — | — |
| R062 | — | 5,6 | R048 | R076 | — | — | — |
| R063 | — | 6,6 | R049 | R077 | — | — | — |
| R064 | Cela do presidiário | 7,6 | — | — | R065 | — | COBERTOR, PRESIDIARIO |
| R065 | — | 8,6 | — | — | R066 | R064 | — |
| R066 | — | 9,6 | R052 | R080 [6] | — | R065 [2] | — |
| R067 | — | 10,6 | R053 | R081 | — | — | — |
| R068 | — | 11,6 | R054 | — | R069 | — | — |
| R069 | — | 12,6 | R055 | R083 | — | R068 | — |
| R070 | — | 13,6 | R056 | R084 | — | — | — |
| R071 | — | 0,7 | — | R085 | R072 | — | — |
| R072 | — | 1,7 | — | R086 | — | R071 | — |
| R073 | — | 2,7 | R059 | R087 | — | — | — |
| R074 | — | 3,7 | R060 | R088 | — | — | — |
| R075 | — | 4,7 | R061 | R089 | — | — | — |
| R076 | — | 5,7 | R062 | R090 | — | — | — |
| R077 | — | 6,7 | R063 | R091 | R078 | — | — |
| R078 | — | 7,7 | — | — | R079 | R077 [2] | GUARDA |
| R079 | Cela inicial | 8,7 | — | — | — | R078 | LENCOL, CAMA |
| R080 | — | 9,7 | R066 [6] | R094 [6] | — | — | — |
| R081 | — | 10,7 | R067 | R095 | — | — | — |
| R082 | — | 11,7 | — | R096 | — | — | BEBIDA |
| R083 | — | 12,7 | R069 | R097 | — | — | — |
| R084 | Poço | 13,7 | R070 | — | — | — | POCO |
| R085 | — | 0,8 | R071 | R099 | — | — | — |
| R086 | — | 1,8 | R072 | R100 | — | — | — |
| R087 | — | 2,8 | R073 | — | — | — | OSSO |
| R088 | — | 3,8 | R074 | R102 | — | — | — |
| R089 | — | 4,8 | R075 | — | R090 | — | — |
| R090 | — | 5,8 | R076 | — | — | R089 | — |
| R091 | — | 6,8 | R077 | — | R092 | — | — |
| R092 | — | 7,8 | — | R106 [3] | R093 [4] | R091 | — |
| R093 | Cadeira elétrica | 8,8 | — | — | — | — | — |
| R094 | — | 9,8 | R080 [6] | R108 [6] | — | — | — |
| R095 | — | 10,8 | R081 | R109 | — | — | — |
| R096 | — | 11,8 | R082 | — | R097 | — | — |
| R097 | — | 12,8 | R083 | — | R098 | R096 | — |
| R098 | — | 13,8 | — | R112 | — | R097 | — |
| R099 | — | 0,9 | R085 | R113 | — | — | — |
| R100 | — | 1,9 | R086 | R114 | — | — | — |
| R101 | — | 2,9 | — | R115 | R102 | — | — |
| R102 | — | 3,9 | R088 [7] | R116 | R103 | R101 | FOGO |
| R103 | — | 4,9 | — | R117 | — | R102 | — |
| R104 | — | 5,9 | — | — | R105 | — | LANTERNA, REVOLVER |
| R105 | — | 6,9 | — | — | R106 | R104 | — |
| R106 | — | 7,9 | R092 | — | R107 | R105 | — |
| R107 | — | 8,9 | — | R121 | — | R106 | — |
| R108 | — | 9,9 | R094 [6] | — | — | — | FOSFOROS |
| R109 | — | 10,9 | R095 | R123 | — | — | — |
| R110 | — | 11,9 | — | R124 | R111 | — | — |
| R111 | — | 12,9 | — | — | R112 | R110 | — |
| R112 | — | 13,9 | R098 | R126 [B] | — | R111 | TERRA |
| R113 | — | 0,10 | R099 | R127 | — | — | — |
| R114 | — | 1,10 | R100 | R128 | — | — | — |
| R115 | — | 2,10 | R101 | R129 | — | — | — |
| R116 | — | 3,10 | R102 [5] | R130 | — | — | — |
| R117 | — | 4,10 | R103 | — | R118 | — | — |
| R118 | — | 5,10 | — | — | R119 | R117 | — |
| R119 | — | 6,10 | — | — | R120 | R118 | — |
| R120 | — | 7,10 | — | — | — | R119 | ESPELHO |
| R121 | — | 8,10 | R107 | R135 | — | — | — |
| R122 | — | 9,10 | — | R136 | R123 | — | — |
| R123 | — | 10,10 | R109 | — | — | R122 | — |
| R124 | — | 11,10 | R110 | R138 | — | — | — |
| R125 | — | 12,10 | — | R139 | R126 | — | — |
| R126 | — | 13,10 | R112 | — | — | R125 | — |
| R127 | — | 0,11 | R113 | — | — | — | AGUA |
| R128 | — | 1,11 | R114 | — | R129 | — | — |
| R129 | — | 2,11 | R115 | — | — | R128 | — |
| R130 | — | 3,11 | R116 | — | R131 | — | — |
| R131 | — | 4,11 | — | — | R132 | R130 | — |
| R132 | — | 5,11 | — | — | R133 | R131 | JORNAL |
| R133 | — | 6,11 | — | — | R134 | R132 | — |
| R134 | — | 7,11 | — | — | R135 | R133 | — |
| R135 | — | 8,11 | R121 | — | — | R134 | — |
| R136 | Casa dos cães | 9,11 | R122 | — | R137 [A] | — | CAES |
| R137 | — | 10,11 | — | — | R138 [D] | R136 [A] | — |
| R138 | Pátio | 11,11 | R124 | — | — | R137 | — |
| R139 | — | 12,11 | R125 | — | R140 | — | — |
| R140 | — | 13,11 | — | — | — | R139 | DINHEIRO |

## Correspondência com o roteiro ilustrado

Os números identificam pontos do mapa e do roteiro fornecidos; não substituem as 140 posições. Itens em uma posição são os do estado inicial e podem ser movidos ou consumidos.

| Ponto | Sala | Referência |
|---:|---|---|
| 1 | R079 | lençol e cama |
| 2 | R078 | guarda e grades |
| 3 | R066 | grades |
| 4 | R064 | cobertor e presidiário |
| 5 | R024 | serra |
| 6 | R035 | tomada e porta |
| 7 | R023 | pilha e baralho |
| 8 | R092 | guarda e cadeira elétrica |
| 9 | R104 | lanterna e revólver |
| 10 | R108 | fósforos |
| 11 | R132 | jornal |
| 12 | R116 | porta |
| 13 | R102 | fogo |
| 14 | R127 | água |
| 15 | R120 | espelho |
| 16 | R057 | fita |
| 17 | R048 | alarme |
| 18 | R006 | feixe de luz |
| 19 | R031 | gravador |
| 20 | R053 | roupa |
| 21 | R136 | cães |
| 22 | R138 | saída para o pátio |
| 23 | R040 | pá |
| 24 | R112 | terra |
| 25 | R140 | dinheiro |
| 26 | R012 | carcereiro |
| 27 | R084 | poço |

## Alterações de passagem

O BASIC altera `PO$(PC,PL)` em ações de apagar fogo, desativar alarmes, cortar ou queimar portas, abrir grades, subornar personagens e cavar. Essas alterações são locais e direcionais: a tabela acima não deve ser tratada como uma lista permanente de saídas nem simetrizada automaticamente. A posição e as condições exatas devem ser conferidas nas sub-rotinas da listagem.
