SRC := $(wildcard ./*.tex) 
IMG := $(wildcard img/*.jpg) $(wildcard img/*.png)
EXP := $(shell find . -name '*.c') $(shell find . -name 'Makefile') \
	$(shell find . -name '*.h')

all: lec.pdf

lec.pdf: $(SRC) $(IMG) $(EXP)
	pdflatex lec

.PHONY: clean
clean:
	rm -f *.dvi *.ps *.pdf *.aux *.toc *.log *.blg *.bbl *.out *.snm *.nav \
		*.vrb
