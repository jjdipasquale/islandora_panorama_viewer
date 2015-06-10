<?php

/**
 * @file
 * Handles all the callbacks.
 *
 * Currently there are three callbacks:
 * - Dimensions: Returns the page dimensions in JSON.
 * - Text: Returns the OCR'd page text as an html string.
 * - Search: Returns solr search results and word coordinates as JSON.
 */

/**
 * Fetches page metadata for use in the viewer such as width and height.
 *
 * Prints a JSON object repersenting the metadata.
 * This function exits.
 *
 * @param AbstractObject $object
 *   The page object to fetch the metadata from.
 */
function islandora_panorama_viewer_dimensions_callback(AbstractObject $object) {
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $dimensions = islandora_paged_content_get_page_dimensions($object);
  drupal_json_output($dimensions);
  drupal_exit();
}

/**
 * Prints the given page objects OCR datastream.
 *
 * This function exits.
 *
 * @param AbstractObject $object
 *   The page object whose OCR data-stream will be used for display.
 */
function islandora_panorama_viewer_text_callback(AbstractObject $object) {
  $ocr_exists = isset($object['OCR']);
  $accessible_ocr = isset($object['OCR']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['OCR']);
  echo $ocr_exists ? (
    $accessible_ocr ?
      htmlspecialchars($object['OCR']->content) :
      t('You do not have permission to view the OCR for this page.')) :
    t('Sorry, but there is no OCR available for the this page.');

  drupal_exit();
}

/**
 * Performs a solr search for the given term within the given panorama.
 *
 * Prints a JSON object repersenting the results.
 * This function exits.
 *
 * @param string $object_id
 *   The PID of the panorama object.
 * @param string $term
 *   The text to search for within the OCR'ed text.
 */
function islandora_panorama_viewer_search_callback($object_id, $term) {
  module_load_include('inc', 'islandora_paged_content', 'includes/highlighting');
  module_load_include('inc', 'islandora_panorama_viewer', 'includes/utilities');
  if (!module_exists('islandora_solr')) {
    drupal_exit();
  }

  $results = islandora_paged_content_perform_solr_highlighting_query($term, array(
    'fq' => array(format_string('!field:("info:fedora/!value" OR "!value")', array(
      '!field' => variable_get('islandora_panorama_viewer_ocr_filter_field', 'RELS_EXT_isMemberOf_uri_ms'),
      '!value' => $object_id,
    ))),
    'hl.tag.pre' => '{{{',
    'hl.tag.post' => '}}}',
    'defType' => 'dismax',
  ));
  $results = islandora_panorama_viewer_remap_highlighting_for_panoramaviewer_results($results);
  drupal_json_output($results);
  drupal_exit();
}

/* jd
*/
function islandora_panorama_viewer_xml_callback(AbstractObject $object) {
  global $base_url;

// get children -- and need to respect order
  $query = <<<EOQ
PREFIX islandora-rels-ext: <http://islandora.ca/ontology/relsext#>
SELECT ?object ?page 
FROM <#ri> 
WHERE
{
  ?object <fedora-rels-ext:isMemberOf> <info:fedora/{$object->id}> ;
  islandora-rels-ext:isSequenceNumber ?page ;
}
ORDER BY ?page
EOQ;
$djatokaUri = variable_get('islandora_paged_content_djatoka_url', 'http://localhost:8080/adore-djatoka');

// ASSUMPTION -- this viewer takes a cubic representation -- there are children {0,5} here
$children = $object->repository->ri->sparqlQuery($query);
  drupal_add_http_header("Content-Type", "text/xml; charset=utf-8");

$rft_id = $base_url . '/islandora/object/'; // need child PID/datastream/JP2/view;
$jp2_request_start = $djatokaUri . 'resolver?url_ver=Z39.88-2004';
$jp2_request_end =  "&amp;svc_id=info:lanl-repo/svc/getRegion&amp;svc_val_fmt=info:ofi/fmt:kev:mtx:jpeg2000&amp;svc.format=image/jpeg&amp;svc.rotate=0&amp;svc.level=";
$order = array();
$returnXML = '<?xml version="1.0" encoding="UTF-8"?>' . "\n";
if (count($children) > 0) {


  $returnXML .= <<<EOXML
<panorama id="">
  <view fovmode="0" pannorth="0">
    <start pan="0" fov="60" tilt="0"/>
    <min pan="0" fov="40" tilt="-90"/>
    <max pan="360" fov="105" tilt="90"/>
  </view>
<test>
		
</test>
  <input
	tilesize="1000"
	tilescale="1.009285051067781"
        prev5url="{$rft_id}{$children[5]['object']['value']}/datastream/JPG/view"
        prev4url="{$rft_id}{$children[4]['object']['value']}/datastream/JPG/view"
        prev3url="{$rft_id}{$children[3]['object']['value']}/datastream/JPG/view"
        prev2url="{$rft_id}{$children[2]['object']['value']}/datastream/JPG/view"
        prev1url="{$rft_id}{$children[1]['object']['value']}/datastream/JPG/view"
        prev0url="{$rft_id}{$children[0]['object']['value']}/datastream/JPG/view"
tile5url="{$rft_id}{$children[5]['object']['value']}/datastream/JPG/view"
tile4url="{$rft_id}{$children[4]['object']['value']}/datastream/JPG/view"
tile3url="{$rft_id}{$children[3]['object']['value']}/datastream/JPG/view"
tile2url="{$rft_id}{$children[2]['object']['value']}/datastream/JPG/view"
tile1url="{$rft_id}{$children[1]['object']['value']}/datastream/JPG/view"
tile0url="{$rft_id}{$children[0]['object']['value']}/datastream/JPG/view"
/>
<!--
tile0url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[0]['object']['value']}/datastream/JP2/view{$jp2_request_end}0"
tile5url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[5]['object']['value']}/datastream/JP2/view{$jp2_request_end}0"
tile4url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[4]['object']['value']}/datastream/JP2/view{$jp2_request_end}0"
tile3url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[3]['object']['value']}/datastream/JP2/view{$jp2_request_end}0"
tile2url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[2]['object']['value']}/datastream/JP2/view{$jp2_request_end}0"
tile1url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[1]['object']['value']}/datastream/JP2/view{$jp2_request_end}0"
<altinput
screensize="1024"
tilesize="960"
tilescale="1.005"
tile0url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[0]['object']['value']}/datastream/JP2/view{$jp2_request_end}1"
tile5url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[5]['object']['value']}/datastream/JP2/view{$jp2_request_end}1"
tile4url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[4]['object']['value']}/datastream/JP2/view{$jp2_request_end}1"
tile3url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[3]['object']['value']}/datastream/JP2/view{$jp2_request_end}1"
tile2url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[2]['object']['value']}/datastream/JP2/view{$jp2_request_end}1"
tile1url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[1]['object']['value']}/datastream/JP2/view{$jp2_request_end}1"
/>
<altinput
screensize="960"
tilesize="700"
tilescale="1.005"
tile0url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[0]['object']['value']}/datastream/JP2/view{$jp2_request_end}3"
tile5url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[5]['object']['value']}/datastream/JP2/view{$jp2_request_end}3"
tile4url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[4]['object']['value']}/datastream/JP2/view{$jp2_request_end}3"
tile3url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[3]['object']['value']}/datastream/JP2/view{$jp2_request_end}3"
tile2url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[2]['object']['value']}/datastream/JP2/view{$jp2_request_end}3"
tile1url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[1]['object']['value']}/datastream/JP2/view{$jp2_request_end}3"
/>
<altinput
screensize="480"
tilesize="416"
tilescale="1.005"
tile0url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[0]['object']['value']}/datastream/JP2/view{$jp2_request_end}2"
tile5url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[5]['object']['value']}/datastream/JP2/view{$jp2_request_end}2"
tile4url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[4]['object']['value']}/datastream/JP2/view{$jp2_request_end}2"
tile3url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[3]['object']['value']}/datastream/JP2/view{$jp2_request_end}2"
tile2url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[2]['object']['value']}/datastream/JP2/view{$jp2_request_end}2"
tile1url="{$jp2_request_start}&amp;rft_id={$rft_id}{$children[1]['object']['value']}/datastream/JP2/view{$jp2_request_end}2"
/>
-->
<control simulatemass="1" lockedmouse="0" lockedkeyboard="0" dblclickfullscreen="0" invertwheel="0" lockedwheel="0" invertcontrol="0" speedwheel="1" sensitivity="2"/>
</panorama>
EOXML;
}
echo $returnXML;
module_invoke_all('exit');
exit();
}



