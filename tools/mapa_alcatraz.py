"""Generate research references from the Portuguese BASIC listing."""

import csv
import io
import json
import re
from collections import deque
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / "docs" / "alcatraz.asc"
DIRECTIONS = (("N", 0, -1), ("S", 0, 1), ("L", 1, 0), ("O", -1, 0))
PASSAGES = {
    "0": "livre",
    "1": "parede",
    "2": "grades",
    "3": "guarda",
    "4": "cadeira elétrica: morte",
    "5": "porta fechada",
    "6": "escuridão: exige lanterna acesa em mãos",
    "7": "fogo: morte",
    "8": "alarme: morte",
    "9": "feixe de luz: morte",
    "A": "cães: exige gravador ligado em mãos",
    "B": "terra",
    "C": "carcereiro: morte",
    "D": "saída do prédio para o pátio",
}


def basic_lines(text):
    matches = list(re.finditer(r"(?m)^(\d+)\s*(?=[A-Za-z'])", text))
    result = {}
    for i, match in enumerate(matches):
        end = matches[i + 1].start() if i + 1 < len(matches) else len(text)
        number = int(match.group(1))
        if number in result:
            raise ValueError(f"Linha BASIC duplicada: {number}")
        result[number] = text[match.end():end].replace("\r", "").replace("\n", " ").strip()
    return result


def data(lines, *numbers):
    entries = []
    for number in numbers:
        line = lines[number]
        if not line.startswith("DATA"):
            raise ValueError(f"Linha {number} não é DATA")
        entries.extend(next(csv.reader(io.StringIO(line[4:].strip()), skipinitialspace=True)))
    return [entry.strip() for entry in entries]


def room_id(x, y):
    if not (0 <= x < 14 and 2 <= y <= 11):
        raise ValueError(f"Coordenada fora do mapa: ({x}, {y})")
    return f"R{(y - 2) * 14 + x + 1:03}"


def initial_data(lines):
    rooms = [data(lines, n) for n in range(3270, 3370, 10)]
    if len(rooms) != 10 or any(len(row) != 14 for row in rooms):
        raise ValueError("A grade BASIC deveria ter 14 colunas e 10 linhas")
    for row in rooms:
        for cell in row:
            if len(cell) != 4 or any(char not in PASSAGES for char in cell):
                raise ValueError(f"Códigos de passagem inválidos: {cell!r}")

    objects = data(lines, 3390, 3400)
    verbs = data(lines, 3430)
    actions = data(lines, 3460, 3470)
    if (len(objects), len(verbs), len(actions)) != (35, 24, 55):
        raise ValueError("Quantidade inesperada de substantivos, verbos ou ações")
    return rooms, objects, verbs, actions


def object_location(obj):
    code = obj[:2]
    if code == "JJ":
        return "fora do jogo inicialmente (JJ)"
    if code == "..":
        return "objeto contextual (..)"
    if len(code) != 2:
        raise ValueError(f"Localização inválida: {obj!r}")
    return room_id(ord(code[0]) - 60, ord(code[1]) - 60)


def destination(x, y, direction, code):
    if code == "1":
        return "—"
    _, dx, dy = DIRECTIONS[direction]
    dest = room_id(x + dx, y + dy)
    return dest if code == "0" else f"{dest} [{code}]"


def map_mark(a, b):
    if a == "1" and b == "1":
        return " "
    if a == "0" and b == "0":
        return "-"
    return "?"


def render_map(rooms, objects):
    by_room = {}
    room_names = {
        "R064": "Cela do presidiário",
        "R079": "Cela inicial",
        "R084": "Poço",
        "R093": "Cadeira elétrica",
        "R136": "Casa dos cães",
        "R138": "Pátio",
    }
    for obj in objects:
        location = object_location(obj)
        if location.startswith("R"):
            by_room.setdefault(location, []).append(obj[2:])

    out = [
        "# Alcatraz — mapa de salas inicial",
        "",
        "Gerado de `docs/alcatraz.asc` por `tools/mapa_alcatraz.py`. "
        "É um mapa de referência, não um jogo implementado.",
        "",
        "Cada posição da grade BASIC corresponde a uma sala, inclusive corredores "
        "vazios. R001 é (PC=0, PL=2), R140 é (13,11) e o início é R079 (8,7). "
        "Norte diminui PL; leste aumenta PC. As conexões abaixo são direcionais "
        "e representam o estado inicial, antes de qualquer ação.",
        "",
        "No diagrama `-` e `|` indicam passagem livre nos dois sentidos; "
        "`?` indica passagem condicional, fatal ou assimétrica. A tabela é "
        "a autoridade para o sentido e a condição. Um destino fatal é mostrado "
        "para localizar a tentativa, mas a morte ocorre antes da entrada. "
        "R093 (cadeira elétrica) não é uma sala normalmente alcançável.",
        "",
        "```text",
        "       " + "   ".join(f"{x:4}" for x in range(14)) + "   PC",
    ]
    for yi, row in enumerate(rooms):
        y = yi + 2
        line = f"PL={y:02}  "
        for x, cell in enumerate(row):
            line += room_id(x, y)
            if x < 13:
                line += " " + map_mark(cell[2], row[x + 1][3]) + " "
        out.append(line)
        if yi < 9:
            links = []
            for x, cell in enumerate(row):
                marker = map_mark(cell[1], rooms[yi + 1][x][0])
                links.append(" |  " if marker == "-" else " ?  " if marker == "?" else "    ")
            out.append(("       " + "   ".join(links)).rstrip())
    out.extend(["```", "", "## Legenda dos códigos", ""])
    for code, description in PASSAGES.items():
        out.append(f"- **{code}** — {description}.")
    out.extend([
        "",
        "Passagens condicionais podem deixar de impedir o avanço por ações "
        "ou mudanças de estado. "
        "A lista não transforma paredes em saídas: consulte as regras do BASIC "
        "e os efeitos sobre `PO$` em cada posição. O código D atravessa após "
        "exibir uma mensagem; não é a vitória final.",
        "",
        "## Salas e saídas iniciais",
        "",
        "Os poucos nomes são descritivos, inferidos dos mapas e dos objetos, "
        "não nomes de salas fornecidos pelo BASIC; as demais posições ficam "
        "sem nome, inclusive os corredores vazios. A coluna de objetos é a "
        "posição inicial, não o inventário atual. Grades e portas são "
        "contextuais no BASIC e aparecem pelas condições de passagem.",
        "",
        "| Sala | Nome descritivo | PC,PL | Norte | Sul | Leste | Oeste | Objetos ou personagens iniciais |",
        "|---|---|---|---|---|---|---|---|",
    ])
    for yi, row in enumerate(rooms):
        y = yi + 2
        for x, cell in enumerate(row):
            rid = room_id(x, y)
            exits = [destination(x, y, i, cell[i]) for i in range(4)]
            labels = ", ".join(by_room.get(rid, [])) or "—"
            name = room_names.get(rid, "—")
            out.append(f"| {rid} | {name} | {x},{y} | {' | '.join(exits)} | {labels} |")
    out.extend([
        "",
        "## Correspondência com o roteiro ilustrado",
        "",
        "Os números identificam pontos do mapa e do roteiro fornecidos; "
        "não substituem as 140 posições. Itens em uma posição são os do "
        "estado inicial e podem ser movidos ou consumidos.",
        "",
        "| Ponto | Sala | Referência |",
        "|---:|---|---|",
    ])
    markers = [
        ("R079", "lençol e cama"), ("R078", "guarda e grades"),
        ("R066", "grades"), ("R064", "cobertor e presidiário"),
        ("R024", "serra"), ("R035", "tomada e porta"),
        ("R023", "pilha e baralho"), ("R092", "guarda e cadeira elétrica"),
        ("R104", "lanterna e revólver"), ("R108", "fósforos"),
        ("R132", "jornal"), ("R116", "porta"), ("R102", "fogo"),
        ("R127", "água"), ("R120", "espelho"), ("R057", "fita"),
        ("R048", "alarme"), ("R006", "feixe de luz"),
        ("R031", "gravador"), ("R053", "roupa"),
        ("R136", "cães"), ("R138", "saída para o pátio"),
        ("R040", "pá"), ("R112", "terra"), ("R140", "dinheiro"),
        ("R012", "carcereiro"), ("R084", "poço"),
    ]
    for index, (rid, label) in enumerate(markers, 1):
        out.append(f"| {index} | {rid} | {label} |")
    out.extend([
        "",
        "## Alterações de passagem",
        "",
        "O BASIC altera `PO$(PC,PL)` em ações de apagar fogo, desativar "
        "alarmes, cortar ou queimar portas, abrir grades, subornar personagens "
        "e cavar. Essas alterações são locais e direcionais: a tabela acima "
        "não deve ser tratada como uma lista permanente de saídas nem "
        "simetrizada automaticamente. A posição e as condições exatas devem "
        "ser conferidas nas sub-rotinas da listagem.",
        "",
    ])
    return "\n".join(out)


def print_fragments(lines):
    entries = []
    for number, line in lines.items():
        if not 1 <= number <= 3095:
            continue
        for match in re.finditer(r"PRINT(?:#\d+)?\s*", line, re.IGNORECASE):
            if line[:match.start()].count('"') % 2:
                continue
            tail = line[match.end():]
            quoted = False
            end = len(tail)
            for i, character in enumerate(tail):
                if character == '"':
                    quoted = not quoted
                elif character == ":" and not quoted:
                    end = i
                    break
            fragments = re.findall(r'"([^"]*)"', tail[:end])
            if fragments:
                entries.append((number, fragments))
    return entries


def action_subroutines(lines):
    targets = []
    for number in (1330, 1350):
        match = re.search(r"GOSUB\s*([\d,\s]+)", lines[number])
        if not match:
            raise ValueError(f"Despacho não encontrado na linha {number}")
        targets.extend(int(n) for n in match.group(1).replace(" ", "").split(","))
    if len(targets) != 55:
        raise ValueError("Quantidade inesperada de destinos de ações")
    return targets


def decode_action(code, verbs, objects):
    verb = ord(code[0]) - 50
    if not 0 <= verb < len(verbs):
        raise ValueError(f"Verbo inválido em ação {code!r}")
    parts = [verbs[verb]]
    for char in code[1:]:
        noun = ord(char) - 50
        if not 0 <= noun < len(objects):
            raise ValueError(f"Substantivo inválido em ação {code!r}")
        parts.append(objects[noun][2:])
    return " + ".join(parts)


def render_reference(lines, objects, verbs, actions):
    targets = action_subroutines(lines)
    out = [
        "# Alcatraz — referência da listagem BASIC",
        "",
        "Gerada de `docs/alcatraz.asc`, listagem em português atribuída a "
        "Wilson Fazzio Martins. `docs/alcatraz.bas` é a versão em inglês; "
        "não presumir que seu vocabulário seja o mesmo.",
        "",
        "## Parser e estado",
        "",
        "- O original recebe até 30 caracteres, converte minúsculas a "
        "maiúsculas e aceita letras e espaços (linhas 920–1000).",
        "- Identifica o verbo pela primeira palavra e o primeiro substantivo "
        "pela palavra seguinte; quando há outro, procura-o após o último "
        "espaço. As comparações são exatas; palavras intermediárias podem "
        "ser ignoradas. A disponibilidade dos objetos é verificada nas "
        "linhas 1520–1560, com exceções próprias.",
        "- `SU$` guarda a posição e o nome dos substantivos: `**` é carregado, "
        "`..` é contextual, `JJ` é posição fora da grade e dois espaços "
        "indicam objeto removido. Soltar um objeto carregado grava a posição "
        "atual (linhas 2610–2640). O limite original é seis objetos: a "
        "sétima tentativa falha (linhas 2530–2550).",
        "- `PO$` contém, para cada posição, códigos de norte, sul, leste e "
        "oeste; `X(0)` a `X(9)` guardam condições. Há 45 combinações exatas "
        "e dez despachos genéricos por verbo. As sub-rotinas e sua ordem "
        "são parte da regra; não inferir que uma combinação sempre terá sucesso.",
        "",
        "## Condições e mecanismos",
        "",
        "- `X(0)`: corda colocada no poço; `X(1)`: corda alcançou a chave; "
        "`X(2)`: pilha na lanterna; `X(3)`: pilha no gravador; "
        "`X(4)`: jornal sob a porta; `X(5)`: fita no gravador; "
        "`X(6)`: serra conectada; `X(7)`: lanterna acesa; "
        "`X(8)`: serra ligada; `X(9)`: gravador tocando. "
        "Ao mudar de posição, o BASIC zera X(6) e X(8) (linha 790).",
        "- Na cela, pedir comida ao guarda desloca-o e deixa a chave fora; "
        "jogar e puxar a corda obtém a chave, que abre as grades. O "
        "presidiário troca comida por dinheiro; o guarda aceita dinheiro.",
        "- A serra ligada à tomada corta a porta em R035. A outra porta, "
        "em R116, abre com jornal colocado sob ela e queimado com fósforos. "
        "O cobertor apaga o fogo; água no alarme e espelho na luz desativam "
        "as respectivas armadilhas.",
        "- A lanterna precisa de pilha e deve estar acesa e carregada para "
        "atravessar a escuridão. O gravador precisa de pilha e fita e deve "
        "estar tocando e carregado ao atravessar os cães. O carcereiro aceita "
        "baralho; a corda no poço permite a fuga. A pá abre a passagem "
        "bloqueada por terra, sem ser necessária ao percurso validado.",
        "- Casos a decidir na adaptação: `PECA` e `DE` têm exceção de "
        "visibilidade (linha 1520), e alguns casos especiais não verificam "
        "explicitamente se o objeto está carregado antes de transferi-lo "
        "(linhas 2330–2360 e 2650–2700). Na linha 2590, `IF J+10` "
        "zera X(4) ao pegar um objeto; preservar ou corrigir esse comportamento "
        "exige decisão e testes, não uma suposição silenciosa.",
        "",
        "## Vocabulário: verbos",
        "",
        ", ".join(f"`{word}`" for word in verbs),
        "",
        "## Substantivos e posição inicial",
        "",
        "| Índice | Nome | Código BASIC | Posição inicial |",
        "|---:|---|---|---|",
    ]
    for index, obj in enumerate(objects):
        out.append(f"| {index} | {obj[2:]} | `{obj[:2]}` | {object_location(obj)} |")
    out.extend([
        "",
        "## Despacho das ações",
        "",
        "A tabela traduz os códigos de `AC$` e aponta para a sub-rotina "
        "BASIC. Entradas genéricas estão marcadas como tal; devem ser "
        "verificadas depois das combinações exatas.",
        "",
        "| Ordem | Combinação | Tipo | Linha BASIC |",
        "|---:|---|---|---:|",
    ])
    for index, (action, target) in enumerate(zip(actions, targets)):
        kind = "exata" if index <= 44 else "genérica por verbo"
        out.append(f"| {index} | {decode_action(action, verbs, objects)} | {kind} | {target} |")
    out.extend([
        "",
        "## Fragmentos literais de mensagens",
        "",
        "Trechos entre aspas extraídos de instruções `PRINT` das linhas "
        "1–3095, inclusive abertura e final. Uma linha pode combinar vários trechos com nomes de "
        "objetos ou outras expressões; a listagem BASIC é a fonte para "
        "montagem, condições e fluxo. Quebras de linha da listagem são "
        "convertidas em espaços.",
        "",
        "| Linha BASIC | Fragmentos literais, na ordem |",
        "|---:|---|",
    ])
    for number, fragments in print_fragments(lines):
        encoded = " + ".join("`" + json.dumps(s, ensure_ascii=False).replace("|", "\\|") + "`" for s in fragments)
        out.append(f"| {number} | {encoded} |")
    out.append("")
    return "\n".join(out)


def render_route(rooms, objects):
    passages = [row[:] for row in rooms]
    locations = {obj[2:]: object_location(obj) for obj in objects}
    room = "R079"
    light = False
    recorder = False
    out = [
        "# Alcatraz — percurso de referência",
        "",
        "Percurso proposto a partir do roteiro fornecido (Luiz Fernando "
        "Fiacadori), do roteiro de Alex Dijkstra e da listagem portuguesa. "
        "As direções foram calculadas sobre os códigos de [MAPA.md](MAPA.md), "
        "com as alterações e condições relevantes indicadas abaixo. "
        "Uma reprodução automatizada no openMSX alcançou a vitória no BASIC "
        "original em inglês (`docs/alcatraz.bas`), usando a tradução dos "
        "comandos; isso não testa todas as ações nem o remake.",
        "",
        "Cada direção leva a uma sala; não pule as posições intermediárias. "
        "Para repetir o teste: `python tests/run_original_route.py "
        "--openmsx caminho\\openmsx.exe`.",
        "",
    ]

    def coords(rid):
        index = int(rid[1:]) - 1
        return index % 14, index // 14 + 2

    def held(name):
        return locations[name] == "inventário"

    def travel(target):
        nonlocal room
        queue = deque([(room, [])])
        seen = {room}
        while queue:
            current, route = queue.popleft()
            if current == target:
                room = current
                return route
            x, y = coords(current)
            for index, (direction, dx, dy) in enumerate(DIRECTIONS):
                code = passages[y - 2][x][index]
                allowed = code in ("0", "D")
                if code == "6":
                    allowed = light and held("LANTERNA")
                if code == "A":
                    allowed = recorder and held("GRAVADOR")
                if not allowed:
                    continue
                next_room = room_id(x + dx, y + dy)
                if next_room not in seen:
                    seen.add(next_room)
                    queue.append((next_room, route + [direction]))
        raise ValueError(f"Percurso não encontrado: {room} -> {target}")

    def step(target, commands):
        route = travel(target)
        out.append(f"## {len([s for s in out if s.startswith('## ')]) + 1}. {target}")
        out.append("")
        out.append("Deslocamento: " + (" → ".join(route) if route else "já está na sala") + ".")
        out.append("")
        out.extend(f"- `{command}`" for command in commands)
        out.append("")

    def move_object(name, source, target):
        if locations[name] != source:
            raise ValueError(f"{name}: esperado {source}, encontrado {locations[name]}")
        if target == "inventário":
            if sum(value == "inventário" for value in locations.values()) >= 6:
                raise ValueError(f"Inventário cheio ao pegar {name}")
        locations[name] = target

    def take(name):
        move_object(name, room, "inventário")

    def drop(name):
        move_object(name, "inventário", room)

    def use(name, target="removido"):
        move_object(name, "inventário", target)

    def change(value):
        x, y = coords(room)
        passages[y - 2][x] = value

    step("R079", ["PEGUE LENCOL", "RASGUE LENCOL", "AMARRE TIRAS"])
    take("LENCOL"); use("LENCOL"); locations["TIRAS"] = "inventário"
    use("TIRAS"); locations["CORDA"] = "inventário"

    step("R078", ["PECA COMIDA GUARDA", "PEGUE COMIDA", "AMARRE CORDA GRADES",
                  "JOGUE CORDA", "PUXE CORDA", "ABRA GRADES CHAVE"])
    locations["COMIDA"] = room; locations["GUARDA"] = "R092"
    take("COMIDA"); locations["CHAVE"] = "inventário"; change("1100")

    step("R066", ["ABRA GRADES CHAVE", "SOLTE CHAVE"])
    change("0610"); drop("CHAVE")

    step("R064", ["PEGUE COBERTOR", "DE COMIDA PRESIDIARIO"])
    take("COBERTOR"); use("COMIDA"); locations["DINHEIRO"] = "inventário"

    step("R024", ["PEGUE SERRA"])
    take("SERRA")

    step("R092", ["DE DINHEIRO GUARDA"])
    use("DINHEIRO", "R140"); locations["GUARDA"] = "fora"; change("1040")

    step("R104", ["PEGUE LANTERNA"])
    take("LANTERNA")

    step("R035", ["COLOQUE SERRA TOMADA", "LIGUE SERRA",
                  "CORTE PORTA SERRA", "SOLTE SERRA"])
    change("0011"); drop("SERRA")

    step("R023", ["PEGUE PILHA", "PEGUE BARALHO",
                  "COLOQUE PILHA LANTERNA", "LIGUE LANTERNA"])
    take("PILHA"); take("BARALHO"); use("PILHA"); light = True

    step("R108", ["PEGUE FOSFOROS"])
    take("FOSFOROS")

    step("R132", ["SOLTE BARALHO", "PEGUE JORNAL"])
    drop("BARALHO"); take("JORNAL")

    step("R116", ["COLOQUE JORNAL PORTA", "QUEIME JORNAL FOSFOROS",
                  "SOLTE FOSFOROS", "TIRE PILHA LANTERNA", "SOLTE LANTERNA"])
    use("JORNAL", room); change("0011"); drop("FOSFOROS")
    locations["PILHA"] = "inventário"; light = False; drop("LANTERNA")

    step("R132", ["PEGUE BARALHO"])
    take("BARALHO")

    step("R102", ["COLOQUE COBERTOR FOGO"])
    use("COBERTOR", room); locations["FOGO"] = "fora"; change("0000")

    step("R127", ["PEGUE AGUA"])
    take("AGUA")

    step("R120", ["PEGUE ESPELHO"])
    take("ESPELHO")

    step("R048", ["COLOQUE AGUA ALARME"])
    use("AGUA"); locations["ALARME"] = "fora"; change("0011")

    step("R057", ["PEGUE FITA"])
    take("FITA")

    step("R006", ["COLOQUE ESPELHO LUZ"])
    use("ESPELHO"); locations["LUZ"] = "fora"; change("1000")

    step("R031", ["PEGUE GRAVADOR"])
    take("GRAVADOR")

    step("R136", ["COLOQUE PILHA GRAVADOR", "COLOQUE FITA GRAVADOR",
                  "LIGUE GRAVADOR"])
    use("PILHA"); use("FITA"); recorder = True

    step("R137", [])
    out.append("Os cães deixam passar com o gravador ligado e carregado.")
    out.append("")

    step("R012", ["DE BARALHO CARCEREIRO"])
    use("BARALHO"); locations["CARCEREIRO"] = "fora"; change("1001")

    step("R084", ["COLOQUE CORDA POCO", "ENTRE POCO"])
    use("CORDA", room)
    out.extend([
        "A entrada no poço com a corda colocada leva à vitória. "
        "A pá, a terra, o dinheiro atrás dela e a roupa não são necessários "
        "neste percurso; podem ser explorados separadamente.",
        "",
    ])
    return "\n".join(out)


def main():
    lines = basic_lines(SOURCE.read_text(encoding="utf-8"))
    rooms, objects, verbs, actions = initial_data(lines)
    outputs = {
        ROOT / "MAPA.md": render_map(rooms, objects),
        ROOT / "REFERENCIA_ORIGINAL.md": render_reference(lines, objects, verbs, actions),
        ROOT / "PERCURSO.md": render_route(rooms, objects),
    }
    for path, content in outputs.items():
        path.write_text(content, encoding="utf-8", newline="\n")
        print(f"Gerado: {path}")


if __name__ == "__main__":
    main()
