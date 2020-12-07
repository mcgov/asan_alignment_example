INCLUDE=$(INCLUDE);$(VCTOOLSINSTALLDIR)crt\src

unaligned:
	cl main.cpp /fsanitize=address /Fe:unaligned.exe /Z7 /Od /D__SANITIZE_ADDRESS__ /link /inferasanlibs

aligned:
	cl main.cpp /fsanitize=address /Fe:aligned.exe /Z7 /Od /D__SANITIZE_ADDRESS__ /DAPPLY_8_BYTE_ALIGNMENT /link /inferasanlibs

unpoison:
	cl unpoison.cpp /fsanitize=address /Od /Z7 /link /inferasanlibs

clean:
	del *.obj *.exe *.lib *.exp *.pdb