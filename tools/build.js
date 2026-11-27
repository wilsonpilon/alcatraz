"use strict";

const fs = require("fs");
const path = require("path");
const { spawnSync } = require("child_process");

const root = path.resolve(__dirname, "..");
const versionPath = path.join(root, "version.json");
const out = path.join(root, "out");
const builder = path.join(root, "MSXgl", "engine", "script", "js", "build.js");
const converter = path.join(root, "MSXgl", "tools", "MSXtk", "bin", "MSXimg.exe");
const sourceImage = path.join(root, "images", "openscreen.jpg");
const args = process.argv.slice(2);
const current = JSON.parse(fs.readFileSync(versionPath, "utf8"));
let next = { ...current, build: current.build + 1 };

if (args.length) {
    if (args.length !== 3 || args[0] !== "--milestone" || !args[1].trim() || !args[2].trim()) {
        console.error('Uso: build.bat [--milestone "Nome" "Subtitulo"]');
        process.exit(1);
    }
    next = { major: current.major + 1, build: 1, codename: args[1], subtitle: args[2] };
}
if (!fs.existsSync(builder)) {
    console.error("MSXgl nao encontrado. Consulte o README.");
    process.exit(1);
}
fs.mkdirSync(out, { recursive: true });
const binary = path.join(out, "alcatraz.com");
const screen = path.join(out, "ALCATRAZ.SC5");
const converted = path.join(out, "opening.tmp");
for (const file of [binary, screen, converted]) {
    if (fs.existsSync(file)) fs.unlinkSync(file);
}
const image = spawnSync(converter, [sourceImage, "-mode", "sc5", "-resize", "256", "212",
    "-size", "256", "212", "-num", "1", "1", "-pal", "custom",
    "--palcount", "14",
    "-out", converted, "-format", "bin", "-bload", "-ret0"], { cwd: root, stdio: "inherit" });
if (image.error || image.status !== 0 || !fs.existsSync(converted)) {
    console.error(`Falha na conversao da imagem: ${image.error || image.status}`);
    process.exit(1);
}
const data = fs.readFileSync(converted);
fs.unlinkSync(converted);
const pixelsEnd = 7 + 256 * 212 / 2;
if (data.length !== pixelsEnd + 28 || data[0] !== 0xFE ||
    data[1] !== 0 || data[2] !== 0 || data[3] !== 0xFF || data[4] !== 0x69) {
    console.error("Formato inesperado na saida do MSXimg.");
    process.exit(1);
}
const pixels = data.subarray(7, pixelsEnd);
if (pixels.some(byte => (byte >> 4) === 15 || (byte & 15) === 15)) {
    console.error("A imagem usa a cor reservada para o logotipo.");
    process.exit(1);
}
const colors = Buffer.concat([Buffer.from([0, 0]), data.subarray(pixelsEnd), Buffer.from([0x70, 0])]);
function nearestColor(target) {
    let best = 1, distance = Infinity;
    for (let i = 1; i <= 14; ++i) {
        const red = (colors[i * 2] >> 4) & 7;
        const blue = colors[i * 2] & 7;
        const green = colors[i * 2 + 1] & 7;
        const d = (red - target) ** 2 + (green - target) ** 2 + (blue - target) ** 2;
        if (d < distance) { best = i; distance = d; }
    }
    return best;
}
fs.writeFileSync(path.join(out, "opening_palette.h"),
    `static const u8 g_opening_palette[32] = { ${[...colors].map(c => `0x${c.toString(16).padStart(2, "0")}`).join(", ")} };\n` +
    `#define OPENING_BLACK ${nearestColor(0)}\n#define OPENING_WHITE ${nearestColor(7)}\n#define OPENING_RED 15\n`);
fs.writeFileSync(screen, data.subarray(0, pixelsEnd));
const number = `${next.major}.${next.build}`;
fs.writeFileSync(path.join(out, "build_version.h"), `#define ALCATRAZ_VERSION "${number}"\n`);
const result = spawnSync(process.execPath, [builder], { cwd: root, stdio: "inherit" });
if (result.error || result.status !== 0 || !fs.existsSync(binary)) {
    console.error(`Falha na compilacao: ${result.error || result.status}`);
    process.exit(1);
}
fs.writeFileSync(versionPath, JSON.stringify(next, null, 2) + "\n");
console.log(`Compilacao ${number}: ${next.codename} - ${next.subtitle}`);
