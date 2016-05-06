jQuery(document).ready(function(){
  jQuery('select#edit-field-badge-type-und').change(function(){
	jQuery("input:radio").attr("checked", false);
  });
});