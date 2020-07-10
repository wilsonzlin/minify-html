Invoke-WebRequest 'https://wilsonl.in/TDM-GCC-64.7z' -OutFile C:\gcc.7z
7z x C:\gcc.7z -oC:\
$oldpath = (Get-ItemProperty -Path 'Registry::HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Session Manager\Environment' -Name PATH).Path
Set-ItemProperty -Path 'Registry::HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Session Manager\Environment' -Name PATH -Value "C:\TDM-GCC-64\bin;$oldpath"
