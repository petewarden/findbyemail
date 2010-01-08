<?php

require_once dirname(__FILE__).'/../../../lib/OAuth.php';
require_once 'PHPUnit/Framework.php';

/**
 * Tests of OAuthRequest
 *
 * The tests works by using OAuthTestUtils::build_request
 * to populare $_SERVER, $_GET & $_POST.
 *
 * Most of the base string and signature tests
 * are either very simple or based upon
 * http://wiki.oauth.net/TestCases
 *
 *
 */

/**
 * A simple utils class for methods needed
 * during some of the tests
 */
class OAuthTestUtils
{

  public static function reset_request_vars()
  {
    $_SERVER = array();
    $_POST = array();
    $_GET = array();
  }

  /**
   * Populates $_{SERVER,GET,POST}
   *
   * TODO: Should query-string params always be added to $_GET.. prolly..
   *
   * @param string $method GET or POST
   * @param string $uri What URI is the request to (eg http://example.com/foo?bar=baz)
   * @param array $params What params should go with the request
   * @param string $auth_header What to set the Authorization header to
   */
  public static function build_request($method, $uri, $params, $auth_header = '')
  {
    self::reset_request_vars();

    $method = strtoupper($method);

    $parts = parse_url($uri);

    $port = @$parts['port'];
    $scheme = $parts['scheme'];
    $host = $parts['host'];
    $path = @$parts['path'];
    $query = @$parts['query'];

    $port or $port = ($scheme == 'https') ? '443' : '80';

    if ($scheme == 'https')
    {
      $_SERVER['HTTPS'] = 'on';
    }

    $_SERVER['REQUEST_METHOD'] = $method;
    $_SERVER['HTTP_HOST'] = $host;
    $_SERVER['SERVER_PORT'] = $port;
    $_SERVER['REQUEST_URI'] = $path . '?' . $query;

    if ($method == 'POST')
    {
      $_SERVER['HTTP_CONTENT_TYPE'] = 'application/x-www-form-urlencoded';
      $_POST = $params;
    }
    else
    {
      $_GET = $params;
    }

    if ($auth_header != '')
    {
      $_SERVER['HTTP_AUTHORIZATION'] = $auth_header;
    }
  }
}

class OAuthRequestTest extends PHPUnit_Framework_TestCase
{

  public function setUp()
  {
    OAuthTestUtils::reset_request_vars();
  }

  public function tearDown()
  {
    OAuthTestUtils::reset_request_vars();
  }

  public function testFromRequestPost()
  {
    OAuthTestUtils::build_request('POST', 'http://testbed/test', array('foo' => 'bar', 'baz' => 'blargh'));
    $r = OAuthRequest::from_request();

    $this->assertEquals('POST', $r->get_normalized_http_method());
    $this->assertEquals('http://testbed/test', $r->get_normalized_http_url());
    $this->assertEquals(array('foo' => 'bar', 'baz' => 'blargh'), $r->get_parameters());

  }

  public function testFromRequestPostGet()
  {
    OAuthTestUtils::build_request('GET', 'http://testbed/test', array('foo' => 'bar', 'baz' => 'blargh'));
    $r = OAuthRequest::from_request();

    $this->assertEquals('GET', $r->get_normalized_http_method());
    $this->assertEquals('http://testbed/test', $r->get_normalized_http_url());
    $this->assertEquals(array('foo' => 'bar', 'baz' => 'blargh'), $r->get_parameters());
  }

  public function testFromRequestHeader()
  {
    $test_header = 'OAuth realm="",oauth_foo=bar,oauth_baz="bla,rgh"';
    OAuthTestUtils::build_request('POST', 'http://testbed/test', array(), $test_header);

    $r = OAuthRequest::from_request();
    $this->assertEquals('POST', $r->get_normalized_http_method());
    $this->assertEquals('http://testbed/test', $r->get_normalized_http_url());
    $this->assertEquals(array('oauth_foo' => 'bar', 'oauth_baz' => 'bla,rgh'), $r->get_parameters(), 'Failed to split auth-header correctly');
  }

  public function testNormalizeParameters()
  {
    OAuthTestUtils::build_request('POST', 'http://testbed/test', array('name' => ''));
    $r = OAuthRequest::from_request();
    $this->assertEquals('name=', $r->get_signable_parameters());

    OAuthTestUtils::build_request('POST', 'http://testbed/test', array('a' => 'b'));
    $r = OAuthRequest::from_request();
    $this->assertEquals('a=b', $r->get_signable_parameters());

    OAuthTestUtils::build_request('POST', 'http://testbed/test', array('a' => 'b', 'c' => 'd'));
    $r = OAuthRequest::from_request();
    $this->assertEquals('a=b&c=d', $r->get_signable_parameters());

    OAuthTestUtils::build_request('POST', 'http://testbed/test', array('a' => array('x!y', 'x y')));
    $r = OAuthRequest::from_request();
    $this->assertEquals('a=x%20y&a=x%21y', $r->get_signable_parameters());

    OAuthTestUtils::build_request('POST', 'http://testbed/test', array('x!y' => 'a', 'x' => 'a'));
    $r = OAuthRequest::from_request();
    $this->assertEquals('x=a&x%21y=a', $r->get_signable_parameters());

    OAuthTestUtils::build_request('POST', 'http://testbed/test', array('a' => 1, 'c' => 'hi there', 'f' => array(25, 50, 'a'), 'z' => array('p', 't')));
    $r = OAuthRequest::from_request();
    $this->assertEquals('a=1&c=hi%20there&f=25&f=50&f=a&z=p&z=t', $r->get_signable_parameters());
  }

  public function testNormalizeHttpUrl()
  {
    OAuthTestUtils::build_request('POST', 'http://example.com', array());
    $r = OAuthRequest::from_request();
    $this->assertEquals('http://example.com', $r->get_normalized_http_url());

    OAuthTestUtils::build_request('POST', 'https://example.com', array());
    $r = OAuthRequest::from_request();
    $this->assertEquals('https://example.com', $r->get_normalized_http_url());

    // Tests that http on !80 and https on !443 keeps the port
    OAuthTestUtils::build_request('POST', 'https://example.com:80', array());
    $r = OAuthRequest::from_request();
    $this->assertEquals('https://example.com:80', $r->get_normalized_http_url());

    OAuthTestUtils::build_request('POST', 'http://example.com:443', array());
    $r = OAuthRequest::from_request();
    $this->assertEquals('http://example.com:443', $r->get_normalized_http_url());
  }

  public function testGetBaseString()
  {
    OAuthTestUtils::build_request('POST', 'http://testbed/test', array('n' => 'v'));
    $r = OAuthRequest::from_request();
    $this->assertEquals('POST&http%3A%2F%2Ftestbed%2Ftest&n%3Dv', $r->get_signature_base_string());

    OAuthTestUtils::build_request('GET', 'http://example.com', array('n' => 'v'));
    $r = OAuthRequest::from_request();

    $this->assertEquals('GET&http%3A%2F%2Fexample.com&n%3Dv', $r->get_signature_base_string());

    $params = array('oauth_version' => '1.0', 'oauth_consumer_key' => 'dpf43f3p2l4k3l03', 'oauth_timestamp' => '1191242090', 'oauth_nonce' => 'hsu94j3884jdopsl', 'oauth_signature_method' => 'PLAINTEXT', 'oauth_signature' => 'ignored');
    OAuthTestUtils::build_request('POST', 'https://photos.example.net/request_token', $params);
    $r = OAuthRequest::from_request();

    $this->assertEquals('POST&https%3A%2F%2Fphotos.example.net%2Frequest_token&oauth_' . 'consumer_key%3Ddpf43f3p2l4k3l03%26oauth_nonce%3Dhsu94j3884j' . 'dopsl%26oauth_signature_method%3DPLAINTEXT%26oauth_timestam' . 'p%3D1191242090%26oauth_version%3D1.0', $r->get_signature_base_string());

    $params = array('file' => 'vacation.jpg', 'size' => 'original', 'oauth_version' => '1.0', 'oauth_consumer_key' => 'dpf43f3p2l4k3l03', 'oauth_token' => 'nnch734d00sl2jdk', 'oauth_timestamp' => '1191242096', 'oauth_nonce' => 'kllo9940pd9333jh', 'oauth_signature' => 'ignored', 'oauth_signature_method' => 'HMAC-SHA1');
    OAuthTestUtils::build_request('GET', 'http://photos.example.net/photos', $params);
    $r = OAuthRequest::from_request();

    $this->assertEquals('GET&http%3A%2F%2Fphotos.example.net%2Fphotos&file%3Dvacation' . '.jpg%26oauth_consumer_key%3Ddpf43f3p2l4k3l03%26oauth_nonce%' . '3Dkllo9940pd9333jh%26oauth_signature_method%3DHMAC-SHA1%26o' . 'auth_timestamp%3D1191242096%26oauth_token%3Dnnch734d00sl2jd' . 'k%26oauth_version%3D1.0%26size%3Doriginal', $r->get_signature_base_string());
  }

  // We only test two entries here. This is just to test that the correct
  // signature method is chosen. Generation of the signatures is tested
  // elsewhere, and so is the base-string the signature build upon.
  public function testBuildSignature()
  {
    $params = array('file' => 'vacation.jpg', 'size' => 'original', 'oauth_version' => '1.0', 'oauth_consumer_key' => 'dpf43f3p2l4k3l03', 'oauth_token' => 'nnch734d00sl2jdk', 'oauth_timestamp' => '1191242096', 'oauth_nonce' => 'kllo9940pd9333jh', 'oauth_signature' => 'ignored', 'oauth_signature_method' => 'HMAC-SHA1');
    OAuthTestUtils::build_request('GET', 'http://photos.example.net/photos', $params);
    $r = OAuthRequest::from_request();

    $cons = new OAuthConsumer('key', 'kd94hf93k423kf44');
    $token = new OAuthToken('token', 'pfkkdhi9sl3r4s00');
    $hmac = new OAuthSignatureMethod_HMAC_SHA1();
    $plaintext = new OAuthSignatureMethod_PLAINTEXT();

    $this->assertEquals('tR3+Ty81lMeYAr/Fid0kMTYa/WM=', $r->build_signature($hmac, $cons, $token));
    $this->assertEquals('kd94hf93k423kf44%26pfkkdhi9sl3r4s00', $r->build_signature($plaintext, $cons, $token));
  }

  public function testSign()
  {
    $params = array('file' => 'vacation.jpg', 'size' => 'original', 'oauth_version' => '1.0', 'oauth_consumer_key' => 'dpf43f3p2l4k3l03', 'oauth_token' => 'nnch734d00sl2jdk', 'oauth_timestamp' => '1191242096', 'oauth_nonce' => 'kllo9940pd9333jh', 'oauth_signature' => 'ignored', 'oauth_signature_method' => 'HMAC-SHA1');
    OAuthTestUtils::build_request('GET', 'http://photos.example.net/photos', $params);
    $r = OAuthRequest::from_request();

    $cons = new OAuthConsumer('key', 'kd94hf93k423kf44');
    $token = new OAuthToken('token', 'pfkkdhi9sl3r4s00');
    $hmac = new OAuthSignatureMethod_HMAC_SHA1();
    $plaintext = new OAuthSignatureMethod_PLAINTEXT();

    $r->sign_request($hmac, $cons, $token);

    $params = $r->get_parameters();
    $this->assertEquals('HMAC-SHA1', $params['oauth_signature_method']);
    $this->assertEquals('tR3+Ty81lMeYAr/Fid0kMTYa/WM=', $params['oauth_signature']);

    $r->sign_request($plaintext, $cons, $token);

    $params = $r->get_parameters();
    $this->assertEquals('PLAINTEXT', $params['oauth_signature_method']);
    $this->assertEquals('kd94hf93k423kf44%26pfkkdhi9sl3r4s00', $params['oauth_signature']);
  }

  public function testToPostdata()
  {
    OAuthTestUtils::build_request('POST', 'http://testbed/test', array('foo' => 'bar', 'baz' => 'blargh'));
    $r = OAuthRequest::from_request();

    $this->assertEquals('baz=blargh&foo=bar', $r->to_postdata());

    OAuthTestUtils::build_request('POST', 'http://testbed/test', array('foo' => array('bar', 'tiki'), 'baz' => 'blargh'));
    $r = OAuthRequest::from_request();

    $this->assertEquals('baz=blargh&foo=bar&foo=tiki', $r->to_postdata());
  }
}
