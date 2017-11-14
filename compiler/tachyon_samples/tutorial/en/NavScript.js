//*==============================================================================
//*    Helpware NavScript.js 1.02
//*      Copyright (c) 2008, Robert Chandler, The Helpware Group
//*      http://helpware.net/FAR/
//*      support@helpware.net
//*    Descriptions:
//*      Adds a Open Navigation link at page top if nav is currently not open
//*       or inside a CHM.
//*    Usage: Anyone may use and modify this file. If you modify it then
//*       please change its name and acknowledge my work in your (c) statement.
//*       Email me your changes if you think that others could benefit too.
//*    Instructions:
//*       http://weblog.helpware.net/?p=459
//*==============================================================================
//*  10-June-2008: 1.00 RWC - Original Version
//*  11-July-2008: 1.01 RWC - Now detects if inside CHM Help file. IsInsideChm()
//*  01-Dec-2015:  1.02 RWC - Added sessionStorage.setItem("hw_rightPath", location.href); Also added GetTopFNameAsSearchStr() 
//*     Works with FAR build 806 Uncompressed help (webhelp) and above. Backward compatible. Helps get around Chrome local browser restrictions. 
  

function WriteOpenNavLink(navLink) {
  var ss = '<div style="font-family: Verdana; font-size: 80%;color: #333333; margin:0px 0px 0px 0px; padding:0px 0px 0px 0px;">'
          +'<a href="'+navLink+'" style="padding:0px 5px 0px 5px;">&lt; Table Of Contents</a>'
          +'</div>';
  /*jslint evil: true */
  document.write(ss);
}

function IsNavOpen() {
  if ((top.right === null) || (top.right === undefined) || (top.right.location === null) || (top.right.location === undefined) || (typeof(top.right.location.href) != "string") || (top.right.location.href === ""))
    return false;  //no nav found
  else
    return true;  //nav found
}

function IsInsideChm() {   //returns true if current file is inside a CHM Help File
  var ra = /::/;
  return (location.href.search(ra) > 0); //If found then then we are in a CHM
  }

///RWC New
function GetTopFNameAsSearchStr()
{
  var topURL = ""; 
  if (typeof(sessionStorage) !== "undefined") {  // is HTML5 localStorage available?
    topURL = sessionStorage.getItem("hw_topPath");  //retrieve item set by parent page
    if (topURL === null)
      topURL = ""; 
  }

  topURL = topURL.substr(topURL.lastIndexOf("/") + 1, topURL.length);   // "http://c:/base/start.htm" --> "start.htm"
  if (topURL !== "")
    topURL = "?" + topURL;
  return topURL;
}


// pass in the directory level. 0 = if this HTML is same level as hh_goto.hh; 1 = of one level down etc
function WriteNavLink(aDirLevel) {
  if ((!IsNavOpen()) && (!IsInsideChm()))
  {
    var prefix = "";
    for (var n=0; n < aDirLevel; n++)
      prefix = prefix + "../";

    //find last back slash in path
    var x = location.href.lastIndexOf("/");       // get last splash of "path/dir/name.htm"
    for (var m=0; m < aDirLevel; m++)
      x = location.href.lastIndexOf("/", x-1);    // get 2nd last slash etc
    var curFileName = location.href.substr(x+1);

    var navLink = prefix + "hh_goto.htm" + GetTopFNameAsSearchStr() + "#" + curFileName;
    WriteOpenNavLink(navLink);
  }
  
  // for Cross domain or protected locations -- allows left frame doc to know path of right frame doc
  if (typeof(sessionStorage) !== "undefined") {  // works in HTML5 
    sessionStorage.setItem("hw_rightPath", location.href);  // remember content URL 
    if (sessionStorage.clickcount) sessionStorage.clickcount = Number(sessionStorage.clickcount) + 1; else sessionStorage.clickcount = 1;
  }  
  
}




