#!/bin/bash
if [ ! -n "$1" ] ;then
    echo ""
    echo 'Error!'
    echo 'Uage:makecreate [objectName]'
    echo ""
    exit
fi
:>Makefile
echo 'cc = gcc' >> Makefile
echo 'prom = '$1 >> Makefile
echo 'deps = $(shell find ./ -name '"'"'*.h'"'"'|sed '"'"'s%.*/%%'"'"')' >> Makefile
echo 'src = $(shell find ./ -name '"'"'*.c'"'"'|sed '"'"'s%.*/%%'"'"')' >> Makefile
echo 'obj = $(src:%.c=%.o)' >> Makefile
echo '' >> Makefile
echo '$(prom):$(obj)' >> Makefile
echo '	$(cc) -o $(prom) $(obj)' >> Makefile
echo '' >> Makefile
echo '%.o:%.c $(deps)' >> Makefile
echo '	$(cc) -c $< -o $@' >> Makefile
echo '' >> Makefile
echo 'clean:' >> Makefile
echo '	rm -rf $(obj) $(prom)' >> Makefile