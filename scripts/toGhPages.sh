#!/bin/bash

set -e

# Build du projet
npm run build

# Création du dossier cible
mkdir -p draft/tmp

# Copies des fichiers
cp client/index.html draft/tmp/index.html
cp client/index.css draft/tmp/index.css
cp -r client/public/assets draft/tmp/assets
cp client/wasm/api.js draft/tmp/api.js
cp client/wasm/api.wasm draft/tmp/api.wasm

# Changement de branche git
git switch gh-pages

# Copies

