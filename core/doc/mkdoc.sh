#!/bin/sh

FILES="osc root key"
EXTRA=".htaccess style.css"

dir="bld"
test -e bld || mkdir bld

verb=1

for file in $FILES ; do
  in="$file.md"
  out="$dir/$file"

  test "$verb" -eq 1 && echo "MD $in -o $out"
  test "$file" = "root" && root="core" || root="."
  pandoc "$in" -t html -o "$out" -s --wrap=none --section-divs --template=tpl.xhtml --variable="root:$root"
done

for file in $EXTRA ; do
  in="$file"
  out="$dir/$in"

  cp "$in" "$out"
done
