<?php

/**
 * Test page for the FindByEmail module. If there's an 'email' URL parameter, it will use the module
 * to look up user information from various services and display portraits and user names. If no email
 * is specified, it will display a search box. 
 *
 */

require ('./findbyemail.php');

if (isset($_REQUEST['email'])) {
	$email = urldecode($_REQUEST['email']);
} else {
	$email = '';
}
?>
<html>
<head>
<title>Test page for the FindByEmail module</title>
</head>
<body>
<div style="padding:20px;">
<center>
<form method="GET" action="index.php">
Email address: <input type="text" size="40" name="email" value="<?=$email?>"/>
</form>
</center>
</div>
<?php
if ($email!='') {

	$userinfolist = find_by_email($email);
	
	if ( count($userinfolist)==0 ) {
		
		print "No user info found for $email";
	
	} else {
		
		foreach ($userinfolist as $servicename => $userinfo) {
			print $servicename.'<br/>';
			print $userinfo['display_name'].'<br/>';
			print $userinfo['user_name'].'<br/>';
			print $userinfo['user_id'].'<br/>';
			print $userinfo['location'].'<br/>';
			print $userinfo['portrait_url'].'<br/>';
			if ( $userinfo['portrait_url']!='' ) {
				print '<img src="'.$userinfo['portrait_url'].'"/><br/>';
			}
			print '<hr/>';
		}
	}
}
?>