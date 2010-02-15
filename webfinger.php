<?php
/**
 * This is an implementation of the WebFinger protocol in PHP.
 *
 * The initial version is heavily based on DeWitt Clinton's Python library:
 * http://code.google.com/p/webfingerclient-dclinton
 * and Brad Fitzpatrick's walkthrough of the Gmail implementation of the protocol:
 * https://groups.google.com/group/webfinger/browse_thread/thread/fb56537a0ed36964?pli=1
 *
 * Licensed under the 2-clause (eg no advertising requirement) BSD license,
 * making it easy to reuse for commercial or GPL projects:
 
 (c) Pete Warden <pete@petewarden.com> http://petewarden.typepad.com/ Feb 14th 2010
 
 Redistribution and use in source and binary forms, with or without modification, are
 permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this 
      list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this 
      list of conditions and the following disclaimer in the documentation and/or 
      other materials provided with the distribution.
   3. The name of the author may not be used to endorse or promote products derived 
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.

 */

require_once ('./http.php');
require_once ('./utility.php');
require_once ('./hkit/hkit.class.php');

// These two strings are placed around the domain name extracted from the email 
//address, and then used to fetch the description of supported services
define ('WEBFINGER_ENDPOINT_PREFIX', 'http://');
define ('WEBFINGER_ENDPOINT_SUFFIX', '/.well-known/host-meta');

// The name of the rel= value to look for to recognize a WebFinger service
define ('WEBFINGER_SERVICE_REL_TYPE', 'lrdd');

// The URL of the hCard specification
define ('HCARD_REL_URL', 'http://microformats.org/profile/hcard');

/**
 * Searches for user information using webfinger, given an email address.
 *
 * @param string $email The email address of the user
 * @return array : of user info arrays, containing service_name, display_name,
 * user_name, user_id, portrait_url and location for any found users. If no users 
 * were found, the result will be an empty array, and any entry may also be an
 * empty string
 */
function webfinger_find_by_email($email) {

    // Obtain the URL to gather user information from
    $query_url = webfinger_query_url_for_email($email);
    if (empty($query_url)) {
        return null;
    }

    // Try to grab the information help on this email address
	$query_result = http_request($query_url);
	if ( !did_http_succeed($query_result) ) {
		return null;
	}

    $query_xml = $query_result['body'];
    $query_data = convert_xml_string_to_array($query_xml);
    
    if (!isset($query_data['xrd']['link'])) {
        return null;
    }

    $query_links = $query_data['xrd']['link'];
    
    // If there was a single link tag, we'll get its contents in $query_links, but
    // if multiple links were present, the contents will be an numeric array of them all.
    // To normalize this, put lone tags into their own array.
    if (!isset($query_links[0])) {
        $query_links_list = array($query_links);
    } else {
        $query_links_list = $query_links;
    }

    // Go through all the links and extract the information from any we recognize
    $result = array();
    foreach ($query_links_list as $link)
    {
        if (!isset($link['@attributes']['rel']) ||
            !isset($link['@attributes']['href'])) {
            continue;
        }
        
        $rel = $link['@attributes']['rel'];
        $href = $link['@attributes']['href'];
        
        // At the moment this only understands hCard formatted pages
        if ($rel===HCARD_REL_URL) {
        
            $hkit = new hKit;

            $hcard_result = $hkit->getByURL('hcard', $href);
            if (empty($hcard_result) ||
                !isset($hcard_result[0])) {
                continue;
            }
            
            $hcard_info = $hcard_result[0];
            
            $user_id = $href;
            $user_name = '';

            if (isset($hcard_info['fn'])) {
                $display_name = $hcard_info['fn'];
            } else {
                $display_name = '';
            }

            if (isset($hcard_info['photo'])) {
                $portrait_url = $hcard_info['photo'];
            } else {
                $portrait_url = '';
            }

            if (isset($hcard_info['adr'])) {
                $location = $hcard_info['adr'];
            } else {
                $location = '';
            }

            $result = array(
                'webfinger' => array(
                    'user_id' => $user_id,
                    'user_name' => $user_name,
                    'display_name' => $display_name,
                    'portrait_url' => $portrait_url,
                    'location' => $location,
                ),
            );

        }
    }
    
    if (empty($result)) {
        return null;
    }
    
    return $result;
}

/**
 * Queries the domain for this email address to see if it supports WebFinger, and
 * if it does returns the right URL to call to get information on the address
 *
 * @param string $email The email address of the user
 * @return string The URL to call to get information, or null if none found
 */
function webfinger_query_url_for_email($email) {

    $domain = get_domain_from_email($email);
    if (empty($domain))
        return null;
    
    // First, ask the server for a list of the services it supports, so we can
    // look through that list for WebFinger
    $endpoint_url = WEBFINGER_ENDPOINT_PREFIX;
    $endpoint_url .= $domain;
    $endpoint_url .= WEBFINGER_ENDPOINT_SUFFIX;
    
	$endpoint_result = http_request($endpoint_url);
	if ( !did_http_succeed($endpoint_result) ) {
		return null;
	}
    
    $endpoint_xml = $endpoint_result['body'];
    $endpoint_data = convert_xml_string_to_array($endpoint_xml);
    
    if (!isset($endpoint_data['xrd']['link'])) {
        return null;
    }
    
    $endpoint_links = $endpoint_data['xrd']['link'];
    
    // If there was a single link tag, we'll get its contents in $endpoint_links, but
    // if multiple links were present, the contents will be an numeric array of them all.
    // To normalize this, put lone tags into their own array.
    if (!isset($endpoint_links[0])) {
        $endpoint_links_list = array($endpoint_links);
    } else {
        $endpoint_links_list = $endpoint_links;
    }
    
    // Now search for a link with the right service rel tag, and get the URL template
    $template = null;
    foreach ($endpoint_links_list as $link)
    {
        if (!isset($link['@attributes']['rel'])) {
            continue;
        }
    
        $rel = $link['@attributes']['rel'];
        if ($rel !== WEBFINGER_SERVICE_REL_TYPE) {
            continue;
        }
            
        if (!isset($link['@attributes']['template'])) {
            continue;
        }
            
        $template = $link['@attributes']['template'];
    }
    
    if (empty($template)) {
        return null;
    }
    
    if (!strpos($template, '{uri}')) {
        return null;
    }
    
    // Finally substitute the actual email address into the generic template
    $result = str_replace('{uri}', urlencode('acct://'.$email), $template);
    
    return $result;
}

/**
 * A utility function to extract a domain name from an email address
 */
function get_domain_from_email($email) {

    $emailparts = explode('@', $email);

    if (!isset($emailparts[1]))
        return null;
    
    return $emailparts[1];
}

?>
