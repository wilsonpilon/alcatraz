# Alcatraz — solução de referência

Percurso proposto a partir do roteiro fornecido (Luiz Fernando Fiacadori), do roteiro de Alex Dijkstra e da listagem portuguesa. As direções foram calculadas sobre os códigos de [MAPA.md](MAPA.md), com as alterações e condições relevantes indicadas abaixo. Uma reprodução automatizada no openMSX alcançou a vitória no BASIC original em inglês (`docs/alcatraz.bas`), usando a tradução dos comandos; isso não testa todas as ações nem o remake.

**Spoilers — referência temporária de desenvolvimento.** No remake atual, apenas R079, R078 e R077 estão implementadas. O restante do percurso descreve o original e ainda precisa ser implementado e validado no remake.

Cada direção leva a uma sala; não pule as posições intermediárias. Para repetir o teste: `python tests/run_original_route.py --openmsx caminho\openmsx.exe`.

## 1. R079

Deslocamento: já está na sala.

- `PEGUE LENCOL`
- `RASGUE LENCOL`
- `AMARRE TIRAS`

## 2. R078

Deslocamento: O.

- `PECA COMIDA GUARDA`
- `PEGUE COMIDA`
- `AMARRE CORDA GRADES`
- `JOGUE CORDA`
- `PUXE CORDA`
- `ABRA GRADES CHAVE`

## 3. R066

Deslocamento: O → N → N → L → L → L → S.

- `ABRA GRADES CHAVE`
- `SOLTE CHAVE`

## 4. R064

Deslocamento: O → O.

- `PEGUE COBERTOR`
- `DE COMIDA PRESIDIARIO`

## 5. R024

Deslocamento: L → L → N → N → N.

- `PEGUE SERRA`

## 6. R092

Deslocamento: S → S → O → O → O → S → S → S → L.

- `DE DINHEIRO GUARDA`

## 7. R104

Deslocamento: S → O → O.

- `PEGUE LANTERNA`

## 8. R035

Deslocamento: L → L → N → O → N → N → N → N.

- `COLOQUE SERRA TOMADA`
- `LIGUE SERRA`
- `CORTE PORTA SERRA`
- `SOLTE SERRA`

## 9. R023

Deslocamento: N → L → S → L → N.

- `PEGUE PILHA`
- `PEGUE BARALHO`
- `COLOQUE PILHA LANTERNA`
- `LIGUE LANTERNA`

## 10. R108

Deslocamento: S → O → N → O → S → S → L → L → L → S → S → S → S.

- `PEGUE FOSFOROS`

## 11. R132

Deslocamento: N → N → N → N → O → O → O → S → S → S → L → S → L → S → S → O → O → O.

- `SOLTE BARALHO`
- `PEGUE JORNAL`

## 12. R116

Deslocamento: O → O → N.

- `COLOQUE JORNAL PORTA`
- `QUEIME JORNAL FOSFOROS`
- `SOLTE FOSFOROS`
- `TIRE PILHA LANTERNA`
- `SOLTE LANTERNA`

## 13. R132

Deslocamento: S → L → L.

- `PEGUE BARALHO`

## 14. R102

Deslocamento: O → O → N → N.

- `COLOQUE COBERTOR FOGO`

## 15. R127

Deslocamento: O → S → S → O → N → N → N → N → O → S → S → S → S.

- `PEGUE AGUA`

## 16. R120

Deslocamento: N → N → N → N → L → S → S → S → S → L → N → N → L → L → S → L → L → L.

- `PEGUE ESPELHO`

## 17. R048

Deslocamento: O → O → O → N → O → N → N → N → N → L → S → S → S → L → N → N → N.

- `COLOQUE AGUA ALARME`

## 18. R057

Deslocamento: S → S → S → O → N → N → N → O → O → S → O → N → N → N → N → O → S → S → S → S.

- `PEGUE FITA`

## 19. R006

Deslocamento: N → N → N → N → L → S → S → S → S → L → N → L → L → S → S → S → L → N → N → N → N → N → N.

- `COLOQUE ESPELHO LUZ`

## 20. R031

Deslocamento: O → O → O → S → L → L → S → O → O.

- `PEGUE GRAVADOR`

## 21. R136

Deslocamento: L → L → N → O → O → N → L → L → L → L → L → L → L → L → S → S → S → S → S → S → S → S → O → S.

- `COLOQUE PILHA GRAVADOR`
- `COLOQUE FITA GRAVADOR`
- `LIGUE GRAVADOR`

## 22. R137

Deslocamento: L.


Os cães deixam passar com o gravador ligado e carregado.

## 23. R012

Deslocamento: L → N → N → L → L → N → O → N → N → N → N → N → O → N.

- `DE BARALHO CARCEREIRO`

## 24. R084

Deslocamento: L → L → S → S → S → S → S.

- `COLOQUE CORDA POCO`
- `ENTRE POCO`

A entrada no poço com a corda colocada leva à vitória. A pá, a terra, o dinheiro atrás dela e a roupa não são necessários neste percurso; podem ser explorados separadamente.
