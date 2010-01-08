// a simplified version of step2 popuplib.js
var PopupManager = {
	popup_window:null,
	interval:null,
	interval_time:80,
	waitForPopupClose: function() {
		if(PopupManager.isPopupClosed()) {
			PopupManager.destroyPopup();
			window.location.reload();
		}
	},
	destroyPopup: function() {
		this.popup_window = null;
		window.clearInterval(this.interval);
		this.interval = null;
	},
	isPopupClosed: function() {
		return (!this.popup_window || this.popup_window.closed);
	},
	open: function(url, width, height) {
		this.popup_window = window.open(url,"",this.getWindowParams(width,height));
		this.interval = window.setInterval(this.waitForPopupClose, this.interval_time);
		
		return this.popup_window;
	},
	getWindowParams: function(width,height) {
		var center = this.getCenterCoords(width,height);
		return "width="+width+",height="+height+",status=1,location=1,resizable=yes,left="+center.x+",top="+center.y;
	},
	getCenterCoords: function(width,height) {
		var parentPos = this.getParentCoords();
		var parentSize = this.getWindowInnerSize();
		
		var xPos = parentPos.width + Math.max(0, Math.floor((parentSize.width - width) / 2));
		var yPos = parentPos.height + Math.max(0, Math.floor((parentSize.height - height) / 2));
		
		return {x:xPos,y:yPos};
	},
	getWindowInnerSize: function() {
		var w = 0;
		var h = 0;
		
		if ('innerWidth' in window) {
			// For non-IE
			w = window.innerWidth;
			h = window.innerHeight;
		} else {
			// For IE
			var elem = null;
			if (('BackCompat' === window.document.compatMode) && ('body' in window.document)) {
				elem = window.document.body;
			} else if ('documentElement' in window.document) {
				elem = window.document.documentElement;
			}
			if (elem !== null) {
				w = elem.offsetWidth;
				h = elem.offsetHeight;
			}
		}
		return {width:w, height:h};
	},
	getParentCoords: function() {
		var w = 0;
		var h = 0;
		
		if ('screenLeft' in window) {
			// IE-compatible variants
			w = window.screenLeft;
			h = window.screenTop;
		} else if ('screenX' in window) {
			// Firefox-compatible
			w = window.screenX;
			h = window.screenY;
	  	}
		return {width:w, height:h};
	}
}