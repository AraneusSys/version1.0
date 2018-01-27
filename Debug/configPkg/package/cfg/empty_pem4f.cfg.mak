# invoke SourceDir generated makefile for empty.pem4f
empty.pem4f: .libraries,empty.pem4f
.libraries,empty.pem4f: package/cfg/empty_pem4f.xdl
	$(MAKE) -f C:\workspace_v6_1_6\araneus3/src/makefile.libs

clean::
	$(MAKE) -f C:\workspace_v6_1_6\araneus3/src/makefile.libs clean

