# This filex is a Makefile

.SILENT:
.PHONY : all clean ps

PAPER     = alvin_remus
PAGE      = 1
GEOMETRY  = 940x1125+0+0
LDIR      = $(CURDIR)/figures/

EPSFILES = $(patsubst $(LDIR)%.fig, 				\
	   $(LDIR)%.eps,$(wildcard $(LDIR)*.fig))		\

# I recommend having something like the following command line 
# alias:
#
#    alias mq 'make dvi PAGE=!* SCALE=6 GEOMETRY=940x1125+0+0'
#
# The page number is taken as a argument and is presumably the 
# pager you're editing at the momemtn. The SCALE and GEOMETRY
# values can be set in the alias to be good for your computer.
#
# Then you can type "mq 10" to build and open the view on p.10

main: $(EPSFILES)
	latex $(PAPER)
	bibtex $(PAPER)
	makeindex $(PAPER)
	latex $(PAPER)

eps: $(EPSFILES)
	echo Done creating EPS files.

pdf: main
	echo; echo; echo Converting $(PAPER).dvi to $(PAPER).pdf....
	dvipdf $(PAPER).dvi
	echo $(PAPER).pdf has been created.

pdfo: main
	dvipdf $(PAPER).dvi
	open $(PAPER).pdf &

clean:
	rm -f $(PAPER).dvi *~  $(PAPER).log $(PAPER).pdf  
	rm -f $(PAPER).out $(PAPER).ind
	rm -f $(PAPER).bbl $(PAPER).blg $(PAPER).ps $(PAPER).aux
	rm -f $(EPSFILES) $(PAPER).toc $(PAPER).idx $(PAPER).ilg
	rm -f $(LDIR)*.bak $(LDIR)*~ 

$(EPSFILES): %.eps: %.fig
	echo $@
	fig2dev -L eps $< $@;

