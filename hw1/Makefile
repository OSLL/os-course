SRC := $(wildcard ./*.tex) 
IMG := $(wildcard img/*.jpg) $(wildcard img/*.png)

all: main.pdf

main.pdf: $(SRC) $(IMG)
	pdflatex main
	pdflatex main

.PHONY: clean
clean:
	rm -f *.dvi *.ps *.pdf *.aux *.toc *.log *.blg *.bbl *.out *.snm *.nav
