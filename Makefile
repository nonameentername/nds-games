.PHONY: all build clean clean-build publish shell-nds

all:
	$(MAKE) shell-nds SHELL_COMMAND='make build'

shell-nds:
	docker run --rm -v "$(CURDIR):$(CURDIR)" -w "$(CURDIR)" werner/devkitpro $(SHELL_COMMAND)

build:
	$(MAKE) -C avalanche

clean:
	$(MAKE) shell-nds SHELL_COMMAND='make clean-build'

clean-build:
	$(MAKE) -C avalanche clean

publish:
	cp avalanche/avalanche.nds public/avalanche/avalanche.nds
