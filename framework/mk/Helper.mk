# $(call get-src-from-dir,dir)
define get-src-from-dir
  $(wildcard $1/*.cpp) $(wildcard $1/*.cc) $(wildcard $1/*.c)
endef

# $(call get-dirs-from-dirspec,dir)
define get-dirs-from-dirspec
  $(wildcard $1)
endef

__src_to = $(subst .c,$1, $(subst .cpp,$1, $(subst .cc,$1,$2)))

# $(call src-to,suffix,src)
define src-to
  $(addprefix obj/,$(call __src_to,$1,$2))
endef

# $(call get-lib-from-dir,dir)
define get-lib-from-dir
  $(wildcard $1/*.a)
endef

# $(call get-lib-from-dir,dir)
define get-lib-from-dir
  $(wildcard $1/*.a)
endef

# $(call debug-print-list,list)
define debug-print-list
  $(foreach word,$1,echo "  $(word)";) echo;
endef

# $(call src-to-o,src)
define src-to-o
  $(call src-to,.o,$1)
endef

# $(call src-to-d,src)
define src-to-d
  $(call src-to,.d,$1)
endef

# $(call src-to-gcda,src)
define src-to-gcda
  $(call src-to,.gcda,$1)
endef

# $(call src-to-gcno,src)
define src-to-gcno
  $(call src-to,.gcno,$1)
endef
