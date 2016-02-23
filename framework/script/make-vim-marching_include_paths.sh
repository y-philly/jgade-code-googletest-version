#!/bin/sh

tmpfile=/tmp/make-vim-marching_include_paths.sh.txt


echo "let include_paths = [" > include_paths.vim

rm -f ${tmpfile}
for p in $@; do
  for c in `find ${p} -type d ! -name "obj*"`; do
    winpath=`cygpath -m ${c}`
    echo "\\ \"${winpath}\",">> ${tmpfile}
  done
done
grep -v "obj" ${tmpfile} >> include_paths.vim

echo "\]" >> include_paths.vim
echo "let g:marching_include_paths += include_paths" >> include_paths.vim
