<?php

/**
 * API provider for the FindByEmail module. If there's an 'email' URL parameter, it will use the module
 * to look up user information from various services and return that information as a JSON object
 *
 */

require ('./findbyemail.php');

if (isset($_REQUEST['email'])) {
	$email = urldecode($_REQUEST['email']);
} else {
	$email = '';
}

$userinfolist = find_by_email($email);

$result = array($email => $userinfolist);

print json_encode($result);

?>