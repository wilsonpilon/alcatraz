"""Render the initial BASIC room grid as a PNG reference map."""

import textwrap
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

import mapa_alcatraz as mapa


ROOT = Path(__file__).resolve().parent.parent
OUTPUT = ROOT / "MAPA.png"
WIDTH, HEIGHT = 3480, 2040
LEFT, TOP = 76, 294
DX, DY = 188, 145
ROOM_W, ROOM_H = 150, 94
PANEL_X = 2765


def font(size, bold=False):
    names = ("arialbd.ttf", "DejaVuSans-Bold.ttf") if bold else ("arial.ttf", "DejaVuSans.ttf")
    for name in names:
        try:
            return ImageFont.truetype(name, size)
        except OSError:
            continue
    raise RuntimeError("Fonte Arial ou DejaVu Sans não encontrada.")


def centered(draw, xy, text, face, fill):
    draw.text(xy, text, font=face, fill=fill, anchor="mm")


def connections(rooms):
    for row, cells in enumerate(rooms):
        for x, cell in enumerate(cells):
            if x < 13:
                other = cells[x + 1][3]
                if cell[2] != "1" or other != "1":
                    yield x, row, "horizontal", cell[2], other
            if row < 9:
                other = rooms[row + 1][x][0]
                if cell[1] != "1" or other != "1":
                    yield x, row, "vertical", cell[1], other


def render(rooms, objects):
    image = Image.new("RGB", (WIDTH, HEIGHT), "#f2f1ed")
    draw = ImageDraw.Draw(image)
    title = font(47, True)
    subtitle = font(23)
    header = font(26, True)
    normal = font(20)
    small = font(16)
    room_face = font(21, True)
    object_face = font(16)
    link_face = font(16, True)

    draw.rectangle((0, 0, WIDTH, 211), fill="#102840")
    draw.rectangle((0, 204, WIDTH, 211), fill="#a84039")
    draw.text((76, 37), "ALCATRAZ", font=title, fill="#ffffff")
    draw.text((77, 105), "MAPA DE SALAS  |  ESTADO INICIAL DO BASIC", font=header, fill="#ebc4b0")
    draw.text((77, 155), "Referência de desenvolvimento · Norte para cima · Contém spoilers", font=subtitle, fill="#d3dfe9")
    draw.text((LEFT, 242), "Cada bloco representa uma posição, inclusive corredores vazios.", font=normal, fill="#34495b")

    by_room = {}
    for obj in objects:
        location = mapa.object_location(obj)
        if location.startswith("R"):
            by_room.setdefault(location, []).append(obj[2:])

    def room_xy(x, row):
        return LEFT + x * DX, TOP + row * DY

    def connection(x1, y1, x2, y2, first, second):
        if first == second == "1":
            return
        is_free = first == second == "0"
        color = "#54778c" if is_free else "#a84d35"
        draw.line((x1, y1, x2, y2), fill=color, width=5)
        if not is_free:
            label = f"{first}/{second}"
            middle = ((x1 + x2) // 2, (y1 + y2) // 2)
            bbox = draw.textbbox(middle, label, font=link_face, anchor="mm")
            draw.rounded_rectangle(
                (bbox[0] - 3, bbox[1] - 2, bbox[2] + 3, bbox[3] + 2),
                radius=3, fill="#f2f1ed",
            )
            centered(draw, middle, label, link_face, "#8c3629")

    for x, row, direction, first, second in connections(rooms):
        px, py = room_xy(x, row)
        if direction == "horizontal":
            connection(px + ROOM_W, py + ROOM_H // 2,
                       px + DX, py + ROOM_H // 2, first, second)
        else:
            connection(px + ROOM_W // 2, py + ROOM_H,
                       px + ROOM_W // 2, py + DY, first, second)

    for row, cells in enumerate(rooms):
        for x, _ in enumerate(cells):
            px, py = room_xy(x, row)
            rid = mapa.room_id(x, row + 2)
            items = by_room.get(rid, [])
            is_start = rid == "R079"
            is_fatal = rid == "R093"
            fill = "#fff3d4" if is_start else "#f8e6e2" if is_fatal else "#e4edf1" if items else "#ffffff"
            outline = "#c1862b" if is_start else "#a84d35" if is_fatal else "#627d8e"
            draw.rounded_rectangle((px, py, px + ROOM_W, py + ROOM_H), radius=8,
                                   fill=fill, outline=outline, width=3)
            centered(draw, (px + ROOM_W // 2, py + 18), rid, room_face, "#18364b")
            if is_start:
                centered(draw, (px + ROOM_W // 2, py + 41), "INÍCIO", small, "#8d5d16")
            elif is_fatal:
                centered(draw, (px + ROOM_W // 2, py + 40), "CADEIRA", small, "#8c3629")
                centered(draw, (px + ROOM_W // 2, py + 59), "ELÉTRICA", small, "#8c3629")
            lines = textwrap.wrap(", ".join(items), width=15)
            for index, line in enumerate(lines):
                item_y = 63 if is_start else 55
                centered(draw, (px + ROOM_W // 2, py + item_y + index * 17), line, object_face, "#334b5b")

    draw.rounded_rectangle((PANEL_X, 240, WIDTH - 65, 1710), radius=12, fill="#ffffff", outline="#cbd6db", width=2)
    x = PANEL_X + 34
    draw.text((x, 273), "COMO LER", font=header, fill="#18364b")
    lines = [
        "Cada R é uma sala ou trecho de corredor.",
        "Objetos indicados são posições iniciais.",
        "O caminho muda conforme suas ações.",
        "",
        "Linha azul: livre nos dois sentidos.",
        "Linha vermelha: condicional, fatal",
        "ou assimétrica no estado inicial.",
        "",
        "Nos códigos X/Y de uma linha horizontal,",
        "X vale da esquerda para a direita;",
        "Y, da direita para a esquerda.",
        "Na vertical, X vale de cima para baixo;",
        "Y, de baixo para cima.",
    ]
    for i, line in enumerate(lines):
        draw.text((x, 326 + 29 * i), line, font=normal, fill="#34495b")

    draw.text((x, 758), "CÓDIGOS DAS PASSAGENS", font=header, fill="#18364b")
    codes = [
        "0  Livre", "1  Parede", "2  Grades", "3  Guarda",
        "4  Cadeira elétrica (fatal)", "5  Porta fechada",
        "6  Escuridão", "7  Fogo (fatal)", "8  Alarme (fatal)",
        "9  Feixe de luz (fatal)", "A  Cães", "B  Terra",
        "C  Carcereiro (fatal)", "D  Saída para o pátio",
    ]
    for i, line in enumerate(codes):
        draw.text((x, 810 + 31 * i), line, font=normal, fill="#34495b")
    draw.text((x, 1293), "R079  Cela inicial", font=normal, fill="#8d5d16")
    draw.text((x, 1326), "R093  Cadeira elétrica", font=normal, fill="#8c3629")
    draw.text((x, 1358), "O destino fatal aparece para localizar", font=normal, fill="#34495b")
    draw.text((x, 1387), "a tentativa; a morte ocorre antes da entrada.", font=normal, fill="#34495b")
    draw.text((x, 1456), "Para detalhes, use MAPA.md.", font=normal, fill="#18364b")
    draw.text((x, 1488), "Não é um mapa jogável do remake.", font=normal, fill="#18364b")

    draw.line((76, 1802, WIDTH - 76, 1802), fill="#c6d0d5", width=2)
    draw.text((76, 1830), "BASE: LISTAGEM ORIGINAL EM PORTUGUÊS · 140 POSIÇÕES · 14 × 10", font=header, fill="#18364b")
    draw.text((76, 1880), "As passagens são direcionais. Consulte MAPA.md para condições completas, nomes e correspondência com o roteiro.", font=subtitle, fill="#526573")
    draw.text((76, 1920), "Wilson Fazzio Martins · Pesquisa e adaptação em desenvolvimento", font=normal, fill="#526573")
    return image


def main():
    lines = mapa.basic_lines(mapa.SOURCE.read_text(encoding="utf-8"))
    rooms, objects, _, _ = mapa.initial_data(lines)
    render(rooms, objects).save(OUTPUT)
    print(f"Mapa salvo em {OUTPUT}")


if __name__ == "__main__":
    main()
