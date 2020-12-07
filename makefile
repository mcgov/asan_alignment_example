INCLUDE=$(INCLUDE);$(VCTOOLSINSTALLDIR)crt\src

all:
	cl main.cpp /fsanitize=address /Z7 /Od /D__SANITIZE_ADDRESS__ /link /inferasanlibs

unpoison:
	cl unpoison.cpp /fsanitize=address /Od /Z7 /link /inferasanlibs

clean:
	del *.obj *.exe *.lib *.exp *.pdb