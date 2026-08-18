#!/bin/bash
# Converts every font in resources/fonts into a C/C++ header in src/fonts.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$SCRIPT_DIR"
INPUT_DIR="$REPO_ROOT/resources/fonts"
OUTPUT_DIR="$REPO_ROOT/src/fonts"

if [[ ! -d "$INPUT_DIR" ]]; then
    echo "Missing input directory: $INPUT_DIR";
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

shopt -s nullglob
font_files=("$INPUT_DIR"/*.{ttf,otf})
shopt -u nullglob

if [[ ${#font_files[@]} -eq 0 ]]; then
    echo "No font files found in $INPUT_DIR";
    exit 1
fi

for font_path in "${font_files[@]}"; do
    font_name=$(basename "$font_path")
    stem="${font_name%.*}"

    # Generate a generic name from the font file
    header_name=$(printf '%s' "$stem" | sed -E 's/([a-z0-9])([A-Z])/\1_\2/g; s/[^a-zA-Z0-9]+/_/g; s/_+/_/g; s/^_//; s/_$//; y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/')
    array_name=$(printf '%s' "$stem" | sed -E 's/([a-z0-9])([A-Z])/\1_\2/g; s/[^a-zA-Z0-9]+/_/g; s/_+/_/g; s/^_//; s/_$//; y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/')
    array_name="${array_name}_DATA"

    byte_count=$(wc -c < "$font_path" | tr -d '[:space:]')
    output_path="$OUTPUT_DIR/${header_name}.h"

    {
        printf '#pragma once\n\n'
        printf '// %s (%s bytes)\n' "$font_name" "$byte_count"
        printf 'static const unsigned char %s[%s] = {\n' "$array_name" "$byte_count"
        od -An -tx1 -v "$font_path" | awk '
            {
                line = ""
                for (i = 1; i <= NF; i++) {
                    line = line "0x" $i
                    if (i < NF) {
                        line = line ", "
                    }
                }
                printf "    %s,\n", line
            }
        '
        printf '};\n\n'
        printf 'static const unsigned int %s_LEN = %s;\n' "$array_name" "$byte_count"
    } > "$output_path"

    echo "Generated $output_path"
done

