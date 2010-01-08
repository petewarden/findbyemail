<?php

/**
 * Using YQL Open Tables to create Yahoo! Open Application - Delicious
 *
 * http://developer.yahoo.com/devtool
 * http://developer.yahoo.com/yql/console
 */

// Include the YOS library.
require dirname(__FILE__).'/../lib/Yahoo.inc';

// debug settings
error_reporting(E_ALL | E_NOTICE); # do not show notices as library is php4 compatable
ini_set('display_errors', true);
YahooLogger::setDebug(true);
YahooLogger::setDebugDestination('LOG');

// use memcache to store oauth credentials via php native sessions
ini_set('session.save_handler', 'memcache');
session_save_path('tcp://localhost:11211?persistent=1&weight=1&timeout=1&retry_interval=15');
session_start();

// Make sure you obtain application keys before continuing by visiting:
// https://developer.yahoo.com/dashboard/createKey.html

// create a Yahoo! Open Application - http://developer.yahoo.com/dashboard
$consumerKey = 'dj0yJmk9WUxPUkhFUWxISWpvJmQ9WVdrOWFYWmhTVzVDTXpBbWNHbzlNVGt4TmpJNU1EazROdy0tJnM9Y29uc3VtZXJzZWNyZXQmeD01Ng--';
$consumerKeySecret = 'f893cf549be5cb37f83b1414e2ff212df2ea4c18';
$applicationId = 'ivaInB30';

// oauth dance if not authenticated
$session = YahooSession::requireSession($consumerKey, $consumerKeySecret, $applicationId);

// get oauthed user guid + profile
$user = $session->getSessionedUser();

$header   = '<h2><img src="http://delicious.com/favicon.ico" title="Delicious" width="16" height="16" />Delicious // social bookmarking</h2>';
$content
= '<div id="bookmarks">';

// if user is logged in and oauth is valid
if(is_object($user))
{
  // load y! profile data
  $profile = $user->getProfile();

  // get yap app instance for yql / small view
  $application = new YahooApplication($consumerKey, $consumerKeySecret);

  // delicious yql
  $yql = "use 'http://www.javarants.com/delicious/delicious.feeds.xml' as delicious.feeds;";
  $yql .= "select * from delicious.feeds";

  if(isset($_REQUEST['username']))
  {
    // validate delicious user and set username in datastore
    $username = htmlspecialchars($_REQUEST['username']);
  }
  else
  {
    // fetch user data store
    $username = apc_fetch('user_'.$user->guid);
  }

  if($username)
  {
    // if user has specified a delicious username
    $yql .= " where username='".$username."';";
  }

  $data = $application->query($yql);
  if(is_object($data) && isset($data->query->results->item))
  {
    // build small view content (my yahoo)
    $content .= '<ul class="delicious">';
      foreach($data->query->results->item as $bookmark):
        $content .= '<li><a href="'.$bookmark->link.'">'.$bookmark->title.'</a></li>';
      endforeach;
    $content .= '</ul>';

    if($username)
    {
      if(isset($_REQUEST['username']))
      {
        //  username is valid (yql returned results), so save for future requests
        apc_store('user_'.$user->guid, $username);
      }

      // update small view for user bookmarks
      $application->setSmallView($user->guid, $header.$content);
    }
  }
  else
  {
    $content .= '<ul><li><a href="http://delicious.com/popular">Invalid username...</a></li></ul>';
  }
}
$content  .= '</div>';

/*
 // available open tables

  use 'http://www.javarants.com/delicious/delicious.feeds.popular.xml' as delicious.feeds.popular;
  use 'http://www.javarants.com/delicious/delicious.feeds.xml' as delicious.feeds;

  use 'http://www.javarants.com/friendfeed/friendfeed.feeds.xml' as friendfeed.feeds;
  use 'http://www.javarants.com/friendfeed/friendfeed.home.xml' as friendfeed.home;
  use 'http://www.javarants.com/friendfeed/friendfeed.profile.xml' as friendfeed.profile;
  use 'http://www.javarants.com/friendfeed/friendfeed.rooms.xml' as friendfeed.rooms;
  use 'http://www.javarants.com/friendfeed/friendfeed.services.xml' as friendfeed.services;
  use 'http://www.javarants.com/friendfeed/friendfeed.updates.xml' as friendfeed.updates;

  use 'http://www.javarants.com/nyt/nyt.article.search.xml' as nyt.article.search;
  use 'http://www.javarants.com/nyt/nyt.bestsellers.history.xml' as nyt.bestsellers.history;
  use 'http://www.javarants.com/nyt/nyt.bestsellers.search.xml' as nyt.bestsellers.search;
  use 'http://www.javarants.com/nyt/nyt.bestsellers.xml' as nyt.bestsellers;
  use 'http://www.javarants.com/nyt/nyt.movies.critics.xml' as nyt.movies.critics;
  use 'http://www.javarants.com/nyt/nyt.movies.picks.xml' as nyt.movies.picks;
  use 'http://www.javarants.com/nyt/nyt.movies.reviews.xml' as nyt.movies.reviews;

  use 'http://www.javarants.com/shopping/shopping.product.search.xml' as shopping.product.search;

  use 'http://www.javarants.com/weather/weather.local.xml' as weather.local;
  use 'http://www.javarants.com/weather/weather.search.xml' as weather.search;

  use 'http://www.javarants.com/dopplr/dopplr.auth.xml' as dopplr.auth;
  use 'http://www.javarants.com/dopplr/dopplr.city.info.xml' as dopplr.city.info;
  use 'http://www.javarants.com/dopplr/dopplr.futuretrips.info.xml' as dopplr.futuretrips.info;
  use 'http://www.javarants.com/dopplr/dopplr.traveller.fellows.xml' as dopplr.traveller.fellows;
  use 'http://www.javarants.com/dopplr/dopplr.traveller.info.xml' as dopplr.traveller.info;
  use 'http://www.javarants.com/dopplr/dopplr.traveller.travelling.xml' as dopplr.traveller.travelling;
  use 'http://www.javarants.com/dopplr/dopplr.trips.info.xml' as dopplr.trips.info;

  use 'http://www.javarants.com/twitter/twitter.user.timeline.xml' as twitter.user.timeline;
  use 'http://www.javarants.com/twitter/twitter.user.profile.xml' as twitter.user.profile;
  use 'http://www.javarants.com/twitter/twitter.user.status.xml' as twitter.user.status;

  use 'http://www.javarants.com/wesabe/wesabe.tags.xml' as wesabe.tags;

  use 'http://www.javarants.com/whitepages/whitepages.search.xml' as whitepages.search;
  use 'http://www.javarants.com/whitepages/whitepages.reverse.xml' as whitepages.reverse;

  use 'http://www.javarants.com/github/github.repo.commits.xml' as github.repo.commits;
  use 'http://www.javarants.com/github/github.user.repos.xml' as github.user.repos;
  use 'http://www.javarants.com/github/github.user.info.xml' as github.user.info;

  use 'http://www.javarants.com/zillow/zillow.search.xml' as zillow.search;
*/

// set content type + language
// header("Content-Type: text/html; charset=utf-8");
// header("Content-Language: en");
/*
echo '<?xml version="1.0" encoding="utf-8"?>';
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" dir="ltr">
  <head profile="http://gmpg.org/xfn/11">
    <title>Delicious // social bookmarking</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="en" />
    <link rel="stylesheet" type="text/css" media="screen" href="http://yui.yahooapis.com/combo?3.0.0pr2/build/cssreset/reset-min.css&amp;3.0.0pr2/build/cssfonts/fonts-min.css&amp;3.0.0pr2/build/cssgrids/grids-min.css&amp;3.0.0pr2/build/cssbase/base-min.css&amp;3.0.0pr2/build/widget/assets/skins/sam/widget.css&amp;3.0.0pr2/build/widget/assets/skins/sam/widget-stack.css&amp;3.0.0pr2/build/overlay/assets/skins/sam/overlay.css" />
    <link rel="stylesheet" type="text/css" media="screen" href="common.css" />
    <link rel="shortcut icon" href="http://delicious.com/favicon.ico" />
    <body>

    <div id="itw" class="yui-d0 yui-skin-sam">
      <?php echo $content; ?>
    </div>

    <script type="text/javascript" src="http://yui.yahooapis.com/combo?3.0.0pr2/build/yui/yui-min.js&amp;3.0.0pr2/build/oop/oop-min.js&amp;3.0.0pr2/build/event/event-min.js&amp;3.0.0pr2/build/dom/dom-min.js&amp;3.0.0pr2/build/node/node-min.js&amp;3.0.0pr2/build/attribute/attribute-min.js&amp;3.0.0pr2/build/base/base-min.js&amp;3.0.0pr2/build/anim/anim-min.js&amp;3.0.0pr2/build/cookie/cookie-min.js&amp;3.0.0pr2/build/io/io-min.js&amp;3.0.0pr2/build/json/json-min.js&amp;3.0.0pr2/build/dump/dump-min.js&amp;3.0.0pr2/build/substitute/substitute-min.js&amp;3.0.0pr2/build/classnamemanager/classnamemanager-min.js&amp;3.0.0pr2/build/widget/widget-min.js&amp;3.0.0pr2/build/widget/widget-position-min.js&amp;3.0.0pr2/build/widget/widget-position-ext-min.js&amp;3.0.0pr2/build/widget/widget-stack-min.js&amp;3.0.0pr2/build/widget/widget-stdmod-min.js&amp;3.0.0pr2/build/overlay/overlay-min.js"></script>
    <script type="text/javascript" src="common.js"></script>
    </body>
</html>
*/
?>
<style type="text/css">
.yui-d0,.yui-d1,.yui-d1f,.yui-d2,.yui-d2f,.yui-d3,.yui-d3f{margin:auto;text-align:left;width:57.69em;}.yui-t1,.yui-t2,.yui-t3,.yui-t4,.yui-t5,.yui-t6{margin:auto;text-align:left;width:100%;}.yui-d0{margin:auto 10px;width:auto;}.yui-d0f{width:100%;}.yui-d2{width:73.076em;}.yui-d2f{width:950px;}.yui-d3{width:74.923em;}.yui-d3f{width:974px;}.yui-b{position:relative;}.yui-main .yui-b{position:static;}.yui-main{width:100%;}.yui-t1 .yui-main,.yui-t2 .yui-main,.yui-t3 .yui-main{float:right;margin-left:-25em;}.yui-t4 .yui-main,.yui-t5 .yui-main,.yui-t6 .yui-main{float:left;margin-right:-25em;}.yui-t1 .yui-b{float:left;width:12.30769em;}.yui-t1 .yui-main .yui-b{margin-left:13.30769em;}.yui-t2 .yui-b{float:left;width:13.84615em;}.yui-t2 .yui-main .yui-b{margin-left:14.84615em;}.yui-t3 .yui-b{float:left;width:23.0769em;}.yui-t3 .yui-main .yui-b{margin-left:24.0769em;}.yui-t4 .yui-b{float:right;width:13.8456em;}.yui-t4 .yui-main .yui-b{margin-right:14.8456em;}.yui-t5 .yui-b{float:right;width:18.4615em;}.yui-t5 .yui-main .yui-b{margin-right:19.4615em;}.yui-t6 .yui-b{float:right;width:23.0769em;}.yui-t6 .yui-main .yui-b{margin-right:24.0769em;}.yui-main .yui-b{float:none;width:auto;}.yui-gb .yui-u,.yui-g .yui-gb .yui-u,.yui-gb .yui-g,.yui-gb .yui-gb,.yui-gb .yui-gc,.yui-gb .yui-gd,.yui-gb .yui-ge,.yui-gb .yui-gf,.yui-gc .yui-u,.yui-gc .yui-g,.yui-gd .yui-u{float:left;}.yui-g .yui-u,.yui-g .yui-g,.yui-g .yui-gb,.yui-g .yui-gc,.yui-g .yui-gd,.yui-g .yui-ge,.yui-g .yui-gf,.yui-gc .yui-u,.yui-gd .yui-g,.yui-g .yui-gc .yui-u,.yui-ge .yui-u,.yui-ge .yui-g,.yui-gf .yui-g,.yui-gf .yui-u{float:right;}.yui-g div.first,.yui-gb div.first,.yui-gc div.first,.yui-gd div.first,.yui-ge div.first,.yui-gf div.first,.yui-g .yui-gc div.first,.yui-g .yui-ge div.first,.yui-gc div.first div.first{float:left;}.yui-g .yui-u,.yui-g .yui-g,.yui-g .yui-gb,.yui-g .yui-gc,.yui-g .yui-gd,.yui-g .yui-ge,.yui-g .yui-gf{width:49.1%;}.yui-gb .yui-u,.yui-g .yui-gb .yui-u,.yui-gb .yui-g,.yui-gb .yui-gb,.yui-gb .yui-gc,.yui-gb .yui-gd,.yui-gb .yui-ge,.yui-gb .yui-gf,.yui-gc .yui-u,.yui-gc .yui-g,.yui-gd .yui-u{width:32%;margin-left:2.0%;}.yui-gc div.first,.yui-gd .yui-u{width:66%;}.yui-gd div.first{width:32%;}.yui-ge div.first,.yui-gf .yui-u{width:74.2%;}.yui-ge .yui-u,.yui-gf div.first{width:24%;}.yui-g .yui-gb div.first,.yui-gb div.first,.yui-gc div.first,.yui-gd div.first{margin-left:0;}.yui-g .yui-g .yui-u,.yui-gb .yui-g .yui-u,.yui-gc .yui-g .yui-u,.yui-gd .yui-g .yui-u,.yui-ge .yui-g .yui-u,.yui-gf .yui-g .yui-u{width:49%;}.yui-g .yui-gc div.first,.yui-gd .yui-g{width:66%;}.yui-g .yui-gc .yui-u,.yui-gb .yui-gc .yui-u{width:32%;margin-right:0;}.yui-gb .yui-gc div.first{width:66%;}.yui-gb .yui-ge .yui-u,.yui-gb .yui-gf .yui-u{margin:0;}.yui-gb .yui-gd div.first{width:32%;}.yui-ge .yui-g{width:24%;}.yui-gf .yui-g{width:74.2%;}.yui-gb .yui-ge div.yui-u,.yui-gb .yui-gf div.yui-u{float:right;}.yui-gb .yui-ge div.first,.yui-gb .yui-gf div.first{float:left;}.yui-ge div.first .yui-gd .yui-u{width:65%;}.yui-ge div.first .yui-gd div.first{width:32%;}
h1{font-size:138.5%;}h2{font-size:123.1%;}h3{font-size:108%;}h1,h2,h3{margin:1em 0;}h1,h2,h3,h4,h5,h6,strong{font-weight:bold;}abbr,acronym{border-bottom:1px dotted #000;cursor:help;}em{font-style:italic;}blockquote,ul,ol,dl{margin:1em;}ol,ul,dl{margin-left:2em;}ol li{list-style:decimal outside;}ul li{list-style:disc outside;}dl dd{margin-left:1em;}th,td{border:1px solid #000;padding:.5em;}th{font-weight:bold;text-align:center;}caption{margin-bottom:.5em;text-align:center;}p,fieldset,table,pre{margin-bottom:1em;}textarea{width:12.25em;}

#itw {
  text-align: left;
  font-family: Tahoma, Georgia;
  background-color: #fff;
  color: #000;
}


img {
  margin: 4px;
}

a {
  text-decoration: none;
  font-weight: bold;
  color: #000;
}

a:hover {
  color: #000;
  text-decoration:underline;
}

div.delicious {
  padding: 0 0 0 0;
  margin: 0 0 0 0;
}

div.delicious ul {
  padding: 0 0 0 0;
  margin: 0 0 0 0;
}

div.delicious ul li {
  list-style-type: none;
  padding: 0 0 0 0;
  margin: 0 0 0 0;
}

div.delicious ul li a {
  font-weight: normal;
}

div.delicious h2 a {
  color: #000;
}
</style>
<div id="itw" class="yui-d0 yui-skin-sam">

  <?php echo $header; ?>

  <yml:form method="POST" view="YahooFullView">
   <fieldset>
    <legend>Delicious Username</legend>
    <input type="text" name="username" />
    <input type="submit" value="Refresh" />
   </fieldset>
  </yml:form>

  <?php echo $content; ?>
</div>