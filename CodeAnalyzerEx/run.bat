echo "Please configure your IIS's root directory to RemoteCodePublish & run it on 8091 port"

timeout /t 10

cd Debug

start Server.exe
start CppCli-WPF-App.exe
