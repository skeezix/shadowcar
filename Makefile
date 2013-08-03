
all:	nothing

nothing:
	@echo "Nothing to do?"

clean:
	find . -name "*~*" -exec rm {} \; -print