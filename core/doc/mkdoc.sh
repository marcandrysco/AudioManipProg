#!/bin/sh

FILES="ctrl filt key math osc root reverb synth"
EXTRA=".htaccess style.css"

dir="bld"
test -e bld || mkdir bld
test -e bld/fig || mkdir bld/fig

verb=1

for file in osc.pg ; do
  test "$verb" -eq 1 && echo "PLOT $file"
  gnuplot "$file"
done

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
