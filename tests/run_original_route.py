"""Replay the documented route against the English BASIC in openMSX."""

import argparse
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parent.parent
VERBS = {
    "PEGUE": "TAKE", "RASGUE": "TEAR", "AMARRE": "TIE", "PECA": "ASK",
    "JOGUE": "THROW", "PUXE": "PULL", "ABRA": "OPEN", "SOLTE": "LEAVE",
    "DE": "GIVE", "COLOQUE": "PUT", "LIGUE": "TURN ON", "CORTE": "CUT",
    "TIRE": "REMOVE", "QUEIME": "BURN", "ENTRE": "GO INTO",
}
NOUNS = {
    "LENCOL": "SHEET", "TIRAS": "STRIPES", "CORDA": "ROPE", "COMIDA": "FOOD",
    "GUARDA": "OFFICER", "GRADES": "BARS", "CHAVE": "KEYS",
    "COBERTOR": "BLANKET", "PRESIDIARIO": "PRISIONER", "SERRA": "SAW",
    "DINHEIRO": "MONEY", "LANTERNA": "FLASHLIGHT", "TOMADA": "PLUG",
    "PORTA": "DOOR", "PILHA": "BATTERY", "BARALHO": "CARDS",
    "FOSFOROS": "MATCHES", "JORNAL": "NEWSPAPER", "FOGO": "FIRE",
    "AGUA": "WATER", "ESPELHO": "MIRROR", "ALARME": "ALARM",
    "FITA": "TAPE", "LUZ": "LIGHT", "GRAVADOR": "RECORDER",
    "CARCEREIRO": "JAILOR", "POCO": "WELL",
}
DIRECTIONS = {"N": (0, -1), "S": (0, 1), "L": (1, 0), "O": (-1, 0)}


def events_from_route(text):
    x, y = 8, 7
    expected_room = None
    events = []
    for line in text.splitlines():
        if line.startswith("## "):
            match = re.fullmatch(r"## \d+\. R(\d{3})", line)
            if not match:
                raise ValueError(f"Título de sala inválido: {line}")
            expected_room = int(match.group(1))
        elif line.startswith("Deslocamento: "):
            directions = line[len("Deslocamento: "):-1]
            if directions != "já está na sala":
                for direction in directions.split(" → "):
                    dx, dy = DIRECTIONS[direction]
                    x += dx
                    y += dy
                    if not (0 <= x < 14 and 2 <= y <= 11):
                        raise ValueError(f"Deslocamento fora do mapa: {x},{y}")
                    events.append(f"{{move {direction} {x} {y}}}")
            if expected_room != (y - 2) * 14 + x + 1:
                raise ValueError(f"Deslocamento não chega à sala R{expected_room:03}")
        elif line.startswith("- `"):
            match = re.fullmatch(r"- `([A-Z ]+)`", line)
            if not match:
                raise ValueError(f"Comando inválido: {line}")
            words = match.group(1).split()
            translated = " ".join([VERBS[words[0]]] + [NOUNS[word] for word in words[1:]])
            events.append(f"{{cmd {{{translated}}}}}")
    if not events or events[-1] != "{cmd {GO INTO WELL}}":
        raise ValueError("Percurso incompleto")
    return events


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--openmsx", default="openmsx.exe", help="Caminho do openMSX")
    args = parser.parse_args()
    events = events_from_route((ROOT / "PERCURSO.md").read_text(encoding="utf-8"))
    with tempfile.TemporaryDirectory(prefix="alcatraz-route-") as directory:
        work = Path(directory)
        shutil.copyfile(ROOT / "docs" / "alcatraz.bas", work / "ALCATRAZ.BAS")
        (work / "events.tcl").write_text("set events {\n" + "\n".join(events) + "\n}\n", encoding="ascii")
        env = os.environ.copy()
        env["ALCATRAZ_ROUTE_WORK"] = str(work)
        result = subprocess.run(
            [args.openmsx, "-machine", "Panasonic_FS-A1WX", "-diska", str(work),
             "-script", str(ROOT / "tests" / "original_route.tcl")],
            env=env, capture_output=True, text=True, timeout=120,
        )
        log_path = work / "route.log"
        if not log_path.exists():
            raise RuntimeError(f"openMSX não produziu log (código {result.returncode}):\n{result.stderr}")
        log = log_path.read_text(encoding="utf-8")
        if result.returncode or not log.rstrip().endswith("PASS vitória BASIC em 3020"):
            raise RuntimeError(f"Falha no percurso original (código {result.returncode}):\n{log}\n{result.stderr}")
        print(f"PASS: {len(events)} eventos; vitória no BASIC original em inglês.")


if __name__ == "__main__":
    main()
