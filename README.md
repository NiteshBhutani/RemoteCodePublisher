# RemoteCodePublisher

Consist of a Remote Code Publisher, NoSQLDb, Dependency Analyzer, local client, and communication channel that supports client access to the publisher from any   internet enabled processor.

### Communication Channel - 
a) HTTP style request/response transactions
b) One-way communication, allowing asynchronous messaging between any two endpoints that are capable of listening for connection requests and connecting to a remote listener.
c) Transmission of byte streams that are set up with one or more negotiation messages followed by transmission of a stream of bytes of specified stream size

### Remote Code Publisher -

a) Accept source code text files, sent from a local client.
b) Support building dependency relationships between code files saved in specific repository folders.
c) Support HTML file creation for all the files in a specified repository folder, including linking information that displays dependency relationships, and supports and navigation based on dependency relationships.
d) Delete stored files, as requested by a local client.

### GUI -
a) Upload one or more source code text files to the Remote Publisher, specifying a category with which those files are associated1.
b) Display file categories, based on the directory structure supported by the Repository.
c) Display all the files in any category.
d) Display all of the files in any category that have no parents.
e) Display the web page for any file in that file list by clicking within a GUI control.

### Dependency Analyzer-
Finds, for each file in a specified file collection, all other files from the file collection on which they depend. File A depends on file B, if and only if, it uses the name of any type or global function or global data defined in file B. It might do that by calling a function or method of a type or by inheriting the type. Note that this intentionally does not record dependencies of a file on files outside the file set, e.g., language and platform libraries.

### NoSqlDb:
Stores and retrieves dependency information in a NoSqlDb<std::string>.
