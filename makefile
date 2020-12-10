INCLUDE=$(INCLUDE);$(VCTOOLSINSTALLDIR)crt\src

COMMON_CL_FLAGS=/fsanitize=address  /Z7 /Od /D__SANITIZE_ADDRESS__ /D_ENABLE_EXTENDED_ALIGNED_STORAGE

aligned:
	cl main.cpp  /Fe:aligned.exe $(COMMON_CL_FLAGS) /link /inferasanlibs

unpoison:
	cl unpoison.cpp /fsanitize=address /Od /Z7 /link /inferasanlibs

all: aligned unpoison 

clean:
	del *.obj *.exe *.lib *.exp *.pdb *.ilk