UT_LIST := LedDriverTest IO util HomeAutomation

# $(call do-component-list,list,target)
define do-component-list
  @list="$1"; \
  for x in $$list; do \
    make -C $$x $2 || exit 1; \
  done
endef

.PHONY: all
all:
	$(call do-component-list,$(UT_LIST),$@)

.PHONY: build
build:
	$(call do-component-list,$(UT_LIST),$@)

.PHONY: coverage
coverage:
	$(call do-component-list,$(UT_LIST),$@)

.PHONY: clean
clean:
	$(call do-component-list,$(UT_LIST),$@)
