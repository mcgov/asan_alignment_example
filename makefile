INCLUDE=$(INCLUDE);$(VCTOOLSINSTALLDIR)crt\src

COMMON_CL_FLAGS=/fsanitize=address /Z7 /Od /D__SANITIZE_ADDRESS__ /D_ENABLE_EXTENDED_ALIGNED_STORAGE

allocate:
	cl allocator\main.cpp  /Fe:allocator.exe $(COMMON_CL_FLAGS) /link /inferasanlibs

unpoison:
	cl bad_unpoison\unpoison.cpp $(COMMON_CL_FLAGS) /link /inferasanlibs

all: allocate unpoison 

clean:
	del *.obj *.exe *.lib *.exp *.pdb *.ilk