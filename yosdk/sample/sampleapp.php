<?php

// Include the YOS library.
require dirname(__FILE__).'/../lib/Yahoo.inc';

// debug settings
error_reporting(E_ALL | E_NOTICE); # do not show notices as library is php4 compatable
ini_set('display_errors', true);
YahooLogger::setDebug(true);
YahooLogger::setDebugDestination('LOG');

// use memcache to store oauth credentials via php native sessions
ini_set('session.save_handler', 'files');
session_save_path('/tmp/');
session_start();

// Make sure you obtain application keys before continuing by visiting:
// https://developer.yahoo.com/dashboard/createKey.html

define("CONSUMER_KEY", "dj0yJmk9WUxPUkhFUWxISWpvJmQ9WVdrOWFYWmhTVzVDTXpBbWNHbzlNVGt4TmpJNU1EazROdy0tJnM9Y29uc3VtZXJzZWNyZXQmeD01Ng--");
define("CONSUMER_SECRET", "f893cf549be5cb37f83b1414e2ff212df2ea4c18");
define("APP_ID", "ivaInB30");

if(array_key_exists("logout", $_GET)) {
	// if a session exists and the logout flag is detected
	// clear the session tokens and reload the page.
	YahooSession::clearSession();
	header("Location: sampleapp.php");
}

// check for the existance of a session.
// this will determine if we need to show a pop-up and fetch the auth url,
// or fetch the user's social data.
$hasSession = YahooSession::hasSession(CONSUMER_KEY, CONSUMER_SECRET, APP_ID);

if($hasSession == FALSE) {
	// create the callback url,
	$callback = YahooUtil::current_url()."?in_popup";

	// pass the credentials to get an auth url.
	// this URL will be used for the pop-up.
	$auth_url = YahooSession::createAuthorizationUrl(CONSUMER_KEY, CONSUMER_SECRET, $callback);
}
else {
	// pass the credentials to initiate a session
	$session = YahooSession::requireSession(CONSUMER_KEY, CONSUMER_SECRET, APP_ID);

	// if the in_popup flag is detected,
	// the pop-up has loaded the callback_url and we can close this window.
	if(array_key_exists("in_popup", $_GET)) {
		close_popup();
		exit;
	}

	// if a session is initialized, fetch the user's profile information
	if($session) {
		// Get the currently sessioned user.
		$user = $session->getSessionedUser();

		// Load the profile for the current user.
		$profile = $user->getProfile();
	}
}

/**
 * Helper method to close the pop-up window via javascript.
 */
function close_popup() {
?>
<script type="text/javascript">
	window.close();
</script>
<?php
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
	<head>
		<title>YOS Social Platform Sample Application</title>

		<!-- Combo-handled YUI JS files: -->
		<script type="text/javascript" src="http://yui.yahooapis.com/combo?2.7.0/build/yahoo-dom-event/yahoo-dom-event.js"></script>
		<script type="text/javascript" src="popupmanager.js"></script>

		<!-- Combo-handled YUI CSS files: -->
		<link rel="stylesheet" type="text/css" href="http://yui.yahooapis.com/combo?2.7.0/build/reset-fonts-grids/reset-fonts-grids.css&2.7.0/build/base/base-min.css">
	</head>
	<body>
		<?php
			if($hasSession == FALSE) {
				// if a session does not exist, output the
				// login / share button linked to the auth_url.
				echo sprintf("<a href=\"%s\" id=\"yloginLink\"><img src=\"http://l.yimg.com/a/i/ydn/social/updt-spurp.png\"></a>\n", $auth_url);
			}
			else if($hasSession && $profile) {
				// if a session does exist and the profile data was
				// fetched without error, print out a simple usercard.
				echo sprintf("<img src=\"%s\"/><p><h2>Hi <a href=\"%s\" target=\"_blank\">%s!</a></h2></p>\n", $profile->image->imageUrl, $profile->profileUrl, $profile->nickname);

				if($profile->status->message != "") {
					$statusDate = date('F j, y, g:i a', strtotime($profile->status->lastStatusModified));
					echo sprintf("<p><strong>&#8220;</strong>%s<strong>&#8221;</strong> on %s</p>", $profile->status->message, $statusDate);
				}

				echo "<p><a href=\"?logout\">Logout</a></p>";
			}
		?>
		<script type="text/javascript">
			var Event = YAHOO.util.Event;
			var _gel = function(el) {return document.getElementById(el)};

			function handleDOMReady() {
				if(_gel("yloginLink")) {
					Event.addListener("yloginLink", "click", handleLoginClick);
				}
			}

			function handleLoginClick(event) {
				// block the url from opening like normal
				Event.preventDefault(event);

				// open pop-up using the auth_url
				var auth_url = _gel("yloginLink").href;
				PopupManager.open(auth_url,600,435);
			}

			Event.onDOMReady(handleDOMReady);
		</script>
	</body>
</html>