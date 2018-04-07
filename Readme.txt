<<<<<<<<<<<Remote Code Publisher - CSE 687 OOD Project 4>>>>>>>>>>>>>>



Note - All the files ( DepAnal.xml, compile.bat, run.bat, serverSourceCodeRepo - containing source code files uploaded to server,
Repositry - contaning server published files ) are present in Project Root/Home Location.

"Repository" folder is the folder in which all the published files are kept.
"serverSourceCodeRepo" is the folder in which all the files uploaded to server are kept. 


To Compile the program :- 

1) Go to Project Home folder(where "Code Publisher.sln" is present)
2) Run "devenv "RemoteCodePublisher.sln" /rebuild debug" -> This will create TestExecutive.exe,RemoteCodePublisherServer.exe and WPFgui.exe in "./Debug" Folder 

To Run the program :-

1) Go to Project Home folder(where CodeAnalysis.sln is present) and 
2)-- Run "TestExecutive.exe  -> This will run the TestExecutive 
  -- Run "RemoteCodePublisherServer.exe "localhost:8080" -> This will run the server	
  -- Run "WPFgui.exe localhost:8081 localhost:8081" - > This will run the client

CommandLine Arguments:-

To run the TestExecutive.exe we have to provide no command line arguments 
To run the RemoteCodePublisherServer.exe we have to provide one command line arguments - Port on which server is running
To run the TestExecutive.exe we have to provide two command line arguments - Client address and server address



