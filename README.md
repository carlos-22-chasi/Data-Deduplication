Overview 

This project implements a secure, process-based filesystem-wide data deduplication algorithm to optimize disk storage and reduce redundancy across large directory hierarchies. It traverses file trees, identifies duplicate files based on content hashes, and replaces duplicates with symbolic links pointing to a single retained copy—freeing up space and streamlining storage usage.

Features

- Duplicate Detection: Scans the file system recursively and identifies files with identical content.

- Content Hashing: Uses cryptographic hash values to accurately detect duplicates, even with different filenames or locations.

- Symbolic Linking: Replaces duplicates with symbolic (soft) links to a single original file.

- Parallel Processing: Constructs a hierarchical process tree to parallelize directory traversal and file analysis for speed and efficiency.

- Secure I/O: Designed with secure file handling and redirection using low-level system calls.

  

How to Run

You can create all of the necessary executable files with
```
  make all
```
Running the program with various root directories can be accomplished with
```
$ ./root_process root_directories/root1
```
Or you can use the make to run with a specific directory:
```
$ make root1
```
Or you can use the make to run with all test cases:
```
$ make final
```
