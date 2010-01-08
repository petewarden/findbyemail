<?php

/**
 *  YOS PHP SDK for accessing social and data apis at Yahoo!
 *
 *  YahooSessionStore provides an interface for implementing session storage backends.
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


/**
 * The session store interface. Developers are free to implement their
 * own session store implementations and pass them to YahooSession::hasSession,
 * YahooSession::requireSession and YahooSession::clearSession.
 *
 * @brief The session store interface.
 */
interface YahooSessionStore {
    /**
     * Indicates if the session store has a request token.
     *
     * @return True if a request token is present, false otherwise.
     */
    function hasRequestToken(){}

    /**
     * Indicates if the session store has an access token.
     *
     * @return True if an access token is present, false otherwise.
     */
    function hasAccessToken(){}

    /**
     * Stores the given request token in the session store.
     *
     * @param $token A PHP stdclass object containing the components of
     *               the OAuth request token.
     * @return True on success, false otherwise.
     */
    function storeRequestToken($token){}

    /**
     * Fetches and returns the request token from the session store.
     *
     * @return The request token.
     */
    function fetchRequestToken(){}

    /**
     * Clears the request token from the session store.
     *
     * @return True on success, false otherwise.
     */
    function clearRequestToken(){}

    /**
     * Stores the given access token in the session store.
     *
     * @param $token A PHP stdclass object containing the components of
     *               the OAuth access token.
     * @return True on success, false otherwise.
     */
    function storeAccessToken($token){}

    /**
     * Fetches and returns the access token from the session store.
     *
     * @return The access token.
     */
    function fetchAccessToken(){}

    /**
     * Clears the access token from the session store.
     *
     * @return True on success, false otherwise.
     */
    function clearAccessToken(){}
}
