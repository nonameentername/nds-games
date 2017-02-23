curdir = $(shell pwd)

define make
	docker run --rm -v $(1):/source -it werner/devkitpro make
endef

define clean
	docker run --rm -v $(1):/source -it werner/devkitpro make clean
endef

all:
	$(call make, $(curdir)/avalanche)

clean:
	$(call clean, $(curdir)/avalanche)
