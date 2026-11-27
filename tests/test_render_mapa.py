import sys
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import mapa_alcatraz as mapa
import render_mapa


class RenderMapaTest(unittest.TestCase):
    def test_connections_preserve_direction_and_omit_walls(self):
        lines = mapa.basic_lines(mapa.SOURCE.read_text(encoding="utf-8"))
        rooms, _, _, _ = mapa.initial_data(lines)
        edges = set(render_mapa.connections(rooms))
        self.assertIn((0, 0, "horizontal", "0", "0"), edges)
        self.assertIn((6, 5, "horizontal", "0", "2"), edges)
        self.assertIn((7, 6, "horizontal", "4", "1"), edges)
        self.assertIn((6, 1, "vertical", "0", "5"), edges)
        self.assertNotIn((1, 0, "horizontal", "1", "1"), edges)


if __name__ == "__main__":
    unittest.main()
