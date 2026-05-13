#!/bin/bash

set -e

make emcc

# Build du projet
npm run build

# Création du dossier cible
mkdir -p draft/tmp

# Copies des fichiers
cp client/index.html draft/tmp/index.html
cp client/index.css draft/tmp/index.css
cp -r client/public/assets draft/tmp
cp client/wasm/api.js draft/tmp/api.js
cp client/wasm/api.wasm draft/tmp/api.wasm
cp client/dist/bundle.js draft/tmp/bundle.js
cp client/dist/worker.js draft/tmp/worker.js

# Changement de branche git
git switch gh-pages

# Copies

cp draft/tmp/index.css index.css
cp -r draft/tmp/assets public
cp -r draft/tmp/api.js public/wasm/api.js
cp -r draft/tmp/api.wasm public/wasm/api.wasm
cp -r draft/tmp/bundle.js public/bundle.js
cp -r draft/tmp/worker.js public/worker.js

