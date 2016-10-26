#!/bin/sh

FILES="ctrl key math osc root synth efx/filt efx/gain efx/gen efx/reverb mod/piano"
EXTRA=".htaccess style.css"

dir="bld"
test -e bld || mkdir bld
test -e bld/fig || mkdir bld/fig
test -e bld/efx || mkdir bld/efx
test -e bld/mod || mkdir bld/mod

verb=1

for file in osc.pg ; do
  test "$verb" -eq 1 && echo "PLOT $file"
  gnuplot "$file"
done

for file in $FILES ; do
  in="$file.md"
  out="$dir/$file"

  test "$verb" -eq 1 && echo "MD $in -o $out"

  case "$file" in
    root ) root="core" ;;
    */* )  root=".." ;;
    * )    root="." ;;
  esac

  pandoc "$in" -t html -o "$out" -s --wrap=none --section-divs --template=tpl.xhtml --variable="root:$root"
done

for file in $EXTRA ; do
  in="$file"
  out="$dir/$in"

  cp "$in" "$out"
done
