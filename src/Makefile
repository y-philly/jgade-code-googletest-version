ctags_option = -R --c++-kinds=+pl --fields=+iaS --extra=+q

.PHONY: tags
tags:
	ctags $(ctags_option)

.PHONY: cscope
cscope:
	cscope -R -q -b
