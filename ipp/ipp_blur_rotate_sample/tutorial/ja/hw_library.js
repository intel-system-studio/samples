// Copyright © 2016, Rob Chandler.
// Please don't use this file without purchasing FAR 5. http://helpware.net/FAR/
// This effectively licenses you to use my code.
// Changes
// RWC: 2016-01-01 - Original version
// RWC: 2016-10-15 - Fix for IE11 blank content screen
// RWC: 2016-12-06 - Fixed odd crash in MS Edge -- sessionStorage changed to window.sessionStorage

//typeof someNonExxistant === "undefined" -- true
//var x; undefined -- if (x) fails
//var x=""; if (x) -- false
//var x=0; if (x) -- false
//var x=false; if (x) -- false
//var xx = document.getElementById("nonExistentID"); -- == null

// window.localStorage - stores data with no expiration date
// window.sessionStorage - stores data for one session (data is lost when the browser tab is closed)
// Note: Name/value pairs are always stored as strings. Remember to convert them to another format when needed!


var _default_StorageKeyPrefix = "Helpware_";
var _HTML5_Storage = (typeof window.sessionStorage !== "undefined");  // HTML5 localStorage available?

function storage_SetItem(key, val, fLocalStorage) {  //defaul is session storage
    if (_HTML5_Storage) {
        if (fLocalStorage) localStorage.setItem(key, val); 
        else window.sessionStorage.setItem(key, val); 
    }
}

function storage_GetItem(key, defaultVal, fLocalStorage) {  //defaul is session storage
    if (_HTML5_Storage) {
        var ret;
        if (fLocalStorage) ret = localStorage.getItem(key); 
        else ret = window.sessionStorage.getItem(key); 
        if (ret !== null)
            return ret;
    }
		if (defaultVal === undefined)
				defaultVal = null;
    return defaultVal;
}

function storage_RemoveItem(key, fLocalStorage) {
    if (_HTML5_Storage) {
        if (fLocalStorage) localStorage.removeItem(key); 
        else window.sessionStorage.removeItem(key); 
    }
}

function storage_RemoveItem(key, fLocalStorage) {
    if (_HTML5_Storage) {
        if (fLocalStorage) localStorage.removeItem(key); 
        else window.sessionStorage.removeItem(key); 
    }
}

function storage_Remove(objId, fLocalStorage) {
    var key = _default_StorageKeyPrefix + objId;
    storage_RemoveItem(key, fLocalStorage);
}

// Persistent Obj - requires HTML5

function storage_Save(objId, fLocalStorage, key) {
    var obj = document.getElementById(objId);
    if (!key) var key = _default_StorageKeyPrefix + objId;
    if (obj && obj.type) {
        if (obj.type == 'checkbox') 
        {
            if (obj.checked) storage_SetItem(key, "true", fLocalStorage);
            else storage_SetItem(key, "false", fLocalStorage);
        } 
        else if (obj.type == 'text') {
            storage_SetItem(key, obj.value, fLocalStorage);
        }
    }
}

function storage_Restore(objId, fLocalStorage, key, defaultValue) {
    var obj = document.getElementById(objId);
    if (!key) var key = _default_StorageKeyPrefix + objId;
    var v = storage_GetItem(key, defaultValue, fLocalStorage);
    if (obj && obj.type && v !== null) {
        if (obj.type == 'checkbox') 
        {
            if (v == "true") obj.checked = true; 
            else if (v == "false") obj.checked = false; 
            else if (defaultValue != null)
                obj.checked = (defaultValue == true);
        }
        else if (obj.type == 'text') {
            obj.value = v;
        }
    }
}

// Keys

function GetEventKeyCode(event) {
    var code = 0;   
    if (event.which) code = event.which;
    else if (event.keyCode) code = event.keyCode;
    return code;
}
      
// Common Code

var _DefaultPage_ = "index.htm";          //overridden by nav HTML files  
var RightFrameFound_AtLeastOnce = false;  //check security blocking

function RightFrameFound() {  //Check no security blocks or missing right frame
		try {
				if ((top.right && top.right.location && top.right.location.href && top.right.document) 
				&& (typeof top.right.location.href == "string") 
      	&& (top.right.location.href !== "")) { 
        	RightFrameFound_AtLeastOnce = true;
        	return true;
      	}
      	else return false;
    }
    catch(err) { return false; }
}

function GetRightPage() {
		if (RightFrameFound())
				return top.right.location.href; 
    else if (_HTML5_Storage) {   // HTML5 localStorage available?
				if (!RightFrameFound_AtLeastOnce) {
        	var rightPath = window.sessionStorage.getItem("hw_rightPath");  // item set by NavScript.js
        	if (rightPath)
          	return rightPath;
    		}	
		}
    return("");
}
  
function CloseNav() {
		try {
				var rightPage = GetRightPage();   
				if (rightPage)
						top.location.href = rightPage;
				else if (typeof treeSelected !== "undefined" && treeSelected) // use toc selection if available
				{ 
						var el = findLinkNode();
		  			if (el)
						top.location.href = el.getAttribute('href');
				}
				else top.location.href = _DefaultPage_;	
	  }
	  catch(err) {
				top.location.href = _DefaultPage_;
		}
}

function TrimURL(aref)  //Strip off leading protocol http:/// and make unix / separators
{
		sRet = aref;
    if ((sRet.substring(0, 7) == "http://") || (sRet.substring(0, 7) == "file://"))
				sRet = sRet.substring(7, sRet.length);
    if (sRet.substring(0, 8) == "https://")
				sRet = sRet.substring(8, sRet.length);
    if (sRet.substring(0, 1) == "/")
				sRet = sRet.substring(1, sRet.length);
    //Scrub embedded codes
    sRet = unescape(sRet);  //Converts %2520 -> %20  (under FireFox)
    sRet = unescape(sRet);  //Converts %20 = " "
    //set all Unix slashes /
    sRet = sRet.replace(/\\/g, "/");
    return sRet;
}

function TrimBookmark(url) {  // trim off #bookmark from URL
		if (url) {
				var i = url.indexOf("#");
				if (i > 0)
						url = url.substr(0, i);
		}	
		return url;
}

function GetDocWidth() 
{ 
		var x = 300; 
		if (self.innerHeight) // all except Explorer 
				x = self.innerWidth; 
		else if (document.documentElement && document.documentElement.clientHeight) // Explorer 6 Strict Mode 
				x = document.documentElement.clientWidth; 
		else if (document.body) // other Explorers 
				x = document.body.clientWidth;

		// This block fixes a bug in IE11 in which sometimes the main content pane is blank when it is an even number of pixels wide.
		// The following "if" block checks that the returned width is not greater than the width of mainDiv,
		// and if the returned width is greater, it is set to the width of mainDiv.
		// The getBoundingClientRect function can return the actual width of the element, keeping the real number
		// A support matrix for the function: http://caniuse.com/#feat=getboundingclientrect

		if (document.body.getBoundingClientRect) {    
			var bodyDivWidth = document.body.getBoundingClientRect().width;
			if (bodyDivWidth && x > bodyDivWidth)
				x = bodyDivWidth;
		}
		// End IE11 bug fix.
		return(x); 
} 

function GetDocHeight() 
{ 
		var y = 400; 
		if (self.innerHeight) // all except Explorer 
				y = self.innerHeight; 
		else if (document.documentElement && document.documentElement.clientWidth) // Explorer 6 Strict Mode 
				y = document.documentElement.clientHeight; 
		else if (document.body) // other Explorers 
				y = document.body.clientHeight; 
		return(y); 
} 









