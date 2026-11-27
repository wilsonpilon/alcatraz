"use strict";

const fs = require("fs");
const path = require("path");
const { spawnSync } = require("child_process");

const root = path.resolve(__dirname, "..");
const versionPath = path.join(root, "version.json");
const out = path.join(root, "out");
const builder = path.join(root, "MSXgl", "engine", "script", "js", "build.js");
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
const number = `${next.major}.${next.build}`;
fs.writeFileSync(path.join(out, "build_version.h"), `#define ALCATRAZ_VERSION "${number}"\n`);
const binary = path.join(out, "alcatraz.com");
if (fs.existsSync(binary)) fs.unlinkSync(binary);
const result = spawnSync(process.execPath, [builder], { cwd: root, stdio: "inherit" });
if (result.error || result.status !== 0 || !fs.existsSync(binary)) {
    console.error(`Falha na compilacao: ${result.error || result.status}`);
    process.exit(1);
}
fs.writeFileSync(versionPath, JSON.stringify(next, null, 2) + "\n");
console.log(`Compilacao ${number}: ${next.codename} - ${next.subtitle}`);
