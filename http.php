<?php
/**
 * A simple API for requesting data from urls. Requires CURL to be installed.
 *
 * There's two functions, http_request($url, $justhead) and http_is_error($response)
 *
 * http_request($url, $justhead) returns a result code and content body for a URL.
 * If $justhead is true, it doesn't fetch the body, just checks for the existence of
 * the URL.
 *
 * http_is_error($response) is a helper function that indicates whether the result
 * of a previous request was an error.
 */

function http_request($url, $justhead=false) {
    $ch = curl_init();

    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true );
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true );
    if ($justhead) {
        curl_setopt($ch, CURLOPT_NOBODY, true );
    }
    
    $response = curl_exec($ch);

    if (!isset($response)) {
        $error = curl_error($ch);
        return array(
            'code' => -1,
            'message' => $error,
        );
    }

    $result = array(
        'code' => curl_getinfo($ch, CURLINFO_HTTP_CODE),
        'message' => '',
        'body' => $response,
    );

    curl_close($ch);

    error_log($url);
    error_log(print_r($result, true));

    return $result;
}

function http_is_error($request)
{
    if (empty($request['code']) || ($request['code']===-1))
        return true;
    else
        return false;
}

?>
