<?php
/**
 * A collection of helper functions
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

/**
 * A utility function for a common case of XML parsing. It returns the value for
 * the first occurrence of the given tag name. It requires the parsing arrays
 * returned from xml_parse_into_struct()
 *
 * @since Unknown
 *
 * @param string $tagname The name of the tag to search for
 * @param array $index The indices of all the tags parsed into an array by xml_parse_into_struct
 * @param array $vals The values of all the tags parsed into an array by xml_parse_into_struct
 * @param string $default If the tag isn't found, this value is returned instead. Defaults to an empty string
 * @return string The value of the first tag, or the default 
 */
function get_first_xml_value($tagname, $index, $vals, $default = '')
{
	$upper_tagname = strtoupper($tagname);
	if ( !isset($index[$upper_tagname]) || !isset($index[$upper_tagname][0]) )
		return $default;
	
	$tag_index = $index[$upper_tagname][0];
	if ( isset($vals[$tag_index]['value']) ) {
		$value = $vals[$tag_index]['value'];
	} else {
		$value = $default;
	}
	
	return $value;
}

/**
 * A utility function for a common case of XML parsing. It returns the value for
 * the given attribute of the first occurrence of the given tag name. It requires 
 * the parsing arrays returned from xml_parse_into_struct()
 *
 * @since Unknown
 *
 * @param string $tagname The name of the tag to search for
 * @param string $attribute_name The name of the attribute within the tag to search for
 * @param array $index The indices of all the tags parsed into an array by xml_parse_into_struct
 * @param array $vals The values of all the tags parsed into an array by xml_parse_into_struct
 * @param string $default If the tag isn't found, this value is returned instead. Defaults to an empty string
 * @return string The value of the first tag, or the default 
 */
function get_first_xml_attribute($tagname, $attribute_name, $index, $vals, $default = '')
{
	$upper_tagname = strtoupper($tagname);
	if ( !isset($index[$upper_tagname]) || !isset($index[$upper_tagname][0]) )
		return $default;
	
	$tag_index = $index[$upper_tagname][0];
	if ( !isset($vals[$tag_index]['attributes']) )
		return $default;

	$attributes = $vals[$tag_index]['attributes'];
	
	$upper_attribute_name = strtoupper($attribute_name);
	if ( !isset($attributes[$upper_attribute_name]) )
		return $default;

	$attribute_value = $attributes[$upper_attribute_name];
	
	return $attribute_value;
}

/**
 * A utility function to make sure that the API call succeeded
 *
 * @since Unknown
 *
 * @param object $http_result The result from the Http request call
 * @return boolean Whether the call succeeded 
 */
function did_http_succeed($http_result)
{
	if ( http_is_error($http_result) )
		return false;

	$user_info_code = $http_result['code'];
	if ( $user_info_code!=200 ) {
		$user_info_message = $http_result['message'];
		error_log("API call failed with code $user_info_code and message '$user_info_message'");
		error_log(print_r($http_result, true));
		return false;
	}
	
	return true;
}

/**
 * A utility function to sign an Amazon REST API call
 * See http://www.a2sdeveloper.com/page-rest-authentication-for-php4.html
 *
 * @since Unknown
 *
 * @param string $input_url The original URL for the API call
 * @return string The input URL with the timestamp and signature parameters added 
 */
function get_signed_amazon_api_url($input_url)
{
	$url_parts = parse_url($input_url);

	$url_query = $url_parts['query'];
	
	$parameters = array();
	parse_str($url_query, $parameters);
	$parameters['Timestamp'] = gmdate("Y-m-d\TH:i:s\Z"); 
	$parameters['Version'] = '2009-03-01';
	$parameters['AWSAccessKeyId'] = AMAZON_API_KEY_PUBLIC;

	ksort($parameters);

	$encoded_parameters = array(); 
	foreach ($parameters as $parameter=>$value) { 
		$parameter = str_replace('_', '.', $parameter); 
		$parameter = str_replace('%7E', '~', rawurlencode($parameter)); 
		$value = str_replace('%7E', '~', rawurlencode($value)); 
		$encoded_parameters[] = $parameter . '=' . $value; 
	} 
	$encoded_string = implode('&', $encoded_parameters);

	$signature_string = 'GET' . chr(10) . $url_parts['host'] . chr(10) . $url_parts['path'] . chr(10) . $encoded_string;

	$signature = urlencode(base64_encode(hash_hmac("sha256", $signature_string, AMAZON_API_KEY_SECRET, True)));
 
	$result = 'http://' . $url_parts['host'] . $url_parts['path'] . '?' . $encoded_string . '&Signature=' . $signature; 

	return $result;
}

/**
 * This is a utility function that takes an XML string and returns the contents
 * as a native PHP array, to make it easier to work with.
 * The main work is done by the recursive function convert_xml_element_to_array()
 *
 * @param string $xml_string The contents of an XML file
 * @return array All the tags in the XML converted into a nested associative array
 */
function convert_xml_string_to_array($xml_string) {

	$xml_root_element = simplexml_load_string($xml_string);
	
	$result = convert_xml_element_to_array($xml_root_element);
	
	return $result;
}

/**
 * This is a utility function that takes a simpleXMLElement object and returns the contents
 * as a nested associative PHP array, to make it easier to work with. It's called recursively
 * to deal with an entire document. Adapted from
 * http://www.ibm.com/developerworks/xml/library/x-xml2jsonphp/
 *
 * @param string $xml_element A single element, possibly containing children
 * @param int $recursion_depth The depth of the call stack
 * @return array All the tags in the XML element converted into an associative array
 */
function convert_xml_element_to_array($xml_element, &$recursion_depth=0) { 

	// If we're getting too deep, bail out
	if ($recursion_depth > 512) {
		return(null);
	}
	
	if (!is_string($xml_element) && 
        !is_array($xml_element) &&
        (get_class($xml_element) == 'SimpleXMLElement')) {
		$xml_element_copy = $xml_element;
		$xml_element = get_object_vars($xml_element);
	}

	if (is_array($xml_element)) {

		$result_array = array();
		if (count($xml_element) <= 0) {
			return (trim(strval($xml_element_copy)));
		}

		foreach($xml_element as $key=>$value) {

			$recursion_depth++; 
			$result_array[strtolower($key)] = 
                convert_xml_element_to_array($value, $recursion_depth);
			$recursion_depth--;
		}

		if ($recursion_depth == 0) {
			$temp_array = $result_array;
			$result_array = array(
				strtolower($xml_element_copy->getName()) => $temp_array,
			);
		}

		return ($result_array);

	} else {
		return (trim(strval($xml_element)));
	}
}

/**
 * This is a utility function to check whether a URL is reachable
 *
 * @param string $url The address to check
 * @return boolean Whether the URL could be reached
 */
function does_url_exist($url) {
				
	$url_result = http_request($url, true);
	
	if ( http_is_error($url_result) )
		return false;

	$status_code = $url_result['code'];
	return ( $status_code==200 );
}

?>
