UT_LIST := LedDriver

# $(call do-ut-list,list,target)
define do-ut-list
  @list="$1"; \
  for x in $$list; do \
    make -C $$x $2 || exit 1; \
  done
endef

.PHONY: all
all:
	$(call do-ut-list,$(UT_LIST),$@)

.PHONY: build
build:
	$(call do-ut-list,$(UT_LIST),$@)

.PHONY: coverage
coverage:
	$(call do-ut-list,$(UT_LIST),$@)

.PHONY: clean
clean:
	$(call do-ut-list,$(UT_LIST),$@)
