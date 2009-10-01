<?

// some vars
$host = "localhost";	 	// host where asterisk manager is running on
$port = 5038;			// default is 5038
$user = "admin";
$pass = "secret";
$ext = "";			// the extension of the phone to use
$context = "from-internal";	// the context to use

// here we go
$number = rtrim(ltrim($_GET[number]));
$CID = "Call <$number>";
$socket = fsockopen ($host, $port);
fputs($socket, "Action: login\r\n");
fputs($socket, "Events: off\r\n");
fputs($socket, "Username: $user\r\n");
fputs($socket, "Secret: $pass\r\n\r\n");
fputs($socket, "Action: originate\r\n");
fputs($socket, "Channel: SIP/$ext\r\n");
fputs($socket, "WaitTime: 30\r\n");
fputs($socket, "CallerId: $CID\r\n");
fputs($socket, "Exten: $number\r\n");
fputs($socket, "Context: $context\r\n");
fputs($socket, "Priority: 1\r\n\r\n");
fputs($socket, "Action: Logoff\r\n\r\n");
sleep(2);
fclose($socket);

?>

