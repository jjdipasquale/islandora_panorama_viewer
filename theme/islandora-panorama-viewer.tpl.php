<?php

/**
 * @file
 *
 * Theme for the PanoVR Panorama Viewer.
 */

?>

<?php
	$module_path = drupal_get_path('module', 'islandora_panorama_viewer');
	$xmlpath = '/panorama_viewer_xml/' . $object . '/xml';
?>


<div id="container" class="islandora-panorama-viewer" style="width:600px;height:600px">
   Loading the PanoVR Panorama Viewer, please wait...
	<script type="text/javascript" src="/<?php echo $module_path; ?>/js/pano2vrgyro.js"></script>
	<script type="text/javascript" src="/<?php echo $module_path; ?>/js/pano2vr_player.js"></script> 
	<script type="text/javascript" src="/<?php echo $module_path; ?>/js/skin.js"></script>
	<script type="text/javascript" src="/<?php echo $module_path; ?>/js/swfobject.js"></script>
	<script type="text/javascript" src="/<?php echo $module_path; ?>/js/hideURL.js"></script>

	<script type="text/javascript">
		// check for CSS3 3D transformations and WebGL
		if (ggHasHtml5Css3D() || ggHasWebGL()) {
		// use HTML5 panorama
			// create the panorama player with the container
			pano=new pano2vrPlayer("container");
			// add the skin object
			skin=new pano2vrSkin(pano,'<?php echo "/" . $module_path . "/"; ?>');
			// load the configuration
			pano.readConfigUrl('<?php echo $xmlpath; ?>');
			// hide the URL bar on the iPhone
			hideUrlBar();
			// add gyroscope controller
			gyro=new pano2vrGyro(pano,"container");
		} else 
		if (swfobject.hasFlashPlayerVersion("9.0.0")) {
			var flashvars = {};
			var params = {};
			params.quality = "high";
			params.bgcolor = "#ffffff";
			params.allowscriptaccess = "sameDomain";
			params.allowfullscreen = "true";
			var attributes = {};
			attributes.id = "pano";
			attributes.name = "pano";
			attributes.align = "middle";
			swfobject.embedSWF(
				"coming_soon.swf", "container", 
				"500", "500",
				"9.0.0", "", 
				flashvars, params, attributes);
			
		}
		</script>
		<noscript>
			<p><b>Please enable Javascript!</b></p>
		</noscript>
</div>
