20160804
!!!Be careful. This service can remove you data!!!
System GladBack manage archive files (rar or zip) of backup files of databases.
It deletes files at setted time, every day according to two types of backup rotation policies.
FiFo - leaves last count of archive files.
GFS - leaves files for last count days, last count weeks, and last count months.
GladBack work as a service under Windows system. Now it is testing under
Windows_Server_2008R2 and Windows_7_x_64.

INSTALLATION
1. Compile project and get executable file GladBack.exe.
2. Move it into folder (for example "c:\GladBack\") with Qt libraries:
	libgcc_s_dw2-1.dll
	libstdc++-6.dll
	Qt5Core.dll
3. Open Windows console as Administrator.
4. Run command: "sc create GladBack binpath= c:\GladBack\GladBack.exe".
5. Create setting file "c:\GladBack\setting.gbs" with strong text formatting:
	A. Type of rotation: FiFo or GFS.
	B. Name of task: One word
	C. Full path to folder with archives
	D. If type is FiFo next line is the number of last archives from current date.
		So if there is number 5 at line in means last 5 archives even if they was
		created not every day.
		If type is GFS next line is the number of archives for last days.
		So if there is number 5 at line in means archives for last 5 days.
		If type is GFS next line is the number of archives for last weeks.
		Will be checked if is the archive for Sunday. If it will not available
		You will not have archive for this week).
		If type is GFS next line is the number of archives for last month.
		Will be checked if is the archive for the fist day of month. If it will not available
		You will not have archive for this month).
	E. Start time: 00:00:00.
6. Copy setting file to "C:\Windows\System32\" folder.
7. Run command "sc Start GladBack".
	
Example settings.gbs
---Start (don't copy this)
FiFo
dbMDT_KH
d:\TestFolder\dbTest1\
20
14:41:00
GFS
dbMDT
d:\TestFolder\dbTest2\
5
8
12
14:42:00
---End (don't copy this)	



