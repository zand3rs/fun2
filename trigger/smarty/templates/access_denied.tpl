{* header *}
{include file="header.tpl"}
	    {* menu *}
	    {include file="menu.tpl"}

    
    <!-- form -->
    <form name="mainform" method="post" action="">

    
	   <!--Pagetitle -->
	   <div id="title">User Not Allowed</div>
           <div id="desc">
                Sorry, access is denied.
	   </div>
	   
	   
            <!-- status msg -->
	    {if $error_msg != ""}
	    	    <div id="errorbox">
	    	    <br/>
	    	        {$error_msg}
	    	        <br/>
	    	    </div>
	    {/if}
	    
            <!--Tablebody-->
	         <div id="wrapper">
	        
		 </div>
    		 <!--buttons-->
		      <div id="buttonh">
		      </div>
		<!--buttons-->

    
    </form>
    <!-- form -->

    
  </div>
  <!-- CONTENT Ends  -->

<!-- footer -->
{* footer *}
{include file="footer.tpl"}