
This is a small utility to execute bulk deletion of files. Input is a file with paths and sizes of the files to delete


**Build:**

	1. Install GCC
	
			sudo apt update
			sudo apt install build-essential
			gcc --version

	2. Locate folder where main.c and helper.c are located (ie: /mnt/c/projects/explore/2024/2024-03-25\ Metadata/tools/mass_deleter/ )

	3. Navigate to a folder where you have write permissions
	
	4. Compile:

			rm -f deleter && SRC=/mnt/c/projects/explore/2024/2024-03-25\ Metadata/tools/mass_deleter/ && gcc -static -O3 "$SRC/main.c" "$SRC/helper.c" -o deleter

	5. This will create an executable named deleter

			ls -lh
			total 3106324
			drwxr-xr-x  3 jose jose      73728 Apr 26 12:28 .
			drwxr-x--- 14 jose jose       4096 Apr 26 12:06 ..
			-rwxr-xr-x  1 jose jose      17480 Apr 26 12:28 deleter

**Help:**	

		./deleter -h
		Mass deleter 2024-April
		deleter -f file [-l log] [-p path_value] [-h] [-d]

			-f       files with list of items to delete. item format: <path>,<size>
			-p       if <path> to delete is not an absolute path then prepend path_value to the file to delete
			-l       log file. Default is ./deleter_log.txt
			-d       dry-run mode, nothing gets deleted
			-h       display this help
			-y       no user confirmation


**Usage:**
	
	Prepare a file with items to delete using where each line is a file to delete. Each line must include the path and the size in bytes separated with a comma.

	$ cat /mnt/c/projects/explore/2024/2024-03-25\ Metadata/files_globaldebug.txt 
	./css/css_Mv-Q5etDPfCldDQ4XlfWbeJvOUZfV9N9TEnZivS-H50.css,51640
	./css/css_DgLpieVLkfGhZCPY2yYbMaHm3iT8QtXQEHjixfp4QNo.css,63841
	./css/css_Mv-Q5etDPfCldDQ4XlfWbeJvOUZfV9N9TEnZivS-H50.css.gz,10130

	Run the program, the only required parameter is -f. All others are optional

	$./deleter -f '/mnt/c/projects/explore/2024/2024-03-25 Metadata/files_globaldebug.txt' -p /home/jose/p2_delete/globaldebug/ -l ./globaldebug.txt -d


**Follow up: **
		you can delete empty folders with find (be careful, always use flags -type d -empty !!!!)
		find /home/jose/p2_delete/globaldebug/ -empty -type d -delete
