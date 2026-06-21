#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEPLOY=false

for arg in "$@"; do
  case "${arg}" in
    --deploy) DEPLOY=true ;;
    --help|-h)
      echo "Usage: $0 [--deploy]"
      echo "  --deploy  Deploy docs to GitHub Pages (push to gh-pages branch)"
      exit 0
      ;;
  esac
done

cd "${ROOT_DIR}"

if ! command -v doxygen &>/dev/null; then
  echo "Error: doxygen not found. Install it with:"
  echo "  sudo apt-get install doxygen graphviz"
  exit 1
fi

echo "==> Generating documentation"
doxygen Doxyfile

echo "==> Documentation generated at docs/html/"

if [ "${DEPLOY}" = true ]; then
  echo "==> Deploying to GitHub Pages"
  if ! command -v gh &>/dev/null; then
    echo "Error: gh CLI not found. Install it first."
    exit 1
  fi

  TEMP_DIR=$(mktemp -d)
  cp -r docs/html/* "${TEMP_DIR}/"

  cd "${TEMP_DIR}"
  git init
  git checkout -b gh-pages
  git add .
  git commit -m "Deploy documentation"
  git remote add origin "$(cd "${ROOT_DIR}" && git remote get-url origin)"
  git push -f origin gh-pages

  rm -rf "${TEMP_DIR}"
  echo "==> Documentation deployed to GitHub Pages"
fi
