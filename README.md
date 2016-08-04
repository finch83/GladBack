<html>
 <h1>GladBack</h1>
<h2>!!!Be careful. This service can remove you data!!!</h2>
 <body>
<p>System GladBack manage archive files (rar or zip) of backup files of databases.<br>
It deletes files at setted time, every day according to two types of backup rotation policies.<br>
FiFo - leaves last count of archive files.<br>
GFS - leaves files for last count days, last count weeks, and last count months.<br>
GladBack work as a service under Windows system. Now it is testing under
Windows_Server_2008R2 and Windows_7_x_64.</p>

<h3>INSTALLATION</h3>
<ol type="1">
<li>Compile project and get executable file GladBack.exe.</li>
<li>Move it into folder (for example "c:\GladBack\") with Qt libraries:</li>
	<ul>
		<li>libgcc_s_dw2-1.dll</li>
		<li>libstdc++-6.dll</li>
		<li>Qt5Core.dll</li>
	</ul>	
<li>Open Windows console as Administrator.</li>
<li>Run command: "sc create GladBack binpath= c:\GladBack\GladBack.exe".</li>
<li>Create setting file "c:\GladBack\setting.gbs" with strong text formatting:
	<ol type="A">
		<li>Type of rotation: FiFo or GFS.</li>
		<li>Name of task: One word.</li>
		<li>Full path to folder with archives.</li>
		<li>If type is FiFo next line is the number of last archives from current date.<br>
			So if there is number 5 at line in means last 5 archives even if they was
			created not every day.<br>
			If type is GFS next line is the number of archives for last days.<br>
			So if there is number 5 at line in means archives for last 5 days.<br>
			If type is GFS next line is the number of archives for last weeks.<br>
			Will be checked if is the archive for Sunday. If it will not available
			You will not have archive for this week).<br>
			If type is GFS next line is the number of archives for last month.<br>
			Will be checked if is the archive for the fist day of month. If it will not available.<br>
			You will not have archive for this month).</li>
		<li>Start time: 00:00:00.</li>
	</ol>	
<li>Copy setting file to "C:\Windows\System32\" folder.</li>
<li>Run command "sc Start GladBack".</li>
</ol>
	
<b>Example settings.gbs</b><br>
FiFo<br>
dbMDT_KH<br>
d:\TestFolder\dbTest1\<br>
20<br>
14:41:00<br>
GFS<br>
dbMDT<br>
d:\TestFolder\dbTest2\<br>
5<br>
8<br>
12<br>
14:42:00<br>
 </body>
</html>