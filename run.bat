cd Debug
TestExecutive.exe 
start cmd /k RemoteCodePublisherServer.exe localhost:8080
WPFgui.exe localhost:8081 localhost:8080
