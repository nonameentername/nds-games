all:
	docker run -v $(abspath avalanche):/source -it werner/devkitpro make

clean:
	docker run -v $(abspath avalanche):/source -it werner/devkitpro make clean
