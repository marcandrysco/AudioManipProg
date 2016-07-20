#!/bin/sh

dir="bld"
test -e bld || mkdir bld
test -e bld/fig || mkdir bld/fig

verb=1

for file in dist.pg ; do
  test "$verb" -eq 1 && echo "PLOT $file"
  gnuplot "$file"
done

for file in index.md dist.md synth.md ; do
  in="$file"
  out="$dir/${in%%.md}"

  test "$verb" -eq 1 && echo "MD $in -o $out"
  test "$file" = "index.md" && root="tut" || root="."
  pandoc "$in" -t html -o "$out" -s --wrap=none --section-divs --template=tpl.xhtml --variable="root:$root"
done

for file in .htaccess style.css ; do
  in="$file"
  out="$dir/$in"

  cp "$in" "$out"
done
