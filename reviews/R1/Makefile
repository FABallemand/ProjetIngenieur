FILENAME=report

all:
	latexmk -pdf -bibtex -pdflatex="pdflatex -interaction=nonstopmode" -use-make -f $(FILENAME).tex
	
.PHONY: clean
clean:
	latexmk -CA