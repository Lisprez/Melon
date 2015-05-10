
#
# Copyright (C) Niklaus F.Schen.
#

#!/bin/bash

#confirm install path
install_path=$1

#build path.c & h
echo -e "\n/*\n * Copyright (C) Niklaus F.Schen.\n */" > include/mln_path.h
echo -e "#ifndef __MLN_PATH_H\n#define __MLN_PATH_H" >> include/mln_path.h
echo -e "\nextern char *mln_get_path(void);" >> include/mln_path.h
echo -e "\n#endif\n" >> include/mln_path.h

echo -e "\n/*\n * Copyright (C) Niklaus F.Schen.\n */" > src/mln_path.c
echo "#include \"mln_path.h\"" >> src/mln_path.c
echo -e "\nchar install_path[] = \"$install_path\";" >> src/mln_path.c
echo -e "\nchar *mln_get_path(void)\n{" >> src/mln_path.c
echo "    return install_path;" >> src/mln_path.c
echo "}" >> src/mln_path.c

#build Makefile
#  test system type
is_linux=0
sysname=`uname -s`
test $sysname = "Linux"
if [ $? -eq 0 ]; then
	is_linux=1
fi

# build mln_ipc.c & h
#    write header file
echo "
/*
 * Copyright (C) Niklaus F.Schen.
 */" > include/mln_ipc.h
echo "#ifndef __MLN_IPC_H" >> include/mln_ipc.h
echo "#define __MLN_IPC_H" >> include/mln_ipc.h
cnt=1
for macro in `ls ipc_handlers`
do
	tmp=`echo $macro|cut -d '/' -f 2|cut -d '.' -f 2`
	printf "%s %s %d\n" "#define" $tmp $cnt >> include/mln_ipc.h
	let cnt+=1 > /dev/null
done
echo "extern void mln_set_ipc_handlers(void);" >> include/mln_ipc.h
echo "#endif" >> include/mln_ipc.h
#    write source file array
echo "
/*
 * Copyright (C) Niklaus F.Schen.
 */" > src/mln_ipc.c
echo "#include \"mln_lex.h\"" >> src/mln_ipc.c
echo "#include \"mln_fork.h\"" >> src/mln_ipc.c
echo "#include \"mln_ipc.h\"" >> src/mln_ipc.c
echo "#include \"mln_log.h\"" >> src/mln_ipc.c
echo "#include \"mln_connection.h\"" >> src/mln_ipc.c
echo "#include \"mln_event.h\"" >> src/mln_ipc.c
echo "#include \"mln_fheap.h\"" >> src/mln_ipc.c
echo "#include \"mln_global.h\"" >> src/mln_ipc.c
echo "#include \"mln_hash.h\"" >> src/mln_ipc.c
echo "#include \"mln_prime_generator.h\"" >> src/mln_ipc.c
echo "#include \"mln_rbtree.h\"" >> src/mln_ipc.c
echo "#include \"mln_string.h\"" >> src/mln_ipc.c
echo "#include <stdio.h>" >> src/mln_ipc.c
echo "#include <stdlib.h>" >> src/mln_ipc.c
echo "#include <string.h>" >> src/mln_ipc.c
echo "#include <unistd.h>" >> src/mln_ipc.c
echo "#include <errno.h>" >> src/mln_ipc.c
echo "#include <sys/types.h>" >> src/mln_ipc.c
echo "
/*
 * IPC only act on A child process and the parent process.
 * If there are some threads in a child process,
 * IPC only act on the control thread (main thread) and the parent process.
 * If you need to send something to the peer,
 * you can call mln_tcp_connection_set_buf() to set connection send buffer
 * and call mln_event_set_fd() to set send event, which means you can
 * customize the send routine.
 */" >> src/mln_ipc.c
#    copy all handler files
echo "" >> src/mln_ipc.c
for file in `ls ipc_handlers/*`
do
	cat $file >> src/mln_ipc.c
	echo "" >> src/mln_ipc.c
done
#    write master handler array
sum=`ls -l ipc_handlers|wc -l`
cnt=2
if [ $sum -ge $cnt ] ; then
	echo "mln_ipc_handler_t ipc_master_handlers[] = {" >> src/mln_ipc.c
	for entry in `ls ipc_handlers`
	do
		prefix=`echo $entry|cut -d '/' -f 2|cut -d '.' -f 1`
		tmp=`echo $entry|cut -d '/' -f 2|cut -d '.' -f 2`
		printf "{%s_master, NULL, %s}" $prefix $tmp >> src/mln_ipc.c
		if [ $cnt -lt $sum ] ; then
			printf "," >> src/mln_ipc.c
		fi
		printf "\n" >> src/mln_ipc.c
		let cnt+=1 > /dev/null
	done
	echo "};" >> src/mln_ipc.c
fi
#    write worker handler array
sum=`ls -l ipc_handlers|wc -l`
cnt=2
if [ $sum -ge $cnt ] ; then
	echo "mln_ipc_handler_t ipc_worker_handlers[] = {" >> src/mln_ipc.c
	for entry in `ls ipc_handlers`
	do
		prefix=`echo $entry|cut -d '/' -f 2|cut -d '.' -f 1`
		tmp=`echo $entry|cut -d '/' -f 2|cut -d '.' -f 2`
		printf "{%s_worker, NULL, %s}" $prefix $tmp >> src/mln_ipc.c
		if [ $cnt -lt $sum ] ; then
			printf "," >> src/mln_ipc.c
		fi
		printf "\n" >> src/mln_ipc.c
		let cnt+=1 > /dev/null
	done
	echo "};" >> src/mln_ipc.c
fi
#    write source file soucre code
echo "void mln_set_ipc_handlers(void)" >> src/mln_ipc.c
echo "{" >> src/mln_ipc.c
sum=`ls -l ipc_handlers|wc -l`
cnt=2
if [ $sum -ge $cnt ] ; then
	echo "    int i;" >> src/mln_ipc.c
	echo "    for (i = 0; i < sizeof(ipc_master_handlers)/sizeof(mln_ipc_handler_t); i++) {" >> src/mln_ipc.c
	echo "        mln_set_master_ipc_handler(&ipc_master_handlers[i]);" >> src/mln_ipc.c
	echo "    }" >>src/mln_ipc.c
	echo "    for (i = 0; i < sizeof(ipc_worker_handlers)/sizeof(mln_ipc_handler_t); i++) {" >> src/mln_ipc.c
	echo "        mln_set_worker_ipc_handler(&ipc_worker_handlers[i]);" >> src/mln_ipc.c
	echo "    }" >>src/mln_ipc.c
fi
echo "}" >> src/mln_ipc.c


#Thread module
#build mln_thread_module.h
echo "
/*
 * Copyright (C) Niklaus F.Schen.
 */" > include/mln_thread_module.h
echo "#ifndef __MLN_THREAD_MODULE_H" >> include/mln_thread_module.h
echo "#define __MLN_THREAD_MODULE_H" >> include/mln_thread_module.h

echo "typedef int (*tmain)(int, char **);" >> include/mln_thread_module.h
echo "" >> include/mln_thread_module.h
echo "typedef struct {" >> include/mln_thread_module.h
echo "    char    *alias;" >> include/mln_thread_module.h
echo "    tmain    thread_main;">> include/mln_thread_module.h
echo "} mln_thread_module_t;" >> include/mln_thread_module.h
echo "" >> include/mln_thread_module.h

nr_func=0
for file in `ls threads`
do
	tmp=`echo $file|cut -d '/' -f 2`
	printf "extern int %s_main(int argc, char **argv);\n" $tmp >> include/mln_thread_module.h
	let nr_func+=1 > /dev/null
done

echo "extern void *mln_get_module_entrance(char *alise);" >> include/mln_thread_module.h
echo "#endif" >> include/mln_thread_module.h
#build mln_thread_module.c
echo "
/*
 * Copyright (C) Niklaus F.Schen.
 */" > src/mln_thread_module.c
echo "#include <stdio.h>" >> src/mln_thread_module.c
echo "#include <stdlib.h>" >> src/mln_thread_module.c
echo "#include <string.h>" >> src/mln_thread_module.c
echo "#include <unistd.h>" >> src/mln_thread_module.c
echo "#include <sys/types.h>" >> src/mln_thread_module.c
echo "#include <errno.h>" >> src/mln_thread_module.c
echo "#include \"mln_log.h\"" >> src/mln_thread_module.c
echo "#include \"mln_types.h\"" >> src/mln_thread_module.c
echo "#include \"mln_path.h\"" >> src/mln_thread_module.c
echo "#include \"mln_thread.h\"" >> src/mln_thread_module.c
echo "#include \"mln_thread_module.h\"" >> src/mln_thread_module.c
echo "" >> src/mln_thread_module.c

printf "mln_thread_module_t module_array[%d] = {\n" $nr_func >> src/mln_thread_module.c
cnt=0
for file in `ls threads`
do
	let cnt+=1 > /dev/null
	tmp=`echo $file|cut -d '/' -f 2`
	if [ $cnt -eq $nr_func ]; then
		printf "{\"%s\", %s_main}};\n\n" $tmp $tmp >> src/mln_thread_module.c
	else
		printf "{\"%s\", %s_main},\n" $tmp $tmp >> src/mln_thread_module.c
	fi
done

for file in `ls threads`
do
	cat threads/$file >> src/mln_thread_module.c
	echo "" >> src/mln_thread_module.c
done

echo "void *mln_get_module_entrance(char *alias)" >> src/mln_thread_module.c
echo "{" >> src/mln_thread_module.c
echo "    mln_thread_module_t *tm = NULL;" >> src/mln_thread_module.c
printf "    for (tm = module_array; tm < module_array + %d; tm++) {\n" $nr_func >> src/mln_thread_module.c
echo "        if (!strcmp(alias, tm->alias)) return tm->thread_main;" >> src/mln_thread_module.c
echo "    }" >> src/mln_thread_module.c
echo "    return NULL;" >> src/mln_thread_module.c
echo "}" >> src/mln_thread_module.c


#  build makefile content
echo "# " > Makefile
echo "# Copyright (C) Niklaus F.Schen." >> Makefile
echo "# " >> Makefile
echo -e "CC\t\t= cc" >> Makefile
echo -e "FLAGS\t\t= -Iinclude -c -Wall -ggdb -Werror -O3" >> Makefile
echo -e "MELON\t\t= melon" >> Makefile
echo -e "OBJS\t\t= \\" >> Makefile
cnt=2
sum=`ls -l src/|wc -l`
for fname in `ls src`
do
	test $fname = "mln_salloc.c"
	if [ $? -eq 0 ]; then
		continue
	fi
        test $fname = "mln_sa_rbtree.c"
	if [ $? -eq 0 ]; then
		continue
	fi
	objname=`echo $fname | cut -d '.' -f 1`".o"
	echo -n "		objs/"$objname >> Makefile
	if [ $cnt -lt $sum ]; then
		echo " \\" >> Makefile
	fi
	let cnt+=1 > /dev/null
done
echo "" >> Makefile
echo -e "SA_LIB_OBJS\t= objs/mln_salloc.o objs/mln_log.o objs/mln_sa_rbtree.o objs/mln_defs.o objs/mln_path.o" >> Makefile
echo -e "SA_LIB\t\t= libsalloc.a" >> Makefile

echo -e ".PHONY :\tcompile install clean" >> Makefile
if [ $is_linux -eq 1 ]; then
	echo -e ".IGNORE:\tcleanuser" >> Makefile
fi

echo "compile: MKDIR \$(OBJS) \$(SA_LIB_OBJS) BUILD_SA_LIB \$(MELON)" >> Makefile
echo "clean:" >> Makefile
echo -e "\trm -fr \$(MELON) objs lib Makefile" >> Makefile
echo "MKDIR :" >> Makefile
echo -e "\t-mkdir objs lib" >> Makefile
if [ $is_linux -eq 1 ]; then
	echo "createuser :" >> Makefile
	echo -e "\t-useradd -M -c \"melon user\" -s /sbin/nologin melon 2> /dev/null" >> Makefile
	echo "cleanuser :" >> Makefile
	echo -e "\t-userdel melon 2> /dev/null" >> Makefile
fi
echo "BUILD_SA_LIB :" >> Makefile
echo -e "\tar -r lib/\$(SA_LIB) \$(SA_LIB_OBJS)" >> Makefile
echo "\$(MELON) :" >> Makefile
if [ $is_linux -eq 1 ]; then
	echo -e "\t\$(CC) -o \$(MELON) \$(OBJS) -ggdb -Wall -pthread -Llib/ -lsalloc -ldl" >> Makefile
else
	echo -e "\t\$(CC) -o \$(MELON) \$(OBJS) -ggdb -Wall -pthread -Llib/ -lsalloc -lc" >> Makefile
fi
echo "install:" >> Makefile
echo -e "\ttest -d $install_path || mkdir -p $install_path" >> Makefile
echo -e "\ttest -d '$install_path/sbin' || mkdir -p '$install_path/sbin'" >> Makefile
echo -e "\tcp -fr \$(MELON) $install_path/sbin" >> Makefile
echo -e "\tcp -fr lib $install_path" >> Makefile
echo -e "\tcp -fr conf $install_path" >> Makefile
if [ $is_linux -eq 0 ]; then
	echo -e "\techo \"Please add user 'melon' before running platform.\"" >> Makefile
fi


for fname in `ls src`
do
	objname=`echo $fname | cut -d '.' -f 1`".o"
	echo "objs/$objname :" >> Makefile
	test $fname = "mln_event.c"
	if [ $? -eq 0 ]; then
	#test event system call
		echo "#include<stdio.h>
		#include<sys/epoll.h>
		int main(void){epoll_create(10);return 0;}" > ev_test.c
		cc -o ev_test ev_test.c 2>/dev/null
		if [ "$?" == "0" ]; then
			echo "event module		[EPOLL]"
			echo -e "\t\$(CC) \$(FLAGS) -o \$@ src/$fname -DMLN_EPOLL" >> Makefile
			rm -f ev_test ev_test.c
			continue
		fi
		echo "#include<stdio.h>
		#include<sys/types.h>
		#include<sys/event.h>
		#include<sys/time.h>
		int main(void){kqueue();return 0;}" > ev_test.c
		cc -o ev_test ev_test.c 2>/dev/null
		if [ "$?" == "0" ]; then
			echo "event module		[KQUEUE]"
			echo -e "\t\$(CC) \$(FLAGS) -o \$@ src/$fname -DMLN_KQUEUE" >> Makefile
			rm -f ev_test ev_test.c
			continue
		fi
		rm -f ev_test ev_test.c
		echo "event module		[SELECT]"
		echo -e "\t\$(CC) \$(FLAGS) -o \$@ src/$fname -DMLN_SELECT" >> Makefile
		continue
	fi
	echo -e "\t\$(CC) \$(FLAGS) -o \$@ src/$fname" >> Makefile
done
echo "Configure done!"

