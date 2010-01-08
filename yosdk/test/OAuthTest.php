<?php

if (!defined('PHPUnit_MAIN_METHOD')) {
    define('PHPUnit_MAIN_METHOD', 'AllTests::main');
}

require_once 'PHPUnit/Framework.php';
require_once 'PHPUnit/TextUI/TestRunner.php';

require_once dirname(__FILE__).'/unit/oauth/OAuthConsumerTest.php';
require_once dirname(__FILE__).'/unit/oauth/OAuthRequestTest.php';
require_once dirname(__FILE__).'/unit/oauth/OAuthSignatureMethodHmacSha1Test.php';
require_once dirname(__FILE__).'/unit/oauth/OAuthSignatureMethodRsaSha1Test.php';
require_once dirname(__FILE__).'/unit/oauth/OAuthTokenTest.php';
require_once dirname(__FILE__).'/unit/oauth/OAuthUtilTest.php';

class AllTests
{

    public static function main()
    {
        PHPUnit_TextUI_TestRunner::run(self::suite());
    }

    public static function suite()
    {
        $suite = new PHPUnit_Framework_TestSuite();

        $suite->addTestSuite('OAuthConsumerTest');
        $suite->addTestSuite('OAuthRequestTest');
        $suite->addTestSuite('OAuthSignatureMethodHmacSha1Test');
        $suite->addTestSuite('OAuthSignatureMethodRsaSha1Test');
        $suite->addTestSuite('OAuthTokenTest');
        $suite->addTestSuite('OAuthUtilTest');

        return $suite;
    }
}

if (PHPUnit_MAIN_METHOD == 'AllTests::main') {
    AllTests::main();
}
