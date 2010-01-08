<?php

/**
 *  YOS PHP SDK for accessing social and data apis at Yahoo!
 *
 *  @package     yos-php-sdk
 *  @author      Yahoo! Developer Network
 *  @example     http://developer.yahoo.com/social/sdk/php/
 *
 *  @copyright   Copyright (c) 2009 Yahoo! Inc. All rights reserved.
 *  @license
 *
 *  The copyrights embodied in the content of this file are licensed under the
 *  BSD (revised) open source license.
 *
 *	Redistribution and use of this software in source and binary forms, with
 *	or without modification, are permitted provided that the following
 *	conditions are met:
 *
 *	* Redistributions of source code must retain the above
 *	  copyright notice, this list of conditions and the
 *	  following disclaimer.
 *
 *	* Redistributions in binary form must reproduce the above
 *	  copyright notice, this list of conditions and the
 *	  following disclaimer in the documentation and/or other
 *	  materials provided with the distribution.
 *
 *	* Neither the name of Yahoo! Inc. nor the names of its
 *	  contributors may be used to endorse or promote products
 *	  derived from this software without specific prior
 *	  written permission of Yahoo! Inc.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 *	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Please see the Yahoo! Developer Network forums for support: http://developer.yahoo.net/forum/
 *
 *  Documentation: http://developer.yahoo.com/social/sdk/php/
 */

// Use OAuthConsumer as a test to see if any other instances of OAuth.php may
// have been included. require_once() won't catch situations where multiple
// copies of OAuth.php are included by different parts of an application.
if(!class_exists("OAuthConsumer")) {
    require_once("OAuth.php");
}

define("OAUTH_PARAMS_IN_HEADERS", "HEADERS");
define("OAUTH_PARAMS_IN_POST_BODY", "POSTBODY");
define("OAUTH_SIGNATURE_PLAINTEXT", "PLAINTEXT");
define("OAUTH_SIGNATURE_HMAC_SHA1", "HMAC_SHA1");

define("YAHOO_YAP_SESSION_TYPE", "YAHOO_YAP_SESSION_TYPE");
define("YAHOO_OAUTH_RT_SESSION_TYPE", "YAHOO_OAUTH_RT_SESSION_TYPE");
define("YAHOO_OAUTH_AT_SESSION_TYPE", "YAHOO_OAUTH_AT_SESSION_TYPE");

$YahooConfig = array(
    "SOCIAL_WS_HOSTNAME" => "social.yahooapis.com",
    "PRESENCE_WS_HOSTNAME" => "social.yahooapis.com",
    "UPDATES_WS_HOSTNAME" => "social.yahooapis.com",
    "QUERY_WS_HOSTNAME" => "query.yahooapis.com",
    "OAUTH_HOSTNAME" => "api.login.yahoo.com",
    "YAP_WS_HOSTNAME" => "appstore.apps.yahooapis.com"
);

$GLOBAL_YAHOO_SESSION = NULL;

$GLOBAL_YAHOO_LOGGER_DEBUG = false;
$GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION = "LOG";

class YahooUtil {
	function current_url() {
		return sprintf("http://%s%s",$_SERVER["HTTP_HOST"],$_SERVER["REQUEST_URI"]);
	}

    function verify_signature($consumer, $token=NULL, $oauth_signature) {
        $oauth_signature_method = new OAuthSignatureMethod_HMAC_SHA1();
        $oauth_consumer = new OAuthConsumer($consumer->key, $consumer->secret);
        $oauth_token = ($token) ? new OAuthToken($token->key, $token->secret) : NULL;
        $oauth_request = OAuthRequest::from_request();

        $ok = $oauth_signature_method->check_signature($oauth_request, $oauth_consumer, $oauth_token, $oauth_signature);

        return $ok;
    }

	function is_yap_canvas() {
		return (isset($_POST['yap_appid'])
			&& isset($_POST['yap_view']));
	}

	function is_response_error($response) {
		return (is_null($response) || $response["code"] != 200);
	}

}

class YahooException extends Exception {

}

/**
 * Logging wrapper for the Yahoo objects.
 *
 * @brief Logging wrapper for the Yahoo objects.
 */
class YahooLogger {
    /**
     * Log a message at the debug level.
     *
     * @param $message The message to log.
     */
    function debug($message, $object = NULL) {

        global $GLOBAL_YAHOO_LOGGER_DEBUG;
        global $GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION;
        if($GLOBAL_YAHOO_LOGGER_DEBUG) {
            if($GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION == "CONSOLE") {
                print("DEBUG - $message\n");
                if(!is_null($object)) {
                    print("DEBUG OBJECT - " . print_r($object, true) . "\n");
                }
            }
            else if($GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION == "LOG") {
                error_log("DEBUG - $message");
                if(!is_null($object)) {
                    error_log("DEBUG OBJECT - " . print_r($object, true));
                }
            }
        }
    }

    /**
     * Log a message at the info level.
     *
     * @param $message The message to log.
     */
    function info($message, $object = NULL) {
        global $GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION;
        if($GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION == "CONSOLE") {
            print("INFO - $message\n");
            if(!is_null($object)) {
                print("INFO OBJECT - " . print_r($object, true) . "\n");
            }
        }
        else if($GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION == "LOG") {
            error_log("INFO - $message");
            if(!is_null($object)) {
                error_log("INFO OBJECT - " . print_r($object, true));
            }
        }
    }

    /**
     * Log a message at the error level.
     *
     * @param $message The message to log.
     */
    function error($message, $object = NULL) {
        global $GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION;
        if($GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION == "CONSOLE") {
            print("ERROR - $message\n");
            if(!is_null($object)) {
                print("ERROR OBJECT - " . print_r($object, true) . "\n");
            }
        }
        else if($GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION == "LOG") {
            error_log("ERROR - $message");
            if(!is_null($object)) {
                error_log("ERROR OBJECT - " . print_r($object, true));
            }
        }
    }

    /**
     * Enables/disables session debugging.
     *
     * @param $debug Boolean to enable/disable debugging.
     */
    function setDebug($debug) {
        global $GLOBAL_YAHOO_LOGGER_DEBUG;
        $GLOBAL_YAHOO_LOGGER_DEBUG = (bool) $debug;
    }

    /**
     * Allows callers to configure where debugging output is sent.
     *
     * @param $destination "LOG" to use error_log, "CONSOLE" to use printf,
     *                     "NULL" to disable all logging output.
     * @return boolean True on success, false on failure.
     */
    function setDebugDestination($destination) {
        global $GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION;
        if($destination == "LOG" || $destination == "CONSOLE" ||
                $destination == "NULL") {
            $GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION = $destination;
            return true;
        }
        else {
            return false;
        }
    }
}


/**
 * Defines a session between an application and the Yahoo! platform.
 *
 * @brief Defines a session between an application and the Yahoo! platform.
 */
class YahooSession {
    /**
     * @private
     */
    var $guid = NULL;

    /**
     * @private
     */
    var $consumer = NULL;

    /**
     * @private
     */
    var $accessToken = NULL;

    /**
     * @private
     */
    var $applicationId = NULL;

    /**
     * @private
     */
    var $client = NULL;

    /**
     * @private
     */
    var $application = NULL;

    /**
     * @private
     */
    function YahooSession($consumer, $accessToken, $applicationId)
    {
        $this->consumer = $consumer;
        $this->accessToken = $accessToken;
        $this->applicationId = $applicationId;
        $this->guid = $accessToken->guid;

        $this->client = new OAuthClient($consumer, $accessToken);

        $this->application = new YahooApplication($consumer->key, $consumer->secret);
				$this->application->token = $this->accessToken;
    }

    /**
     * @private
     */
    function getConsumer() {
        return $this->consumer;
    }

    /**
     * @private
     */
    function getAccessToken() {
        return $this->accessToken;
    }

    /**
     * @private
     */
    function getApplicationId() {
        return $this->applicationId;
    }

    /**
     * Gets the currently sessioned user.
     *
     * @return YahooUser The currently sessioned YahooUser.
     */
    function getSessionedUser() {
        return new YahooUser($this, $this->guid, true);
    }

    /**
     * Gets the user who owns the application install.
     * Only valid when viewed in YAP, otherwise will default
     * to the logged-in user.
     *
     * @return YahooUser The currently sessioned YahooUser.
     */
    function getOwner() {
        if(isset($this->accessToken->owner)) {
            return $this->getUser($this->accessToken->owner);
        }
        else {
            return $this->getSessionedUser();
        }
    }

    /**
     * Gets the user indicated by the GUID given.
     *
     * @param $guid The GUID of the user to get.
     * @return YahooUser The user indicated by the GUID given.
     */
    function getUser($guid) {
        return new YahooUser($this, $guid, isset($this->guid) && ($guid == $this->guid));
    }

	/**
     * Executes the given YQL query.
     *
     * @param $yql The query to execute.
	 * @param $env A URL to a YQL environment file.
     * @return The response or NULL if the request fails..
     */
    function query($yql, $env=NULL) {
        return $this->application->query($yql, $env);
    }

    /**
     * @private
     */
    function redirectForAuthorization($consumerKey, $consumerSecret, $callback = NULL, $sessionStore = NULL) {
        $url = YahooSession::createAuthorizationUrl($consumerKey, $consumerSecret, $callback, $sessionStore);

        if(!is_null($url)) {
            header(sprintf("Location: %s", $url));
            exit();
        }
        else {
			// TODO: throw a YahooException
            YahooLogger::error("Failed to create authorization URLs");
        }
    }

    /**
     * Destroys the current session, effectively logging out the current
     * user.
     *
     * @param $sessionStore The session store implementation to clear. See
     *                      YahooSessionStore for more information. If no
     *                      session store is provided, clearSession will
     *                      instantiate a NativeSessionStore and use that.
     */
    function clearSession($sessionStore = NULL) {
        global $GLOBAL_YAHOO_SESSION;

        if(is_null($sessionStore)) {
            $sessionStore = new NativeSessionStore();
        }

        $sessionStore->clearRequestToken();
        $sessionStore->clearAccessToken();

        $GLOBAL_YAHOO_SESSION = NULL;
    }

    /**
     * Checks to see if there is a session in this PHP page request.
     * Doesn't cause any redirects for the user to log in, for that
     * you should call requireSession().
     *
     * @param $consumerKey The OAuth consumer key.
     * @param $consumerSecret The OAuth consumer key secret.
     * @param $applicationId The application ID, optional.
     * @param $sessionStore The session store implementation to use. See
     *                      YahooSessionStore for more information. If no
     *                      session store is provided, clearSession will
     *                      instantiate a NativeSessionStore and use that.
     * @return boolean True if a session is present, false otherwise.
     */
    function hasSession($consumerKey, $consumerSecret, $applicationId = NULL, $sessionStore = NULL, $verifier = NULL)
    {
        if(is_null($sessionStore)) {
            $sessionStore = new NativeSessionStore();
        }

		if(is_null($verifier) && array_key_exists("oauth_verifier", $_GET)) {
            $verifier = $_GET["oauth_verifier"];
        }

        $session = YahooSession::initSession($consumerKey, $consumerSecret, $applicationId, FALSE, NULL, $sessionStore, $verifier);
        return !is_null($session);
    }

    /**
     * Requires that there be a session in this PHP page request. Generates
     * a redirect for the user to log in, if necessary. You must call
     * requireSession() before any data is sent back to the user in order
     * for the redirect to work.
     *
     * @param $consumerKey The OAuth consumer key.
     * @param $consumerSecret The OAuth consumer key secret.
     * @param $applicationId The application ID, optional.
     * @param $callback The callback URL to redirect the user to after
     *                  they verify the application access. If no callback
     *                  is provided, the current page URL will be used.
     * @param $sessionStore The session store implementation to use. See
     *                      YahooSessionStore for more information. If no
     *                      session store is provided, clearSession will
     *                      instantiate a NativeSessionStore and use that.
     * @param $verifier The oauth_verifier returned by the OAuth servers
     *                  after authorization. Passing NULL indicates that
     *                  authorization was completed previously or that
     *                  requireSession() should look for oauth_verifier in
     *                  the $_GET superglobal.
     * @return YahooSession The current session or NULL if a session cannot
     *                      be established.
     */
    function requireSession($consumerKey, $consumerSecret, $applicationId = NULL,
	                        $callback = NULL, $sessionStore = NULL, $verifier = NULL)
    {
        if(is_null($sessionStore)) {
            $sessionStore = new NativeSessionStore();
        }

        if(is_null($verifier) && array_key_exists("oauth_verifier", $_GET)) {
            $verifier = $_GET["oauth_verifier"];
        }

        return YahooSession::initSession($consumerKey, $consumerSecret, $applicationId, TRUE, $callback, $sessionStore, $verifier);
    }

    /**
     * Creates authorization URLs, allowing applications to manage their
     * user experience when the user needs to be sent to Yahoo! to authorize
     * the application to access their account.
     *
     * @param $consumerKey The OAuth consumer key.
     * @param $consumerSecret The OAuth consumer key secret.
     * @param $callback The callback URL to redirect the user to after
     *                  they verify the application access. If no callback
     *                  is provided, the current page URL will be used.
     *                  Use the "oob" callback for desktop clients or for
     *                  web clients where no callback should be used.
     * @param $sessionStore The session store implementation to use. See
     *                      YahooSessionStore for more information. If no
     *                      session store is provided, createAuthorizationUrl
     *                      will instantiate a NativeSessionStore and use that.
     * @return stdclass A PHP object with two properties: "urlWithCallback"
     *                  and "urlWithoutCallback". This allows the application
     *                  to mix and match authorizations that do and don't
     *                  have callbacks in the URLs. urlWithoutCallback is
     *                  useful for JavaScript popup windows while
     *                  urlWithCallback is useful for normal <a href>
     *                  tags.
     */
    function createAuthorizationUrl($consumerKey, $consumerSecret, $callback = NULL, $sessionStore = NULL)
    {
        global $GLOBAL_YAHOO_SESSION;

        if(is_null($sessionStore)) {
            $sessionStore = new NativeSessionStore();
        }

        // No callback URL supplied. Build one from the current URL.
        if(is_null($callback)) {
            $callback = YahooUtil::current_url();
        }

        // Redirect the user to log in.
        $requestToken = YahooAuthorization::getRequestToken($consumerKey, $consumerSecret, $callback);

        if(!is_null($requestToken))
        {
            $sessionStore->storeRequestToken($requestToken);

            $url = YahooAuthorization::createAuthorizationUrl($requestToken, $callback);
            return $url;
        }
        else
        {
            YahooLogger::error("Failed to create request token");
            $GLOBAL_YAHOO_SESSION = NULL;
            return null;
        }
    }

	function initSessionFromYAP($consumerKey, $consumerSecret, $appid)
	{
		global $GLOBAL_YAHOO_SESSION;

		if(!YahooUtil::is_yap_canvas()) {
			// TODO: throw a YahooException
			return NULL;
		}

		$consumer = new stdclass();
        $consumer->key = $consumerKey;
        $consumer->secret = $consumerSecret;

		if ($consumer->key != $_POST["yap_consumer_key"]) {
            error_log("Consumer key from YAP does not match provided key.");
			// TODO: throw a YahooException
            $GLOBAL_YAHOO_SESSION = NULL;
            return;
        }

		$signature_ok = YahooUtil::verify_signature($consumer, null, $_REQUEST['oauth_signature']);

        if (!$signature_ok)
        {
            error_log("Signature from YAP failed.");
			// TODO: throw a YahooException
            $GLOBAL_YAHOO_SESSION = NULL;
            return;
        }

        $accessToken = new stdclass();
        $accessToken->key = $_POST["yap_viewer_access_token"];
        $accessToken->secret = $_POST["yap_viewer_access_token_secret"];
        $accessToken->guid = $_POST["yap_viewer_guid"];
        $accessToken->owner = $_POST["yap_owner_guid"];
        $accessToken->tokenExpires = -1;

        //YahooLogger::debug("YAP AT: " . $accessToken->key . " ATS: " . $accessToken->secret);

        $applicationId = $_POST["yap_appid"];
        $GLOBAL_YAHOO_SESSION = new YahooSession($consumer, $accessToken, $applicationId);

        return $GLOBAL_YAHOO_SESSION;
	}

    /**
     * @private
     */
    function initSession($consumerKey, $consumerSecret, $applicationId, $redirect, $callback, $sessionStore, $verifier)
    {
        global $GLOBAL_YAHOO_SESSION;

        if(!is_null($GLOBAL_YAHOO_SESSION)) {
            return $GLOBAL_YAHOO_SESSION;
        }

        $consumer = new stdclass();
        $consumer->key = $consumerKey;
        $consumer->secret = $consumerSecret;

        $checkSession = YahooSession::checkSession($type, $sessionStore);

        if(!$checkSession) {
            // There doesn't appear to be a session here.
            if($redirect)  {
                $GLOBAL_YAHOO_SESSION = NULL;
                YahooSession::redirectForAuthorization($consumerKey, $consumerSecret, $callback, $sessionStore);
            }
            else {
                // Don't redirect the user, just inform the caller that
                // no session is present.
				// TODO: throw a YahooException
                $GLOBAL_YAHOO_SESSION = NULL;
            }
        }
        else if($type == YAHOO_OAUTH_AT_SESSION_TYPE) {
            // Found an OAuth Access Token session.
            $accessToken = $sessionStore->fetchAccessToken();
            $now = time();

            YahooLogger::debug("OAuth AT: " . $accessToken->key . "   ATS: ". $accessToken->secret);

            if($accessToken->consumer != $consumerKey)
            {
                YahooLogger::error("Consumer key for token does not match the defined Consumer Key. The Consumer Key has probably changed since the user last authorized the application.");
                YahooSession::clearSession($sessionStore);

                if($redirect) {
                    YahooSession::redirectForAuthorization($consumerKey, $consumerSecret, $callback, $sessionStore);
                }
            }

            if($accessToken->tokenExpires >= 0) {
                YahooLogger::debug('AT Expires in: ' . ($accessToken->tokenExpires - $now));
            }

            if(($accessToken->tokenExpires >= 0) && ($accessToken->tokenExpires - $now) < 30) {
                // The access token will expire in less than 30 seconds or
                // it may have expired already. Try to get a new one.
                YahooSession::accessTokenExpired($accessToken, $consumer, $applicationId, $sessionStore);
            }
            else {
                // The access token is still good for a little while, continue using it.
                $GLOBAL_YAHOO_SESSION = new YahooSession($consumer, $accessToken, $applicationId);
            }
        }
        else if($type == YAHOO_OAUTH_RT_SESSION_TYPE)
        {
            if(is_null($verifier)) {
                // Can't proceed without the oauth_verifier, treat it as
                // though there's no session present.
                $sessionStore->clearRequestToken();

				// TODO: throw a YahooException
                $GLOBAL_YAHOO_SESSION = NULL;
            }

            // Found an OAuth Request Token session.
            $requestToken = $sessionStore->fetchRequestToken();

            $accessToken = YahooAuthorization::getAccessToken($consumerKey, $consumerSecret, $requestToken, $verifier);

            if(!is_null($accessToken)) {
                $sessionStore->storeAccessToken($accessToken);
                $sessionStore->clearRequestToken();

                $GLOBAL_YAHOO_SESSION = new YahooSession($consumer, $accessToken, $applicationId);
            }
            else if($redirect)
						{
                // TODO: Add redirect counter so this doesn't happen over and over and over when Yahoo! is completely busted.
                // The fetch for the access token failed. Generate a new
                // request token and try again.
                $GLOBAL_YAHOO_SESSION = NULL;
                YahooSession::redirectForAuthorization($consumerKey, $consumerSecret, $callback, $sessionStore);
            }
            else
						{
                // Don't redirect the user, just inform the caller that
                // no session is present.
                $sessionStore->clearRequestToken();
                $GLOBAL_YAHOO_SESSION = NULL;
            }
        }
				else if($type == YAHOO_YAP_SESSION_TYPE)
        {
            // Found a YAP session.
            $GLOBAL_YAHOO_SESSION = YahooSession::initSessionFromYAP($consumerKey, $consumerSecret, $applicationId);
        }
        else
        {
            trigger_error("Unknown session type found", E_USER_ERROR);
			// TODO: throw a YahooException
            $GLOBAL_YAHOO_SESSION = NULL;
        }

        return $GLOBAL_YAHOO_SESSION;
    }

    /**
     * @private
     */
    function accessTokenExpired($accessToken, $consumer, $applicationId, $sessionStore)
		{
        global $GLOBAL_YAHOO_SESSION;

        $now = time();
        if(($accessToken->handleExpires === -1) ||
                ($now < $accessToken->handleExpires)) {
            // Either the access session handle doesn't expire
            // or it hasn't expired yet. Get a new access token.
            $newAccessToken = YahooAuthorization::getAccessToken(
                    $consumer->key, $consumer->secret, $accessToken, null);
            if(is_null($newAccessToken)) {
                YahooLogger::error("Failed to fetch access token");
                $GLOBAL_YAHOO_SESSION = NULL;
            }

            $sessionStore->storeAccessToken($newAccessToken);

            YahooLogger::debug("Got new AT/ATS from ASH!");
            YahooLogger::debug("OAuth AT: " . $newAccessToken->key . "   ATS: ". $newAccessToken->secret);

            $GLOBAL_YAHOO_SESSION = new YahooSession(
                    $consumer, $newAccessToken, $applicationId);
        }
        else
				{
            // The access token is expired and we don't have
            // a sufficient access session handle to renew
            // the access token. Clear the cookie and redirect
            // to authorization point or return a NULL session.
            $sessionStore->clearAccessToken();

            if ($redirect) {
                YahooSession::redirectForAuthorization($consumer->key, $consumer->secret, $callback, $sessionStore);
            } else {
                $GLOBAL_YAHOO_SESSION = NULL;
            }
        }
    }

    /**
     * @private
     *
     * Checks to see if the current PHP page request has a session and, if so,
     * indicates what type of session is present.
     *
     * @param[out] $sessionType The session type present, if any.
     * @return boolean True if a session is present, false otherwise.
     */
    function checkSession(&$sessionType, $sessionStore) {
        if(array_key_exists("yap_appid", $_POST)) {
            $sessionType = YAHOO_YAP_SESSION_TYPE;
            return true;
        }
        else if($sessionStore->hasAccessToken()) {
            $sessionType = YAHOO_OAUTH_AT_SESSION_TYPE;
            return true;
        }
        else if($sessionStore->hasRequestToken()) {
            $sessionType = YAHOO_OAUTH_RT_SESSION_TYPE;
            return true;
        }
        else {
            return false;
        }
    }
}

/**
 * Represents a Yahoo! application.
 *
 * @brief Represents a Yahoo! application.
 */
class YahooApplication {
    /**
     * @private
     */
    var $consumer = NULL;

    /**
     * @private
     * @deprecated
     */
    var $client = NULL;

	/**
	 * @private
	 */
	var $token = NULL;

    /**
     * Constructs a new YahooApplication object.
     *
     * @param $consumerKey The consumer key of the application.
     * @param $consumerKeySecret The consumer key secret of the application.
     */
    function YahooApplication($consumerKey, $consumerKeySecret) {
        $this->consumer = new OAuthConsumer($consumerKey, $consumerKeySecret);
    }

    /**
     * Sets the small view for the user given by the GUID.
     *
     * @param $guid The GUID of the user to set the small view for.
     * @param $content The content to set the small view to.
     * @return True on success, false otherwise.
     */
    function setSmallView($guid, $content) {
        global $YahooConfig;

		$client = new OAuthClient($this->consumer, NULL);

        $request_url = sprintf("http://%s/v1/cache/view/small/%s", $YahooConfig["YAP_WS_HOSTNAME"], urlencode($guid));
        $response = $client->put($request_url, "text/html;charset=utf-8", $content);

        return !(YahooUtil::is_response_error($response));
    }

    /**
     * Executes the given YQL query.
     *
     * @param $yql The query to execute.
	   * @param $env A URL to a YQL environment file.
     * @return The response or NULL if the request fails..
     */
    function query($yql, $env=NULL)
		{
        global $YahooConfig;

				$client = new OAuthClient($this->consumer, $this->token);

				$request_url = sprintf("http://%s/v1/yql",$YahooConfig["QUERY_WS_HOSTNAME"]);
				$params = array('q' => $yql, 'format' => 'json', 'env' => 'http://datatables.org/alltables.env');

				if(!is_null($env)) {
					$params['env'] = $env;
				}

        $response = $client->get($request_url, $params, 30);

        if(YahooUtil::is_response_error($response)) {
            return NULL;
        }

        $resultSet = json_decode($response["responseBody"]);

        return $resultSet;
    }
}

/**
 * Represents a Yahoo! user.
 *
 * @brief Represents a Yahoo! user.
 */
class YahooUser {
    /**
     * @private
     */
    var $session = NULL;

    /**
     * @private
     */
    var $guid = NULL;

    /**
     * @private
     */
    var $sessioned = false;

    /**
     * @private
     */
    var $client = NULL;

    /**
     * @private
     */
    function YahooUser($session, $guid, $sessioned) {
        $this->session = $session;
        $this->client = $session->client;
        $this->guid = $guid;
        $this->sessioned = $sessioned;
    }

		/**
     * Gets the user's status message.
     *
     * @return The status of the user or NULL if the fetch fails.
     */
    function getStatus() {
        global $YahooConfig;

				$request_url = sprintf("http://%s/v1/user/%s/profile/status",
				$YahooConfig["SOCIAL_WS_HOSTNAME"],urlencode($this->guid));

        $response = $this->client->get($request_url);

        if(is_null($response)) {
            return NULL;
        }
        else if($response["code"] == 404) {
            // No presence is set, return an empty presence.

            $status = new stdclass();
            $status->message = "";
						$status->lastStatusModified = NULL;
            $status->uri = NULL;
            return $status;
        }
        else if($response["code"] != 200) {
            return NULL;
        }
        else {
            $rsp = json_decode($response["responseBody"]);
            return $rsp->status;
        }
    }

		/**
     * Sets the user's status message.
     *
     * @param $message The new status message for the user.
     * @return The status message on success, NULL on failure.
     */
    function setStatus($message) {
        global $YahooConfig;

        if(!$this->sessioned) {
            YahooLogger::error("Can't set the status of an unsessioned user");
            return NULL;
        }

				$message = array("message" => $message);
        $status = array("status" => $message);
				$status_json = json_encode($status);

				$request_url = sprintf("http://%s/v1/user/%s/profile/status", $YahooConfig["SOCIAL_WS_HOSTNAME"], $this->guid);

        $response = $this->client->put($request_url, "application/json", $status_json);

        if(YahooUtil::is_response_error($response)) {
            return NULL;
        }

        $status = json_decode($response["responseBody"]);
        return $status;
    }

    /**
     * Gets the updates for the current user.
     *
     * @param $start The starting offset to list updates from. (default = 0)
     * @param $count The number of updates to request. (default = 10)
     * @return An array of updates for the current user.
     */
    function getUpdates($start = 0, $count = 10) {
        $parameters = array("start" => $start, "count" => $count, "transform" => '(sort "pubDate" numeric descending (all))');
				$updates = $this->get_resource("updates", $parameters);

        return $updates->updates;
    }

    /**
     * Gets the updates for the connections of the current user.
     *
     * @param $start The starting offset to list updates from.
     * @param $count The number of updates to request.
     * @return A list of updates for the connections of the current user.
     */
    function getConnectionUpdates($start = 0, $count = 10) {
        $parameters = array("start" => $start, "count" => $count, "transform" => '(sort "pubDate" numeric descending (all))');
				$updates = $this->get_resource("updates/connections", $parameters);

        return $updates->updates;
    }

    /**
     * Inserts an update for the current user.
     *
     * @param $suid Identifier that globally unique for a given
     *              collectionId within producing source.
     * @param $title Title for the update.
     * @param $link Link back to the cause of the event.
     * @param $description Descriptive text associated with the update,
     *                     optional.
     * @param $date The date of the update event, optional, defaults to now.
     */
    function insertUpdate($suid, $title, $link, $description="", $date=NULL) {
        global $YahooConfig;

        // Make sure this YahooUser is sessioned.
        if(!$this->sessioned) {
            YahooLogger::error("Can't insert updates for an unsessioned user");
            return NULL;
        }

        if (is_null($date)) {
            $date = time();
        }

        // Make sure an application ID was given.
				$appid = $this->session->getApplicationId();
        if(empty($appid)) {
            YahooLogger::error("No application ID given, can't insert update");
            return NULL;
        }

        $source = sprintf("APP.%s", $appid);

				$update = array(
					"collectionID" => $this->guid,
					"collectionType" => "guid",
					"class" => "app",
					"source" => $source,
					"type" => 'appActivity',
					"suid" => $suid,
					"title" => $title,
					"description" => $description,
					"link" => $link,
					"pubDate" => (string)$date
				);

				$update_body = array("updates" => array($update));
				$update_body_json = json_encode($update_body);

				$request_url = sprintf("http://%s/v1/user/%s/updates/%s/%s", $YahooConfig["UPDATES_WS_HOSTNAME"], $this->guid, $source, urlencode($suid));

        $response = $this->client->put($request_url, "application/json", $update_body_json);

        if(YahooUtil::is_response_error($response)) {
            return NULL;
        }

        return TRUE;
    }

    /**
     * Deletes the update of the given SUID. Only allows deleting updates
     * that were inserted by your own application. You won't be able to
     * delete updates from other applications.
     *
     * @param $suid The SUID of the update to be deleted.
     * @return boolean True on success, false on failure.
     */
    function deleteUpdate($suid) {
        global $YahooConfig;

        // Make sure this YahooUser is sessioned.
        if(!$this->sessioned) {
            YahooLogger::error("Can't delete updates for an unsessioned user");
            return FALSE;
        }

        // Make sure an application ID was given.
				$appid = $this->session->getApplicationId();
        if( empty($appid) ) {
            YahooLogger::error("No application ID given, can't delete update");
            return FALSE;
        }

        $source = sprintf("APP.%s", $appid);

				$request_url = sprintf("http://%s/v1/user/%s/updates/%s/%s", $YahooConfig["UPDATES_WS_HOSTNAME"], $this->guid, $source, urlencode($suid));

        $response = $this->client->delete($request_url);

        if(YahooUtil::is_response_error($response)) {
            return NULL;
        }

        return TRUE;
    }

    /**
     * Loads the extended profile of the current user.
     *
     * @return The extended profile of the current user.
     */
    function getProfile() {
        global $YahooConfig;

				$profile = $this->get_resource("profile");

        return $profile->profile;
    }

    /**
     * Gets a list of connections for the current user.
     *
     * @param[in,out] $start The starting offset.
     * @param[in,out] $count The number of connections to fetch.
     * @param[out] $total The total number of contacts available.
     * @return List of connections for the current user.
     */
    function getConnections(&$start, &$count, &$total) {
        global $YahooConfig;

				$parameters = array("view" => "usercard", "start" => $start, "count" => $count);
				$connections = $this->get_resource("connections",$parameters);

        $start = $connections->connections->start;
        $count = $connections->connections->count;
        $total = $connections->connections->total;

        return $connections->connections->connection;
    }

    /**
     * Gets a list of contacts for the current user.
     *
     * @param $start The starting offset.
     * @param $count The number of contacts to fetch.
     * @return List of contacts for the current user.
     */
    function getContacts($start = 0, $count = 10) {
        global $YahooConfig;

        if(!$this->sessioned) {
            YahooLogger::error("Can't get contacts for an unsessioned user");
            return NULL;
        }

				$parameters = array("view" => "tinyusercard", "start" => $start, "count" => $count);
				$contacts = $this->get_resource("contacts",$parameters);

        return $contacts;
    }

    /**
     * Sets the small view for the current user.
     *
     * @param $content The content to set the small view to.
     * @return True on success, false otherwise.
     */
    function setSmallView($content) {
        return $this->session->application->setSmallView($this->guid, $content);
    }

	/**
	 * @private
	 */
	function get_resource($resource, $parameters=array())
	{
		global $YahooConfig;

		$request_url = sprintf("http://%s/v1/user/%s/%s",
		$YahooConfig["SOCIAL_WS_HOSTNAME"], urlencode($this->guid), $resource);

		$response = $this->client->get($request_url,$parameters);

		if(YahooUtil::is_response_error($response)) {
			return NULL;
		}

		$data = json_decode($response["responseBody"]);

		return $data;
	}

	///////////////////////////////////////////////////////////////////////////
	// Deprecated methods
	///////////////////////////////////////////////////////////////////////////

		/**
     * Loads the extended profile of the current user.
     * @deprecated As of 1.2, replaced by getProfile.
     * @return The extended profile of the current user.
     */
    function loadProfile() {

		// method renamed, keeping for compatibility.
		YahooLogger::info("loadProfile is deprecated since 1.2: Please use getProfile");

		return $this->getProfile();
	}

		/**
     * Lists the updates for the current user.
     * @deprecated As of 1.2, replaced by getUpdates.
     *
     *
     * @param $start The starting offset to list updates from. (default = 0)
     * @param $count The number of updates to request. (default = 10)
     * @return A list of updates for the current user.
     */
	function listUpdates($start = 0, $count = 10) {

		// method renamed, keeping for compatibility.
		YahooLogger::info("listUpdates is deprecated since 1.2: Please use getUpdates");

		return $this->getUpdates($start, $count);
	}

		/**
     * Gets the updates for the connections of the current user.
     * @deprecated As of 1.2, replaced by getConnectionUpdates.
     * @param $start The starting offset to list updates from.
     * @param $count The number of updates to request.
     * @return An array of updates for the connections of the current user.
     */
    function listConnectionUpdates($start = 0, $count = 10) {
		// method renamed, keeping for compatibility.
		YahooLogger::info("listConnectionUpdates is deprecated since 1.2: Please use getConnectionUpdates");

		return $this->getConnectionUpdates($start, $count);
	}

    /**
     * Gets the presence of the user, including the status.
     *
     * @return The presence of the user or NULL if the fetch fails.
     * @deprecated As of 1.2, replaced by getStatus
     */
    function getPresence() {
        global $YahooConfig;

				YahooLogger::info("getPresence is deprecated since 1.2: Please use getStatus.");

				$request_url = sprintf("http://%s/v1/user/%s/presence/presence",
				$YahooConfig["PRESENCE_WS_HOSTNAME"],urlencode($this->guid));

        $response = $this->client->get($request_url);

        if(is_null($response)) {
            return NULL;
        }
        else if($response["code"] == 404) {
            // No presence is set, return an empty presence.
            $presence = new stdclass();
            $presence->value = new stdclass();
            $presence->value->status = "";
            return $presence;
        }
        else if($response["code"] != 200) {
            return NULL;
        }
        else {
            $presence = json_decode($response["responseBody"]);
            return $presence->presence;
        }
    }

    /**
     * Sets the presence of the user.
     *
     * @param $status The new status message for the user.
     * @return The status message on success, NULL on failure.
     * @deprecated As of 1.2, replaced by setStatus
     */
    function setPresence($status) {
        global $YahooConfig;

				YahooLogger::info("setPresence is deprecated since 1.2: Please use setStatus");

        if(!$this->sessioned) {
            YahooLogger::error("Can't set the presence of an unsessioned user");
            return NULL;
        }

        $presence = array("status" => $status);
				$presence_json = json_encode($presence);

				$request_url = sprintf("http://%s/v1/user/%s/presence/presence", $YahooConfig["PRESENCE_WS_HOSTNAME"], $this->guid);

        $response = $this->client->put($request_url, "application/json", $presence_json);

        if(YahooUtil::is_response_error($response)) {
            return NULL;
        }

        $presence = json_decode($response["responseBody"]);
        return $presence;
    }

	///////////////////////////////////////////////////////////////////////////
	// End Deprecated methods
	///////////////////////////////////////////////////////////////////////////
}

/**
 * @private
 */
class YahooAuthorization {
    function getRequestToken($consumerKey, $consumerSecret, $callback) {
        global $YahooConfig;

        if(is_null($callback)) {
            $callback = "oob";
        }

        $consumer = new OAuthConsumer($consumerKey, $consumerSecret);
        $client = new OAuthClient($consumer, NULL, OAUTH_PARAMS_IN_POST_BODY, OAUTH_SIGNATURE_HMAC_SHA1);

        $request_url = sprintf("https://%s/oauth/v2/get_request_token", $YahooConfig["OAUTH_HOSTNAME"]);
				$parameters = array("oauth_callback" => $callback);

        $response = $client->post($request_url, "application/x-www-form-urlencoded", $parameters);

        if(is_null($response)) {
            YahooLogger::error("OAuth call to get request token failed");
            return NULL;
        }

        parse_str($response["responseBody"], $token);

        if($response["code"] != 200) {
            $problem = array_key_exists("oauth_problem", $token) ?
                    $token["oauth_problem"] : "unknown problem";
            YahooLogger::error("Failed to create request token: $problem");
            return NULL;
        }

        if(!array_key_exists("oauth_callback_confirmed", $token) ||
                !$token["oauth_callback_confirmed"]) {
            // Callback wasn't confirmed.
            YahooLogger::error("Failed to create request token: callback was not confirmed");
            return NULL;
        }

        $requestToken = new stdclass();
        $requestToken->key = $token["oauth_token"];
        $requestToken->secret = $token["oauth_token_secret"];
        return $requestToken;
    }

    function createAuthorizationUrl($requestToken) {
        global $YahooConfig;

        if(!is_object($requestToken) || !property_exists($requestToken, "key")) {
            YahooLogger::error("Request token doesn't have a 'key' property");
            return NULL;
        }

        return sprintf("https://%s/oauth/v2/request_auth?oauth_token=%s", $YahooConfig["OAUTH_HOSTNAME"], urlencode($requestToken->key));
    }

    function getAccessToken($consumerKey, $consumerSecret, $requestToken, $verifier) {
        $at = YahooAuthorization::getAccessTokenProxy($consumerKey, $consumerSecret, $requestToken, $verifier);

        if(is_null($at)) {
            // Failed to fetch the access token, sleep for 250ms and
            // then try one more time.
            YahooLogger::info("Failed to fetch access token, retrying");
            usleep(250000);
            $at = YahooAuthorization::getAccessTokenProxy($consumerKey, $consumerSecret, $requestToken, $verifier);
        }

        return $at;
    }

    function getAccessTokenProxy($consumerKey, $consumerSecret, $requestToken, $verifier) {
        global $YahooConfig;

        $request_url = sprintf("https://%s/oauth/v2/get_token", $YahooConfig["OAUTH_HOSTNAME"]);

        $consumer = new OAuthConsumer($consumerKey, $consumerSecret);

        $parameters = array();
        if(property_exists($requestToken, "sessionHandle")) {
            $parameters["oauth_session_handle"] = $requestToken->sessionHandle;
        }

        if(!is_null($verifier)) {
            $parameters["oauth_verifier"] = $verifier;
        }

        $client = new OAuthClient($consumer, $requestToken, OAUTH_PARAMS_IN_POST_BODY);

        $response = $client->post($request_url, "application/x-www-form-urlencoded", $parameters);

        if(is_null($response)) {
            YahooLogger::error("OAuth call to get access token failed");
            return NULL;
        }

        parse_str($response["responseBody"], $token);

        if($response["code"] != 200) {
            YahooLogger::error("Failed to fetch access token: " . $token["oauth_problem"]);
            return NULL;
        }

        $now = time();

        $accessToken = new stdclass();
        $accessToken->key = $token["oauth_token"];
        $accessToken->secret = $token["oauth_token_secret"];
        $accessToken->guid = $token["xoauth_yahoo_guid"];
        $accessToken->consumer = $consumerKey;
        $accessToken->sessionHandle = $token["oauth_session_handle"];

        // Check to see if the access token ever expires.
        YahooLogger::debug('AT expires in '.$token['oauth_expires_in'].'; ASH expires in '.$token["oauth_authorization_expires_in"]);
        if(array_key_exists("oauth_expires_in", $token)) {
            $accessToken->tokenExpires = $now + $token["oauth_expires_in"];
        }
        else {
            $accessToken->tokenExpires = -1;
        }

        // Check to see if the access session handle ever expires.
        if(array_key_exists("oauth_authorization_expires_in", $token)) {
            $accessToken->handleExpires = $now +
                    $token["oauth_authorization_expires_in"];
        }
        else {
            $accessToken->handleExpires = -1;
        }
        return $accessToken;
    }
}

/**
 * Cookie-based implementation of the session store. This is the default
 * session storage used by the Y!OS PHP SDK. Developers are free to
 * implement their own session store implementations and pass them to
 * YahooSession::hasSession, YahooSession::requireSession and
 * YahooSession::clearSession. By default, if no session store is passed
 * to YahooSession::hasSession or YahooSession::requireSession, an instance
 * of a NativeSessionStore is used.
 *
 * @brief Cookie-based implementation of the session store.
 */
class CookieSessionStore {
    /**
     * Indicates if the session store has a request token.
     *
     * @return True if a request token is present, false otherwise.
     */
    function hasRequestToken() {
        return array_key_exists("yosdk_rt", $_COOKIE) && (strlen($_COOKIE["yosdk_rt"]) > 0);
    }

    /**
     * Indicates if the session store has an access token.
     *
     * @return True if an access token is present, false otherwise.
     */
    function hasAccessToken() {
        return array_key_exists("yosdk_at", $_COOKIE) && (strlen($_COOKIE["yosdk_at"]) > 0);
    }

    /**
     * Stores the given request token in the session store.
     *
     * @param $token A PHP stdclass object containing the components of
     *               the OAuth request token.
     * @return True on success, false otherwise.
     */
    function storeRequestToken($token) {
        if(!headers_sent()) {
            return setcookie("yosdk_rt", base64_encode(json_encode($token)), time() + 600);
        }
        else {
            return false;
        }
    }

    /**
     * Fetches and returns the request token from the session store.
     *
     * @return The request token.
     */
    function fetchRequestToken() {
        return json_decode(base64_decode($_COOKIE["yosdk_rt"]));
    }

    /**
     * Clears the request token from the session store.
     *
     * @return True on success, false otherwise.
     */
    function clearRequestToken() {
        if(!headers_sent()) {
            return setcookie("yosdk_rt", "", time() - 600);
        }
        else {
            return false;
        }
    }

    /**
     * Stores the given access token in the session store.
     *
     * @param $token A PHP stdclass object containing the components of
     *               the OAuth access token.
     * @return True on success, false otherwise.
     */
    function storeAccessToken($token) {
        if(!headers_sent()) {
            return setcookie("yosdk_at", base64_encode(json_encode($token)),
                    time() + (30 * 24 * 60 * 60));
        }
        else {
            return false;
        }
    }

    /**
     * Fetches and returns the access token from the session store.
     *
     * @return The access token.
     */
    function fetchAccessToken() {
        return json_decode(base64_decode($_COOKIE["yosdk_at"]));
    }

    /**
     * Clears the access token from the session store.
     *
     * @return True on success, false otherwise.
     */
    function clearAccessToken() {
        if(!headers_sent()) {
            return setcookie("yosdk_at", "", time() - 600);
        }
        else {
            return false;
        }
    }
}

/**
 * PHP session based implementation of the session store. This is the default
 * session storage used by the Y!OS PHP SDK. Developers are free to
 * implement their own session store implementations and pass them to
 * YahooSession::hasSession, YahooSession::requireSession and
 * YahooSession::clearSession. By default, if no session store is passed
 * to YahooSession::hasSession or YahooSession::requireSession, an instance
 * of a NativeSessionStore is used.
 *
 * @brief Native php session based implementation of the session store, by default
 * stored on file system, but can be database or memcache backend.
 */
class NativeSessionStore {
    /**
     * Indicates if the session store has a request token.
     *
     * @return True if a request token is present, false otherwise.
     */
    function hasRequestToken() {
        return array_key_exists("yosdk_rt", $_SESSION) && (strlen($_SESSION["yosdk_rt"]) > 0);
    }

    /**
     * Indicates if the session store has an access token.
     *
     * @return True if an access token is present, false otherwise.
     */
    function hasAccessToken() {
        return array_key_exists("yosdk_at", $_SESSION) && (strlen($_SESSION["yosdk_at"]) > 0);
    }

    /**
     * Stores the given request token in the session store.
     *
     * @param $token A PHP stdclass object containing the components of the OAuth request token.
     */
    function storeRequestToken($token) {
				$_SESSION['yosdk_rt'] = json_encode($token);
    }

    /**
     * Fetches and returns the request token from the session store.
     *
     * @return The request token.
     */
    function fetchRequestToken() {
        return json_decode($_SESSION["yosdk_rt"]);
    }

    /**
     * Clears the request token from the session store.
     *
     */
    function clearRequestToken() {
				unset($_SESSION['yosdk_rt']);
    }

    /**
     * Stores the given access token in the session store.
     *
     * @param $token A PHP stdclass object containing the components of the OAuth access token.
     */
    function storeAccessToken($token) {
				$_SESSION['yosdk_at'] = json_encode($token);
    }

    /**
     * Fetches and returns the access token from the session store.
     *
     * @return The access token.
     */
    function fetchAccessToken() {
        return json_decode($_SESSION["yosdk_at"]);
    }

    /**
     * Clears the access token from the session store.
     *
     */
    function clearAccessToken() {
				unset($_SESSION['yosdk_at']);
    }
}

/**
 * A simple OAuth client class for making 2 and 3 legged OAuth HTTP requests.
 *
 * @brief A simple OAuth client class for making 2 and 3 legged OAuth HTTP requests.
 */
class OAuthClient {
    /**
     * @private
     */
    var $consumer = NULL;

    /**
     * @private
     */
    var $token = NULL;

    /**
     * @private
     */
    var $defaultTimeout = 3;

    /**
     * @private
     */
    var $oauthParamsLocation = NULL;

    /**
     * @private
     */
    var $signatureMethod = NULL;

		/**
		 * @private
		 */
    var $accepts = "application/json";

    /**
     * Constructs a new OAuth client.
     *
     * @param $consumer The OAuthConsumer object to use for the requests.
     * @param $token The OAuthToken to use for the requests. Optional.
     * @param $oauthParamsLocation OAUTH_PARAMS_IN_HEADERS or OAUTH_PARAMS_IN_POST_BODY, depending on where you want the OAuth parameters to show up. Optional, defaults to using the headers.
     * @param $signatureMethod OAUTH_SIGNATURE_PLAINTEXT or OAUTH_SIGNATURE_HMAC_SHA1, depending on what request signing mechanism to use. Optional, defaults to HMAC SHA1 signatures.
     */
    function OAuthClient($consumer, $token = NULL, $oauthParamsLocation = OAUTH_PARAMS_IN_HEADERS, $signatureMethod = OAUTH_SIGNATURE_HMAC_SHA1) {
        $this->consumer = $consumer;
        $this->token = $token;
        $this->oauthParamsLocation = $oauthParamsLocation;

        if($signatureMethod == OAUTH_SIGNATURE_HMAC_SHA1) {
            $this->signatureMethod = new OAuthSignatureMethod_HMAC_SHA1();
        }
        else if($signatureMethod == OAUTH_SIGNATURE_PLAINTEXT) {
            $this->signatureMethod = new OAuthSignatureMethod_PLAINTEXT();
        }
        else {
            YahooLogger::error("Invalid signature method: $signatureMethod");
        }
    }

    /**
     * Executes a properly signed OAuth HTTP GET request.
     *
     * @param $url The URL to request.
     * @param $queryParameters Any query string parameters to be sent in the request.
     * @param $timeout Optional, the number of seconds to wait for the request to return.
     * @return The response object.
     */
    function get($url, $queryParameters = array(), $timeout = NULL) {
        if(strpos($url, "?") !== FALSE) {
            YahooLogger::error("Put the query parameters in the second argument to OAuthClient::get(), not in the URL itself: URL = $url");
            return NULL;
        }

        return $this->request(array(
                "method" => "GET",
                "url" => $url,
                "query" => $queryParameters,
                "timeout" => $timeout));
    }

    /**
     * Executes a properly signed OAuth HTTP DELETE request.
     *
     * @param $url The URL to request.
     * @param $queryParameters Any query string parameters to be sent in the request.
     * @param $timeout Optional, the number of seconds to wait for the request to return.
     * @return The response object.
     */
    function delete($url, $queryParameters = array(), $timeout = NULL) {
        if(strpos($url, "?") !== FALSE) {
            YahooLogger::error("Put the query parameters in the second argument to OAuthClient::delete(), not in the URL itself: URL = $url");
            return NULL;
        }

        return $this->request(array(
                "method" => "DELETE",
                "url" => $url,
                "query" => $queryParameters,
                "timeout" => $timeout));
    }

    /**
     * Executes a properly signed OAuth HTTP PUT request.
     *
     * @param $url The URL to request.
     * @param $contentType The Content-Type of the PUT data.
     * @param $content The raw content to be PUT.
     * @param $timeout Optional, the number of seconds to wait for the request to return.
     * @return The response object.
     */
    function put($url, $contentType, $content, $timeout = NULL) {
        return $this->request(array(
                "method" => "PUT",
                "url" => $url,
                "content" => $content,
                "contentType" => $contentType,
                "timeout" => $timeout));
    }

    /**
     * Executes a properly signed OAuth HTTP POST request.
     *
     * @param $url The URL to request.
     * @param $contentType The Content-Type of the POST data.
     * @param $content The content to be POST.
     * @param $timeout Optional, the number of seconds to wait for the request to return.
     * @return The response object.
     */
    function post($url, $contentType = "application/x-www-form-urlencoded",
                $content = array(), $timeout = NULL) {
        return $this->request(array(
                "method" => "POST",
                "url" => $url,
                "content" => $content,
                "contentType" => $contentType,
                "timeout" => $timeout));
    }

    /**
     * @private
     */
    function request($request) {
        if(!array_key_exists("content", $request)) {
            $request["content"] = array();
        }
        if(!array_key_exists("query", $request)) {
            $request["query"] = array();
        }

        if(is_array($request["content"])) {
            $combinedParams = array_merge(
                    $request["query"], $request["content"]);
        }
        else {
            $combinedParams = $request["query"];
        }

        $oauthRequest = OAuthRequest::from_consumer_and_token(
                $this->consumer, $this->token, $request["method"],
                $request["url"], $combinedParams);
        $oauthRequest->sign_request($this->signatureMethod, $this->consumer,
                $this->token);

        $headers = array("Accept: " . $this->accepts);
        if($this->oauthParamsLocation == OAUTH_PARAMS_IN_HEADERS) {
            $headers[] = $oauthRequest->to_header();
        }
        if(!empty($request["content"]) || $this->oauthParamsLocation == OAUTH_PARAMS_IN_POST_BODY) {
            $headers[] = "Content-Type: " . $request["contentType"];
        }

        if(!empty($request["query"])) {
            $requestUrl = sprintf("%s?%s", $request["url"], oauth_http_build_query($request["query"]));
        }
        else {
            $requestUrl = $request["url"];
        }

        $requestTimeout = array_key_exists("timeout", $request) ?
        $request["timeout"] : $this->defaultTimeout;

        $ch = curl_init($requestUrl);
        curl_setopt($ch, CURLOPT_TIMEOUT, $requestTimeout);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
        curl_setopt($ch, CURLOPT_CUSTOMREQUEST, $request["method"]);
        if(($this->oauthParamsLocation == OAUTH_PARAMS_IN_POST_BODY) ||
                (!empty($request["content"]) && is_array($request["content"]))) {
            // Content is an array, URL encode it.
            if($this->oauthParamsLocation == OAUTH_PARAMS_IN_POST_BODY) {
                $request["content"] = $oauthRequest->to_postdata();
                curl_setopt($ch, CURLOPT_POSTFIELDS, $request["content"]);
            }
            else {
                curl_setopt($ch, CURLOPT_POSTFIELDS, oauth_http_build_query($request["content"]));
            }
        }
        else if(!empty($request["content"])) {
            // Content is raw.
            curl_setopt($ch, CURLOPT_POSTFIELDS, $request["content"]);
        }

        // Enable compressed responses from the servers.
        curl_setopt($ch, CURLOPT_ENCODING, "");

        // Set the user agent so the SDK properly identifies itself for
        // usage tracking purposes. Include the version of the SDK and
        // the version of PHP being used.
        $sdkVersion = "1.2";
        $agent = sprintf("YosPhpSdk/%s php/%s", $sdkVersion, phpversion());
        curl_setopt($ch, CURLOPT_USERAGENT, $agent);

        $headerParser = new YahooHeaderParser();
        curl_setopt($ch, CURLOPT_HEADERFUNCTION, array(&$headerParser, "read"));
        $response = curl_exec($ch);
        if(is_bool($response) && !$response) {
            YahooLogger::error("Error making libcurl request(" . $requestUrl . "): " . curl_error($ch));
            return NULL;
        }

        $response = array(
            'method' => $request["method"],
            'url' => $requestUrl,
            'code' => curl_getinfo($ch, CURLINFO_HTTP_CODE),
            'requestHeaders' => $headers,
            'requestBody' => !empty($request["content"]) ? $request["content"] : NULL,
            'responseHeaders' => $headerParser->headers,
            'responseBody' => $response
            );
        if($response["code"] != 200) {
            YahooLogger::error("HTTP request failed", $response);

            $this->checkExpired($response["code"], $headerParser);
            return NULL;
        }
        YahooLogger::debug("HTTP request details", $response);

        return $response;
    }

    /**
     * Checks to see if the code and headers indicate an expired OAuth token.
     * If so, requests a new one.
     *
     * @private
     */
    function checkExpired($code, $headerParser) {
        if ($code != 401) return; // HTTP Unauthorized
        $authenticateHeader = $headerParser->get('WWW-Authenticate');
        if (!$authenticateHeader) return;
        if (!preg_match('/oauth_problem="([^"]+)"/', $authenticateHeader, $match)) return;
        $oauth_problem = $match[1];
        if ($oauth_problem == 'token_expired') {
            YahooLogger::error('Access token expired. Please fetch a new one');
        }
        if ($oauth_problem == 'consumer_key_unknown') {
            YahooLogger::error('Consumer Key unkown.  Please check that the Consumer Key is valid.');
        }
        if ($oauth_problem == 'additional_authorization_required') {
            YahooLogger::error('The app identified by this Consumer Key is not authorized to access this resource.  Authorization is defined under Access Scopes on the application\'s settings page.');
        }
    }
}

/**
 * @private
 */
class YahooHeaderParser {
    var $headers = array();

    function YahooHeaderParser() {
    }

    function read($ch, $header) {
        $pos = strpos($header, ":");
        if($pos !== FALSE) {
            $name = substr($header, 0, $pos);
            $value = trim(substr($header, $pos + 1));
            $this->headers[$name] = $value;
        }
        return strlen($header);
    }

    function get($name) {
        if(array_key_exists($name, $this->headers)) {
            return $this->headers[$name];
        }
        else {
            return NULL;
        }
    }
}

/**
 * Interface to modify the underlying configuration of the library.
 */
class YahooConfig {
    function setSocialWsHostname($hostname) {
        global $YahooConfig;
        $YahooConfig["SOCIAL_WS_HOSTNAME"] = $hostname;
    }

    function setPresenceWsHostname($hostname) {
        global $YahooConfig;
        $YahooConfig["PRESENCE_WS_HOSTNAME"] = $hostname;
    }

    function setUpdatesWsHostname($hostname) {
        global $YahooConfig;
        $YahooConfig["UPDATES_WS_HOSTNAME"] = $hostname;
    }

    function setQueryWsHostname($hostname) {
        global $YahooConfig;
        $YahooConfig["QUERY_WS_HOSTNAME"] = $hostname;
    }

    function setOauthHostname($hostname) {
        global $YahooConfig;
        $YahooConfig["OAUTH_HOSTNAME"] = $hostname;
    }

    function setYapWsHostname($hostname) {
        global $YahooConfig;
        $YahooConfig["YAP_WS_HOSTNAME"] = $hostname;
    }
}

/**
 * An OAuth compatible version of http_build_query. http_build_query
 * doesn't work because it turns spaces into "+", which isn't allowed
 * by OAuth.
 */
function oauth_http_build_query($parameters) {
    $strings = array();
    foreach($parameters as $name => $value) {
        $strings[] = sprintf("%s=%s", rawurlencode($name), rawurlencode($value));
    }
    $query = implode("&", $strings);
    return $query;
}

/**
 * PHP4/5 compatibility functions
 */
if(!function_exists("property_exists")) {
    function property_exists( $class, $property ) {
        if ( is_object( $class ) ) {
            $vars = get_object_vars( $class );
        } else {
            $vars = get_class_vars( $class );
        }
        return array_key_exists( $property, $vars );
    }
}

// If json_decode doesn't exist, then php-json must not be included in this
// version of PHP. Include fake versions of json_encode/json_decode that
// are backed by the native PHP php-json library, which is available in PEAR.
if(!function_exists("json_decode")) {
    // Only include JSON.php if someone else hasn't already. Depending on
    // the operating environment, other code may have brought their own
    // version of that source code.
    if(!class_exists("Services_JSON")) {
        include_once("JSON.php");
    }

    function json_decode($json) {
        $js = new Services_JSON();
        return $js->decode($json);
    }

    function json_encode($value) {
        $js = new Services_JSON();
        return $js->encode($value);
    }
}
