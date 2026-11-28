import importlib.util
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parent.parent
SPEC = importlib.util.spec_from_file_location("mapa_alcatraz", ROOT / "tools" / "mapa_alcatraz.py")
MAP = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MAP)
ROUTE_SPEC = importlib.util.spec_from_file_location("original_route", ROOT / "tests" / "run_original_route.py")
ROUTE = importlib.util.module_from_spec(ROUTE_SPEC)
ROUTE_SPEC.loader.exec_module(ROUTE)


class MapaAlcatrazTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.lines = MAP.basic_lines(MAP.SOURCE.read_text(encoding="utf-8"))
        cls.rooms, cls.objects, cls.verbs, cls.actions = MAP.initial_data(cls.lines)

    def test_connections_stay_in_grid_and_preserve_fatal_one_way_edge(self):
        asymmetric = []
        reverse = (1, 0, 3, 2)
        for row_index, row in enumerate(self.rooms):
            y = row_index + 2
            for x, cell in enumerate(row):
                for direction, (_, dx, dy) in enumerate(MAP.DIRECTIONS):
                    if cell[direction] == "1":
                        continue
                    MAP.room_id(x + dx, y + dy)
                    other = self.rooms[y + dy - 2][x + dx]
                    if other[reverse[direction]] == "1":
                        asymmetric.append((MAP.room_id(x, y), cell[direction], MAP.room_id(x + dx, y + dy)))
        self.assertEqual(asymmetric, [("R092", "4", "R093")])

    def test_objects_and_actions_come_from_original_tables(self):
        objects = {obj[2:]: MAP.object_location(obj) for obj in self.objects}
        self.assertEqual(objects["LENCOL"], "R079")
        self.assertEqual(objects["CARCEREIRO"], "R012")
        self.assertEqual(objects["GRADES"], "objeto contextual (..)")
        self.assertEqual(objects["CORDA"], "fora do jogo inicialmente (JJ)")
        targets = MAP.action_subroutines(self.lines)
        decoded = [MAP.decode_action(action, self.verbs, self.objects) for action in self.actions]
        self.assertEqual(targets[decoded.index("ABRA + GRADES + CHAVE")], 2280)
        self.assertEqual(targets[decoded.index("SOLTE")], 2610)

    def test_print_extraction_includes_thenprint_and_dynamic_fragments(self):
        fragments = dict(MAP.print_fragments(self.lines))
        self.assertIn("grades", fragments[520][0])
        self.assertIn("pegar mais nada", fragments[2550][0])
        self.assertEqual(fragments[1680], ["As ", " transformaram-se numa ", "!"])

    def test_route_checks_reachability_and_item_locations(self):
        self.assertIn("`ENTRE POCO`", MAP.render_route(self.rooms, self.objects))
        blocked = [row[:] for row in self.rooms]
        blocked[5][8] = "1111"
        with self.assertRaisesRegex(ValueError, "Percurso não encontrado"):
            MAP.render_route(blocked, self.objects)
        missing = [obj.replace("DCLENCOL", "JJLENCOL") for obj in self.objects]
        with self.assertRaisesRegex(ValueError, "LENCOL"):
            MAP.render_route(self.rooms, missing)

    def test_checked_in_references_match_generator(self):
        expected = {
            "src/game_map_data.h": MAP.render_game_map(self.rooms),
            "MAPA.md": MAP.render_map(self.rooms, self.objects),
            "REFERENCIA_ORIGINAL.md": MAP.render_reference(self.lines, self.objects, self.verbs, self.actions),
            "RESOLVE.md": MAP.render_route(self.rooms, self.objects),
        }
        for name, content in expected.items():
            with self.subTest(name=name):
                self.assertEqual((ROOT / name).read_text(encoding="utf-8"), content)

    @unittest.skipUnless(shutil.which("gcc"), "gcc necessário para testar o mapa em C")
    def test_game_map_matches_all_original_exits(self):
        with tempfile.TemporaryDirectory(prefix="alcatraz-map-") as directory:
            work = Path(directory)
            (work / "msxgl.h").write_text(
                "typedef unsigned char u8;\ntypedef char c8;\ntypedef unsigned char bool;\n",
                encoding="ascii",
            )
            (work / "test.c").write_text(
                '#include <stdio.h>\n#include "game_map.h"\n'
                'int main(void) {\n'
                '  int room, direction;\n'
                '  for (room = 1; room <= GAME_ROOM_COUNT; ++room)\n'
                '    for (direction = GAME_NORTH; direction <= GAME_WEST; ++direction)\n'
                '      printf("%c %u %u\\n", GameMap_Passage(room, direction), '
                'GameMap_Destination(room, direction), GameMap_RoomReady(room));\n'
                '  return 0;\n}\n',
                encoding="ascii",
            )
            binary = work / "test.exe"
            subprocess.run(
                ["gcc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                 "-I", str(work), "-I", str(ROOT / "src"),
                 str(ROOT / "src" / "game_map.c"), str(work / "test.c"),
                 "-o", str(binary)], check=True,
            )
            result = subprocess.run([str(binary)], check=True, capture_output=True, text=True)
        actual = result.stdout.splitlines()
        expected = []
        for row_index, row in enumerate(self.rooms):
            for x, cell in enumerate(row):
                room = row_index * 14 + x + 1
                for direction, (_, dx, dy) in enumerate(MAP.DIRECTIONS):
                    dest = 0 if cell[direction] == "1" else room + dy * 14 + dx
                    expected.append(f"{cell[direction]} {dest} {int(room in (77, 78, 79))}")
        self.assertEqual(actual, expected)

    def test_emulator_events_check_destinations_and_translate_commands(self):
        route = (ROOT / "RESOLVE.md").read_text(encoding="utf-8")
        events = ROUTE.events_from_route(route)
        self.assertIn("{cmd {OPEN BARS KEYS}}", events)
        self.assertEqual(events[-1], "{cmd {GO INTO WELL}}")
        with self.assertRaisesRegex(ValueError, "não chega à sala"):
            ROUTE.events_from_route(route.replace("## 2. R078", "## 2. R077"))


if __name__ == "__main__":
    unittest.main()
