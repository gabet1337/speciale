thesis:
	latex thesis.tex
	bibtex thesis
	latex thesis.tex
	./fixdvi.sh thesis
	ps2pdf14 -dPDFSETTINGS=/prepress -dEmbedAllFonts=true thesis.ps thesis.tmp
	mv -f thesis.tmp thesis.pdf
