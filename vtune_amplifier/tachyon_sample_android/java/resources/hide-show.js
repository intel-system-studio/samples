$(document).ready(function(){
		  
// By default, h2 and h3 sections expand/collapse. If you want h4 sections to all expand/collapse:
// Replace all occurrences of ["h3] with ["h3,h4] (not including brackets).

   // jQuery methods go here...
	  
   // Add Buttons at the beginning of the content_block
   $("#contentblock").prepend('<div id="page_controls"><p><a href="#expand" id="expandAll" class="page_controls">Expand All</a> &nbsp;&nbsp; <a href="#hide" id="collapseAll" class="page_controls">Hide All</a> &nbsp;&nbsp; <a href="#print" id="printPage" class="page_controls">Print</a> &nbsp;&nbsp; <a href="#" id="reset" class="page_controls">Refresh</a></p></div>');

	  
	  // Hide all divs that have an h3 in them, but show the h3 headings.
		  $("h3").siblings().hide();
	  // Hide the Documentation and Legal sections, but show their headings.
	  // Commented out to remove this behavior April 18, 2016
		  // $("h2:contains('Documentation'), h2:contains('Legal')").siblings().hide(); 
	  // Don't hide the TOC
		  $("#mainmenu").children().show();
		  
		  
	  //Create expand and collapse icons
		  var expand_icon  =  "<a href='#'><div><img src='resources/spacer.png' width='1' height='1' border='0'/></div></a>";
		  var collapse_icon = "<a href='#'><div><img src='resources/spacer.png' width='1' height='1' border='0'/></div></a>";
		  // Prepend an expand (+)  icon to all h3 headings and certain h2 headings.
		  // $( "h2" ).each(function( i ) {
		    // Commented out the line that includes Documentation and Legal to remove this behavior for those sections April 18, 2016
		  // $( "h3, h2:contains('Documentation'), h2:contains('Legal')" ).not($("#mainmenu h3")).each(function( i ) {
		  $( "h3" ).not($("#mainmenu h3")).each(function( i ) {			  
			  $(this).prepend(expand_icon);  // Prepend the new elements
			  // $(this).addClass( "collapsed" ); // Use a new class 'collapsed' as a flag to show section is collapsed.
			  $("div", this).addClass( "expand_button" ); 			  
		  });
		  // Prepend a collapse (-) icon to certain h2 headings.
		  // Commented out the line that includes Documentation and Legal to remove this behavior for those sections April 18, 2016
		  // $( "h2" ).not($("h2:contains('Documentation'), h2:contains('Legal')")).each(function( i ) {
		  $( "h2" ).each(function( i ) {
			  $(this).prepend(collapse_icon);  // Prepend the new elements
			  // $(this).addClass( "expanded" );  // Use a new class 'expanded' as a flag to show section is expanded.
			  $("div", this).addClass( "collapse_button" );  
		  });
	  
	  // Clicking a H2 or H3 toggles (shows/hides) the content in the same div (siblings).
	  // It also changes the icon by swapping the .expand_button/.collapse_button classes.
		
		  $("h3,h2").click(function(){
			 $(this).not($("#mainmenu h3")).siblings().slideToggle(200); //Expand or collapse the section.
			 if ( $("div", this).hasClass("expand_button") ) {

				 $("div", this).removeClass("expand_button");
				 $("div", this).addClass( "collapse_button");  // Use a new class 'expanded' as a flag to show section is expanded.
				 
				 
				 } else {
					 $("div", this).removeClass("collapse_button");
					 $("div", this).addClass( "expand_button"); // Use a new class 'collapsed' as a flag to show section is collapsed.
				
				 };	 
					return false;
		  });



		  
		  
		  // Expand All
		  // Clicking Expand All expands all content in the contentblock except mainmenu (the TOC).
		  $("#expandAll").click(function() {
			  $("h2, h3, h4").not($("#mainmenu h3")).siblings().slideDown(200); //Expand all sections except the mainmenu.
			
			     // Now that it's expanded, change the button to a collapse button.			
				 $(".expand_button").addClass( "collapse_button");  // Use a new class 'collapse_button' as a flag to show section is expanded.
				 $(".expand_button").removeClass("expand_button");
				 return false;				 
		  });


		  // Collapse All
		  // Clicking Collapse All collapses allcontent in the contentblock except mainmenu (the TOC).
		  $("#collapseAll").click(function() {
			  $("h2, h3, h4").not($("#mainmenu h3")).siblings().slideUp(200); //Collapse all sections except the mainmenu.			 
				
			     // Now that it's collapsed, change the button to an expand button.				
				 $(".collapse_button").addClass( "expand_button");  // Use a new class 'expand_button' as a flag to show section is expanded.
				 $(".collapse_button").removeClass("collapse_button");
				 return false;				 
		  });


		  // Print All
		  // Clicking Print expands all content in the contentblock except mainmenu (the TOC).
		  // Then it prints the window.
		  $("#printPage").click(function() {
			  $("h2, h3, h4").not($("#mainmenu h3")).siblings().slideDown(0, function(){
			  }); //Expand all sections except the mainmenu.
			
			     // Now that it's expanded, change the button to a collapse button.
				 $(".expand_button").addClass( "collapse_button");   // Use a new class 'collapse_button' as a flag to show section is expanded.
				 $(".expand_button").removeClass("expand_button");
				 
				 window.print();
				 return false;				 
		  });	
	
    $('#reset').click(function() {
        location.reload();
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
