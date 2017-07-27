$(document).ready(function(){
		  
// By default, h2 and h3 sections expand/collapse. If you want h4 sections to all expand/collapse:
// Replace all occurrences of ["h3] with ["h3,h4] (not including brackets).

   // jQuery methods go here...
	  
	  // Hide all divs that have an h3 in them, but show the h3 headings.
		  $("h3").siblings().hide();
	  // Hide the Documentation and Legal sections, but show their headings.
		  $("h2:contains('Documentation'), h2:contains('Legal')").siblings().hide(); 
	  // Don't hide the TOC
		  $("#mainmenu").children().show();
		  
		  
	  //Create expand and collapse icons
		  var expand_icon  =  "<a href='#'><div><img src='resources/spacer.png' width='1' height='1' border='0'/></div></a>";
		  var collapse_icon = "<a href='#'><div><img src='resources/spacer.png' width='1' height='1' border='0'/></div></a>";
		  // Prepend an expand (+)  icon to all h3 headings and certain h2 headings.
		  // $( "h2" ).each(function( i ) {
		  $( "h3, h2:contains('Documentation'), h2:contains('Legal')" ).not($("#mainmenu h3")).each(function( i ) {
			  $(this).prepend(expand_icon);  // Prepend the new elements
			  // $(this).addClass( "collapsed" ); // Use a new class 'collapsed' as a flag to show section is collapsed.
			  $("div", this).addClass( "expand_button" ); 			  
		  });
		  // Prepend a collapse (-) icon to certain h2 headings.
		  $( "h2" ).not($("h2:contains('Documentation'), h2:contains('Legal')")).each(function( i ) {
			  $(this).prepend(collapse_icon);  // Prepend the new elements
			  // $(this).addClass( "expanded" );  // Use a new class 'expanded' as a flag to show section is expanded.
			  $("div", this).addClass( "collapse_button" );  
		  });
	  
	  // Clicking a H2 or H3 toggles (shows/hides) the content in the same div (siblings).
	  // It also changes the icon by swapping the .expand_button/.collapse_button classes.
		
		  $("h3,h2").click(function(){
			 $(this).not($("#mainmenu h3")).siblings().toggle(200); //Expand or collapse the section.
			 if ( $("div", this).hasClass("expand_button") ) {

				 $("div", this).removeClass("expand_button");
				 $("div", this).addClass( "collapse_button");  // Use a new class 'expanded' as a flag to show section is expanded.
				 
				 
				 } else {
					 $("div", this).removeClass("collapse_button");
					 $("div", this).addClass( "expand_button"); // Use a new class 'collapsed' as a flag to show section is collapsed.
				
				 };	 
					return false;
		  });

	  // Clicking a link in the TOC expands the section you click on.
			$(".mainmenulinks, #footer a").click(function() {
				var section_id = $(this).attr("href");
				// In an element with the ID contained in the var section_id,
				// show all children of that element. The 'find' function is for
				// finding children.
				$(section_id).find("*").show();
				// $(section_id).find("*").toggle();
				// Toggle icons to "-" when showing the content.
				// selector: Search for child div that has class expand_button or collapse_button. 
				$(section_id).find("div.expand_button").removeClass("expand_button").addClass( "collapse_button");
			});
});
