<?php
/**
 * This file contains the API keys needed to access the services that
 * findbyemail pulls its information from.
 *
 * Instructions on obtaining those keys are included inline.
 *
 */

// Obtain Flickr keys at http://www.flickr.com/services/apps/create/apply
define ('FLICKR_API_KEY_PUBLIC', '');
define ('FLICKR_API_KEY_SECRET', '');

// Sign up for a Yahoo key at https://developer.apps.yahoo.com/wsregapp/
define ('YAHOO_API_KEY_APP', '');
define ('YAHOO_API_KEY_SHARED', '');
define ('YAHOO_API_KEY_APP_ID', '');

// Get your 43things API key at http://www.43things.com/account/webservice_setup
define ('FORTYTHREETHINGS_API_KEY', '');

// Visit http://developer.zoominfo.com/ to sign up and get a zoominfo key
define ('ZOOMINFO_API_KEY', '');

// Sign up to the Vimeo advanced API program at http://www.vimeo.com/api/applications/new
define ('VIMEO_API_KEY_PUBLIC', '');
define ('VIMEO_API_KEY_SECRET', '');

// You need an account set up for Amazon's Product Advertising API. There's
// country-specific sign-up pages, but for the US you can go to
// https://affiliate-program.amazon.com/gp/flex/advertising/api/sign-in.html
define ('AMAZON_API_KEY_PUBLIC', '');
define ('AMAZON_API_KEY_SECRET', '');

// Create an account at DandyID and check the 'I want an API key' box:
// http://www.dandyid.org/beta/signup
define ('DANDYID_API_KEY_PUBLIC', '');

// Go to http://www.rapleaf.com/developer and click "Request API access"
define ('RAPLEAF_API_KEY', '');

// Get a key from http://developer.aim.com/manageKeys.jsp
define ('AIM_API_KEY', '');

// Email jon at intensedebate.com to request an API key for their API
define ('INTENSEDEBATE_API_KEY', '');

// Register at http://developer.jigsaw.com/member/register for the keys
define ('JIGSAW_API_KEY', '');

if (FLICKR_API_KEY_PUBLIC==='')
    die('You need to edit config.php to add your own API keys before you can use this module');

?>
