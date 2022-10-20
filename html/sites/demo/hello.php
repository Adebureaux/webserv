<?php
// This example illustrates the "HTTP/" special case
// Better alternatives in typical use cases include:
// 1. header($_SERVER["SERVER_PROTOCOL"] . " 404 Not Found");
//    (to override http status messages for clients that are still using HTTP/1.0)
// 2. http_response_code(404); (to use the default message)
header("HTTP/1.1 404 Not Found");
?>

<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8" />
        <title>PHP date</title>
    </head>
    <body>
        <h1>PHP date</h1>
        <p>Today's date is <?php echo date('d/m/Y h:i:s'); ?>.</p>
    </body>
</html>
