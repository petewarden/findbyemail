<?php

require_once dirname(__FILE__).'/../../../lib/OAuth.php';
require_once 'PHPUnit/Framework.php';

class OAuthConsumerTest extends PHPUnit_Framework_TestCase {
	public function testConvertToString() {
		$consumer = new OAuthConsumer('key', 'secret');
		$this->assertEquals('OAuthConsumer[key=key,secret=secret]', (string) $consumer);
	}
}