<html>
 <head>
  <title>PHP Test</title>
 </head>
 <body>
 <?php
 	echo ' <h1>webserv cgi</h1>';
	echo "<h2> hello ". htmlspecialchars($_GET["name"]) . "</h2>";
	?
 </body>
</html>
