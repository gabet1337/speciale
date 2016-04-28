thesis:
	latex thesis.tex
	bibtex thesis
	latex thesis.tex
	dvips -t letter -o thesis.tmp thesis.dvi
	mv -f thesis.tmp thesis.ps
	ps2pdf14 -dPDFSETTINGS=/prepress -dEmbedAllFonts=true thesis.ps thesis.tmp
	mv -f thesis.tmp thesis.pdf
