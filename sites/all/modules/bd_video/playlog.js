// $Id: playlog.js,v 1.1.2.2 2009/12/02 13:09:42 jbrown Exp $

// Copyright 2009 Jonathan Brown


var bdVideoPlayLogStartTime = 0;

function bdVideoPlayLog(player) {

  $.get(Drupal.settings.basePath + 'system/bd_video/playlog/' + 
    player.getClip().video_id + '/' + bdVideoPlayLogStartTime + '/' + player.getStatus().time);
}


function bdVideoPlayLogSeek(clip, time) {

  bdVideoPlayLog(this);

  bdVideoPlayLogStartTime = time;

  return true;
}


function bdVideoPlayLogStop(clip) {

  bdVideoPlayLog(this);

  bdVideoPlayLogStartTime = 0;
}


Drupal.behaviors.bd_video = function() {

  $(window).bind('unload', function(e){

    $f('*').each(function() { 
      
      if(this.getClip().playlog)
        bdVideoPlayLog(this);
    });
  });
}

